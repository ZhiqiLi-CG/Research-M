
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
		//check if an Array contains invalid values, like nan, inf
		template<class T, int side = HOST> 
		bool Numerical_Check(const Array<T,side>& arr, const std::string& name = "", bool crash_on_fail = true) {
			int invalid_cnt = 0;
			for (int i = 0; i < arr.size(); i++) {
				//std::cout << "i: " << i << std::endl;
				//std::cout << "thing: " << arr[i] << std::endl;
				if (!Is_Valid_Number(arr[i])) {
					Info("ArrayFunc::Numerical_Check fails for {} at index {}: {}", name, i, arr[i]);
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
		template<class T, int side = HOST> int Largest_Norm_Element(const Array<T, side>& arr) {//return -1 on empty		
				int idx = -1; real max_norm = -1.0;
				for (int i = 0; i < arr.size(); i++) {
				real v = AuxFunc::Norm<T>(arr[i]);
				if (v >= max_norm) {
					idx = i;
					max_norm = v;
				}
			}
			return idx;
		}
		template<class T, int side = HOST> real Largest_Norm(const Array<T, side>& arr) {
			int idx = Largest_Norm_Element(arr);
			if (idx < 0) return 0;
			else return AuxFunc::Norm<T>(arr[idx]);
		}
		//check deviation
		template<int side = HOST>
		real Max_Abs_Error(const real x0, const Array<real, side>& arr) {
			real err = 0;
			for (const real& a : arr) {
				err = std::max(err, abs(x0 - a));
			}
			return err;
		}
		template<int side = HOST>
		real Upper_Rel_Error(const real x0, const Array<real, side>& arr) {//only count for arr[i]>x0. ignore all arr[i]<x0. 
			real err = 0;
			for (const real& a : arr) {
				err = std::max(err, a - x0);
			}
			return err / x0;
		}
		template<int side = HOST>
		real Max_Rel_Error(const real x0, const Array<real, side>& arr){
			AuxFunc::Assert(x0 > 0, "MAx_Rel_Error: must x0>0");
			return abs(Max_Abs_Error(x0, arr) / x0);
		}
		template<int side = HOST>
		real L2_Abs_Error(const real x0, const Array<real, side>& arr) {
			real sum = 0;
			for (auto a : arr) {
				sum += (x0 - a) * (x0 - a);
			}
			return sqrt(sum) / arr.size();
		}
		template<int side = HOST>
		real L2_Rel_Error(const real x0, const Array<real,side>& arr) {
			AuxFunc::Assert(x0 > 0, "L2_Rel_Error: must x0>0");
			return abs(L2_Abs_Error(x0, arr) / x0);
		}

		//Sequential operations
		template<class T, int side = HOST> void Resize_To(Array<T>& a, const int n, const T val) {
			a.resize(n);
			#ifdef RESEARCHM_ENABLE_CUDA
				thrust::fill(a.begin(), a.end(), val)
			#else
				std::fill(a.begin(), a.end(), val);
			#endif

		}

		//Parallel Array Operations
		template<class T, int side = HOST> void Array_Add(Array<T>& a, const Array<T>& b, const real c = 1) {//a+=b*c		
			AuxFunc::Assert(a.size() == b.size(), "ArrayFunc::Array_Sum: size unmatch");
			#pragma omp parallel for
			for (int i = 0; i < a.size(); i++) {
				a[i] += b[i] * c;
			}
		}
		template<class T, int side = HOST> void Copy(Array<T>& a, const Array<T>& b) {
			AuxFunc::Assert(a.size() == b.size(), "ArrayFunc::Copy: size unmatch");
			int n = a.size();
			#pragma omp parallel for
			for (int i = 0; i < n; i++) {
				a[i] = b[i];
			}
		}
		template<class Fvoid, class T,int side=HOST> void Exec_Each(Fvoid f, Array<T>& a) {
			int N = a.size();
			if constexpr (side == HOST) {
				#pragma omp parallel for
				for (int i = 0; i < N; i++) f(i);
			}
			else constexpr if (side == DEVICE) {

			}
		}
		template<class F1int,int side=HOST> void Calc_Each(F1int f, Array<decltype(f(0)), side>& arr) {
			int N = arr.size();
			arr.resize(N);
			if constexpr (side == HOST) {
				#pragma omp parallel for
				for (int i = 0; i < N; i++) arr[i] = f(i);
			}
			else constexpr if(side == DEVICE) {

			}
		}
}

#endif	