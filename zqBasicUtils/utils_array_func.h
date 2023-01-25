
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
		template<class Fvoid, class T, int side = HOST> void Exec_Each(Fvoid f, Array<T, side>& a) {
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
			else if constexpr (side == DEVICE) {
				thrust::transform(
					counting(0),
					counting(0) + N,
					arr.begin(),
					f
				);
			}
#endif
		}

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
			const T* b_ptr = get_ptr<T,side>(b);
			T* a_ptr = get_ptr<T, side>(a);
			auto add_f = [a_ptr, b_ptr, c]
#ifdef RESEARCHM_ENABLE_CUDA
				__device__ __host__
#endif
				(const int idx)->T {
				return a_ptr[idx] + b_ptr[idx] * c;
			};
			zq::utils::Calc_Each<decltype(add_f), side>(
				add_f,
				a
				);
		}
		
		// Parallel Max Operations
		//https://mangoroom.cn/parallel-programming/learn-openmp-reduction.html
		template<class T, int side = HOST>
		int Array_Max(Array<T, side>& a, T& val) {//max(a)		
			if (a.size()==0) return 0;
			if constexpr (side == HOST) {
				val = a[0];
#pragma omp parallel for reduction(max:val)
				for (int i = 1; i < a.size(); ++i)
				{
					val = myMax(val, a[i]);
				}
			}
#ifdef RESEARCHM_ENABLE_CUDA
			else if constexpr (side == DEVICE) {
				//https ://blog.csdn.net/andeyeluguo/article/details/80093801
				thrust::device_ptr<T> d_ptr = thrust::device_pointer_cast(a);
				val = *(thrust::max_element(d_ptr, d_ptr + length));
			}
#endif
			return 1;
		}

		// Parallel Min Operations
		template<class T, int side = HOST>
		int Array_Min(Array<T, side>& a, T& val) {//a+=b*c		
			if (a.size() == 0) return 0;
			if constexpr (side == HOST) {
				val = a[0];
#pragma omp parallel for reduction(max:val)
				for (int i = 1; i < a.size(); ++i)
				{
					val = myMin(val, a[i]);
				}
			}
#ifdef RESEARCHM_ENABLE_CUDA
			else if constexpr (side == DEVICE) {
				//https ://blog.csdn.net/andeyeluguo/article/details/80093801
				thrust::device_ptr<T> a_ptr = thrust::device_pointer_cast(a);
				val = *(thrust::min_element(a_ptr, a_ptr +a.size()));
			}
#endif
			return 1;
		}

		// Parallel Sum Operations
		template<class T, int side = HOST>
		int Array_Sum(Array<T, side>& a, T& val) {//a+=b*c		
			if (a.size() == 0) return 0;
			if constexpr (side == HOST) {
				val = a[0];
#pragma omp parallel for reduction(+:val)
				for (int i = 1; i < a.size(); ++i)
				{
					val += a[i];
				}
			}
#ifdef RESEARCHM_ENABLE_CUDA
			else if constexpr (side == DEVICE) {
				//https ://blog.csdn.net/andeyeluguo/article/details/80093801
				val=thrust::reduce(a.begin, a.end(), (T)0, thrust::plus <T>());
			}
#endif
			return 1;
		}

		// Parallel Cond Sum Operations
		template<class T,class FT,int side = HOST>
		int Array_Count_If(Array<T, side>& a, int& val, const FT& cond) {//a+=b*c		
			if (a.size() == 0) return 0;
			if constexpr (side == HOST) {
				val = (T)0;
#pragma omp parallel for reduction(+:val)z
				for (int i = 0; i < a.size(); ++i)
				{
						val += FT(a[i])?1:0;
				}
			}
#ifdef RESEARCHM_ENABLE_CUDA
			else if constexpr (side == DEVICE) {
				//https ://blog.csdn.net/andeyeluguo/article/details/80093801
				val = thrust::count_if(a.begin, a.end(), cound);
			}
#endif
			return 1;
		}

		// Parallel Abs Operations
		template<class T, int side = HOST>
		int Array_Abs(const Array<T, side>& a, Array<T, side>& res) {//a+=b*c		
			if (a.size() == 0) return 0;
			res.resize(a.size());
			if constexpr (side == HOST) {
				#pragma omp parallel for
				for (int i = 0; i < res.size(); ++i)
				{
					res[i]  = abs(a[i]);
				}
			}
#ifdef RESEARCHM_ENABLE_CUDA
			else if constexpr (side == DEVICE) {
				T* a_ptr = thrust::raw_pointer_cast(&a[0]);
				Calc_Each(
					[a_ptr]__device__ __host__ (const int i) ->T {
					return abs(a_ptr[i]);
					}
					, res
				);
			}
#endif
			return 1;
		}

		// Parallel Abs Sum Operations
		template<class T, int side = HOST>
		int Array_Abs_Sum(Array<T, side>& a, T& val) {//a+=b*c		
			if (a.size() == 0) return 0;
			if constexpr (side == HOST) {
				val = a[0];
#pragma omp parallel for reduction(+:val)
				for (int i = 1; i < a.size(); ++i)
				{
					val += abs(a[i]);
				}
			}
#ifdef RESEARCHM_ENABLE_CUDA
			else if constexpr (side == DEVICE){
				Array<T, side> tem(a.size());
				Array_Abs<T, side>(a,tem);
				val = thrust::reduce(tem.begin, tem.end(), (T)0, thrust::plus <T>());
			}
#endif
			return 1;
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


	}
}

#endif	