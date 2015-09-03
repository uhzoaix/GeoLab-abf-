#pragma once
namespace GCL{
namespace Utilities{

class ParaCoordinate
{
public:
	ParaCoordinate(int idx,double x,double y);
	~ParaCoordinate();

public:
	int idx_;
	double para_coor_x;
	double para_coor_y;
};

}
}

