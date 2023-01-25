/***********************************************************/
/**	\file
	\brief		Linear square regression and PCA
	\author		Zhiqi Li, based on yzLib of Dr. Yizhong Zhang
	\date		1/1/2023
*/
/***********************************************************/
#ifndef __MATH_LS_REGRESSION_H__
#define __MATH_LS_REGRESSION_H__

#include <vector>
#include <zqBasicMath/math_matrix.h>
#include <zqBasicMath/math_fixed_matrix.h>
#include <zqBasicMath/math_vector_utils.h>

namespace zq {

	//	========================================
	///@{
	/**	@name	Fitting Straight Line 2D
	*/
	//	========================================

	/**
		line fitting from 2D points, the line is y = k x + b

		the error is calculated in y direction, so k cannot be too big

		\param	k			return the slope of the line
		\param	b			return the y-intercept of the line
		\param	xy_ptr		pointer to the array of points, data arranged in xy_xy_xy_ format
		\param	point_num	number of points in the array
		\return				whether regression succeed
	*/
	template<typename T>
	#ifdef  RESEARCHM_ENABLE_CUDA
		__host__ __device__
	#endif
	inline int fitLineLeastSquare(T& k, T& b, const T* xy_ptr, int point_num) {
		if (point_num < 2)
			return 0;

		Matrix2x2<PROMOTE_T_TO_FLOAT>	mat(0, 0, 0, 0);
		Vec2<PROMOTE_T_TO_FLOAT>		rhs(0, 0);

		//	fill the matrix
		for (int i = 0; i < point_num; i++) {
			T x = xy_ptr[i * 2];
			T y = xy_ptr[i * 2 + 1];
			mat.data[0][0] += x * x;
			mat.data[0][1] += x;
			rhs.x += x * y;
			rhs.y += y;
		}
		mat.data[1][0] = mat.data[0][1];
		mat.data[1][1] = point_num;

		Vec2<PROMOTE_T_TO_FLOAT>	res = mat.Inverse() * rhs;
		k = res[0];
		b = res[1];
		return 1;
	}

	/**
		line fitting from 2D points, the line is y = k x + b

		the error is calculated in y direction, so k cannot be too big

		\param	k		return the slope of the line
		\param	b		return the y-intercept of the line
		\param	xy		list of the points
		\return			whether regression succeed
	*/
	template<typename T>
	inline int fitLineLeastSquare(T& k, T& b, const std::vector<Vec2<T>>& xy) {
		if (xy.empty())
			return 0;
		return fitLineLeastSquare(k, b, (T*)&xy[0], xy.size());
	}

	/**
		line fitting from 2D points, the line is x = a y + b

		the error is calculated in x direction, so parameter a cannot be too big

		\param	a			return the inverse of slope of the line
		\param	b			return the x-intercept of the line
		\param	xy_ptr		pointer to the array of points, data arranged in xy_xy_xy_ format
		\param	point_num	number of points in the array
		\return				whether regression succeed
	*/
	template<typename T>
	#ifdef  RESEARCHM_ENABLE_CUDA
		__host__ __device__
	#endif
	inline int fitLineLeastSquareX(T& a, T& b, const T* xy_ptr, int point_num) {
		if (point_num < 2)
			return 0;

		Matrix2x2<PROMOTE_T_TO_FLOAT>	mat(0, 0, 0, 0);
		Vec2<PROMOTE_T_TO_FLOAT>		rhs(0, 0);

		//	fill the matrix
		for (int i = 0; i < point_num; i++) {
			T x = xy_ptr[i * 2];
			T y = xy_ptr[i * 2 + 1];
			mat.data[0][0] += y * y;
			mat.data[0][1] += y;
			rhs.x += x * y;
			rhs.y += x;
		}
		mat.data[1][0] = mat.data[0][1];
		mat.data[1][1] = point_num;

		Vec2<PROMOTE_T_TO_FLOAT>	res = mat.Inverse() * rhs;
		a = res[0];
		b = res[1];
		return 1;
	}

