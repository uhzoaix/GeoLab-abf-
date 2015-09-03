#include "pluginlaplaciansmoothing.h"
#include <Core/DataStructures/OMesh.h>
#include <Core/gclnamespace.h>
#include <Eigen/Sparse>
#include <Utilities/OpenMeshAlgorithms/LeastSquareLaplacian/LS_LaplacianEigenSolver.h>
#include <Core/Utilities/ProgressTool.h>
namespace GCL { namespace Plugins {

LaplacianSmoothingPlugin::LaplacianSmoothingPlugin()
{
    this->setObjectName("LaplacianSmoothing");
    initParameters();
}

int LaplacianSmoothingPlugin::getFilterType() const
{
    return  MeshFilterInterface::FILTER_SMOOTHING;
}

void LaplacianSmoothingPlugin::initParameters()
{
    // 1: 获得参数列表的引用
    RichParameters& paras = this->Parameters();
    // 2: 添加一个int型参数
    paras.addParameter("steps",(int)3,tr("Smoothing steps"),tr("The number of the times that the whole algorithm is iterated"));
    // 3: 添加一个StringList型参数
    QStringList strlist;
    strlist.push_back(tr("Local"));
    strlist.push_back(tr("Global"));

    paras.addParameter("Type",strlist,0,tr("Type"),tr("Local or Global Laplacian smoothing"));
    // 4: 添加三个bool型参数
    paras.addParameter("1D_Boudary",true,tr("1D Boundary Smooth"),tr("..."));
    paras.addParameter("ctg_weight",false,tr("Cotangent weighting"),tr("..."));
    paras.addParameter("only_selected",false,tr("Affect only selected surface"),tr("..."));
    // 5: 添加一个double型参数
    paras.addParameter("steplength",(double)0.2,tr("Step Length"),
                       tr("Step length for each iteration"));
    // 6: 设置该算法的名称
    paras.setName("Laplacian Smoothing");
    paras.setLabel(tr("Laplacian Smoothing")); // 前面加tr为可翻译项
    paras.setIntroduction(tr("Laplacian Smoothing algorithms: the minimal surface."
                             "Contributed by Shiwei Wang(shiweiw7@mail.ustc.edu.cn)"));
}



bool LaplacianSmoothingPlugin::apply()
{
    if(!this->getMesh()) return false;
    OMesh& mesh = *this->getMesh();
    RichParameters& paras = this->Parameters();
    int type  = paras.getListIndex("Type");
    int step = paras.getValueInt("steps");
    double steplength = paras.getValueDouble("steplength");
    if(step < 0) step = 0;
    if(step > 100) step = 100;
    if(steplength < 0) steplength = 0;
    if(steplength > 1) steplength = 1;
    while(step--)
    {
        if(type == 0)
        {
            localLaplacianSmoothing(mesh,steplength);
        }
        else
        {
            globalLaplacianSmoothing(mesh,steplength);
        }
    }
    mesh.update_normals();
    updateMesh(Geometry_Dirty);
    return true;
}

void LaplacianSmoothingPlugin::localLaplacianSmoothing(OMesh &mesh, double steplen)
{
    for(size_t i=0; i < mesh.getPointsNumber(); i++)
    {
        OMesh::VertexHandle vh = mesh.vertex_handle_(i);
        Vec3 center(0,0,0);
        int count = 0;
        for(OMesh::ConstVertexVertexIter cvviter=mesh.cvv_iter_(vh); cvviter; cvviter++)
        {
            center += mesh.point_(*cvviter);
            count++;
        }
        if(count)
        {
            center = center * 1.0 / (Scalar)(count);
        }
        mesh.point_(vh) = mesh.point_(vh) - (mesh.point_(vh) - center) * steplen;
    }
}

void LaplacianSmoothingPlugin::globalLaplacianSmoothing(OMesh &mesh, double steplen)
{

    std::shared_ptr< Utilities::LeastSquareLaplacian > solver( new Utilities::LS_LaplacianEigenSolver<Eigen::SimplicialLDLT<Eigen::SparseMatrix<double> > >());
    solver->compute(mesh,steplen);

    std::vector<Vec3> lb,b,x;
    for(size_t i=0; i < mesh.getPointsNumber(); i++)
    {
        lb.push_back(Vec3(0));
        b.push_back(mesh.getVertexPosition(i));
    }
    solver->solve(lb,b,x);
    for(size_t i=0; i < mesh.getPointsNumber(); i++)
    {
        mesh.setVertexPosition(i,x[i]);
    }
}



}}
