#include "IsolevelExtractor.h"

#include <Core/DataStructures/BarycentricCoordinates.h>
#include <Core/Utilities/LogSystem.h>
#include <Utilities/OpenMeshAlgorithms/Geometry/BasicMeasuring/BasicMeasuring.h>
#include <Utilities/OpenMeshAlgorithms/Geometry/NearestTriangleSearch/NearestTriangleSearch.h>

#include <queue>
#include <vector>

#ifndef fmin
#define fmin(x,y) ((x)<(y)?(x):(y))
#endif
#ifndef fmax
#define fmax(x,y) ((x)<(y)?(x):(y))
#endif

namespace GCL{ namespace Utilities {

    using namespace std;

IsolevelExtractor::IsolevelExtractor( shared_ptr<OMesh> _mesh, OpenMesh::VPropHandleT<Vec2> _vertexFunction ){
	setFunction(_vertexFunction);
	init(_mesh);
}

IsolevelExtractor::IsolevelExtractor( shared_ptr<OMesh> _mesh, OpenMesh::HPropHandleT<Vec2> _halfedgeFunction ){
	setFunction(_halfedgeFunction);
	init(_mesh);
}

void IsolevelExtractor::init( shared_ptr<OMesh> _mesh ){

	shared_ptr<OMesh> meshCopy= shared_ptr<OMesh>(new OMesh(*_mesh.get()));
	mesh=meshCopy;

	mesh->add_property(marked,std::string("IsolevelExtractor::marked"));
	for(OMesh::EdgeIter eI=mesh->edges_begin();eI!=mesh->edges_end();++eI){
		mesh->property(marked,eI)=false;
	}
	originalMesh = _mesh;
	spacing=1.;
	tolerance=0.0000000001;

}

void IsolevelExtractor::setFunction( OpenMesh::VPropHandleT<Vec2> _vertexFunction ){
	useVertexFunction=true;
	vertexFunction=_vertexFunction;
}

void IsolevelExtractor::setFunction( OpenMesh::HPropHandleT<Vec2> _halfedgeFunction ){
	useVertexFunction=false;
	halfedgeFunction=_halfedgeFunction;
}

OpenMesh::BasePropHandleT<Vec2> IsolevelExtractor::getFunction() const{
	if(useVertexFunction){
		return vertexFunction;
	}else{
		return halfedgeFunction;
	}
}

Vec2 IsolevelExtractor::function(OMesh::HalfedgeHandle he ) const {
	if(useVertexFunction){
		return mesh->property(vertexFunction,mesh->to_vertex_handle(he));
	}else{
		return mesh->property(halfedgeFunction,he);
	}
}

void IsolevelExtractor::setFunction(OMesh::HalfedgeHandle he, const Vec2& v ){
	if(useVertexFunction){
		mesh->property(vertexFunction,mesh->to_vertex_handle(he))=v;
	}else{
		mesh->property(halfedgeFunction,he)=v;
	}
}

    //Does the edge he cross a isolevel in dimension d?
    bool IsolevelExtractor::crosses( OMesh::HalfedgeHandle he, int d ){
		Scalar t0=function(he)[d]/spacing;
		Scalar t1=function(mesh->prev_halfedge_handle(he))[d]/spacing;
      return static_cast<int>(floor(t0)) != static_cast<int>(floor(t1));
    }

    bool IsolevelExtractor::crosses( OMesh::HalfedgeHandle he, int d, Scalar level){
	  return 0. < (function(he)[d]-level)*(function(mesh->prev_halfedge_handle(he))[d]-level);
    }

	bool IsolevelExtractor::crosses( OMesh::HalfedgeHandle he ){
      return crosses( he,0)||crosses(he,1);
    }

    //Does the face fh cross a isolevel in dimension d?
    bool IsolevelExtractor::crosses( OMesh::FaceHandle fh, int d){
      for(OMesh::FaceHalfedgeIter it=mesh->fh_iter(fh); it; ++it){
        if(crosses(it.handle(),d)) return true;
      } return false;
    }

    bool IsolevelExtractor::crosses( OMesh::FaceHandle fh, int d, Scalar level ){
      for(OMesh::FaceHalfedgeIter it=mesh->fh_iter(fh); it; ++it){
        if(crosses(it.handle(),d,level)) return true;
      } return false;
    }

    bool IsolevelExtractor::crosses( OMesh::FaceHandle fh ){
      return crosses(fh,0)||crosses(fh,1);
    }