	/**
		line fitting from 2D points, the line is x = a y + b

		the error is calculated in x direction, so parameter a cannot be too big

		\param	a		return the inverse of slope of the line
		\param	b		return the x-intercept of the line
		\param	xy		list of the points
		\return			whether regression succeed
	*/
	template<typename T>
	inline int fitLineLeastSquareX(T& a, T& b, const std::vector<Vec2<T>>& xy) {
		if (xy.empty())
			return 0;

		return fitLineLeastSquareX2D(a, b, (T*)&xy[0], xy.size());
	}

	/**
		line fitting from 2D points with PCA

		for PCA, the error the perpendicular distance from each point to the line

		\param	mean_x		return center of the points, x coordinate
		\param	mean_y		return center of the points, y coordinate
		\param	pa_x		return principle axis, x
		\param	pa_y		return principle axis, y
		\param	xy_ptr		pointer to the array of points, data arranged in xy_xy_xy_ format
		\param	point_num	number of points in the array
		\return				whether regression succeed
	*/
	template<typename T>
#ifdef  RESEARCHM_ENABLE_CUDA
	__host__ __device__
#endif
	inline int fitLinePCA(T& mean_x, T& mean_y, T& pa_x, T& pa_y, const T* xy_ptr, int point_num) {
		if (point_num < 2)
			return 0;

		//	calculate mean
		PROMOTE_T_TO_FLOAT cx = 0, cy = 0;
		for (int i = 0; i < point_num; i++) {
			cx += xy_ptr[i * 2];
			cy += xy_ptr[i * 2 + 1];
		}
		cx /= point_num;
		cy /= point_num;

		//	calculate covariance matrix
		Matrix2x2<PROMOTE_T_TO_FLOAT>	cov(0, 0, 0, 0);
		for (int i = 0; i < point_num; i++) {
			PROMOTE_T_TO_FLOAT x_dif = xy_ptr[i * 2] - cx;
			PROMOTE_T_TO_FLOAT y_dif = xy_ptr[i * 2 + 1] - cy;
			cov.data[0][0] += x_dif * x_dif;
			cov.data[0][1] += x_dif * y_dif;
			cov.data[1][1] += y_dif * y_dif;
		}
		cov.data[1][0] = cov.data[0][1];
		cov /= point_num - 1;

		//	calculate eigen values
		Com2<PROMOTE_T_TO_FLOAT>	evec1, evec2;
		PROMOTE_T_TO_FLOAT			eval1, eval2;
		int eigens = cov.Eigen(evec1, eval1, evec2, eval2);
		if (!eigens)
			return 0;

		//	write result
		mean_x = cx;
		mean_y = cy;
		pa_x = evec1[0];
		pa_y = evec1[1];

		return 1;
	}

	/**
		line fitting from 2D points with PCA

		for PCA, the error the perpendicular distance from each point to the line

		\param	mean			return center of the points
		\param	principle_axis	return principle axis
		\param	xy				list of the points
		\return					whether regression succeed
	*/
	template<typename T>
	inline int fitLinePCA(Vec2<T>& mean, Vec2<T>& principle_axis, const std::vector<Vec2<T>>& xy) {
		if (xy.empty())
			return 0;

		return fitLinePCA(mean.x, mean.y, principle_axis.x, principle_axis.y, (T*)&xy[0], xy.size());
	}

