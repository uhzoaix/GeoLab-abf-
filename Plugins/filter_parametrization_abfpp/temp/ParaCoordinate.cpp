#include "ParaCoordinate.h"
namespace GCL{ namespace Utilities{

ParaCoordinate::ParaCoordinate(int idx,double x,double y)
{
	idx_ = idx;
	para_coor_x = x;
	para_coor_y = y;
}


ParaCoordinate::~ParaCoordinate()
{
}

} }