    Scalar IsolevelExtractor::intersect( OMesh::HalfedgeHandle he, int d, Scalar level){
	  return intersect( function(mesh->prev_halfedge_handle(he))[d], function(he)[d],level);
    }

    Scalar IsolevelExtractor::intersect( Scalar ta, Scalar tb, Scalar level){
      bool swapped=false;
      if(ta>tb){
        swapped=true;
        std::swap(ta,tb);
      }
      if(level-ta < tolerance || tb-level < tolerance || fabs(tb-ta) < tolerance )
        return -1;

      Scalar s=(level-ta)/(tb-ta);
      assert(s>=0. && s<= 1.);
      if(swapped) s=1.-s;
      return s;
    }

    void IsolevelExtractor::intersect( OpenMesh::VectorT<BarycentricCoordinates,2>& bcs ,  OMesh::FaceHandle fh, int d){

        bcs[0]=BarycentricCoordinates(-1.,-1.,-1.);
        bcs[1]=BarycentricCoordinates(-1.,-1.,-1.);

        if(!crosses(fh,d))
            return;

        //Extract texture coordinates
        OMesh::FaceHalfedgeIter fhI=mesh->fh_iter(fh);
        Vec2  vt0 = function(fhI.handle()); ++fhI;
        Vec2  vt1 = function(fhI.handle()); ++fhI;
        Vec2  vt2 = function(fhI.handle());

        //Find lowest intersecting level
        Scalar level=         ceil(vt0[d]/spacing);
        level=     fmin(level,ceil(vt1[d]/spacing));
        level=     fmin(level,ceil(vt2[d]/spacing));

        //Find highest intersecting level
        Scalar lh=         floor(vt0[d]/spacing);
        lh=        fmax(lh,floor(vt1[d]/spacing));
        lh=        fmax(lh,floor(vt2[d]/spacing));

        for(int l=static_cast<int>(level); l<=static_cast<int>(lh);++l){
            intersect(bcs,fh,d,l);
            if(bcs[0].isVertex(tolerance)){
                if(bcs[1].isVertex(tolerance)){
                    OpenMesh::EdgeHandle eh=mesh->edge_handle(mesh->find_halfedge(bcs[0].getVertex(mesh,fh,tolerance),bcs[1].getVertex(mesh,fh,tolerance)));
                    mesh->property(marked,eh)=true;
                    continue;
                }
                if(!bcs[1].isValid())
                    continue;
            }
            return;
        }
    }

    void IsolevelExtractor::intersect( OpenMesh::VectorT<BarycentricCoordinates,2>& bcs ,  OMesh::FaceHandle fh, int d, Scalar level ){

      bcs[0]=BarycentricCoordinates(-1.,-1.,-1.);
      bcs[1]=BarycentricCoordinates(-1.,-1.,-1.);
      int i=0;

      //Extract texture coordinates
      OMesh::FaceHalfedgeIter fhI=mesh->fh_iter(fh);
      Vec2  vt0 = function(fhI.handle()); ++fhI;
      Vec2  vt1 = function(fhI.handle()); ++fhI;
      Vec2  vt2 = function(fhI.handle());

      //Check if vertices intersect with level
      if(fabs(vt0[d]-level)<tolerance){ bcs[i]=BarycentricCoordinates(1.,0.,0.); i++; if(i>=2) return;}
      if(fabs(vt1[d]-level)<tolerance){ bcs[i]=BarycentricCoordinates(0.,1.,0.); i++; if(i>=2) return;}
      if(fabs(vt2[d]-level)<tolerance){ bcs[i]=BarycentricCoordinates(0.,0.,1.); i++; if(i>=2) return;}

      //Check if edges intersect with level
      fhI=mesh->fh_iter(fh); Scalar s;
      s=intersect(fhI.handle(),d,level);
      if(s>-0.){
        bcs[i]=BarycentricCoordinates(s,0.,1.-s);
        i++; if(i>=2) return;
      } ++fhI;
      s=intersect(fhI.handle(),d,level);
      if(s>-0.){
        bcs[i]=BarycentricCoordinates(1.-s,s,0.);
        i++; if(i>=2) return;
      } ++fhI;
      s=intersect(fhI.handle(),d,level);
      if(s>-0.){
        bcs[i]=BarycentricCoordinates(0,1.-s,s);
        i++; if(i>=2) return;
      }

      return;

    }

