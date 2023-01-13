/***********************************************************/
/**	
\file
\brief		Example of Matrix
\details	This file illustrates how to use matrix in zqLib, based on yzLib of Dr Yizhong Zhang
\author		Zhiqi Li
\date		12/20/2022
*/
/***********************************************************/
#include <iostream>
#include<zqBasicMath/math_vector.h>
#include<zqBasicMath/math_matrix.h>
#include<zqBasicMath/math_utils.h>

int main() {
	{
		std::cout << "==============================" << std::endl;
		std::cout << "Test Matrix2x2:" << std::endl;
		zq::Matrix2x2<double>	M1, M2;
		M1.SetIdentity();
		M2.SetRotationDeg(90);
		std::cout << "M1: " << std::endl << M1;
		std::cout << "M2: " << std::endl << M2;
		std::cout << "M1 * M2: " << std::endl << M1 * M2;
		std::cout << "--------------------" << std::endl;
	}

	std::cout << "==============================" << std::endl;
	std::cout << "Test Matrix3x3:" << std::endl;
	zq::Matrix3x3d	M3x3;
	M3x3.SetRotationDeg(zq::Vec3d(1, 1, 0), 90);
	std::cout << "Rotate around (1,1,0) 90deg: " << std::endl << M3x3 << std::endl;

	std::cout << "==============================" << std::endl;
	std::cout << "Test Matrix4x4:" << std::endl;
	zq::Matrix4x4d	M4x4;
	M4x4.SetRotationDeg(zq::Vec3d(1, 1, 0), 90);
	std::cout << "Rotate around (1,1,0) 90deg: " << std::endl << M4x4 << std::endl;

	return 0;
}