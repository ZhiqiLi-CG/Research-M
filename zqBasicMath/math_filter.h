/***********************************************************/
/**	\file
	\brief		filter function
	\details	filter function, reproduction of Topological Methods for the Analysis of High Dimensional Data Sets and 3D Object Recognition
	\author		Zhiqi Li
	\date	    1/22/2022

*/
/***********************************************************/
#ifndef __MATH_FILTER_H__
#define __MATH_FILTER_H__

#include <assert.h>
#include <iostream>
#include <math.h>
#include <zqBasicMath/math_vector.h>
#include <zqBasicMath/math_matrix.h>
#include <zqBasicMath/math_distance.h>
#include <zqBasicMath/math_vector_utils.h>
#include<ResearchM_config.h>
#ifdef  RESEARCHM_ENABLE_CUDA
#include <zqBasicUtils/utils_cuda.h>
#include <zqBasicUtils/utils_array.h>
#endif //  RESEARCHM_ENABLE_CUDA

namespace zq {
	namespace math {
		template<class T, int d=-1>
		class Filter_1D_Base {
		public:
			static const int res_dim = 1;
#ifdef RESEARCHM_ENABLE_CUDA
			__device__ __host__
#endif
			Filter_1D_Base(){}

			
#ifdef RESEARCHM_ENABLE_CUDA
			__device__ __host__
#endif
			Vec<T, 1> operator()(const Vec<T, d>& p)  const {
				printf("Functor of Filter_1D_base could not be used");
				throw "Functor of Filter_1D_base could not be used";
				exit(1);
			}

			template<class T>
#ifdef RESEARCHM_ENABLE_CUDA
			__device__ __host__
#endif
			Vec<T, 1> operator()(const T* p)  const {
				printf("Functor of Filter_1D_base could not be used");
				throw "Functor of Filter_1D_base could not be used";
				exit(1);
			}
		};

		template<class T, int d=-1>
		class Filter_2D_Base {
		public:
			static const int  res_dim = 2;
#ifdef RESEARCHM_ENABLE_CUDA
			__device__ __host__
#endif
				Filter_2D_Base(){}
			
#ifdef RESEARCHM_ENABLE_CUDA
			__device__ __host__
#endif
				Vec<T,2> operator()(const Vec<T, d>& p)  const {
				printf("Functor of Filter_2D_base could not be used");
				throw "Functor of Filter_2D_base could not be used";
				exit(1);
			}

#ifdef RESEARCHM_ENABLE_CUDA
			__device__ __host__
#endif
				Vec<T, 2> operator()(const T* p)  const {
				printf("Functor of Filter_2D_base could not be used");
				throw "Functor of Filter_2D_base could not be used";
				exit(1);
			}
		};

		template<class T, int d = -1>
		class XCoordFilter:public Filter_1D_Base<T,d> {
		public:
			static const int res_dim = 1;
#ifdef RESEARCHM_ENABLE_CUDA
			__device__ __host__
#endif
				XCoordFilter() {}

#ifdef RESEARCHM_ENABLE_CUDA
			__device__ __host__
#endif
			Vec<T,1> operator()(const Vec<T, d>& p)  const {
				return Vec<T,1>(p[0]);
			}

#ifdef RESEARCHM_ENABLE_CUDA
			__device__ __host__
#endif
			Vec<T, 1> operator()(const T* p) const {
				return Vec<T, 1>(p[0]);
			}
		};

		template<class T, int d>
		class YCoordFilter :public Filter_1D_Base<T, d> {
		public:
			static const int res_dim = 1;
#ifdef RESEARCHM_ENABLE_CUDA
			__device__ __host__
#endif
			YCoordFilter() {}

#ifdef RESEARCHM_ENABLE_CUDA
			__device__ __host__
#endif
			Vec<T, 1> operator()(const Vec<T, d>& p) const {
				return Vec<T, 1>(p[1]);
			}

#ifdef RESEARCHM_ENABLE_CUDA
			__device__ __host__
#endif
			Vec<T, 1> operator()(const T* p) const {
				return Vec<T, 1>(p[1]);
			}
		};
		template<class T, int d>

		class XYCoordFilter :public Filter_2D_Base<T,d> {
		public:
			static const int res_dim = 2;
#ifdef RESEARCHM_ENABLE_CUDA
			__device__ __host__
#endif
			XYCoordFilter() {}

#ifdef RESEARCHM_ENABLE_CUDA
			__device__ __host__
#endif
			Vec<T,2> operator()(const Vec<T, d>& p) const{
				return Vec<T,2>(p[0],p[1]);
			}

#ifdef RESEARCHM_ENABLE_CUDA
			__device__ __host__
#endif
			Vec<T, 2> operator()(const T* p)  const {
				return Vec<T, 2>(p[0], p[1]);
			}
		};
	}
}

#endif	//	__ZQ_VECTOR_H__