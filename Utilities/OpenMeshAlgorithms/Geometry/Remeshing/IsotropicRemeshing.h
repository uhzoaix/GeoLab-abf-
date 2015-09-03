#ifndef ISOTROPIC_REMESHING_H
#define ISOTROPIC_REMESHING_H
#include <Utilities/OpenMeshAlgorithms/gcl_export.h>
#include <Core/DataStructures/OMesh.h>
#include <Utilities/OpenMeshAlgorithms/Geometry/BasicMeasuring/BasicMeasuring.h>
#include <Utilities/OpenMeshAlgorithms/Geometry/NearestTriangleSearch/NearestTriangleSearch.h>

namespace GCL { namespace Utilities {

  class GCL_OPENMESHALGORITHMS_SHARED_EXPORT IsotropicRemeshing {

  public:


    explicit IsotropicRemeshing(Scalar _targetLength, Scalar _upperbound_scale = 1.25, Scalar _lowerbound_scale = 0.8);
    ~IsotropicRemeshing();
    
    void remesh(OMesh* _mesh,unsigned int _iterations);
    
    void splitLongEdges(OMesh* _mesh);   
    void collapseShortEdges(OMesh* _mesh);
    void equalizeValences(OMesh* _mesh);
    void tangentialRelaxation(OMesh* _mesh);
    void projectBack(OMesh* _mesh, OMesh* _oldMesh, NearestTriangleSearch& nts);
    void areaEqualization(OMesh* _mesh);

    //public parameter
    Scalar targetLength_;

    bool split_;
    bool collapse_;
    bool equalizeValence_;
    bool equalizeArea_;
    bool relax_;
    bool project_;

    int collapseIterations_;
    int valenceIterations_;

    bool fixSelectedFaces_;      //Fixes all selected faces completely
    bool preserveSelectedEdges_; //Chains of selected edges can be subdivided/collapsed, but do not move

  private:

    //utilitiy functions to mark and preserve edges:
    inline void setPreserved( OMesh* _mesh, OMesh::EHandle eh ){
      _mesh->status(eh).set_selected(true);
    }

    inline bool isPreserved( const OMesh* _mesh, OMesh::EHandle eh ){
      if(!preserveSelectedEdges_) return false;
      return _mesh->status(eh).selected(); }

    inline bool isPreserved( const OMesh *_mesh, OMesh::VHandle vh ){
      if(!preserveSelectedEdges_) return false;
      for(OMesh::ConstVertexEdgeIter veI=_mesh->cve_iter(vh); veI; ++veI )
        if( _mesh->status(veI).selected() ) return true;
      return false;
    }

	inline int isPreservedNumber( const OMesh *_mesh, OMesh::VHandle vh ){
	  if(!preserveSelectedEdges_) return 0;
	  int counter = 0;
	  for(OMesh::ConstVertexEdgeIter veI=_mesh->cve_iter(vh); veI; ++veI )
		if( _mesh->status(veI).selected() ) counter++;
	  return counter;
	}

	inline bool isPreservationBoundary( const OMesh *_mesh, OMesh::VHandle vh ){
	  if(!preserveSelectedEdges_) return false;
	  int c=0;
	  for(OMesh::ConstVertexEdgeIter veI=_mesh->cve_iter(vh); veI; ++veI ){
		if( _mesh->status(veI).selected() ) c++;
	  }
	  if(c==1) return true;
	  return false;
	}

    inline bool isFix( const OMesh* _mesh, OMesh::FHandle fh ){
      if(!fixSelectedFaces_) return false;
      return _mesh->status(fh).selected(); }

    inline bool isFix(const OMesh *_mesh, OMesh::VHandle vh){
      if(!fixSelectedFaces_) return false;
      for(OMesh::ConstVertexFaceIter vfI=_mesh->cvf_iter(vh); vfI; ++vfI )
        if( _mesh->status(vfI).selected() ) return true;
      return false;
    }

    inline bool isFix(const OMesh *_mesh, OMesh::EHandle eh){
	  OMesh::FHandle f10 = _mesh->face_handle(_mesh->halfedge_handle(eh, 0));
	  OMesh::FHandle f01 = _mesh->face_handle(_mesh->halfedge_handle(eh, 1));
	  bool b0 = false;
	  if(f10.is_valid()) b0 = isFix(_mesh,f10);
	  bool b1 = false;
	  if(f01.is_valid()) b1 = isFix(_mesh,f01);
	  return b0 || b1;
    }

    //parameter:
    Scalar lowerBoundLength_;
    Scalar higherBoundLength_;

    //data:
    OpenMesh::VPropHandleT<OMesh::Point> update_;

	//debug:
	bool anyPreservationCycle( OMesh* _mesh){
		for(OMesh::FaceIter it=_mesh->faces_begin();it!=_mesh->faces_end();++it){

			bool p=true;
			for(OMesh::FaceEdgeIter feIt=_mesh->fe_iter(it);feIt;++feIt){
				if(!isPreserved(_mesh,feIt)) p=false;
			}
			if(p) return true;


/*			Scalar area=utilities::BasicMeasuring::triangleSurfaceArea(it,_mesh);
			if(area<0.00000000000000001){
				return false;
			}
			*/
		}
		return false;
	}
  };

}}//namespaces

#endif// ISOTROPIC_REMESHING_H
