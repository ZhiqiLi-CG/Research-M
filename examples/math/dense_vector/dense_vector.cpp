/***********************************************************/
/**	
\file
\brief		Example of Quaternion
\details	This file illustrates how to use Quaternion in yzLib
\author		Zhiqi Li
\date		12/20/2020
*/
/***********************************************************/
#include <iostream>
#include<zqBasicMath/math_vector.h>
#include<zqBasicMath/math_matrix.h>
#include<zqBasicMath/math_quaternion.h>
#include<zqBasicMath/math_lookup_table.h>
#include<zqBasicMath/math_dense.h>
#include<zqBasicMath/math_dense_solver.h>

int main() {
	zq::DenseMatrix<int> m_int(5, 5);
	zq::DenseVector<int> v_int(5);
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			m_int[i][j] = i + j;
		}
		v_int[i] = i;
	}
	std::cout << "The matrix:" << std::endl;
	std::cout << m_int<<std::endl;
	std::cout << "The vector:" << std::endl;
	std::cout << v_int << std::endl;
	auto first_mul = m_int * v_int;
	zq::DenseMatrix<int> second_mul = m_int * m_int * v_int;
	std::cout << "The first mul matrix:" << std::endl;
	std::cout << first_mul << std::endl;
	std::cout << "The second mu matrix:" << std::endl;
	std::cout << second_mul << std::endl;

	auto first_muls = m_int * 3;
	std::cout << "The first mul matrix with scalar:" << std::endl;
	std::cout << first_muls << std::endl;
	auto first_muls2 = 1.1*m_int;
	std::cout << "The first mul matrix with scalar:" << std::endl;
	std::cout << first_muls2 << std::endl;

	return 0;
}