	/**
		line fitting from weighted 2D points with PCA

		for PCA, the error the perpendicular distance from each point to the line

		\param	mean_x		return center of the points, x coordinate
		\param	mean_y		return center of the points, y coordinate
		\param	pa_x		return principle axis, x
		\param	pa_y		return principle axis, y
		\param	xy_ptr		pointer to the array of points, data arranged in xy_xy_xy_ format
		\param	w_ptr		pointer to the array of weight
		\param	point_num	number of points in the array
		\return				whether regression succeed
	*/
	template<typename T>
#ifdef  RESEARCHM_ENABLE_CUDA
	__host__ __device__
#endif
	inline int fitLineWeightedPCA(T& mean_x, T& mean_y, T& pa_x, T& pa_y, const T* xy_ptr, const T* w_ptr, int point_num) {
		if (point_num < 2)
			return 0;

		//	calculate mean
		PROMOTE_T_TO_FLOAT cx = 0, cy = 0, w_sum = 0;
		for (int i = 0; i < point_num; i++) {
			cx += xy_ptr[i * 2] * w_ptr[i];
			cy += xy_ptr[i * 2 + 1] * w_ptr[i];
			w_sum += w_ptr[i];
		}
		cx /= w_sum;
		cy /= w_sum;

		//	calculate covariance matrix
		Matrix2x2<PROMOTE_T_TO_FLOAT>	cov(0, 0, 0, 0);
		for (int i = 0; i < point_num; i++) {
			PROMOTE_T_TO_FLOAT x_dif = xy_ptr[i * 2] - cx;
			PROMOTE_T_TO_FLOAT y_dif = xy_ptr[i * 2 + 1] - cy;
			cov.data[0][0] += x_dif * x_dif * w_ptr[i];
			cov.data[0][1] += x_dif * y_dif * w_ptr[i];
			cov.data[1][1] += y_dif * y_dif * w_ptr[i];
		}
		cov.data[1][0] = cov.data[0][1];
		cov /= point_num - 1;

		//	calculate eigen values
		Com2<PROMOTE_T_TO_FLOAT>	evec1, evec2;
		PROMOTE_T_TO_FLOAT			eval1, eval2;
		int eigens = cov.Eigen(evec1, eval1, evec2, eval2);
		if (!eigens)
			return 0;

		//	write result
		mean_x = cx;
		mean_y = cy;
		pa_x = evec1[0];
		pa_y = evec1[1];

		return 1;
	}

	/**
		line fitting from weighted 2D points with PCA

		for PCA, the error the perpendicular distance from each point to the line

		\param	mean			return center of the points
		\param	principle_axis	return principle axis
		\param	xy				list of the points
		\param	w				weight of each point
		\return					whether regression succeed
	*/
	template<typename T>
	inline int fitLineWeightedPCA(Vec2<T>& mean, Vec2<T>& principle_axis, const std::vector<Vec2<T>>& xy, const std::vector<T>& w) {
		if (xy.empty() || xy.size() != w.size())
			return 0;

		return fitLineWeightedPCA(mean.x, mean.y, principle_axis.x, principle_axis.y, (T*)&xy[0], (T*)&w[0], xy.size());
	}

	///@}

	//	========================================
	///@{
	/**	@name	Fitting Plane in 3D
	*/
	//	========================================
	/**
	fit a plane with 3d points, the plane is z = Ax + By + C

	the fitting is better if the plane is perpendicular to z axis

	\param	A			return coef A
	\param	B			return coef B
	\param	C			return coef C
	\param	xyz_ptr		pointer to 3d point cloud
	\param	point_num	how many points
	\return				whether fit succeed
*/
	template<typename T>
#ifdef  RESEARCHM_ENABLE_CUDA
	__host__ __device__
#endif
	inline int fitPlaneLeastSquare(T& A, T& B, T& C, const T* xyz_ptr, int point_num) {
		if (point_num < 3)
			return 0;

		Matrix3x3<PROMOTE_T_TO_FLOAT>	mat(0, 0, 0, 0, 0, 0, 0, 0, 0);
		Vec3<PROMOTE_T_TO_FLOAT>		rhs(0, 0, 0);

		//	fill the matrix
		for (int i = 0; i < point_num; i++) {
			T x = xyz_ptr[i * 3];
			T y = xyz_ptr[i * 3 + 1];
			T z = xyz_ptr[i * 3 + 2];
			mat.data[0][0] += x * x;
			mat.data[0][1] += x * y;
			mat.data[0][2] += x;
			mat.data[1][1] += y * y;
			mat.data[1][2] += y;
			rhs.x += x * z;
			rhs.y += y * z;
			rhs.z += z;
		}
		mat.data[1][0] = mat.data[0][1];
		mat.data[2][0] = mat.data[0][2];
		mat.data[2][1] = mat.data[1][2];
		mat.data[2][2] = point_num;

		Vec3<PROMOTE_T_TO_FLOAT>	res = mat.Inverse() * rhs;
		A = res[0];
		B = res[1];
		C = res[2];
		return 1;
	}

