#include "MeshTriangle.h"
#include <qmath.h>

MeshTriangle::MeshTriangle(int index1, int index2, int index3)
{
	index_[0] = index1;
	index_[1] = index2;
	index_[2] = index3;

	reconstruct_flag_ = false;
}

MeshTriangle::~MeshTriangle()
{
}

void MeshTriangle::InputAngle(double a1,double a2,double a3)
{
	angles_[0] = a1;
	angles_[1] = a2;
	angles_[2] = a3;
}

void MeshTriangle::InputLength(double l1,double l2,double l3)
{
	angles_[0] = acos((l2*l2 + l3*l3 - l1*l1) / (2 * l2*l3));
	angles_[1] = acos((l1*l1 + l3*l3 - l2*l2) / (2 * l1*l3));
	angles_[2] = acos((l2*l2 + l1*l1 - l3*l3) / (2 * l1*l2));
}

void MeshTriangle::InputVertex( double c1x, double c1y, double c1z,
								double c2x, double c2y, double c2z,
								double c3x, double c3y, double c3z)
{
	InputLength(sqrt((c2x - c3x)*(c2x - c3x) + (c2y - c3y)*(c2y - c3y) + (c2z - c3z)*(c2z - c3z)),
				sqrt((c1x - c3x)*(c1x - c3x) + (c1y - c3y)*(c1y - c3y) + (c1z - c3z)*(c1z - c3z)),
				sqrt((c2x - c1x)*(c2x - c1x) + (c2y - c1y)*(c2y - c1y) + (c2z - c1z)*(c2z - c1z)));
}