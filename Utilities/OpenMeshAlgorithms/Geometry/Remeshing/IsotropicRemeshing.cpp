#include "IsotropicRemeshing.h"
#include <Core/Utilities/ProgressTool.h>

namespace GCL { namespace Utilities {

IsotropicRemeshing::IsotropicRemeshing(Scalar _targetLength, Scalar _upperbound_scale, Scalar _lowerbound_scale) {
  targetLength_ = _targetLength;
  lowerBoundLength_ = _upperbound_scale * targetLength_;
  higherBoundLength_ = _lowerbound_scale * targetLength_;

  collapseIterations_=100;
  valenceIterations_    =100;

  split_ = true;
  collapse_ = true;
  equalizeValence_ = true;
  equalizeArea_ = true;
  relax_ = true;
  project_ = true;

  fixSelectedFaces_ = false;
  preserveSelectedEdges_ = false;
}


IsotropicRemeshing::~IsotropicRemeshing() {
}


void IsotropicRemeshing::remesh(OMesh* _mesh,unsigned int _iterations) {
  //store the old mesh
  OMesh oldMesh = *(_mesh);

  //compute the nearest point on mesh for each point on curves
  NearestTriangleSearch nts;
  nts.initializeKDTreeBasedSearchStructure(&oldMesh, 12, 30);

  _mesh->add_property(update_);
  _mesh->update_normals();

  //meshBackup_ = *_mesh;
  for (unsigned int i=0; i<_iterations; ++i) {
    if(split_)            splitLongEdges(_mesh);
	if(collapse_)         collapseShortEdges(_mesh);
    if(equalizeValence_)  equalizeValences(_mesh);
    if(relax_)            tangentialRelaxation(_mesh);
    if(project_)          projectBack(_mesh, &oldMesh, nts);
  }
  _mesh->garbage_collection();
  _mesh->remove_property(update_);
  _mesh->update_normals();

  nts.destroyKDTreeBasedSearchStructure();
}

void IsotropicRemeshing::splitLongEdges(OMesh* _mesh) {
  OMesh::EIter     e_it, e_end;
  OMesh::VHandle   v0, v1, vh;
  OMesh::EHandle   eh, e0, e1;
  OMesh::FHandle   f0, f1, f2, f3;
  bool            finished;
  int             i;
  PROGRESS_INIT(100,"splitLongEdges");
  for (finished=false, i=0; !finished && i<100; ++i) {
    finished = true;

    for (e_it=_mesh->edges_begin(), e_end=_mesh->edges_end(); e_it!=e_end; ++e_it) {

      if(isFix(_mesh,e_it)) continue;

      OMesh::HHandle h10 = _mesh->halfedge_handle(e_it, 0);
      OMesh::HHandle h01 = _mesh->halfedge_handle(e_it, 1);

      v0  = _mesh->to_vertex_handle(h10);
      v1  = _mesh->to_vertex_handle(h01);

      const OMesh::Point& p0 = _mesh->point(v0);
      const OMesh::Point& p1 = _mesh->point(v1);

      if ( (p0-p1).length() > higherBoundLength_ ) {
        vh = _mesh->add_vertex((p0+p1)*0.5);
        _mesh->set_normal(vh, (_mesh->normal(v0) + _mesh->normal(v1)).normalize());
        _mesh->split(e_it, vh);

        if(isPreserved(_mesh,e_it)){
          OMesh::HHandle he = _mesh->find_halfedge(v0,vh);
          if(he.is_valid()) setPreserved( _mesh, _mesh->edge_handle(he));
          he = _mesh->find_halfedge(v1,vh);
          if(he.is_valid()) setPreserved( _mesh, _mesh->edge_handle(he));
        }

        finished = false;
      }
    }
    PROGRESS_POS(i);
  }
  PROGRESS_FINISH();
}

void IsotropicRemeshing::collapseShortEdges(OMesh* _mesh) {
  OMesh::EIter     e_it, e_end;
  OMesh::CVVIter   vv_it;
  OMesh::VHandle   v0, v1;
  OMesh::HHandle   h0, h1, h01, h10;
  bool            finished, b0, b1;
  int             i;
  bool            hcol01, hcol10;
  PROGRESS_INIT(collapseIterations_,"collapseShortEdges");
  for (finished=false, i=0; !finished && i<collapseIterations_; ++i) {
    finished = true;

    for (e_it=_mesh->edges_begin(), e_end=_mesh->edges_end(); e_it!=e_end;++e_it) {
      if (!_mesh->status(e_it).deleted()) { // might already be deleted

		if(isFix(_mesh,e_it)) continue;

        h10 = _mesh->halfedge_handle(e_it, 0);
        h01 = _mesh->halfedge_handle(e_it, 1);

        v0  = _mesh->to_vertex_handle(h10);
        v1  = _mesh->to_vertex_handle(h01);

		if(!isPreserved(_mesh,e_it)){
			if(isPreserved(_mesh,v0) && isPreserved(_mesh,v1)) continue;
		}

		//curve cross each other case
		if (isPreservedNumber(_mesh,v0) > 2 || isPreservedNumber(_mesh, v1) > 2) continue;

        const OMesh::Point& p0 = _mesh->point(v0);
        const OMesh::Point& p1 = _mesh->point(v1);

        if ((p0-p1).length() < lowerBoundLength_) {
          // get status
          b0 = _mesh->is_boundary(v0);
          b1 = _mesh->is_boundary(v1);
          hcol01 = hcol10 = true;

          // boundary rules (dont collapse boundary to interior)
          if (b0 && b1) { if (!_mesh->is_boundary(e_it)) continue; }
          else if (b0) hcol01 = false;
          else if (b1) hcol10 = false;

          // Testing before each collapse whether the collapse would produce an edge that is longer than high
          OMesh::VertexVertexIter vvIt;
          vvIt=_mesh->vv_iter(v0);
         
		  for(;vvIt;++vvIt) {
            OMesh::Point pointA = _mesh->point(v1);
            OMesh::Point pointB = _mesh->point(vvIt);
            if((pointA-pointB).length() >higherBoundLength_) {
              hcol01 = false;
            }
          }       
          vvIt=_mesh->vv_iter(v1);
          for(;vvIt;++vvIt) {
            OMesh::Point pointA = _mesh->point(v0);
            OMesh::Point pointB = _mesh->point(vvIt);
            if((pointA-pointB).length()>higherBoundLength_) {
              hcol10 = false;
            }
          }                                                   

          // topological rules
          if (hcol01)  hcol01 = _mesh->is_collapse_ok(h01);
          if (hcol10)  hcol10 = _mesh->is_collapse_ok(h10);

		  // ensure preservation
		  if (hcol01){

			  if(isPreserved(_mesh,v0) && !isPreserved(_mesh,v1))
				  hcol01=false;

			  OMesh::HHandle h20=_mesh->next_halfedge_handle(_mesh->next_halfedge_handle(h01));
			  hcol01 = hcol01 && !isPreserved(_mesh,_mesh->edge_handle(h20));

			  OMesh::HHandle h04=_mesh->next_halfedge_handle(h10);
			  hcol01 = hcol01 && !isPreserved(_mesh,_mesh->edge_handle(h04));

			  hcol01 = hcol01 && !isPreservationBoundary(_mesh,v0);

		  }
		  if (hcol10){
			  if(isPreserved(_mesh,v1) && !isPreserved(_mesh,v0))
				  hcol10=false;

			  OMesh::HHandle h20=_mesh->next_halfedge_handle(_mesh->next_halfedge_handle(h10));
			  hcol10 = hcol10 && !isPreserved(_mesh,_mesh->edge_handle(h20));

			  OMesh::HHandle h04=_mesh->next_halfedge_handle(h01);
			  hcol10 = hcol10 && !isPreserved(_mesh,_mesh->edge_handle(h04));

			  hcol10 = hcol10 && !isPreservationBoundary(_mesh,v1);

		  }

          // both collapses possible: collapse into vertex w/ higher valence
          if (hcol01 && hcol10) {
            if (_mesh->valence(v0) > _mesh->valence(v1))
              hcol10 = false;
            else
              hcol01 = false;
          }

          // try v1 -> v0
          if (hcol10) {
            _mesh->collapse(h10);
            finished = false;
          }

          // try v0 -> v1
          else if (hcol01) {
			_mesh->collapse(h01);
            finished = false;
          }
        }
      }
    }
    PROGRESS_POS(i);
  }
  PROGRESS_FINISH();

  _mesh->garbage_collection();

  if (i==collapseIterations_) DebugLog << "Collapse stoped after " << collapseIterations_ << " iterations.\n";
}

void IsotropicRemeshing::equalizeValences(OMesh* _mesh) {
  OMesh::EIter     e_it, e_end;
  OMesh::VHandle   v0, v1, v2, v3;
  OMesh::HHandle   h10;
  int             val0, val1, val2, val3;
  int             val_opt0, val_opt1, val_opt2, val_opt3;
  int             ve0, ve1, ve2, ve3, ve_before, ve_after;
  bool            finished;
  int             i;

  // flip all edges
  PROGRESS_INIT(valenceIterations_,"equalizeValences");
  for (finished=false, i=0; !finished && i<valenceIterations_; ++i) {
    finished = true;

    for (e_it=_mesh->edges_begin(), e_end=_mesh->edges_end(); e_it!=e_end; ++e_it) {
      if (!_mesh->is_boundary(e_it)) {

		if( isPreserved(_mesh,e_it )) continue;
        if(isFix(_mesh,e_it)) continue;

        OMesh::HHandle h10 = _mesh->halfedge_handle(e_it, 0);
        v0 = _mesh->to_vertex_handle(h10);
		OMesh::HHandle h02 = _mesh->next_halfedge_handle(h10);
		v2 = _mesh->to_vertex_handle(h02);

		OMesh::HHandle h01 = _mesh->opposite_halfedge_handle(h10);
		v1 = _mesh->to_vertex_handle(h01);
		OMesh::HHandle h13 = _mesh->next_halfedge_handle(h01);
		v3 = _mesh->to_vertex_handle(h13);

		OMesh::EHandle e02 = _mesh->edge_handle(h02);
		OMesh::EHandle e12 = _mesh->edge_handle(_mesh->next_halfedge_handle(h02));
		OMesh::EHandle e13 = _mesh->edge_handle(h13);
		OMesh::EHandle e03 = _mesh->edge_handle(_mesh->next_halfedge_handle(h13));

        Scalar angle021 = BasicMeasuring::triangleAngleBAC(_mesh->point(v2),_mesh->point(v0),_mesh->point(v1));
		bool p021 = isPreserved(_mesh,e02) && isPreserved(_mesh,e12) && angle021 > (0.95*M_PI);

        Scalar angle031 = BasicMeasuring::triangleAngleBAC(_mesh->point(v3),_mesh->point(v0),_mesh->point(v1));
		bool p031 = isPreserved(_mesh,e03) && isPreserved(_mesh,e13) && angle031 > (0.95*M_PI);
		if ( ( p021 || p031) && _mesh->is_flip_ok(e_it)) {
			_mesh->flip(e_it);
			finished = false;
			continue;
		}

		bool p203 = isPreserved(_mesh,e02) && isPreserved(_mesh,e03);
		bool p213 = isPreserved(_mesh,e13) && isPreserved(_mesh,e12);
		if( p203 || p213 ) continue;

        val0 = _mesh->valence(v0);
        val1 = _mesh->valence(v1);
        val2 = _mesh->valence(v2);
        val3 = _mesh->valence(v3);

        val_opt0 = (_mesh->is_boundary(v0) ? 4 : 6);
        val_opt1 = (_mesh->is_boundary(v1) ? 4 : 6);
        val_opt2 = (_mesh->is_boundary(v2) ? 4 : 6);
        val_opt3 = (_mesh->is_boundary(v3) ? 4 : 6);

        ve0 = (val0 - val_opt0);
        ve1 = (val1 - val_opt1);
        ve2 = (val2 - val_opt2);
        ve3 = (val3 - val_opt3);

        ve_before = ve0*ve0 + ve1*ve1 + ve2*ve2 + ve3*ve3;
         
        --val0;  --val1;
        ++val2;  ++val3;

        ve0 = (val0 - val_opt0);
        ve1 = (val1 - val_opt1);
        ve2 = (val2 - val_opt2);
        ve3 = (val3 - val_opt3);

        ve_after = ve0*ve0 + ve1*ve1 + ve2*ve2 + ve3*ve3;

        if (ve_before > ve_after && _mesh->is_flip_ok(e_it)) {
          _mesh->flip(e_it);
          finished = false;
        }
      }
    }
    PROGRESS_POS(i);
  }
  PROGRESS_FINISH();
}

void IsotropicRemeshing::tangentialRelaxation(OMesh* _mesh) {
  OMesh::VIter     v_it, v_end(_mesh->vertices_end());
  OMesh::CVVIter   vv_it;
  OMesh::Scalar    valence;
  OMesh::Point     u, n;

  PROGRESS_INIT(20,"tangentialRelaxation");
  for (int iters=0; iters<20; ++iters) {
    for (v_it=_mesh->vertices_begin(); v_it!=v_end; ++v_it) {
      if (!_mesh->is_boundary(v_it)) {

        if(isPreserved(_mesh,v_it) || isFix(_mesh,v_it))
          continue;

        u.vectorize(0.0);
        valence = 0;
       
        for (vv_it=_mesh->cvv_iter(v_it); vv_it; ++vv_it) {
          u += _mesh->point(vv_it);
          ++valence;
        }
       
        if (valence) {
          u *= (float)(1.0/valence);
          u -= _mesh->point(v_it);
          n  = _mesh->normal(v_it);
          n *= u*n;
          u -= n;
        }
       
        _mesh->property(update_, v_it) = u;
      }
    }
       
    for (v_it=_mesh->vertices_begin(); v_it!=v_end; ++v_it)
      if (!_mesh->is_boundary(v_it))
        _mesh->point(v_it) += _mesh->property(update_, v_it);
    PROGRESS_POS(iters);
  }
  PROGRESS_FINISH();
}

void IsotropicRemeshing::projectBack(OMesh* _mesh, OMesh* _oldMesh, NearestTriangleSearch& nts)
{
  for (OMesh::VertexIter vIt = _mesh->vertices_begin(); vIt != _mesh->vertices_end(); ++vIt)
  {
    OMesh::Point p = _mesh->point(vIt.handle());
    OMesh::FaceHandle fh;
    nts.computeClosestTriangleOfPoint(p, _oldMesh, fh);
    OMesh::ConstFaceVertexIter fvI = _oldMesh->cfv_iter(fh);
    OMesh::Point p0 = _oldMesh->point(fvI.handle()); ++fvI;
    OMesh::Point p1 = _oldMesh->point(fvI.handle()); ++fvI;
    OMesh::Point p2 = _oldMesh->point(fvI.handle());
    OMesh::Point projectedPoint;
    BasicMeasuring::squaredDistancePointToTriangle(p, p0, p1, p2, projectedPoint);
    _mesh->point(vIt) = projectedPoint;
  }
}

void IsotropicRemeshing::areaEqualization(OMesh* _mesh) {
  OMesh::VIter     v_it, v_end(_mesh->vertices_end());
  OMesh::CVVIter   vv_it;
  OMesh::Scalar    valence;
  OMesh::Point     u, n;
  PROGRESS_INIT(5,"areaEqualization");
  for (int iters=0; iters<5; ++iters) {
    // smooth
    for (v_it=_mesh->vertices_begin(); v_it!=v_end; ++v_it) {
      if (!_mesh->is_boundary(v_it)) {

        if(isPreserved(_mesh,v_it) || isFix(_mesh,v_it))
          continue;

        u.vectorize(0.0);
        valence = 0;
       
        Scalar totalNeighborAreas= 0.0;
       
        for (vv_it=_mesh->cvv_iter(v_it); vv_it; ++vv_it) {
          OMesh::VFIter vfIt;
          // compute neighbor area
         
          Scalar neighborArea = 0.0;
          for(vfIt=_mesh->vf_iter(vv_it);vfIt;++vfIt) {
              OpenMesh::FaceHandle fh= vfIt.handle();
              neighborArea = BasicMeasuring::triangleSurfaceArea(fh,_mesh);
          }
          totalNeighborAreas += neighborArea;
          u += _mesh->point(vv_it)*static_cast<OMesh::Scalar>(neighborArea);
          ++valence;
        }
       
       
        if (valence) {
          Scalar lambda = 0.5;
          u *= static_cast<OMesh::Scalar>(1.0/totalNeighborAreas);
          u -= _mesh->point(v_it);
          n  = _mesh->normal(v_it);
          n *= (u*n);
          u -= n*static_cast<OMesh::Scalar>(lambda);
        }
       
        _mesh->property(update_, v_it) = u;
      }
    }
       
    for (v_it=_mesh->vertices_begin(); v_it!=v_end; ++v_it)
      if (!_mesh->is_boundary(v_it))
        _mesh->point(v_it) += _mesh->property(update_, v_it);
    PROGRESS_POS(iters);
  }
  PROGRESS_FINISH();
}

}}//namespaces
