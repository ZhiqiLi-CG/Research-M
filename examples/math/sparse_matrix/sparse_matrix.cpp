/***********************************************************/
/**	
\file
\brief		Example of sparse matrix
\details	This file illustrates how to use Quaternion in yzLib
\author		Zhiqi Li
\date		12/27/2022
*/
/***********************************************************/
#include <iostream>
#include<zqBasicMath/math_sparse_convert.h>
#include<zqBasicMath/math_sparse_matrix.h>
#include<zqBasicMath/math_sparse_vector.h>

int main() {
	std::vector<zq::SparseMatrixBase<int>*> matrix_ptr{
		new zq::SparseMatrixCoo<int>(3, 4),
		new zq::SparseMatrixCooSym<int>(3),
		new zq::SparseMatrixCSR<int>(3, 4),
		new zq::SparseMatrixCSRSym<int>(3),
		new zq::SparseMatrixLIL<int>(3, 4) };
	// test insert
	for (int i = 0; i < 5; i++) {
		matrix_ptr[i]->InsertElement(100, 1, 2);
	}
	// test check exists
	for (int i = 0; i < 5; i++) {
		std::cout<<matrix_ptr[i]->IsSymmetric();
	}
	// test sparse vector
	zq::SparseVector<float> vec(5);
	std::cout << vec << std::endl;
	vec[0] = 12;
	vec[3] = 122;
	std::cout << "test the insert vaule of sparse vec" << std::endl;
	std::cout << vec << std::endl;
	zq::SparseVector<float> vec1(5);
	vec1[0] = 12;
	vec1[3] = 122;
	vec1[4] = 1224;
	auto vec2 = vec + vec1;
	std::cout << "test the add vaule of sparse vec" << std::endl;
	std::cout << vec << std::endl;
	std::cout << vec1 << std::endl;
	std::cout << vec2 << std::endl;
	vec2 = vec - vec1;
	std::cout << "test the minus vaule of sparse vec" << std::endl;
	std::cout << vec << std::endl;
	std::cout << vec1 << std::endl;
	std::cout << vec2 << std::endl;
	std::cout << "test the multi scalr vaule of sparse vec" << std::endl;
	vec2 = vec * 0.5;
	std::cout << vec << std::endl;
	std::cout << vec2 << std::endl;
	vec2 = vec / 2.0;
	std::cout << vec << std::endl;
	std::cout << vec2 << std::endl;

	vec2 = vec;
	vec2 += vec1;
	std::cout << "test the self add vaule of sparse vec" << std::endl;
	std::cout << vec << std::endl;
	std::cout << vec1 << std::endl;
	std::cout << vec2 << std::endl;
	vec2 = vec;
	vec2 -= vec1;
	std::cout << "test the self minus vaule of sparse vec" << std::endl;
	std::cout << vec << std::endl;
	std::cout << vec1 << std::endl;
	std::cout << vec2 << std::endl;
	std::cout << "test the self multi scalr vaule of sparse vec" << std::endl;
	vec2 = vec;
	vec2 *= 0.5;
	std::cout << vec << std::endl;
	std::cout << vec2 << std::endl;
	vec2 = vec;
	vec2 /= 2;
	std::cout << vec << std::endl;
	std::cout << vec2 << std::endl;

	// test the sparse matrix and the sparse vector
	zq::SparseMatrixLIL<int> lil(3, 4);
	lil.InsertElement(1, 0, 0);
	lil.InsertElement(2, 1, 1);
	lil.InsertElement(3, 2, 2);
	lil.InsertElement(4, 0, 3);
	zq::SparseVector<int> vec_row(4);
	vec_row = lil.GetSparseRow(0);
	std::cout << vec_row << std::endl;
	vec_row = lil.GetSparseRow(1);
	std::cout << vec_row << std::endl;
	vec_row = lil.GetSparseRow(2);
	std::cout << vec_row << std::endl;
	zq::SparseVector<int> vec_row2(4);
	vec_row2[0] = 99;
	vec_row2[2] = 98;
	lil.SetSparseRow(vec_row2, 1);
	vec_row = lil.GetSparseRow(0);
	std::cout << vec_row << std::endl;
	vec_row = lil.GetSparseRow(1);
	std::cout << vec_row << std::endl;
	vec_row = lil.GetSparseRow(2);
	std::cout << vec_row << std::endl;

	return 0;
}