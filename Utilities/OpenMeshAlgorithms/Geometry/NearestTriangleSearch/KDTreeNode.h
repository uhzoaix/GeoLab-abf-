#ifndef GCL_KD_TREE_NODE_H
#define GCL_KD_TREE_NODE_H

#include <Core/DataStructures/OMesh.h>
#include <vector>

namespace GCL { namespace Utilities {



	typedef struct KDTreeAxiAlignedBoundingBox_{
    Vec3 minCorner;
    Vec3 maxCorner;
	}KDTreeAxiAlignedBoundingBox;


	enum COORDINATE_AXIS{
		X_AXIS=0,
        Y_AXIS=1,
        Z_AXIS=2
	};

	typedef struct KDTreeNode_{
		
        KDTreeNode_()
        {
            isLeaf = false;
            coordinateAxisOfSplitting = X_AXIS;
            splittingPlaneCoordinate = 0.0;
            radius = 0.0;
            faceHandles = NULL;
            leftChild = NULL;
            rightChild = NULL;
            parent = NULL;
        }



		bool isLeaf;

		KDTreeAxiAlignedBoundingBox boundingBox;

        int coordinateAxisOfSplitting;

		double splittingPlaneCoordinate;

        double radius;

        Vec3 center;

		std::vector<OMesh::FaceHandle> * faceHandles;

		struct KDTreeNode_ * leftChild;
		struct KDTreeNode_ * rightChild;
		struct KDTreeNode_ * parent;
	
	} KDTreeNode;

}}

#endif
