#include "GuiNodeFactory.h"
#include <OpenMesh/Core/IO/IOInstances.hh>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <QFileInfo>
#include <Core/SceneGraph/OpenMeshNode.h>
#include <Core/DataStructures/OMesh.h>
#include <GUI/Application/MainApplication.h>
#include <PluginCore/PluginManager/PluginManager.h>
#include <Core/DataStructures/DepthImage.h>
#include <Core/SceneGraph/DepthImageNode.h>
#include <PluginCore/PluginInterface/MeshIOInterface.h>
#include <PluginCore/PluginInterface/ImageIOInterface.h>
namespace GCL { namespace GUI {

GuiNodeFactory::GuiNodeFactory(MainApplication &_mainapplication)
    :mainapplication_(_mainapplication)
{

}

GuiNodeFactory::~GuiNodeFactory()
{

}

SceneGraph::OpenMeshNode *GuiNodeFactory::createOpenMeshNode(const QString &filename, SceneGraph::Node *parent)
{
    if(!parent) return NULL;

    Plugins::MeshIOInterface* meshio_plugin =  mainapplication_.getPluginManager()->getImportMeshPluginByFilename(filename);
    if(!meshio_plugin) return NULL;
    std::shared_ptr<OMesh> mesh(new OMesh);
    if(meshio_plugin->importMesh(filename,mesh))
    {
        mainapplication_.addRecentFiles(filename);
        mesh->update_normals();
        mesh->MeshNormalize();
        SceneGraph::OpenMeshNode* pPointer = new SceneGraph::OpenMeshNode(mesh,parent);
        return pPointer;
    }
    return NULL;
}

SceneGraph::DepthImageNode *GuiNodeFactory::createDepthImageNode(const QString &filename, SceneGraph::Node *parent)
{
    if(!parent) return NULL;

    Plugins::ImageIOInterface* imageio_plugin =  mainapplication_.getPluginManager()->getImportImagePluginByFilename(filename);
    if(!imageio_plugin) return NULL;
    std::shared_ptr<DepthImage> depthimage(new DepthImage());
    if(imageio_plugin->importDepthImage(filename,depthimage))
    {
        depthimage->normalize();
        SceneGraph::DepthImageNode* pPointer = new SceneGraph::DepthImageNode(depthimage,parent);
        pPointer->nodeUpdated(Totally_Dirty);
        return pPointer;
    }
    return NULL;
}


}}
