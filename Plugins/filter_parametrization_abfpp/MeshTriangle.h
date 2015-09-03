#pragma once
class MeshTriangle
{
// The class is to save info from a triangle in mesh
// have: angles,three vert'index in mesh

public:
	MeshTriangle(int index1, int index2, int index3);
	~MeshTriangle();

public:
	void InputAngle (double a1, double a2, double a3);
	void InputLength(double l1, double l2, double l3);
	void InputVertex(double c1x,double c1y,double c1z,
					 double c2x,double c2y,double c2z,
					 double c3x,double c3y,double c3z);
public:
	// e.g. angle[0] aligns to index[0]
	double angles_ [3];
	int index_[3];
	// when all nodes in a triangle have been projected, flag to be true;
	bool reconstruct_flag_;
};

