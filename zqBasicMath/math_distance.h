/***********************************************************/
/**	\file
	\brief		distance for Vec and Dense_Vector
	\details	distance for Vec and Dense_vector
	\author		Zhiqi Li
	\date	    1/20/2022

*/
/***********************************************************/
#ifndef __MATH_DISTANCE_H__
#define __MATH_DISTANCE_H__

#include <assert.h>
#include <iostream>
#include <zqBasicMath/math_vector.h>
#include <zqBasicMath/math_matrix.h>
#include <zqBasicMath/math_dense.h>

#include<ResearchM_config.h>
#ifdef  RESEARCHM_ENABLE_CUDA
#include <zqBasicUtils/utils_cuda.h>
#include <zqBasicUtils/utils_array.h>
#endif //  RESEARCHM_ENABLE_CUDA

namespace zq {
	namespace math {
		enum DistType {
			EUCLIDEAN,
			MANHATTAN,
			CHEBYSHEV,
			MINKOWSKI
		};
		template<class T, int d>
		T distance(const Vec<T,d>& v1, const Vec<T, d>& v2, DistType type, int d=2) {
			if (type == DistType::EUCLIDEAN) {
				T sum = (T)0;
				for (int i = 0; i < d; i++) {
					sum += (v1[i] - v2[i]) * (v1[i] - v2[i]);
				}
				return sum;
			}
			else if (type == DistType::MANHATTAN) {
				T sum = (T)0;
				for (int i = 0; i < d; i++) {
					sum += abs(v1[i] - v2[i]);
				}
				return sum;
			}
			else if (type == DistType::CHEBYSHEV) {
				T val = (T)0;
				for (int i = 0; i < d; i++) {
					sum += abs(v1[i] - v2[i]);
				}
				return sum;
			}
		}
	}
}

#endif	//	__ZQ_VECTOR_H__