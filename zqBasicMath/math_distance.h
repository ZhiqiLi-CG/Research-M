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
#include <zqBasicMath/math_weight.h>
#include <zqBasicUtils/utils_array.h>
#include <zqBasicUtils/utils_array_func.h>

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
#ifdef RESEARCHM_ENABLE_CUDA
		__device__ __host__
#endif 
		T distance(const Vec<T,d>& v1, const Vec<T, d>& v2, DistType type, int p=2) {
			return distance<T>(&v1[0], &v2[0], d, type, p);
		}

		template<class T>
#ifdef RESEARCHM_ENABLE_CUDA
		__device__ __host__
#endif
		T distance(const T* v1,const T* v2, int d, DistType type,  int p = 2) {
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
					if (abs(v1[i] - v2[i]) > val) val = abs(v1[i] - v2[i]);
				}
				return val;
			}
			else if (type == DistType::MINKOWSKI) {
				T sum = (T)0;
				for (int i = 0; i < d; i++) {
					sum += pow(abs(v1[i] - v2[i]), p);
				}
				return pow(sum, 1 / p);
			}
		}
		
		enum ClusterDistType {
			SINGLELINK,
			COMPLETELINK,
			UPGMA,
			WPGMA
		};

		/// cluster Distance function have two version:
		///	1.  Each calculation between 2 points are on GPU
		/// 2.  The who calculation is on GPU
		template<class T, ClusterDistType type, DistType dist_type>
#ifdef RESEARCHM_ENABLE_CUDA
		__device__ __host__
#endif
		T clusterDistance(
			const T** v1,
			const T** v2,
			int n,
			int m,
			int d,
			int p=2,
			Weight<WeightType::CLUSTER>* w= nullptr
		) {
			int ans = (T)-1;
			for (int i = 0; i < n;i++) {
				for (int j = 0; j < m; j++) {
					int dist=distance<T,dist_type>(v1[i], v2[j], d, p);
					if (ans < 0) {
						if constexpr (type == ClusterDistType::SINGLELINK) {
							ans = dist;
						}
						else if constexpr(type == ClusterDistType::COMPLETELINK) {
							ans = dist;
						}
						else if constexpr(type == ClusterDistType::UPGMA) {
							ans = dist;
						}
						else if constexpr (type == ClusterDistType::WPGMA) {
							ans = w->weight<T>(dist)*dist;
						}
					};
					else {
						if constexpr (type == ClusterDistType::SINGLELINK) {
							if (dist < ans) ans = dist;
						}
						else if constexpr (type == ClusterDistType::COMPLETELINK) {
							if (dist > ans) ans = dist;
						}
						else if constexpr (type == ClusterDistType::UPGMA) {
							ans += dist;
						}
						else if constexpr (type == ClusterDistType::WPGMA) {
							ans += w->weight<T>(dist) * dist;
						}
					}
				}
			}
			if constexpr (type == ClusterDistType::UPGMA) {
				ans /= (m*n);
			}
			else if constexpr (type == ClusterDistType::WPGMA) {
				ans /= (m * n);
			}
		}

		template<class T, int d,ClusterDistType type, DistType dist_type>
#ifdef RESEARCHM_ENABLE_CUDA
		__device__ __host__