	OMesh::VertexHandle IsolevelExtractor::cut( const OpenMesh::VectorT<BarycentricCoordinates,2>& bcs, OMesh::FaceHandle fh ){

      bool debug=false;

      BarycentricCoordinates bc=bcs[0];
      if(bcs[0].isVertex(tolerance)){
          bc=bcs[1];

          if(bcs[1].isVertex(tolerance)){
              OpenMesh::EdgeHandle eh = mesh->edge_handle(mesh->find_halfedge(bcs[0].getVertex(mesh,fh,tolerance),bcs[1].getVertex(mesh,fh,tolerance)));
              mesh->property(marked,eh)=true;
			  return OMesh::VertexHandle();
          }

          if(!bcs[1].isValid())
			  return OMesh::VertexHandle();
      }

      if(bc.isEdge(tolerance)){
        Scalar t;
        OMesh::HalfedgeHandle he= bc.getHalfedge(mesh,fh,t,tolerance);
        OMesh::EdgeHandle edge=mesh->edge_handle(he);

/*
                  1
                /   \
               /1   2\
              /       \
             0-0-----3-2
              \       /
               \5   4/
                \   /
                  3
*/

        //Extract texture coordinates
        OpenMesh::VectorT<OMesh::HalfedgeHandle,6> hes;
        hes[0]=he;
        hes[1]=mesh->next_halfedge_handle(hes[0]);
        hes[2]=mesh->next_halfedge_handle(hes[1]);
        hes[3]=mesh->opposite_halfedge_handle(he);
        hes[4]=mesh->next_halfedge_handle(hes[3]);
        hes[5]=mesh->next_halfedge_handle(hes[4]);


        //Add the new point p4
        OMesh::Point p = BasicMeasuring::interpolate(mesh.get(),he,t);
        OMesh::VertexHandle v4=mesh->add_vertex(p);

        //The texture coordinates of v4 on side of he 0 resp. he 3
        OpenMesh::VectorT<Vec2,2> txs4;
		txs4[0] = (1.-t)*function(hes[2]) + t*function(hes[0]);
		txs4[1] = (1.-t)*function(hes[3]) + t*function(hes[5]);


        OpenMesh::VectorT<OMesh::VertexHandle,6> vhs;
        for(unsigned int i=0;i<6;i++)
            vhs[i]=mesh->to_vertex_handle(hes[i]);

        OpenMesh::VectorT<Vec2,6> txs;
        for(unsigned int i=0;i<6;i++)
			txs[i]=function(hes[i]);

        mesh->split(edge,v4);

        if(debug) DebugLog<<"Adding v["<<v4.idx()<<"]"<<std::endl;

        for(unsigned int o=0;o<2;++o){

            //Skip the second (2) triangles if we are splitting a boundary edge
            if(o==1 && mesh->is_boundary(edge))
				return v4;

            for(unsigned int i=0;i<2;++i){

                int k=i+o*3;
                OMesh::HalfedgeHandle hef=mesh->find_halfedge(v4,vhs[k]);
                if(hef.is_valid()){
					setFunction(hef,txs[k]);
                    if(debug) DebugLog<<"txc(he["<<hef.idx()<<"])="<<txs[k][0]<<" "<<txs[k][1]<<std::endl;
                }

                k=i+1+o*3;
                hef=mesh->find_halfedge(vhs[k-1],vhs[k]);
                if(hef.is_valid()){
					setFunction(hef,txs[k]);
                    if(debug) DebugLog<<"txc(he["<<hef.idx()<<"])="<<txs[k][0]<<" "<<txs[k][1]<<std::endl;
                }

                hef=mesh->find_halfedge(vhs[k],v4);
                if(hef.is_valid()){
					setFunction(hef,txs4[o]);
                    if(debug) DebugLog<<"txc(he["<<hef.idx()<<"])="<<txs[k][0]<<" "<<txs[k][1]<<std::endl;
                }
            }
        }
        //}

        if(debug)DebugLog<<std::endl;

		return v4;
      }

	  return OMesh::VertexHandle();
    }

