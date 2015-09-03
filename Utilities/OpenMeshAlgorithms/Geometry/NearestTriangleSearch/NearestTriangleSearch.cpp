#include "NearestTriangleSearch.h"
#include <Utilities/OpenMeshAlgorithms/Geometry/BasicMeasuring/BasicMeasuring.h>
#include <algorithm> 
#include <limits>

#include <cmath>

namespace GCL { namespace Utilities {

NearestTriangleSearch::NearestTriangleSearch(){


}

void NearestTriangleSearch::initializeKDTreeBasedSearchStructure(OMesh * inputObjectDataStructure,unsigned int maxDepth,  unsigned int minNumberOfTriangles){
	
	m_inputObjectDataStructure = inputObjectDataStructure;
	buildKDTreeWithMedianSplit(inputObjectDataStructure,maxDepth,minNumberOfTriangles);
	
}

void NearestTriangleSearch::destroyKDTreeBasedSearchStructure(){
	destroyNode(kDTreeRootNode);
	delete kDTreeRootNode;
	kDTreeRootNode = NULL;
}

void NearestTriangleSearch::destroyNode(KDTreeNode * node){
	if(node->isLeaf){
		if (node->faceHandles!=NULL){
			node->faceHandles->clear();
			delete node->faceHandles;
			node->faceHandles=NULL;
		}
	}
	else if(node->leftChild->isLeaf && node->rightChild->isLeaf){
		destroyNode(node->leftChild);
		destroyNode(node->rightChild);
		delete node->leftChild;
		delete node->rightChild;
		node->leftChild=NULL;
		node->rightChild=NULL;
		node->isLeaf = true;
		destroyNode(node);
	}
	else{
		destroyNode(node->leftChild);
		destroyNode(node->rightChild);
	}
}


void NearestTriangleSearch::computeClosestTriangleOfPoint(Vec3  & queryPoint,OMesh * inputMesh,OMesh::FaceHandle & closestTriangle){
  KDTreeNode * leafWithQueryPoint;
  KDTreeNode * farthestAncestorNode = new KDTreeNode;

  // recursively traverse kdtree to find leaf containing the query point
  leafWithQueryPoint = findLeafWithQueryPoint(queryPoint,kDTreeRootNode);

  // find minimum distance of all triangles in leaf node
  Scalar minDistanceOfAllTriangleInLeafNode = closestTriangleOfPointInCell(leafWithQueryPoint,queryPoint,inputMesh,closestTriangle);

  //track back up the tree to see if an ancestor cell covers another possible nearest triangle
  farthestAncestorNode = leafWithQueryPoint;
  unsigned int counter = 0;
  //printf("=======\n");
  getFarthestAncestorNodeWithPossibleNearestTriangleOfPoint(leafWithQueryPoint,&farthestAncestorNode,queryPoint,minDistanceOfAllTriangleInLeafNode,counter);
  Scalar temp;
  closestTriangleOfPointInDescendantCells(farthestAncestorNode,queryPoint,inputMesh,closestTriangle,minDistanceOfAllTriangleInLeafNode,&temp);
}


void NearestTriangleSearch::getFarthestAncestorNodeWithPossibleNearestTriangleOfPoint(KDTreeNode * nodeOfSubSpace,KDTreeNode ** farthestAncestorNode,Vec3  & queryPoint, Scalar minDistanceOfAllTriangleInLeafNode, unsigned int counter){
       
  if(nodeOfSubSpace->parent != NULL){
    if(fabs(nodeOfSubSpace->parent->splittingPlaneCoordinate-queryPoint[nodeOfSubSpace->parent->coordinateAxisOfSplitting])<minDistanceOfAllTriangleInLeafNode){
      *farthestAncestorNode = nodeOfSubSpace->parent;
     
      /*if (counter>5){
      printf("counter: %d\n",counter);
      printf("mindistanceToTriangleInLeafNode: %f, ",minDistanceOfAllTriangleInLeafNode);
      printf("splittingPlaneCoordinate: %f, ",nodeOfSubSpace->parent->splittingPlaneCoordinate);
      printf("queryPoint: %f\n",queryPoint[nodeOfSubSpace->parent->coordinateAxisOfSplitting]);
      }*/
    }
    getFarthestAncestorNodeWithPossibleNearestTriangleOfPoint(nodeOfSubSpace->parent,farthestAncestorNode,queryPoint,minDistanceOfAllTriangleInLeafNode,counter+1);
  }    
}



void NearestTriangleSearch::closestTriangleOfPointInDescendantCells(KDTreeNode * nodeOfSubSpace,Vec3  & queryPoint,OMesh * inputMesh,OMesh::FaceHandle & closestTriangle, Scalar minDistanceOfAllTriangleInLeafNode, Scalar * minDistance){
       
        if(!nodeOfSubSpace->isLeaf){
               
                Scalar minDistanceInLeftCell;
                Scalar minDistanceInRightCell;
               
                OMesh::FaceHandle closestTriangleInLeftCell;
                OMesh::FaceHandle closestTriangleInRightCell;
               
                // if splitting plane intersects ball of query point with radius the smallest distance to leaf
                if(fabs(nodeOfSubSpace->splittingPlaneCoordinate-queryPoint[nodeOfSubSpace->coordinateAxisOfSplitting])<minDistanceOfAllTriangleInLeafNode){
                        closestTriangleOfPointInDescendantCells(nodeOfSubSpace->leftChild,queryPoint,inputMesh,closestTriangleInLeftCell, minDistanceOfAllTriangleInLeafNode,&minDistanceInLeftCell);
                        closestTriangleOfPointInDescendantCells(nodeOfSubSpace->rightChild,queryPoint,inputMesh,closestTriangleInRightCell, minDistanceOfAllTriangleInLeafNode,&minDistanceInRightCell);
               
                        if(minDistanceInLeftCell<minDistanceInRightCell){
                                *minDistance = minDistanceInLeftCell;
                                closestTriangle = closestTriangleInLeftCell;
                        }
                        else{
                                *minDistance = minDistanceInRightCell;
                                closestTriangle = closestTriangleInRightCell;
                        }
                }
                else if(queryPoint[nodeOfSubSpace->coordinateAxisOfSplitting] < nodeOfSubSpace->splittingPlaneCoordinate)
                {
                        closestTriangleOfPointInDescendantCells(nodeOfSubSpace->leftChild,queryPoint,inputMesh,closestTriangle, minDistanceOfAllTriangleInLeafNode,minDistance);
                }
                else
                {
                        closestTriangleOfPointInDescendantCells(nodeOfSubSpace->rightChild,queryPoint,inputMesh,closestTriangle, minDistanceOfAllTriangleInLeafNode,minDistance);
                }
        }
        else{
                *minDistance = closestTriangleOfPointInCell(nodeOfSubSpace,queryPoint,inputMesh,closestTriangle);
        }
}


Scalar NearestTriangleSearch::closestTriangleOfPointInCell(KDTreeNode * nodeOfSubSpace,Vec3  & queryPoint,OMesh * inputObjectDataStructure,OMesh::FaceHandle & closestTriangle){
	OMesh::ConstFaceVertexIter cfvIt;
  Scalar minQueryPointTriangleDistance = std::numeric_limits<Scalar>::max();
	OMesh::FaceHandle nearestTriangleHandle;
	for(unsigned int i=0;i<nodeOfSubSpace->faceHandles->size();i++){
		
		OMesh::FaceHandle currentTriangleHandle = nodeOfSubSpace->faceHandles->at(i);

		cfvIt = inputObjectDataStructure->cfv_iter(currentTriangleHandle);

		OMesh::Point vertex1 = inputObjectDataStructure->point(cfvIt.handle());
		++cfvIt;
		OMesh::Point vertex2 = inputObjectDataStructure->point(cfvIt.handle());
		++cfvIt;
		OMesh::Point vertex3 = inputObjectDataStructure->point(cfvIt.handle());

    Scalar currentDistance = std::sqrt(BasicMeasuring::squaredDistancePointToTriangle(queryPoint,vertex1,vertex2,vertex3));

		if(currentDistance<minQueryPointTriangleDistance){
				nearestTriangleHandle = currentTriangleHandle;
				minQueryPointTriangleDistance = currentDistance;
		}		
	}
	closestTriangle = nearestTriangleHandle;
	return minQueryPointTriangleDistance;
} 

KDTreeNode * NearestTriangleSearch::findLeafWithQueryPoint(Vec3  & queryPoint,KDTreeNode * entryNode){
	if(!entryNode->isLeaf){
		
        int currentCoordinateAxisOfSplitting = entryNode->coordinateAxisOfSplitting;
    Scalar currentSplittingPlaneCoordinate = entryNode->splittingPlaneCoordinate;

		if(queryPoint[currentCoordinateAxisOfSplitting]<currentSplittingPlaneCoordinate){
			return findLeafWithQueryPoint(queryPoint,entryNode->leftChild);
		}
		else{
			return findLeafWithQueryPoint(queryPoint,entryNode->rightChild);
		}
	}
	else{
		return entryNode;
	}
}

void NearestTriangleSearch::buildKDTreeWithMedianSplit(OMesh * inputObjectDataStructure,unsigned int maxDepth, unsigned int minNumberOfTriangles){
	
		unsigned int currentKDTreeDepth = 0;

		// initialize root node
		kDTreeRootNode = new KDTreeNode;
		kDTreeRootNode->isLeaf = true;
		kDTreeRootNode->parent = NULL;
		kDTreeRootNode->coordinateAxisOfSplitting = X_AXIS;
		kDTreeRootNode->splittingPlaneCoordinate = 0.0;
		
		OMesh::FaceIter fIt=inputObjectDataStructure->faces_begin();
		OMesh::FaceIter fEnd=inputObjectDataStructure->faces_end();
		kDTreeRootNode->faceHandles = new std::vector<OMesh::FaceHandle> ;
		kDTreeRootNode->faceHandles->clear();
		for(;fIt!=fEnd;++fIt){
			kDTreeRootNode->faceHandles->push_back(fIt.handle());
		}

		kDTreeRootNode->boundingBox.minCorner = getMinCornerOfBB(kDTreeRootNode);
		kDTreeRootNode->boundingBox.maxCorner = getMaxCornerOfBB(kDTreeRootNode);

		subdivideSpace(inputObjectDataStructure,kDTreeRootNode,maxDepth,minNumberOfTriangles,currentKDTreeDepth);
}

void NearestTriangleSearch::subdivideSpace(OMesh * inputObjectDataStructure,KDTreeNode * nodeOfSubSpace,unsigned int maxDepth, unsigned int minNumberOfTriangles,unsigned int currentKDTreeDepth){
	if(!terminateKDTreeConstruction(nodeOfSubSpace,currentKDTreeDepth,maxDepth,minNumberOfTriangles)){
				
		// subdivide node //////////////////////////////////////////////////////
		
			// update global state
		currentKDTreeDepth++;
		
			// update current Node	
		nodeOfSubSpace->isLeaf = false;
		computeCoordinateOfSplitting(inputObjectDataStructure,nodeOfSubSpace);
		
		// create children ////////////////////////////////////////////////////
		
		nodeOfSubSpace->leftChild		= new KDTreeNode;
		nodeOfSubSpace->rightChild	= new KDTreeNode;
		
		nodeOfSubSpace->leftChild->parent = nodeOfSubSpace;
		nodeOfSubSpace->rightChild->parent = nodeOfSubSpace;

		nodeOfSubSpace->leftChild->isLeaf = true;
		nodeOfSubSpace->rightChild->isLeaf	= true;
		
		if(nodeOfSubSpace->coordinateAxisOfSplitting==X_AXIS){
			nodeOfSubSpace->leftChild->coordinateAxisOfSplitting=Y_AXIS;
			nodeOfSubSpace->rightChild->coordinateAxisOfSplitting=Y_AXIS;
		
		}
		else if(nodeOfSubSpace->coordinateAxisOfSplitting==Y_AXIS){
			nodeOfSubSpace->leftChild->coordinateAxisOfSplitting=Z_AXIS;
			nodeOfSubSpace->rightChild->coordinateAxisOfSplitting=Z_AXIS;			
		}

		else if(nodeOfSubSpace->coordinateAxisOfSplitting==Z_AXIS){
			nodeOfSubSpace->leftChild->coordinateAxisOfSplitting=X_AXIS;
			nodeOfSubSpace->rightChild->coordinateAxisOfSplitting=X_AXIS;
		}
	
		// Reference Triangles into new leafs
		sortTrianglesIntoChildCells(inputObjectDataStructure,nodeOfSubSpace); //TODO
		
		getBoundingBoxLeftChild(nodeOfSubSpace, nodeOfSubSpace->coordinateAxisOfSplitting, nodeOfSubSpace->splittingPlaneCoordinate, nodeOfSubSpace->leftChild);
		getBoundingBoxRightChild(nodeOfSubSpace, nodeOfSubSpace->coordinateAxisOfSplitting, nodeOfSubSpace->splittingPlaneCoordinate, nodeOfSubSpace->rightChild);

		// free unnecessary current node memory, which are the references to the triangles
		nodeOfSubSpace->faceHandles->clear(); 
		delete nodeOfSubSpace->faceHandles;
		nodeOfSubSpace->faceHandles=NULL;
		
		// recursion /////////////////////////////////////////////////////////
		subdivideSpace(inputObjectDataStructure,nodeOfSubSpace->leftChild,maxDepth,minNumberOfTriangles,currentKDTreeDepth);
		subdivideSpace(inputObjectDataStructure,nodeOfSubSpace->rightChild,maxDepth,minNumberOfTriangles,currentKDTreeDepth);
	}
}

void NearestTriangleSearch::sortTrianglesIntoChildCells(OMesh * inputObjectDataStructure,KDTreeNode * nodeOfSubSpace){
	OMesh::FaceVertexIter fvIt;
  Vec3  point0,point1,point2;

  Scalar triangleLowerBound=0.0;
  Scalar triangleUpperBound=0.0;
	
	nodeOfSubSpace->leftChild->faceHandles = new std::vector<OMesh::FaceHandle>;
	nodeOfSubSpace->rightChild->faceHandles = new std::vector<OMesh::FaceHandle>;
	
	nodeOfSubSpace->leftChild->faceHandles->clear();
	nodeOfSubSpace->rightChild->faceHandles->clear();
	
//	unsigned int debugNumberOfTrianglesInTwoChildren=0;
//	unsigned int debugNumberOfTrianglesLEFT=0;
//	unsigned int debugNumberOfTrianglesRIGHT=0;

	for(unsigned int i=0;i<nodeOfSubSpace->faceHandles->size();i++){
		fvIt = inputObjectDataStructure->fv_iter(nodeOfSubSpace->faceHandles->at(i));
    triangleLowerBound = std::numeric_limits<Scalar>::max();
    triangleUpperBound = -std::numeric_limits<Scalar>::max();
		
		point0 = inputObjectDataStructure->point(fvIt);

		if(point0[nodeOfSubSpace->coordinateAxisOfSplitting]<triangleLowerBound){
			triangleLowerBound = point0[nodeOfSubSpace->coordinateAxisOfSplitting];
		} 
		if(point0[nodeOfSubSpace->coordinateAxisOfSplitting]>triangleUpperBound){
			triangleUpperBound = point0[nodeOfSubSpace->coordinateAxisOfSplitting];
		}
		point1 = inputObjectDataStructure->point(++fvIt);

		if(point1[nodeOfSubSpace->coordinateAxisOfSplitting]<triangleLowerBound){
			triangleLowerBound = point1[nodeOfSubSpace->coordinateAxisOfSplitting];
		} 
		if(point1[nodeOfSubSpace->coordinateAxisOfSplitting]>triangleUpperBound){
			triangleUpperBound = point1[nodeOfSubSpace->coordinateAxisOfSplitting];
		}
		point2 = inputObjectDataStructure->point(++fvIt);

		if(point2[nodeOfSubSpace->coordinateAxisOfSplitting]<triangleLowerBound){
			triangleLowerBound = point2[nodeOfSubSpace->coordinateAxisOfSplitting];
		} 
		if(point2[nodeOfSubSpace->coordinateAxisOfSplitting]>triangleUpperBound){
			triangleUpperBound = point2[nodeOfSubSpace->coordinateAxisOfSplitting];
		}
		
		// sort into left children
		if(triangleUpperBound<nodeOfSubSpace->splittingPlaneCoordinate){
			nodeOfSubSpace->leftChild->faceHandles->push_back(nodeOfSubSpace->faceHandles->at(i));
		}	
		// sort into right children
		else if(triangleLowerBound>nodeOfSubSpace->splittingPlaneCoordinate){
			nodeOfSubSpace->rightChild->faceHandles->push_back(nodeOfSubSpace->faceHandles->at(i));	
		}
		// sort into left and right children
		else if((triangleLowerBound<=nodeOfSubSpace->splittingPlaneCoordinate) && (triangleUpperBound>=nodeOfSubSpace->splittingPlaneCoordinate)){
			nodeOfSubSpace->leftChild->faceHandles->push_back(nodeOfSubSpace->faceHandles->at(i));	
			nodeOfSubSpace->rightChild->faceHandles->push_back(nodeOfSubSpace->faceHandles->at(i));
		}
	}
	
}


void NearestTriangleSearch::computeCoordinateOfSplitting(OMesh * inputObjectDataStructure,KDTreeNode * nodeOfSubSpace){
	OMesh::FaceVertexIter fvIt;
  Vec3  point0,point1,point2,centerOfGravity;
  std::vector<Scalar> coordinateOfSplittingList;
  Scalar coordinateOfSplitting=0.0;
	coordinateOfSplittingList.clear();
    int currentCoordinateAxisOfSplitting = nodeOfSubSpace->coordinateAxisOfSplitting;
	// speed it up no need to use triangle points
	for(unsigned int i=0;i<nodeOfSubSpace->faceHandles->size();i++){
		fvIt = inputObjectDataStructure->fv_iter(nodeOfSubSpace->faceHandles->at(i));
		point0 = inputObjectDataStructure->point(fvIt);
		point1 = inputObjectDataStructure->point(++fvIt);
		point2 = inputObjectDataStructure->point(++fvIt);
		centerOfGravity = (point0+point1+point2)/3.0;
		
		coordinateOfSplittingList.push_back(centerOfGravity[currentCoordinateAxisOfSplitting]);
		
	}
	sort(coordinateOfSplittingList.begin(), coordinateOfSplittingList.end()); //introsert, not quicksort :-)
	
	coordinateOfSplitting = coordinateOfSplittingList[coordinateOfSplittingList.size()/2];
	
	nodeOfSubSpace->splittingPlaneCoordinate=coordinateOfSplitting;
	coordinateOfSplittingList.clear();
}

bool NearestTriangleSearch::terminateKDTreeConstruction(KDTreeNode * nodeOfSubSpace, unsigned int currentKDTreeDepth, unsigned int maxDepth, unsigned int minNumberOfTriangles){
	// abort: cell intersects with too few triangles and current tree is deep enough
	if ((nodeOfSubSpace->faceHandles->size()<=minNumberOfTriangles) || (currentKDTreeDepth==maxDepth)){ // TODO
		return true;
	}
	else{ 
		return false;
	}
}

bool NearestTriangleSearch::intersect(const Ray& ray, Vec3 & vertex, Vec3 & normal)
{
  Vec3  ts = getTs(kDTreeRootNode->boundingBox.minCorner, kDTreeRootNode->boundingBox.maxCorner, ray, kDTreeRootNode->coordinateAxisOfSplitting, kDTreeRootNode->splittingPlaneCoordinate);
	return intersectRecursive(ray, kDTreeRootNode, ts[0], ts[1], ts[2], vertex, normal); 
}

Vec3  NearestTriangleSearch::getTs(Vec3  minCorner, Vec3  maxCorner, const Ray& argRay, int splitAxis, Scalar splitPoint) const
{
  Scalar t[6];
  Scalar tt;
  Scalar intCoord1, intCoord2;
  Vec3  b0 = minCorner;
  Vec3  b1 = maxCorner;


	Ray ray = argRay;
	if (ray.direction[0] == 0)
		ray.direction[0] = 0.0000001;
	if (ray.direction[1] == 0)
		ray.direction[1] = 0.0000001;
	if (ray.direction[2] == 0)
		ray.direction[2] = 0.0000001;

	tt = (b1[0] - ray.point[0]) / ray.direction[0];
	intCoord1 = ray.point[1] + ray.direction[1] * tt;
	intCoord2 = ray.point[2] + ray.direction[2] * tt;
	if (intCoord1 < b1[1] && intCoord1 > b0[1] && intCoord2 < b1[2] && intCoord2 > b0[2] && tt > ray.min_t && tt < ray.max_t)
		t[0] = tt;
	else
		t[0] = 3.4e38;

	tt = (b0[0] - ray.point[0]) / ray.direction[0];
	intCoord1 = ray.point[1] + ray.direction[1] * tt;
	intCoord2 = ray.point[2] + ray.direction[2] * tt;
	if (intCoord1 < b1[1] && intCoord1 > b0[1] && intCoord2 < b1[2] && intCoord2 > b0[2] && tt > ray.min_t && tt < ray.max_t)
		t[1] = tt;
	else
		t[1] = 3.4e38;


	tt = (b0[1] - ray.point[1]) / ray.direction[1];
	intCoord1 = ray.point[0] + ray.direction[0] * tt;
	intCoord2 = ray.point[2] + ray.direction[2] * tt;
	if (intCoord1 < b1[0] && intCoord1 > b0[0] && intCoord2 < b1[2] && intCoord2 > b0[2] && tt > ray.min_t && tt < ray.max_t)
		t[2] = tt;
	else
		t[2] = 3.4e38;

	tt = (b1[1] - ray.point[1]) / ray.direction[1];
	intCoord1 = ray.point[0] + ray.direction[0] * tt;
	intCoord2 = ray.point[2] + ray.direction[2] * tt;
	if (intCoord1 < b1[0] && intCoord1 > b0[0] && intCoord2 < b1[2] && intCoord2 > b0[2] && tt > ray.min_t && tt < ray.max_t)
		t[3] = tt;
	else
		t[3] = 3.4e38;


	tt = (b0[2] - ray.point[2]) / ray.direction[2];
	intCoord1 = ray.point[0] + ray.direction[0] * tt;
	intCoord2 = ray.point[1] + ray.direction[1] * tt;
	if (intCoord1 < b1[0] && intCoord1 > b0[0] && intCoord2 < b1[1] && intCoord2 > b0[1] && tt > ray.min_t && tt < ray.max_t)
		t[4] = tt;
	else
		t[4] = 3.4e38;


	tt = (b1[2] - ray.point[2]) / ray.direction[2];
	intCoord1 = ray.point[0] + ray.direction[0] * tt;
	intCoord2 = ray.point[1] + ray.direction[1] * tt;
	if (intCoord1 < b1[0] && intCoord1 > b0[0] && intCoord2 < b1[1] && intCoord2 > b0[1] && tt > ray.min_t && tt < ray.max_t)
		t[5] = tt;
	else
		t[5] = 3.4e38;

	
  Scalar tMin = 3.4e38;
  Scalar tMax = -3.4e38;
	for (int i = 0;i < 6; i++)
	{
		if (t[i] == 3.4e38) continue;
		else
		{
			if (t[i] < tMin) tMin = t[i];
			if (t[i] > tMax) tMax = t[i];
		}
	}
	
  Scalar tSplit= -ray.min_t; //added initialization to get rid of compiler warning
	if (splitAxis == 0)
		tSplit = (splitPoint - ray.point[0]) / ray.direction[0];
	else if (splitAxis == 1)
		tSplit = (splitPoint - ray.point[1]) / ray.direction[1];
	else if (splitAxis == 2)
		tSplit = (splitPoint - ray.point[2]) / ray.direction[2];
	if (!(tSplit < ray.max_t && tSplit > ray.min_t))
		tSplit = 3.4e38;

  return Vec3 (tMin, tMax, tSplit);
}

bool NearestTriangleSearch::intersectRecursive(const Ray& ray, KDTreeNode * currentNode, Scalar tMin, Scalar tMax, Scalar tSplit, Vec3 & vertex, Vec3 & normal)
{

  (void) tMin; //just to make it used

	KDTreeNode* nearChild;
	KDTreeNode* farChild;

  Vec3  nearTs;
  Vec3  farTs;

	if (currentNode->isLeaf && currentNode->faceHandles->size() == 0)
		return false;
	else
	{
		if (currentNode->isLeaf)
		{
      Scalar nearestIntersection = 3.4e38;
      Scalar tempIntersection = 3.4e38;
			//OMesh::FaceHandle triangle;
			int indexOfTriangle = -1;
			for (unsigned int i = 0; i < currentNode->faceHandles->size(); i++)
			{
				tempIntersection = intersectTriangleWithRay(currentNode->faceHandles->at(i), ray, vertex, normal);
				if (tempIntersection)
				{
					if (tempIntersection != -1 && tempIntersection < nearestIntersection)
					{
						nearestIntersection = tempIntersection;
						indexOfTriangle = i;
					}
				}
			}

			if (indexOfTriangle != -1)
			{	
				//OMesh::FaceVertexIter fvIt = m_inputObjectDataStructure->fv_iter(currentNode->faceHandles->at(indexOfTriangle));
				//vertex = m_inputObjectDataStructure->point(fvIt);
				//normal = m_inputObjectDataStructure->normal(fvIt);
				return true;
			}
			else
				return false;
		}
		else
		{
      Vec3  tsLeftChild = getTs(currentNode->leftChild->boundingBox.minCorner, currentNode->leftChild->boundingBox.maxCorner, ray, currentNode->leftChild->coordinateAxisOfSplitting, currentNode->leftChild->splittingPlaneCoordinate);
      Vec3  tsRightChild = getTs(currentNode->rightChild->boundingBox.minCorner, currentNode->rightChild->boundingBox.maxCorner, ray, currentNode->rightChild->coordinateAxisOfSplitting, currentNode->rightChild->splittingPlaneCoordinate);
			
      if (tsLeftChild[0] == (Scalar)3.4e38 && tsRightChild[0] == (Scalar)3.4e38)
				return false;
      else if (tsLeftChild[0] == (Scalar)3.4e38)
				return intersectRecursive(ray, currentNode->rightChild, tsRightChild[0], tsRightChild[1], tsRightChild[2], vertex, normal);
      else if (tsRightChild[0] == (Scalar)3.4e38)
				return intersectRecursive(ray, currentNode->leftChild, tsLeftChild[0], tsLeftChild[1], tsLeftChild[2], vertex, normal);
			
			if (tsLeftChild[0] < tsRightChild[0])
			{
				nearChild = currentNode->leftChild;
				farChild = currentNode->rightChild;

				nearTs = tsLeftChild;
				farTs = tsRightChild;
			}
			else
			{
				farChild = currentNode->leftChild;
				nearChild = currentNode->rightChild;

				nearTs = tsRightChild;
				farTs = tsLeftChild;
			}

			if (tSplit > tMax)
			{
				return intersectRecursive(ray, nearChild, nearTs[0], nearTs[1], nearTs[2], vertex, normal);
			}
			else
			{
				bool intDataNearChild = intersectRecursive(ray, nearChild, nearTs[0], nearTs[1], nearTs[2], vertex, normal);
				if (intDataNearChild == true)
					return true;
				else
					return intersectRecursive(ray, farChild, farTs[0], farTs[1], farTs[2], vertex, normal);
			}
		}
	}
	

}

Scalar NearestTriangleSearch::intersectTriangleWithRay(OMesh::FaceHandle triangle, const Ray &ray, Vec3 & vertex, Vec3 & normal) {
	//=========Exercise 1.3.3===========//
    
	
	//insert code here
	OMesh::FaceVertexIter fvIt = m_inputObjectDataStructure->fv_iter(triangle);
  Vec3  m_p1 = m_inputObjectDataStructure->point(fvIt);
	++fvIt;
  Vec3  m_p2 = m_inputObjectDataStructure->point(fvIt);
	++fvIt;
  Vec3  m_p3 = m_inputObjectDataStructure->point(fvIt);
	
	//I implemented the solution in the book Phsically Based Rendering - From Theory to Implementation
  Vec3  d = ray.direction;
  Vec3  o = ray.point;
  Vec3  e1 = m_p2 - m_p1;
  Vec3  e2 = m_p3 - m_p1;
  Vec3  s1 = cross(d, e2);
  Scalar divisor = dot(s1, e1);
	if (divisor == 0)
		return -1;
	
  Vec3  tt = o - m_p1;
  Scalar invDiv = 1 / divisor;
	
  Scalar b1 = dot(tt, s1) * invDiv;

	if (b1 < 0 || b1 > 1)
		return -1;

  Vec3  s2 = cross(tt, e1);
  Scalar b2 = dot(d, s2) * invDiv;

	if (b2 < 0 || b1 + b2 > 1)
		return -1;

  Scalar t = dot(e2, s2) * invDiv;
  Scalar b3 = 1 - b1 - b2;


	vertex = ray.getPointOnRay(t);

	fvIt = m_inputObjectDataStructure->fv_iter(triangle);
  Vec3  normal1 = m_inputObjectDataStructure->normal(fvIt);
  ++fvIt;
  Vec3  normal2 = m_inputObjectDataStructure->normal(fvIt);
  ++fvIt;
  Vec3  normal3 = m_inputObjectDataStructure->normal(fvIt);

  normal = Vec3 (normal1 * b1 + normal2 * b2 + normal3 * b3);


	if (t > ray.min_t && t < ray.max_t)
	{
		return t;
	}
	else
		return -1;
	
	
	//==================================//
}

Scalar NearestTriangleSearch::dot(const Vec3 & v1, const Vec3 & v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]; 
}

Vec3  NearestTriangleSearch::cross(const Vec3 & v1, const Vec3 & v2)
{
  Scalar dest[3];
	dest[0] = v1[1] * v2[2] - v1[2] * v2[1];
	dest[1] = -v1[0] * v2[2] + v1[2] * v2[0];
	dest[2] = v1[0] * v2[1] - v1[1] * v2[0];

  return Vec3 (dest[0], dest[1], dest[2]);
}

Vec3  NearestTriangleSearch::getMinCornerOfBB(KDTreeNode * node)
{
  Vec3  minCorner;
	OMesh::FaceVertexIter fvIt;
  Vec3  point;
  minCorner[0] = std::numeric_limits<Scalar>::max();
  minCorner[1] = std::numeric_limits<Scalar>::max();
  minCorner[2] = std::numeric_limits<Scalar>::max();

	for(unsigned int i = 0; i < node->faceHandles->size(); i++)
	{
		fvIt = m_inputObjectDataStructure->fv_iter(node->faceHandles->at(i));
		for (unsigned int j = 0; j < 3; j++)
		{
			point = m_inputObjectDataStructure->point(fvIt);
			if (point[0] < minCorner[0])
				minCorner[0] = point[0];
			if (point[1] < minCorner[1])
				minCorner[1] = point[1];
			if (point[2] < minCorner[2])
				minCorner[2] = point[2];

			++fvIt;
		}	
	}
  return Vec3 (minCorner);
}

Vec3  NearestTriangleSearch::getMaxCornerOfBB(KDTreeNode * node)
{
  Vec3  maxCorner;
	OMesh::FaceVertexIter fvIt;
  Vec3  point;
  maxCorner[0] = -std::numeric_limits<Scalar>::max();
  maxCorner[1] = -std::numeric_limits<Scalar>::max();
  maxCorner[2] = -std::numeric_limits<Scalar>::max();

	for(unsigned int i = 0; i < node->faceHandles->size(); i++)
	{
		fvIt = m_inputObjectDataStructure->fv_iter(node->faceHandles->at(i));
		for (unsigned int j = 0; j < 3; j++)
		{
			point = m_inputObjectDataStructure->point(fvIt);
			if (point[0] > maxCorner[0])
				maxCorner[0] = point[0];
			if (point[1] > maxCorner[1])
				maxCorner[1] = point[1];
			if (point[2] > maxCorner[2])
				maxCorner[2] = point[2];

			++fvIt;
		}	
	}
  return Vec3 (maxCorner);
}

void NearestTriangleSearch::getBoundingBoxLeftChild(KDTreeNode * node, int spAxis, Scalar spCoord, KDTreeNode * leftChild)
{
  Vec3  b0, b1;

	b0 = node->boundingBox.minCorner;

	if (spAxis == 0)
	{
		b1[0] = spCoord;
		b1[1] = node->boundingBox.maxCorner[1];
		b1[2] = node->boundingBox.maxCorner[2];
	}
	else if (spAxis == 1)
	{
		b1[0] = node->boundingBox.maxCorner[0];
		b1[1] = spCoord;
		b1[2] = node->boundingBox.maxCorner[2];
	}
	else if (spAxis == 2)
	{
		b1[0] = node->boundingBox.maxCorner[0];
		b1[1] = node->boundingBox.maxCorner[1];
		b1[2] = spCoord;
	}

	leftChild->boundingBox.minCorner = b0;
	leftChild->boundingBox.maxCorner = b1;
}

void NearestTriangleSearch::getBoundingBoxRightChild(KDTreeNode * node, int spAxis, Scalar spCoord, KDTreeNode * rightChild)
{
  Vec3  b0, b1;

	b1 = node->boundingBox.maxCorner;

	if (spAxis == 0)
	{
		b0[0] = spCoord;
		b0[1] = node->boundingBox.minCorner[1];
		b0[2] = node->boundingBox.minCorner[2];
	}
	else if (spAxis == 1)
	{
		b0[0] = node->boundingBox.minCorner[0];
		b0[1] = spCoord;
		b0[2] = node->boundingBox.minCorner[2];
	}
	else if (spAxis == 2)
	{
		b0[0] = node->boundingBox.minCorner[0];
		b0[1] = node->boundingBox.minCorner[1];
		b0[2] = spCoord;
	}

	rightChild->boundingBox.minCorner = b0;
	rightChild->boundingBox.maxCorner = b1;
}

Scalar NearestTriangleSearch::getDistanceToBox(const Vec3 & _point, KDTreeAxiAlignedBoundingBox& _box) const{
    Vec3  distance(0.0f,0.0f,0.0f);
		for(int i=0;i<3;i++){
			if(_point[i]<_box.minCorner[i]){
				distance[i]=_box.minCorner[i]-_point[i];
			}else if(_point[i]>_box.maxCorner[i]){
				distance[i]=_point[i]-_box.maxCorner[i];
			}else{
				distance[i]=0.0f;
			}
		}
		return distance.length();
	}

}}//namespaces