#endif
		T clusterDistance(
			const Vec<T, d>* v1,
			const Vec<T, d>* v2,
			int n,
			int m,
			int p = 2,
			Weight<WeightType::CLUSTER>* w = nullptr
		) {
		int ans = (T)-1;
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				int dist = distance<T,d, dist_type>(v1[i], v2[j], dist_type, p);
				if (ans < 0) {
					if constexpr (type == ClusterDistType::SINGLELINK) {
						ans = dist;
					}
					else if constexpr (type == ClusterDistType::COMPLETELINK) {
						ans = dist;
					}
					else if constexpr (type == ClusterDistType::UPGMA) {
						ans = dist;
					}
					else if constexpr (type == ClusterDistType::WPGMA) {
						ans = w->weight<T>(dist) * dist;
					}
				};
				else {
					if constexpr (type == ClusterDistType::SINGLELINK) {
						if (dist < ans) ans = dist;
					}
					else if constexpr (type == ClusterDistType::COMPLETELINK) {
						if (dist > ans) ans = dist;
					}
					else if constexpr (type == ClusterDistType::UPGMA) {
						ans += dist;
					}
					else if constexpr (type == ClusterDistType::WPGMA) {
						ans += w->weight<T>(dist) * dist;
					}
				}
			}
		}
		if constexpr (type == ClusterDistType::UPGMA) {
			ans /= (m * n);
		}
		else if constexpr (type == ClusterDistType::WPGMA) {
			ans /= (m * n);
		}
	}

		template<class T, DistType dist_type, int side = HOST>
		void distanceMatrix(
			const Array<T*, side>& v1, // row
			const Array<T*, side>& v2, // column
			int n,
			int m,
			int d,
			Array<T, side>& mat,
			int p = 2
		) {
#ifdef RESEARCHM_ENABLE_CUDA
			T** v1_ptr = thrust::raw_pointer_cast(&v1[0]);
			T** v2_ptr = thrust::raw_pointer_cast(&v2[0]);
#else
			T** v1_ptr = &v1[0];
			T** v2_ptr = &v2[0];
#endif
			zq::utils::Calc_Each(
				[v1_ptr, v2_ptr]
#ifdef RESEARCHM_ENABLE_CUDA
			__device__ __host__
#endif
				(const int idx) ->T {
					return distance<T, dist_type>(v1_ptr[idx / n], v2_ptr[idx % m],d,p);
				}
					, mat
			);
		}

		template<class T, int d,DistType dist_type, int side = HOST>
		void distanceMatrix(
			const Array<Vec<T,d>, side>& v1, // row
			const Array<Vec<T,d>, side>& v2, // column
			int n,
			int m,
			Array<T, side>& mat,
			int p = 2
		) {
#ifdef RESEARCHM_ENABLE_CUDA
			Vec<T, d>* v1_ptr = thrust::raw_pointer_cast(&v1[0]);
			Vec<T, d>* v2_ptr = thrust::raw_pointer_cast(&v2[0]);
#else
			Vec<T, d>* v1_ptr = &v1[0];
			Vec<T, d>* v2_ptr = &v2[0];
#endif
			zq::utils::Calc_Each(
				[v1_ptr, v2_ptr]
#ifdef RESEARCHM_ENABLE_CUDA
			__device__ __host__
#endif
				(const int idx)->T {
					return distance<T,d, dist_type>(v1_ptr[idx / n], v2_ptr[idx % m], p);
			}
			, mat
			);
		}

		template<class T,int side>
		T distanceStrategy(Array<T, side>& mat, ClusterDistType type, Weight<WeightType::CLUSTER>* w = nullptr) {
			T val;
			if constexpr (type == ClusterDistType::SINGLELINK) {
				if (!zq::utils::Array_Min<T, side>(mat,val)){
					printf("Empty array cannot call distance Strategy");
					exit(1);
				}
			}
			else if constexpr (type == ClusterDistType::COMPLETELINK) {
				if (!zq::utils::Array_Max<T, side>(mat,val)){
					printf("Empty array cannot call distance Strategy");
					exit(1);
				}
			}
			else if constexpr (type == ClusterDistType:::UPGMA) {
				if (!zq::utils::Array_Sum<T, side>(mat, val)) {
					printf("Empty array cannot call distance Strategy");
					exit(1);
				}
				val /= 2;
			}
			else if constexpr (type == ClusterDistType::WPGMA) {
				Array<T, side> tem(mat.size());
#ifdef RESEARCHM_ENABLE_CUDA
				T* mat_ptr = thrust::raw_pointer_cast(&mat[0]);
#else
				T* mat_ptr = &mat[0];
#endif
				zq::utils::Calc_Each(
					[mat_ptr,w]
#ifdef RESEARCHM_ENABLE_CUDA
				__device__ __host__
#endif
					(const int idx)->T {
						return mat_ptr[idx]*w->weight(mat_ptr[idx]);
					},
					tem;
				);
				if (!zq::utils::Array_Add<T, side>(tem, val)) {
					printf("Empty array cannot call distance Strategy");
					exit(1);
				}
				val /= 2;
			}
			return val;
		}

		template<class T, ClusterDistType type, DistType dist_type, int side=HOST>
		T clusterDistance(
			const Array<T*,side>& v1,
			const Array<T*,side>& v2,
			int n,
			int m,
			int d,
			int p = 2,
			Weight<WeightType::CLUSTER>* w = nullptr
		) {
			Array<T, side> mat(m * n);
			distanceMatrix<T,dist_type, side>(
				v1, // row
				v2, // column
				n,
				m,
				d,
				mat,
				p
			);
			return distanceStrategy<T,side>(mat,type);
		}

		template<class T,int d, ClusterDistType type, DistType dist_type, int side = HOST>
		T clusterDistance(
			const Array<Vec<T,d>, side>& v1,
			const Array<Vec<T, d>, side>& v2,
			int n,
			int m,
			int p = 2,
			Weight<WeightType::CLUSTER>* w = nullptr
		) {
			Array<T, side> mat(m * n);
			distanceMatrix<T,d,dist_type,side>(
				v1, // row
				v2, // column
				n,
				m,
				mat,
				p
			);
			return distanceStrategy<T, side>(mat, type);
		}
	}
}

#endif	//	__MATH_DISTANCEs_H__