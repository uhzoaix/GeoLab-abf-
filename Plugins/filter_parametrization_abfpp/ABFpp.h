#pragma once
#include "MeshTriangle.h"
#include <vector>
#include <Eigen/Core>
namespace GCL{

class OMesh;
namespace Utilities{

// this class is to found matrix or vector of ABF++
class ABFpp
{
public:
	ABFpp();
	~ABFpp();

public:
	// PAY ATTENTION: don't forget to initialize lambda
	void InitProblem(OMesh* mesh,std::vector <MeshTriangle>& triangles,unsigned int NumInter);

	// the norm of grad F(x)
	double NormGradF();

public:
	//assisting function
	// get angle to vertex and face,e.g. return angle of vertex1 in face1
	double GetAngle(int f_idx,int v_idx);
	double GetAngleAlpha(int idx);
	double GetAngleAlpha(int f_idx,int v_idx);
	// in triangle t,k th vertex,check if it is boundary vertex
	inline bool CheckBoundary(int t,int k);

	// get the alpha part's partial derivatives
	double CalculateAlphaPara(int t, int k);
	// get the alpha's len part's partial derivatives
	double CalculateAlphaLenPara(int t,int k);

	// get the lambda_tri part's partial derivatives
	double CalculateTriPara(int f_idx);

	// get the lambda_plan part's partial derivatives
	double CalculatePlanPara(int v_idx);

	// get the lambda_len part's partial derivatives
	double CalculateLenPara(int v_idx);

	// the value of object function
	double ObjectFuncValue();

protected:
	// unknown planar angles 
	std::vector <double>	   alpha_;
	// optimal angles triangle vecotr
	std::vector <MeshTriangle> beta_triangles_;
	// parameters for Lagranege Multipliers
	//std::vector <double>	   lambda;
	std::vector <double>	   lambda_tri_;
	std::vector <double>	   lambda_plan_;
	std::vector <double>	   lambda_len_;
	//int pos_lambda_plan_;		// iterator's position
	//int pos_lambda_len_;

	OMesh* mesh_;

	unsigned int num_interior_;
};

}
}