	/**
		fit a plane with 3d points, the plane is z = Ax + By + C

		the fitting is better if the plane is perpendicular to z axis

		\param	A		return coef A
		\param	B		return coef B
		\param	C		return coef C
		\param	xyz		list of the point
		\return			whether fit succeed
	*/
	template<typename T>
	inline int fitPlaneLeastSquare(T& A, T& B, T& C, const std::vector<Vec3<T>>& xyz) {
		if (xyz.empty())
			return 0;

		return fitPlaneLeastSquare(A, B, C, (T*)&xyz[0], xyz.size());
	}

	/**
		fit a plane with 3d points which minimize the distance in normal direction

		\param	center_x	return center coordinate of the plane
		\param	center_y	return center coordinate of the plane
		\param	center_z	return center coordinate of the plane
		\param	normal_x	i/o, given the approximate of the plane, return the calculated normal of the plane
		\param	normal_y	i/o, given the approximate of the plane, return the calculated normal of the plane
		\param	normal_z	i/o, given the approximate of the plane, return the calculated normal of the plane
		\param	xyz_ptr		pointer to 3d point cloud
		\param	point_num	how many points
		\return				whether fit succeed
	*/
	template<typename T>
#ifdef  RESEARCHM_ENABLE_CUDA
	__host__ __device__
#endif
	inline int fitPlaneLeastSquareNormalDirection(T& center_x, T& center_y, T& center_z,
		T& normal_x, T& normal_y, T& normal_z,
		const T* xyz_ptr, int point_num) {
		if (point_num < 3)
			return 0;

		//	calculate rotation matrix
		Matrix3x3<PROMOTE_T_TO_FLOAT>	rot;
		Vec3<PROMOTE_T_TO_FLOAT>		normal(normal_x, normal_y, normal_z);
		Vec3<PROMOTE_T_TO_FLOAT>		old_normal(normal);
		Vec3<PROMOTE_T_TO_FLOAT>		z(0, 0, 1);
		if (normal.Length() < 1e-4)		//	if invalid guess normal is given, we just treate it to be +z
			normal = zq::Vec3d(0, 0, 1);
		else if (normal.z < 0)
			normal *= -1;
		if (normal.z > 0.995)	//	very close to z direction, don't need to rotate
			rot.SetIdentity();
		else					//	need to rotate in order to improve fitting result
			rot.SetRotationRad(zq::cross(normal, z), zq::math::angleRadBetweenVectors(normal, z));

		Matrix3x3<PROMOTE_T_TO_FLOAT>	mat(0, 0, 0, 0, 0, 0, 0, 0, 0);
		Vec3<PROMOTE_T_TO_FLOAT>		rhs(0, 0, 0);
		Vec3<T>							center(0, 0, 0);

		//	fill the matrix
		for (int i = 0; i < point_num; i++) {
			Vec3<T> p(xyz_ptr + i * 3);
			p = rot * p;
			mat.data[0][0] += p.x * p.x;
			mat.data[0][1] += p.x * p.y;
			mat.data[0][2] += p.x;
			mat.data[1][1] += p.y * p.y;
			mat.data[1][2] += p.y;
			rhs.x += p.x * p.z;
			rhs.y += p.y * p.z;
			rhs.z += p.z;

			center.x += p.x;
			center.y += p.y;
		}
		mat.data[1][0] = mat.data[0][1];
		mat.data[2][0] = mat.data[0][2];
		mat.data[2][1] = mat.data[1][2];
		mat.data[2][2] = point_num;
		center.z = rhs.z;

		center /= point_num;

		//	calculate plane function
		Vec3<PROMOTE_T_TO_FLOAT>	res = mat.Inverse() * rhs;
		normal.x = -res[0];
		normal.y = -res[1];
		normal.z = 1;
		normal.SetNormalize();

		//	rotate back
		rot.SetInverse();
		center = rot * center;
		normal = rot * normal;
		if (zq::dot(old_normal, normal) < 0)
			normal *= -1;

		center_x = center.x;
		center_y = center.y;
		center_z = center.z;
		normal_x = normal.x;
		normal_y = normal.y;
		normal_z = normal.z;

		return 1;
	}

