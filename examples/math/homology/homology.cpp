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
#include<zqBasicMath/math_homology.h>
#include<zqBasicMath/math_dense.h>
int main() {
	/// The test case is from: https://zhuanlan.zhihu.com/p/41264363
	std::vector<std::vector<int>> complex_index{
		{0},{1},{2},{3},
		{0,1},{1,2},{0,2},{2,3},{1,3},
		{0,1,2}
	};
	std::vector<int> index{ 0,1,2,3 };
	zq::Simplical_Complex<int> sc(&index[0], 4, complex_index);
	std::cout << "First, test the NChain" << std::endl;
	for (int i = 0; i < 4; i++) {
		std::vector<zq::Simplex<int>> result;
		sc.NChain(i, result);
		std::cout << "chain of index:" << i << std::endl;
		for (int j = 0; j < result.size(); j++) {
			for (int k = 0; k < result[j].Dim(); k++) {
				printf("%d ", result[j].index(k));
			}printf("\n");
		}
	}
	std::cout << "Second, test the boundary matrix" << std::endl;
	for (int i = 0; i < 3; i++) {
		zq::DenseMatrix<int> bounadry_m = sc.BoundaryMatrix(i-1, i );
		printf("Bounadry matrix for index %d and %d:\n", i-1, i);
		std::cout << bounadry_m << std::endl;
	}
	std::cout << "Third, test the reduced boundary matrix" << std::endl;
	for (int i = 0; i < 3; i++) {
		int rank, nullity;
		zq::DenseMatrix<int> bounadry_m = sc.ReducedBoundaryMatrix(i-1, i, rank, nullity);
		printf("Bounadry matrix for index %d and %d, rank:%d,nullity:%d\n", i-1, i,rank,nullity);
		std::cout << bounadry_m << std::endl;
	}
	std::cout << "Fourth, test the betti number" << std::endl;
	std::vector<int> betti=sc.BettiNumber(3);
	for (int i = 0; i < betti.size(); i++) {
		printf("%d ", betti[i]);
	}
	printf("\n");
	return 0;
}