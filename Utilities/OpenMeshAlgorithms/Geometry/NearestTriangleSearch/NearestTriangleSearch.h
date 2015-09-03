#ifndef NEAREST_TRIANGLE_SEARCH_H
#define NEAREST_TRIANGLE_SEARCH_H
#include <Utilities/OpenMeshAlgorithms/gcl_export.h>
#include <Core/Math/MathDefines.h>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Utils/vector_cast.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <vector>
#include "Ray.h"
#include "KDTreeNode.h"

namespace GCL { namespace Utilities {

/**
 * @brief The NearestTriangleSearch class
 * @author  zhazha... (zhe dai ma ge dou kan bu dong, zen me yong lo)
 * 
 */

class GCL_OPENMESHALGORITHMS_SHARED_EXPORT NearestTriangleSearch{

	public:


    NearestTriangleSearch();

    void initializeKDTreeBasedSearchStructure(OMesh * inputObjectDataStructure,unsigned int maxDepth, unsigned int minNumberOfTriangles);
    void destroyKDTreeBasedSearchStructure();

    void computeClosestTriangleOfPoint(Vec3 & queryPoint,OMesh * inputObjectDataStructure,OMesh::FaceHandle & closestTriangle);
		
    bool intersect(const Ray& ray, Vec3& vertex, Vec3& normal);

	private:	

		void destroyNode(KDTreeNode * node);

    void getFarthestAncestorNodeWithPossibleNearestTriangleOfPoint(KDTreeNode* nodeOfSubSpace, KDTreeNode** farthestAncestorNode, Vec3& queryPoint, Scalar minDistanceOfAllTriangleInLeafNode, unsigned int counter);

    void closestTriangleOfPointInDescendantCells(KDTreeNode* nodeOfSubSpace, Vec3 &queryPoint, OMesh* inputMesh, OMesh::FaceHandle& closestTriangle, Scalar minDistanceOfAllTriangleInLeafNode, Scalar* minDistance);

    Scalar closestTriangleOfPointInCell(KDTreeNode * nodeOfSubSpace,Vec3 & queryPoint,OMesh * inputObjectDataStructure,OMesh::FaceHandle & closestTriangle);

		void buildKDTreeWithMedianSplit(OMesh * inputObjectDataStructure,unsigned int maxDepth, unsigned int minNumberOfTriangles);

		void subdivideSpace(OMesh * inputObjectDataStructure,KDTreeNode * nodeOfSubSpace,unsigned int maxDepth, unsigned int minNumberOfTriangles,unsigned int currentKDTreeDepth);

		void computeCoordinateOfSplitting(OMesh * inputObjectDataStructure,KDTreeNode * nodeOfSubSpace);

		bool terminateKDTreeConstruction(KDTreeNode * nodeOfSubSpace, unsigned int currentKDTreeDepth, unsigned int maxDepth, unsigned int minNumberOfTriangles);

		void sortTrianglesIntoChildCells(OMesh * inputObjectDataStructure,KDTreeNode * nodeOfSubSpace);

    Vec3 getTs(Vec3 minCorner, Vec3 maxCorner, const Ray& argRay, int splitAxis, Scalar splitPoint) const;
    bool intersectRecursive(const Ray& ray, KDTreeNode * currentNode, Scalar tMin, Scalar tMax, Scalar tSplit, Vec3& vertex, Vec3& normal);
    Scalar intersectTriangleWithRay(OMesh::FaceHandle triangle, const Ray &ray, Vec3& vertex, Vec3& normal);
    Scalar dot(const Vec3& v1, const Vec3& v2);
    Vec3 cross(const Vec3& v1, const Vec3& v2);
    Vec3 getMinCornerOfBB(KDTreeNode * node);
    Vec3 getMaxCornerOfBB(KDTreeNode * node);
    void getBoundingBoxLeftChild(KDTreeNode * node, int spAxis, Scalar spCoord, KDTreeNode * leftChild);
    void getBoundingBoxRightChild(KDTreeNode * node, int spAxis, Scalar spCoord, KDTreeNode * rightChild);

    Scalar getDistanceToBox(const Vec3& _point, KDTreeAxiAlignedBoundingBox& _box) const;

    KDTreeNode * findLeafWithQueryPoint(Vec3 & queryPoint,KDTreeNode * entryNode);


		KDTreeNode * kDTreeRootNode;

		OMesh * m_inputObjectDataStructure;

		////////////////// reuse ////////////////////
			

};

}}//namespaces

#endif
