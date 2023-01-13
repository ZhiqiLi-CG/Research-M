/***********************************************************/
/**	
\file
\brief		Example of cuda math array
\details	This file illustrates how to use math cuda array
\author		Zhiqi Li
\date		12/27/2022
*/
/***********************************************************/
#include<ResearchM_config.h>
#ifdef RESEARCHM_ENABLE_CUDA
	#include <iostream>
	#include<vector>
	#include<zqBasicMath/math_sparse_convert.h>
	#include<zqBasicMath/math_sparse_matrix.h>
	#include<zqBasicMath/math_sparse_vector.h>
	#include<zqBasicMath/math_matrix.h>
	#include<zqBasicMath/math_io.h>
	#include<zqBasicMath/math_LS_regression.h>
	#include<zqBasicMath/math_MLS_regression.h>
	#include<zqBasicUtils/utils_lookup_table.h>
	class A
	{
	public:
		int a = 0;
		int b = 0;
	public:
		__device__ __host__
		inline A(int a, int b) :a(a), b(b) {}

		__device__ __host__
		inline A() {}

		__device__ __host__
		inline void  print() {
			printf("(%d,%d)   ", a, b);
		}
		__device__ __host__
		inline A operator + (const A& vec) const {
			return A(vec.a + a, vec.b + b);
		}
	};
	
	/// This test the use of template in device
	template<class T>
	class B {
	public:
		T x, y;
		using Type=T;
		B(){
		}
		B(T x,T y):x(x),y(y){}
	};
	template<class T>
	void test_template(T t) {
		//T::Type b = 10.9;
		float b = 10;
		printf("*******%f*******", b);
	}
	template <template<class> class H, class S>
	void teamplte_f(const H<S>& value) {
		printf("++++++++%f++++++", value.x);
	}
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

		/// Second test basic function of cuda and thrust,
		auto g_f2 = [] __device__ __host__(const int idx)->int {
			A a1(1,idx);
			A a2(idx,4);
			A a = a1 + a2;
			a.print();
			return idx;
		};
		thrust::device_vector<int> aa(300);
		thrust::transform(
			counting(0),
			counting(0)+10,
			aa.begin(),
			g_f2
		);

		/// test vec2
		thrust::device_vector<zq::Vec2f> vec2f_1(100);
		thrust::device_vector<zq::Vec2f> vec2f_2(100);
		for (int i = 0; i < 100; i++) {
			vec2f_1[i] = zq::Vec2f(0.1+i,i+1);
		}
		for (int i = 0; i < 100; i++) {
			vec2f_2[i] = zq::Vec2f(0.2+i,i+2);
		}

		auto vec2f_ptr1=thrust::raw_pointer_cast(&vec2f_1[0]);
		auto vec2f_ptr2=thrust::raw_pointer_cast(&vec2f_2[0]);
		auto vec2f_phi = [vec2f_ptr1, vec2f_ptr2] __device__ __host__(const int idx)->zq::Vec2f {
			zq::Vec2f test(1.0, 2.2);
			test += zq::Vec2f(0.9, 0.8);
			printf("(%f %f),", test[0], test[1]);
			return (vec2f_ptr1[idx]- vec2f_ptr2[idx]);
		};
		thrust::transform(
			counting(0),
			counting(0) + 10,
			vec2f_1.begin(),
			vec2f_phi
		);
		for (int i = 0; i < 100; i++) {
			zq::Vec2f x=vec2f_1[i];
			std::cout  << x << "\n";
		}
		test_template(B<float>());
		test_template(B<int>());
		teamplte_f<B,float>(B<float>(1.3,1.9));
		return 0;

	}
#endif