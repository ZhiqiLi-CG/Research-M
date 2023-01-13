
/***********************************************************/
/**	\file
	\brief		Array func of HOST or DEVICE
	\author		Zhiqi Li
	\date		5/26/2022
*/
/***********************************************************/
#ifndef __UTILS_ARRAY_FUNC_BASIC_H__
#define __UTILS_ARRAY_FUNC_BASIC_H__

#include<ResearchM_config.h>
#ifdef RESEARCHM_ENABLE_CUDA
	#include<zqBasicUtils/utils_cuda.h>
#endif // RESEARCHM_ENABLE_CUDA
#include<zqBasicUtils/utils_array.h>
#include<zqBasicUtils/utils_aux_func.h>
#include <iostream>
namespace zq {
	namespace utils {
		//check if an Array contains invalid values, like nan, inf
		template<class T, int side = HOST>
		bool Numerical_Check(const Array<T, side>& arr, const std::string& name = "", bool crash_on_fail = true) {
			int invalid_cnt = 0;
			Array<T, HOST> tem_arr = arr;
			for (int i = 0; i < tem_arr.size(); i++) {
				//std::cout << "i: " << i << std::endl;
				//std::cout << "thing: " << arr[i] << std::endl;
				if (!Is_Valid_Number(tem_arr[i])) {
					Info("ArrayFunc::Numerical_Check fails for {} at index {}: {}", name, i, tem_arr[i]);
					invalid_cnt++;
					if (invalid_cnt >= 10) {
						Info(".....");
						break;
					}
				}
			}
			if (invalid_cnt) {
				if (crash_on_fail) exit(1);
				else return false;
			}
			return true;
		}

		//check deviation
		template<int side = HOST>
		real Max_Abs_Error(const real x0, const Array<real, side>& arr) {
			real err = 0;
			Array<real, HOST> tem_arr = arr;
			for (const real& a : tem_arr) {
				err = std::max(err, abs(x0 - a));
			}
			return err;
		}
		template<int side = HOST>
		real Upper_Rel_Error(const real x0, const Array<real, side>& arr) {//only count for arr[i]>x0. ignore all arr[i]<x0. 
			real err = 0;
			Array<real, HOST> tem_arr = arr;
			for (const real& a : tem_arr) {
				err = std::max(err, a - x0);
			}
			return err / x0;
		}
		template<int side = HOST>
		real Max_Rel_Error(const real x0, const Array<real, side>& arr) {
			Assert(x0 > 0, "MAx_Rel_Error: must x0>0");
			return abs(Max_Abs_Error(x0, arr) / x0);
		}
		template<int side = HOST>
		real L2_Abs_Error(const real x0, const Array<real, side>& arr) {
			real sum = 0;
			Array<real, HOST> tem_arr = arr;
			for (auto a : tem_arr) {
				sum += (x0 - a) * (x0 - a);
			}
			return sqrt(sum) / tem_arr.size();
		}
		template<int side = HOST>
		real L2_Rel_Error(const real x0, const Array<real, side>& arr) {
			Assert(x0 > 0, "L2_Rel_Error: must x0>0");
			return abs(L2_Abs_Error(x0, arr) / x0);
		}

		//Sequential operations
		template<class T, int side = HOST> 
		void Resize_To(Array<T, side>& a, const int n, const T val) {
			a.resize(n);
			#ifdef RESEARCHM_ENABLE_CUDA
			thrust::fill(a.begin(), a.end(), val);
			#else
				std::fill(a.begin(), a.end(), val);
			#endif

		}
		//Parallel Array Operations
		template<class T, int side = HOST> 
		void Array_Add(Array<T,side>& a, const Array<T, side>& b, const real c = 1) {//a+=b*c		
			Assert(a.size() == b.size(), "ArrayFunc::Array_Sum: size unmatch");
			if constexpr (side == HOST) {
				#pragma omp parallel for
				for (int i = 0; i < a.size(); i++) {
					a[i] += b[i] * c;
				}
			}
			#ifdef RESEARCHM_ENABLE_CUDA
				else if constexpr(side == DEVICE) {
					real* b_ptr = thrust::raw_pointer_cast(&b[0]);
					real* a_ptr = thrust::raw_pointer_cast(&a[0]);
					Calc_Each(
						[a_ptr,b_ptr, c] __device__ __host__(const int idx)->real {
							return a_ptr[idx]+b_ptr[idx] * c;
						},
						a
					);
				}
			#endif
		}

		template<class T, int side = HOST> void Copy(Array<T,side>& a, const Array<T, side>& b) {
			Assert(a.size() == b.size(), "ArrayFunc::Copy: size unmatch");
			int n = a.size();
			if constexpr (side == HOST) {
				#pragma omp parallel for
				for (int i = 0; i < n; i++) {
					a[i] = b[i];
				}
			}
			#ifdef RESEARCHM_ENABLE_CUDA
				else if constexpr (side == DEVICE) {
					thrust::copy(b.begin(), b.end(),a );
				}
			#endif
		}
		template<class Fvoid, class T, int side = HOST> void Exec_Each(Fvoid f, Array<T,side>& a) {
			int N = a.size();
			if constexpr (side == HOST) {
				#pragma omp parallel for
				for (int i = 0; i < N; i++) f(i);
			}
			#ifdef RESEARCHM_ENABLE_CUDA
				else if constexpr (side == DEVICE) {
					thrust::for_each(
						counting(0),
						counting(0) + N,
						f
					);
				}
			#endif
		}
		// For side == DEVICE, f must be have pointer reference
		template<class F1int, int side = HOST> void Calc_Each(F1int f, Array<decltype(f(0)), side>& arr) {
			int N = arr.size();
			arr.resize(N);
			if constexpr (side == HOST) {
				#pragma omp parallel for
				for (int i = 0; i < N; i++) arr[i] = f(i);
			}
			#ifdef RESEARCHM_ENABLE_CUDA
				else if constexpr  (side == DEVICE) {
					thrust::transform(
						counting(0),
						counting(0)+N,
						arr.begin(),
						f
					);
				}
			#endif
		}
	}
}

#endif	