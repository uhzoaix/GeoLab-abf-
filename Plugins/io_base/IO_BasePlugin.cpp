#include "IO_BasePlugin.h"

#include <OpenMesh/Core/IO/IOInstances.hh>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <Core/DataStructures/OMesh.h>
#include <Core/Utilities/ProgressTool.h>
#include <QFileInfo>

namespace GCL { namespace Plugins {


IO_BasePlugin::IO_BasePlugin(QObject *parent) :
    MeshIOInterface(parent)
{
    this->setObjectName("io_base");
}





QList<MeshIOInterface::Format> IO_BasePlugin::importFormats() const
{
    QList<MeshIOInterface::Format> list;
    list.append(MeshIOInterface::Format(tr("Alias Wavefront Object"),"obj"));
    list.append(MeshIOInterface::Format(tr("STL File format"),"stl"));
    list.append(MeshIOInterface::Format(tr("OFF File format"),"off"));
    list.append(MeshIOInterface::Format(tr("PLY File format"),"ply"));
    list.append(MeshIOInterface::Format(tr("OMR File format"),"omr"));
    return list;
}

QList<MeshIOInterface::Format> IO_BasePlugin::exportFormats() const
{
    QList<MeshIOInterface::Format> list;
    list.append(MeshIOInterface::Format(tr("Alias Wavefront Object"),"obj"));
    list.append(MeshIOInterface::Format(tr("STL File format"),"stl"));
    list.append(MeshIOInterface::Format(tr("OFF File format"),"off"));
    list.append(MeshIOInterface::Format(tr("PLY File format"),"ply"));
    list.append(MeshIOInterface::Format(tr("OMR File format"),"omr"));
    return list;
}

bool IO_BasePlugin::importMesh(const QString &filename,const std::shared_ptr<OMesh>& mesh)
{
    QFileInfo fileInfo(filename);
    if(fileInfo.suffix() == "obj")
    {
        return this->importMesh_By_OBJ_Format(filename,mesh);
    }
    else if(fileInfo.suffix() == "stl")
    {
        return this->importMesh_By_STL_Format(filename,mesh);
    }
    return OpenMesh::IO::read_mesh((*mesh),filename.toLocal8Bit().data());
}

bool IO_BasePlugin::exportMesh(const QString &filename, const std::shared_ptr<OMesh> &mesh)
{
    return OpenMesh::IO::write_mesh((*mesh),filename.toLocal8Bit().data());
}

bool IO_BasePlugin::importMesh_By_OBJ_Format(const QString &filename, const std::shared_ptr<OMesh> &mesh)
{
    FILE*fp = fopen(filename.toLocal8Bit().data(),"r");
    if(!fp)
    {
        DebugLog<<"importMesh_By_OBJ_Format : open file failed"<<std::endl;
        return NULL;
    }
    const int maxSize = 512;
    char pLine[maxSize];
    fseek(fp,0,SEEK_END);
    double filesize = ftell(fp);
    PROGRESS_INIT(100,"Import OBJ File...");
    fseek(fp,0,SEEK_SET);
    //    std::vector< Vec3 > normalList;
    //    std::vector< Vec3 > texcordList;
    int progressCount = 0;
    while(fgets(pLine,maxSize,fp))
    {
        double filepos = ftell(fp);
        if(filepos / filesize * 100 > progressCount)
        {
            progressCount = filepos / filesize * 100;
            PROGRESS_POS(progressCount);
        }
        if(pLine[0] == 'v' && pLine[1] == ' ')
        {
            Vec3 pos;
            //            Vec4 color(0.86,0.86,0.86,1);
            char* tok =strtok(pLine," ");
            for(int i=0; i < 3; i++)
            {
                tok = strtok(NULL," ");
                if(!tok) break;
                if(i < 3)
                {
                    pos[i] = (Scalar)atof(tok);
                }
            }
            mesh->add_vertex(pos);
            //            pvert->setColor(color);
        }
        else if(pLine[0] == 'f' && pLine[1] == ' ')
        {

            char* tok = strtok(pLine," ");
            std::vector<unsigned int> fid;
            while((tok = strtok(NULL," ")))
            {

                char temp[100];
                strcpy(temp,tok);
                temp[strcspn(temp,"/")] = 0;
                int id = (int)strtol(temp,NULL,10) - 1;
                if(id >= 0)
                    fid.push_back(id);
            }

            if(fid.size() < 3) continue;
            for(int i=1; i < (int)fid.size() - 1; i++)
            {
                if(fid[0] < mesh->n_vertices_()  && fid[i] < mesh->n_vertices_() && fid[i+1] < mesh->n_vertices_())
                {
                    OMesh::VHandles vhandles;
                    vhandles.push_back(mesh->vertex_handle_(fid[0]));
                    vhandles.push_back(mesh->vertex_handle_(fid[1]));
                    vhandles.push_back(mesh->vertex_handle_(fid[2]));
                    mesh->add_face_(vhandles);
                }
            }

        }
    }
    fclose(fp);
    PROGRESS_FINISH();
    DebugLog<<"Import Vertex Number: "<< mesh->n_vertices_()<<" Face Number: "<< mesh->getFacesNumber()<<std::endl;
    return true;
}

bool IO_BasePlugin::importMesh_By_STL_Format(const QString &filename, const std::shared_ptr<OMesh> &mesh)
{
    bool isBinary = false;
    FILE*fp = fopen(filename.toLocal8Bit().data(),"r");
    if(!fp)
    {
        DebugLog<<"ParseMesh3DBySTL : open file failed"<<std::endl;
        return NULL;
    }
    char SolidStr[6];
    char SolidUpper[6] = "SOLID";
    char SolidLower[6] = "solid";
    fread(SolidStr,sizeof(char),5,fp);
    for(int i=0; i < 5; i++)
    {
        if(SolidStr[i] != SolidUpper[i] && SolidStr[i] != SolidLower[i])
        {
            isBinary = true;
            break;
        }
    }

    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp);
    int facenum;



