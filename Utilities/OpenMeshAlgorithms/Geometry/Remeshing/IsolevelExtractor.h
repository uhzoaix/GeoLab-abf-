#ifndef ISOLEVEL_EXTRACTOR_H
#define ISOLEVEL_EXTRACTOR_H
#include <Utilities/OpenMeshAlgorithms/gcl_export.h>
#include <Core/DataStructures/OMesh.h>
#include <Core/DataStructures/BarycentricCoordinates.h>
#include <Utilities/OpenMeshAlgorithms/Geometry/NearestTriangleSearch/NearestTriangleSearch.h>
#include <memory>

namespace GCL{
    class BarycentricCoordinates;
}

namespace GCL{ namespace Utilities {

  using namespace std;

  class GCL_OPENMESHALGORITHMS_SHARED_EXPORT IsolevelExtractor {

  public:

    typedef std::vector<OpenMesh::EdgeHandle> EdgeHandles;
    typedef OpenMesh::EPropHandleT<bool> EdgeMarking;

  public:

	  /**
		Constructor.
		\param Function Either a vertex or a halfedge Property Handle of the function to extract the isolevels from
		*/
    explicit IsolevelExtractor( shared_ptr<OMesh> _mesh, OpenMesh::VPropHandleT<Vec2> _vertexFunction );
    explicit IsolevelExtractor( shared_ptr<OMesh> _mesh, OpenMesh::HPropHandleT<Vec2> _halfedgeFunction );

      ~IsolevelExtractor(){}

	/**
	  Set the Function of which the isolevels should be extracted
	 */
    void setFunction( OpenMesh::VPropHandleT<Vec2> _vertexFunction );
    void setFunction( OpenMesh::HPropHandleT<Vec2> _halfedgeFunction );
    OpenMesh::BasePropHandleT<Vec2> getFunction() const;

	/**
	  Set/get the spacing of the isolevels to extract
	  */
    void setSpacing( Scalar s){spacing=s;}
    Scalar getSpacing(){return spacing;}

	/**
	  Set/get the tolerance of the extraction procedure
	  */
    void setTolerance( Scalar t){tolerance=t;}
    Scalar getTolerance(){return tolerance;}

	/**
	  Set/get if extraction should be polylines or quads
	  */
	void setQuadsOnly( bool q){quadsOnly=q;}
	bool getQuadsOnly(){return quadsOnly;}

    void extractIsolevel( int d, Scalar level, Scalar min, Scalar max, std::vector<std::pair<Vec3 ,Vec3 > >& path, OMesh::FaceHandle& fEnd, const OMesh::FaceHandle& fStart=OMesh::InvalidFaceHandle );

    void markEdges( EdgeHandles markedEdges, bool overwrite=false );
    void markEdges( EdgeMarking markedEdges, bool overwrite=false );
    EdgeMarking getMarkedEdges() const { return marked;}

    void unmarkEdges();

    /** Extractes a Polygonal Mesh qmesh from the isolevels in member mesh. */
    void extractQuadMesh( shared_ptr<OMesh> qMesh );

    /** Remeshes the member mesh along its isolevels. The edges along ioslevels are marked in the property marked */
    void remeshIsolevels();

    /** Extracts the marked edges into the polymesh q */
    void extractQuadMeshFromMarkedEdges( shared_ptr<OMesh> q, OpenMesh::EPropHandleT<bool> _marked = OpenMesh::EPropHandleT<bool>() );

  public:

    /** Does the Halfedge he cross a isolevel in dimension d? */
    bool crosses( OMesh::HalfedgeHandle he, int d );
    bool crosses( OMesh::HalfedgeHandle he, int d, Scalar level);
    bool crosses( OMesh::HalfedgeHandle he );

    /** Parameter t with t=0 at from(he) an t=1 at to(he), -1 if no intersection found at least 'tolerance' away from the vertices, or als0 -1 in case the whole edge intersects. Detect this by checking the vertices directly. */
    Scalar intersect( OMesh::HalfedgeHandle he, int d, Scalar level);
    Scalar intersect( Scalar ta, Scalar tb, Scalar level);

    /** Does the face fh cross a isolevel in dimension d? */
    bool crosses( OMesh::FaceHandle fh, int d); //uses Isolevel::spacing
    bool crosses( OMesh::FaceHandle fh, int d, Scalar level );
    bool crosses( OMesh::FaceHandle fh );

    /** Intersect the triangle fh with an isolevel in dimension d */
    void intersect( OpenMesh::VectorT<BarycentricCoordinates,2>& bcs ,  OMesh::FaceHandle fh, int d); //Uses Isolevel::spacing, reports lowest isolevel
    void intersect( OpenMesh::VectorT<BarycentricCoordinates,2>& bcs ,  OMesh::FaceHandle fh, int d, Scalar level );
	OMesh::VertexHandle cut( const OpenMesh::VectorT<BarycentricCoordinates,2>& bcs, OMesh::FaceHandle fh );

  private:
	void init( shared_ptr<OMesh> mesh );

    Vec2 function(OMesh::HalfedgeHandle he ) const;
    void setFunction(OMesh::HalfedgeHandle he, const Vec2& v );

  protected:

    shared_ptr<OMesh> mesh;
    shared_ptr<OMesh> originalMesh;
    NearestTriangleSearch nts;
    Scalar spacing;
    Scalar tolerance;
    OpenMesh::EPropHandleT<bool> marked;

	bool quadsOnly;
	bool useVertexFunction;
    OpenMesh::VPropHandleT<Vec2> vertexFunction;
    OpenMesh::HPropHandleT<Vec2> halfedgeFunction;


  };

}}//namespaces

#endif