    void IsolevelExtractor::extractIsolevel( int d, Scalar level, Scalar min, Scalar max, std::vector<std::pair<Vec3,Vec3> >& path, OMesh::FaceHandle& fEnd,  const OMesh::FaceHandle& fStart/*=OMesh::InvalidFaceHandle*/ ){

      Q_UNUSED(d);
      Q_UNUSED(level);
      Q_UNUSED(min);
      Q_UNUSED(max);
      Q_UNUSED(fEnd);
      Q_UNUSED(fStart);

      //Extract the he-texcoord bounding box
      Vec2 txcMin = mesh->texcoord2D(mesh->halfedges_begin().handle());
      Vec2 txcMax = txcMin;
      for(OMesh::HalfedgeIter hI=mesh->halfedges_begin(); hI!=mesh->halfedges_end();++hI){
        txcMin.min(mesh->texcoord2D(hI));
        txcMax.max(mesh->texcoord2D(hI));
      }

      for(int dd=0;dd<2;dd++){
      for(int i=static_cast<int>(floor(txcMin[dd]));i<=static_cast<int>(ceil(txcMax[dd]));++i){

          Scalar li=static_cast<Scalar>(i);

      for(OMesh::FaceIter fI=mesh->faces_begin(); fI!=mesh->faces_end();++fI)
      {
        OpenMesh::VectorT<BarycentricCoordinates,2> bcs;
        intersect(bcs,fI.handle(),dd,li);

        if(bcs[0].isValid() && bcs[1].isValid()){
          std::pair<Vec3,Vec3> edge(bcs[0].ToCartesian(mesh,fI.handle()),bcs[1].ToCartesian(mesh,fI.handle()));
          path.push_back(edge);
          mesh->set_color(fI,OMesh::Color(0.7,0.1,0.1,1.));
        }else if(bcs[0].isValid()){
          mesh->set_color(fI,OMesh::Color(0.1,0.7,0.1,1.));
        }
      }

    }}


    }