	/**
		fit a plane with 3d points which minimize the distance in normal direction

		\param	center	return center coordinate of the plane
		\param	normal	i/o, given the approximate of the plane, return the calculated normal of the plane
		\param	xyz		list of the point
		\return			whether fit succeed
	*/
	template<typename T>
	inline int fitPlaneLeastSquareNormalDirection(Vec3<T>& center, Vec3<T>& normal, const std::vector<Vec3<T>>& xyz) {
		if (xyz.empty())
			return 0;

		return fitPlaneLeastSquareNormalDirection(center.x, center.y, center.z,
			normal.x, normal.y, normal.z, (T*)&xyz[0], xyz.size());
	}

	/**
		fit a plane with 3d points which minimize the distance in normal direction

		\param	center_x	return center coordinate of the plane
		\param	center_y	return center coordinate of the plane
		\param	center_z	return center coordinate of the plane
		\param	normal_x	i/o, given the approximate of the plane, return the calculated normal of the plane
		\param	normal_y	i/o, given the approximate of the plane, return the calculated normal of the plane
		\param	normal_z	i/o, given the approximate of the plane, return the calculated normal of the plane
		\param	xyz_ptr		pointer to 3d point cloud
		\param	w_ptr		pointer to weight of each point
		\param	point_num	how many points
		\return				whether fit succeed
	*/
	template<typename T>
#ifdef  RESEARCHM_ENABLE_CUDA
	__host__ __device__
#endif
	inline int fitPlaneWeightedLeastSquareNormalDirection(T& center_x, T& center_y, T& center_z,
		T& normal_x, T& normal_y, T& normal_z,
		const T* xyz_ptr, const T* w_ptr, int point_num) {
		if (point_num < 3)
			return 0;

		//	calculate rotation matrix
		Matrix3x3<PROMOTE_T_TO_FLOAT>	rot;
		Vec3<PROMOTE_T_TO_FLOAT>		normal(normal_x, normal_y, normal_z);
		Vec3<PROMOTE_T_TO_FLOAT>		old_normal(normal);
		Vec3<PROMOTE_T_TO_FLOAT>		z(0, 0, 1);
		if (normal.Length() < 1e-4)		//	if invalid guess normal is given, we just treate it to be +z
			normal = zq::Vec3d(0, 0, 1);
		else if (normal.z < 0)
			normal *= -1;
		if (normal.z > 0.995)	//	very close to z direction, don't need to rotate
			rot.SetIdentity();
		else					//	need to rotate in order to improve fitting result
			rot.SetRotationRad(zq::cross(normal, z), zq::math::angleRadBetweenVectors(normal, z));

		Matrix3x3<PROMOTE_T_TO_FLOAT>	mat(0, 0, 0, 0, 0, 0, 0, 0, 0);
		Vec3<PROMOTE_T_TO_FLOAT>		rhs(0, 0, 0);
		Vec3<T>							center(0, 0, 0);

		//	fill the matrix
		for (int i = 0; i < point_num; i++) {
			Vec3<T> p(xyz_ptr + i * 3);
			T		w = w_ptr[i];
			p = rot * p;
			mat.data[0][0] += p.x * p.x * w;
			mat.data[0][1] += p.x * p.y * w;
			mat.data[0][2] += p.x * w;
			mat.data[1][1] += p.y * p.y * w;
			mat.data[1][2] += p.y * w;
			mat.data[2][2] += w;
			rhs.x += p.x * p.z * w;
			rhs.y += p.y * p.z * w;
			rhs.z += p.z * w;

			center.x += p.x * w;
			center.y += p.y * w;
		}
		mat.data[1][0] = mat.data[0][1];
		mat.data[2][0] = mat.data[0][2];
		mat.data[2][1] = mat.data[1][2];
		center.z = rhs.z;

		center /= mat.data[2][2];

		//	calculate plane function
		Vec3<PROMOTE_T_TO_FLOAT>	res = mat.Inverse() * rhs;
		normal.x = -res[0];
		normal.y = -res[1];
		normal.z = 1;
		normal.SetNormalize();

		//	rotate back
		rot.SetInverse();
		center = rot * center;
		normal = rot * normal;
		if (zq::dot(old_normal, normal) < 0)
			normal *= -1;

		center_x = center.x;
		center_y = center.y;
		center_z = center.z;
		normal_x = normal.x;
		normal_y = normal.y;
		normal_z = normal.z;

		return 1;
	}

