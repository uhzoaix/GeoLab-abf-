#ifndef ABFPPPARAMETRIZATIONPLUGIN_H
#define ABFPPPARAMETRIZATIONPLUGIN_H

#include "ABFppEigenSolver.h"
#include "MeshTriangle.h"
#include "ParaCoordinate.h"
#include <PluginCore\PluginInterface\MeshFilterInterface.h>
#include <Core/DataStructures/OMesh.h>
#include <vector>

namespace GCL {

class OMesh;
namespace Plugins {
/*
 * @brief: ABFppParametrizationPlugin Class
			angle based flattening plus plus algorithm

 * @author: uhzoaix(xiangyu zhu)
 * @date:  I don't know when it will be finished
 * @ref: ABF++: Fast and Robust Angle Based Flattening By A.Sheffer , B.Levy , etc
 */

class ABFppParametrizationPlugin : public MeshFilterInterface
{
	Q_OBJECT
	GCL_MESH_PLUGIN_IID_EXPORTER(GCL_MESHFILTER_INTERFACE_IID)
	Q_INTERFACES(GCL::Plugins::MeshFilterInterface)

public:
	explicit ABFppParametrizationPlugin();
	~ABFppParametrizationPlugin();

	int getFilterType() const;
	void initParameters();

public slots:
	bool apply();

public:
	// To found equations in angle space and solve it
	// We use the same names of variables as in the paper

	//INITIALIZATION
	void Init(OMesh* mesh);
	// visit each vertex to find all triangles
	void FindTriangle(OMesh* mesh);
	
	// PROCESS
	// parametrization for meshes up to 300k 
	// epsilon is the error in Newton's iterative method
	void DirectABFpp(OMesh& mesh,double epsilon);

	// parametrization for huge meshes
	// TODO: hierarchy ABF++,but we need simpilification
	void HierarchyABFpp(OMesh& mesh,double epsilon);

	// REMESH
	// to reconstruct the mesh by angle
	void Reconstruct();
	void UpdateToParametrization(OMesh& mesh);

public:
	// assisting functions

	//the number of angles
	unsigned int  AngleNum();
	double GetAngle(int f_idx,int v_idx);
	void SetAngle(int f_idx,int v_idx,double angle);
	// the number of parameters: lambda
	unsigned int ParameterNum();
	// get the interior points' number
	unsigned int InPointNum();

	// check if the mesh has boundary
	bool has_boundary();

	// Project a node giving the face and the edge
	void ProjectNode(OMesh::HalfedgeHandle heh);


private:
	OMesh* mesh_;
	// check if mesh is a triangle mesh
	bool valid_mesh_flag_;
	// all triangles in mesh to save angles&vert indices
	std::vector <MeshTriangle> mesh_triangles_;
	// parametrization 2D coordinates
	std::vector <Utilities::ParaCoordinate> para_coors_;
};

}}

#endif // ABFPPPARAMETRIZATIONPLUGIN_H