    void IsolevelExtractor::extractQuadMesh( shared_ptr<OMesh> qMesh){

		int w=0;
		bool finished=false;
		do{
			finished=true;

			for(OMesh::FaceIter fI=mesh->faces_begin(); fI!=mesh->faces_end(); ++fI)
			{
				bool isCut=true;
				do{ //cut fI as long as it crosses a isolevel
					isCut=false;

					for(int d=0;d<2;d++){
						if(crosses(fI.handle(),d)){
							OpenMesh::VectorT<BarycentricCoordinates,2> bcs;
							intersect(bcs,fI.handle(),d);
							OMesh::VertexHandle vNew=cut(bcs,fI.handle());

							if(vNew.is_valid()){
								isCut=true; finished=false;
							}
						}
					}

				}while(isCut);
			}
			w++;
		}while(!finished);

		mesh->update_normals();

        //DebugLog << "#whiles: " << w << std::endl;

        //Cut the mesh as long as there is a face with an isolevel which is not a mesh edge yet:
		/*
		int w=0;
        bool cutted=true;
        while(cutted){
			cutted=false;
			for(int d=0;d<2&&!cutted;d++){
				for(OMesh::FaceIter fI=mesh->faces_begin(); fI!=mesh->faces_end() && !cutted ;++fI)
                {
                    if(crosses(fI.handle(),d)){
                        OpenMesh::VectorT<BarycentricCoordinates,2> bcs;
                        intersect(bcs,fI.handle(),d);
						OMesh::VertexHandle vNew=cut(bcs,fI.handle());
						cutted = vNew.is_valid();


                    }
                }
            }
			w++;
        }
        mesh->update_normals();

        DebugLog << "#whiles: " << w << std::endl;
		*/

        //Adding the visited property
        OpenMesh::HPropHandleT<bool> visited;
        mesh->add_property(visited,std::string("IsolevelExtractor::extractQuadMesh::visited"));
        for(OMesh::HalfedgeIter hI=mesh->halfedges_begin(); hI!=mesh->halfedges_end(); ++hI)
          mesh->property(visited,hI)=false;

        //Marking boundary
        for(OMesh::EdgeIter eI=mesh->edges_begin(); eI!=mesh->edges_end(); ++eI){
          if(mesh->is_boundary(eI))
            mesh->property(marked,eI)=true;
        }

        //Adding vertices, copying halfedge texcoords
        //if(!qMesh->has_halfedge_texcoords2D())
        //  qMesh->request_halfedge_texcoords2D();
        OpenMesh::VPropHandleT<bool> vM;
        qMesh->add_property(vM,std::string("IsolevelExtractor::extractQuadMesh::vM"));
        int c=0;
        for(OMesh::VertexIter vI=mesh->vertices_begin();vI!=mesh->vertices_end();++vI){
            OMesh::VertexHandle vh=qMesh->add_vertex(mesh->point(vI.handle()));
            qMesh->property(vM,vI)=false;
            assert(vh.idx()==c);
            c++;
        }

        //Adding faces: Iterate along marked halfedges until back at the start: This gives us a face in the quad mesh.
        for(OMesh::HalfedgeIter hI=mesh->halfedges_begin(); hI!=mesh->halfedges_end(); ++hI){

          if( !(mesh->property(marked,(mesh->edge_handle(hI)))) || mesh->property(visited,hI) || mesh->is_boundary(hI))
            continue;

          OMesh::HalfedgeHandle he=hI.handle();
          OMesh::HalfedgeHandle hStart=he;
          std::vector<OMesh::VertexHandle> vertices;

          do{
			  if(getQuadsOnly()){
				int q=0;
				for( OMesh::VertexEdgeIter veI=mesh->ve_iter(mesh->to_vertex_handle(he)); veI;++veI){
					if(mesh->property(marked,veI)) q++;
				}
				if(q>2 || mesh->status(mesh->to_vertex_handle(he)).selected() ){ //This includes singular cases, e.g. q==3 or q>4
					vertices.push_back(mesh->to_vertex_handle(he));
				}
			  }else{
				vertices.push_back(mesh->to_vertex_handle(he));
			  }
            mesh->property(visited,he)=true;

            //find next he:
            he=mesh->next_halfedge_handle(he);
            while(!(mesh->property(marked,(mesh->edge_handle(he)))))
              he=mesh->next_halfedge_handle(mesh->opposite_halfedge_handle(he));

          }while(he!=hStart);

		  //assert(vertices.size()>=3);
          if(vertices.size()<3) ErrorLog<< "Warning: Degenerate Face in IsolevelExtraction." << std::endl;

          qMesh->add_face(vertices);
        }

        //remove isolated vertices
        qMesh->delete_isolated_vertices();

        //get normals from original mesh by projecting each vertex of the new to it and linear interpolation in barycentric coordinates
        if(!qMesh->has_vertex_normals())
          qMesh->request_vertex_normals();

		if(!qMesh->has_vertex_texcoords2D())
			qMesh->request_vertex_texcoords2D();

        nts.initializeKDTreeBasedSearchStructure(originalMesh.get(),12,30);

        for(OMesh::VertexIter vI=qMesh->vertices_begin();vI!=qMesh->vertices_end();++vI){

          //Find closest point
          Vec3 p = qMesh->point(vI);
          OMesh::FaceHandle fh;
          nts.computeClosestTriangleOfPoint(p,originalMesh.get(),fh);

          //Find barycentric coordinates
          Vec3 projectedP;
          BarycentricCoordinates bc;
          BasicMeasuring::squaredDistancePointToTriangle(p,originalMesh.get(),fh,projectedP,bc);

          //interpolate the normal:
          int i=0;
          OMesh::Normal n(0.,0.,0.);
          for( OMesh::ConstFaceVertexIter fvI = originalMesh->cfv_iter(fh); fvI; ++fvI,i++){
            n += bc[i]*originalMesh->normal(fvI.handle());
          }
		  n.normalize();
		  qMesh->set_normal(vI,n);

		  //interpolate texture coordinates:
		  /* //this causes problem if extracting from vertex coords. Disable it for now.
		  i=0;
          Vec2 t(0.);
		  for( OMesh::ConstFaceHalfedgeIter it = originalMesh->cfh_iter(fh); it; ++it,i++){
			  if(useVertexFunction){
				  t += bc[i]*originalMesh->property(vertexFunction,mesh->to_vertex_handle(it));
			  }else{
				  t += bc[i]*originalMesh->property(halfedgeFunction,it);
			  }
		  }
		  qMesh->set_texcoord2D(vI,t);
		  */

        }

        qMesh->garbage_collection();
        mesh->remove_property(visited);
        qMesh->remove_property(vM);
    }


    void IsolevelExtractor::markEdges( EdgeHandles markedEdges, bool overwrite/*=false*/ ){
        if( overwrite ) unmarkEdges();
        for(EdgeHandles::const_iterator it=markedEdges.begin(); it!=markedEdges.end(); ++it)
            mesh->property(marked,*it);
    }

    void IsolevelExtractor::markEdges( EdgeMarking markedEdges, bool overwrite/*=false*/  ){
        if( overwrite ) unmarkEdges();
        for(OMesh::EdgeIter eI=mesh->edges_begin(); eI!=mesh->edges_end(); ++eI)
            mesh->property(marked,eI)=mesh->property(markedEdges,eI);
    }

    void IsolevelExtractor::unmarkEdges(){
        for(OMesh::EdgeIter eI=mesh->edges_begin(); eI!=mesh->edges_end(); ++eI)
            mesh->property(marked,eI)=false;
    }



}}//namespaces