	/**
		fit a plane with 3d points which minimize the distance in normal direction

		\param	center	return center coordinate of the plane
		\param	normal	i/o, given the approximate of the plane, return the calculated normal of the plane
		\param	xyz		list of the point
		\param	w		list of weight of each point
		\return			whether fit succeed
	*/
	template<typename T>
	inline int fitPlaneWeightedLeastSquareNormalDirection(Vec3<T>& center,
		Vec3<T>& normal,
		const std::vector<Vec3<T>>& xyz,
		const std::vector<T>& w) {
		if (xyz.empty() || xyz.size() != w.size())
			return 0;

		return fitPlaneWeightedLeastSquareNormalDirection(center.x, center.y, center.z,
			normal.x, normal.y, normal.z, (T*)&xyz[0], (T*)&w[0], xyz.size());
	}

	/**
	line fitting from 2D points, the line is y = k1 x1 + k2 x2,...,

	the error is calculated in y direction, so k cannot be too big

	\param	d			the dim of x
	\param	k			return the slope of the line, which is a pointer to array
	\param	xy_ptr		pointer to the array of points, data arranged in x1x2,...y_x1x2,...y_x1x2,...y_ format
	\param	point_num	number of points in the array
	\return				whether regression succeed
*/
	template<typename T, int d>
#ifdef  RESEARCHM_ENABLE_CUDA
	__host__ __device__
#endif
	inline int fitLeastSquare(T* coef, const T* xy_ptr, int point_num) {
		if (point_num < 2)
			return 0;
		T mat[d][d]={0};
		T rhs[d]={0};
		for (int k = 0; k < point_num; k++) {
			for (int i = 0; i < d; i++) {
				for (int j = 0; j < d; j++) {
					mat[i][j] += xy_ptr[k * (d + 1) + i]* xy_ptr[k * (d + 1) + j];
				}
				rhs[i] += xy_ptr[k * (d + 1) + i] * xy_ptr[k * (d + 1) + d];
			}
		}
		//printf("ppp,%d",d);
		matrixSetInverse(&(mat[0][0]), d);
		matrixMultiplyMatrix(coef, &(mat[0][0]), &(rhs[0]), d, d, 1);
		//for (int i = 0; i < d; i++)
		//	printf("]%f,", coef[i]);
		//printf("\n");
		return 1;
	}

	/**
		line fitting from 2D points, the line is y = k x + b

		the error is calculated in y direction, so k cannot be too big

		\param	k		return the slope of the line
		\param	b		return the y-intercept of the line
		\param	xy		list of the points
		\return			whether regression succeed
	*/
	template<typename T>
	inline int fitLeastSquare(std::vector<T>& k, const std::vector<std::vector<T>>& xy) {
		if (xy.empty())
			return 0;
		return fitLeastSquare(k,(T*)&xy[0], xy.size());
	}

	///@}
}

#endif	