/***********************************************************/
/**	\file
	\brief		regression 
	\author		Zhiqi Li
	\date		1/1/2023
*/
/***********************************************************/
#ifndef __MATH_MLS_REGRESSION_H__
#define __MATH_MLS_REGRESSION_H__

#include <vector>
#include <zqBasicMath/math_matrix.h>
#include <zqBasicMath/math_LS_regression.h>

namespace zq {
	//	========================================
	///@{
	/**	@name	MLS
	*/
	//	========================================
	template<typename T>
	#ifdef  RESEARCHM_ENABLE_CUDA
		__host__ __device__
	#endif
	T MLS_Weight(T x0, T y0, T x1, T y1){
			T dis2 = pow(x0 - x1,2) + pow(y0 - y1,2);
		return exp(-dis2 / 2);
	}
	template<typename T>
#ifdef  RESEARCHM_ENABLE_CUDA
		__host__ __device__
#endif
	T MLS_Weight(T x0, T x1) {
		T dis2 = pow(x0 - x1, 2);
		return exp(-dis2 / 2);
	}
	/**
	line fitting from 2D points, the line is y = c0+c1*x+c2*x^2

	the error is calculated in y direction, so k cannot be too big

	\param	c0			return the first coef
	\param	c1			return the second coef
	\param	c2			return the third coef
	\param	xy_ptr		pointer to the array of points, data arranged in xyxy,.... format
	\param	point_num	number of points in the array
	\return				whether regression succeed
	*/
	template<typename T>
	#ifdef  RESEARCHM_ENABLE_CUDA
		__host__ __device__
	#endif
	inline int fitMovingLeastSquare2D(T* coef, T* xy_ptr, int point_num,T* origin) {
		T* mls_xy=new T[4 * point_num];
		for (int i = 0,int j = 0; i < point_num; i+=4,j+=2) {
			T w = MLS_Weight(xy_ptr[j], origin[0]);
			mls_xy[i] = (T)1;
			mls_xy[i + 1] = w * xy_ptr[j];
			mls_xy[i + 2] = w * xy_ptr[j]* xy_ptr[j];
			mls_xy[i + 3] = w * xy_ptr[j+1];
		}
		int res=fitLeastSquare<3>(coef, mls_xy, point_num);
		delete[] mls_xy;
		return res;
	}
	/**
	line fitting from 2D points, the line is y = c0+c1*x+c2*y+c3*x^2+c4*y^2+c5*xy

	the error is calculated in y direction, so k cannot be too big

	\param	coef			return the coef
	\param	xy_ptr		pointer to the array of points, data arranged in xyxy,.... format
	\param	point_num	number of points in the array
	\return				whether regression succeed
	*/
	template<typename T>
#ifdef  RESEARCHM_ENABLE_CUDA
		__host__ __device__
#endif
	inline int fitMovingLeastSquare3D(T* coef, T* xy_ptr, int point_num, T* origin) {
		T* mls_xy= new T[7 * point_num];
		for (int i = 0, j = 0; i < point_num; i += 7, j += 3) {
			T w = MLS_Weight(xy_ptr[j], xy_ptr[j + 1], origin[0], origin[1]);
			mls_xy[i] = (T)1;
			mls_xy[i + 1] = w * xy_ptr[j];
			mls_xy[i + 2] = w * xy_ptr[j + 1];
			mls_xy[i + 3] = w * xy_ptr[j] * xy_ptr[j];
			mls_xy[i + 4] = w * xy_ptr[j + 1] * xy_ptr[j+1];
			mls_xy[i + 5] = w * xy_ptr[j + 1] * xy_ptr[j];
			mls_xy[i + 6] = w * xy_ptr[j+2];
		}
		int res=fitLeastSquare<T,6>(coef, mls_xy, point_num);
		delete[] mls_xy;
		return res;
	}

	///@}
}

#endif	