    fseek(fp,80,SEEK_SET);
    fread(&facenum,sizeof(int),1,fp);


    int expected_file_size = 80 + 4 + facenum * (sizeof(short) + sizeof(float) * 3 * 4);
    if(file_size == expected_file_size) isBinary = true;

    fclose(fp);
    std::vector< std::vector<Vec3 > > vdata;
    // read data from file and save in vdata
    if(isBinary)
    {
        fp = fopen(filename.toLocal8Bit().data(),"rb");
        if(!fp)
        {
            DebugLog<<"ParseMesh3DBySTL : open file failed in binary"<<std::endl;
        }
        fseek(fp,80,SEEK_SET);
        fread(&facenum,sizeof(int),1,fp);

        PROGRESS_INIT(facenum * 2,"Import STL file...");
        //        std::map< Vec3, int> vertPosMap;
        for(int fid=0; fid < facenum; fid++)
        {
            float xx,yy,zz;
            unsigned short attr;
            fread(&xx,sizeof(xx),1,fp);
            fread(&yy,sizeof(yy),1,fp);
            fread(&zz,sizeof(zz),1,fp);

            std::vector< Vec3 > pos;
            std::set< Vec3 > validVec; // check if there are two vertexs at the same position
            for(int vid=0; vid < 3; vid++)
            {
                fread(&xx,sizeof(xx),1,fp);
                fread(&yy,sizeof(yy),1,fp);
                fread(&zz,sizeof(zz),1,fp);
                pos.push_back(Vec3(xx,yy,zz));
                validVec.insert(pos[vid]);
            }
            fread(&attr,sizeof(attr),1,fp);
            if(validVec.size() < 3)
            {
                //                InfoLog << "Degenerate triangle detect "<<std::endl;
            }
            else
            {
                vdata.push_back(pos);
            }
            if(fid % 10 == 0)
            {
                PROGRESS_POS(fid);
            }
        }

        fclose(fp);

    }
    else // !isBinary
    {
        fp = fopen(filename.toLocal8Bit().data(),"r");
        if(fp == NULL)
        {
            DebugLog << "Parse Mesh3D by STL :: Open file failed in ascii "<<std::endl;
            return false;
        }
        // skip the first line of the file
        while(getc(fp) != '\n')
        {
        }
        // read file
        while(!feof(fp))
        {

            float xx, yy, zz;
            int ret = fscanf(fp,"%*s %*s %f %f %f\n",&xx,&yy,&zz);
            if(ret != 3) continue;
            ret = fscanf(fp,"%*s %*s");
            std::vector< Vec3 > pos;
            std::set< Vec3 > validVec; // check if there are two vertexs at the same position
            for(int vid=0; vid < 3; vid++)
            {
                ret = fscanf(fp,"%*s %f %f %f\n",&xx,&yy,&zz);
                if(ret != 3)
                {
                    ErrorLog << "error: parse position error" <<std::endl;
                }
                pos.push_back(Vec3(xx,yy,zz));
                validVec.insert(pos[vid]);
            }
            ret = fscanf(fp,"%*s");
            ret = fscanf(fp,"%*s");
            if(validVec.size() != 3)
            {
                //                InfoLog << "Degenerate triangle detect "<<std::endl;
            }
            else
            {
                vdata.push_back(pos);
            }
        }
        fclose(fp);
    }
    // Generate LightMesh3D from vdata
    std::map<Vec3, OMesh::VertexHandle>            vMap;
    std::map<Vec3, OMesh::VertexHandle>::iterator  vMapIt;
    for(size_t i=0; i < vdata.size(); i++)
    {
        OMesh::VHandles vhandles;
        for(int j=0; j < 3; j++)
        {
            Vec3 v = vdata[i][j];
            // has vector been referenced before?
            if ((vMapIt=vMap.find(v)) == vMap.end())
            {
                // No : add vertex and remember idx/vector mapping
                OMesh::VertexHandle handle = mesh->add_vertex(v);
                vhandles.push_back(handle);
                vMap[v] = handle;
            }
            else
                // Yes : get index from map
                vhandles.push_back(vMapIt->second);
        }
        // Add face only if it is not degenerated
        if ((vhandles[0] != vhandles[1]) &&
                (vhandles[0] != vhandles[2]) &&
                (vhandles[1] != vhandles[2]))
        {


            mesh->add_face(vhandles);
        }
        if(i % 10 == 0)
        {
            PROGRESS_POS(vdata.size() + i);
        }
    }
    PROGRESS_FINISH();
    return true;
}


}}
