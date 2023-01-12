/***********************************************************/
/**	
\file
\brief		Example of cuda math array
\details	This file illustrates how to use math cuda array
\author		Zhiqi Li
\date		12/27/2022
*/
/***********************************************************/
#include <iostream>
#include<zqBasicMath/math_sparse_convert.h>
#include<zqBasicMath/math_sparse_matrix.h>
#include<zqBasicMath/math_sparse_vector.h>
#include<zqBasicMath/math_vector.h>
#include<zqBasicMath/math_matrix.h>
int main() {
	/// Firsr, test thrust
	thrust::device_vector<int> a(300);
	counting idxfirst(0);
	counting idxlast = idxfirst + a.size();
	auto g_f = [] __device__ __host__(const int idx)->int {
		printf("idx:%d,",idx);
		return idx;
	};
	thrust::transform(
		idxfirst,
		idxlast,
		a.begin(),
		g_f
	);
	return 0;

}