/***********************************************************/
/**	\file
	\brief		Dense Solver
	\details	dense Solver 
	\author		Zhiqi Li, based on yzLib of Dr. Yizhong Zhang
	\date		12/24/2022
*/
/***********************************************************/
#ifndef __MATH_DENSE_SOLVER_H__
#define __MATH_DENSE_SOLVER_H__

#include <assert.h>
#include <iostream>
#include <vector>
#include <math.h>
#include<zqBasicMath/math_type_promote.h>
#include<zqBasicMath/math_const.h>
#include<zqBasicMath/math_dense.h>


namespace zq{

//	========================================
///@{
/**	@name Non-menber Vector Functions
*/
//	========================================
template<class T>
inline int Cols(const DenseMatrix<T>& den_matrix) {
	if (den_matrix.Dim() == 0) return 0;
	return den_matrix[0].Dim();
}
template<class T>
inline int Rows(const DenseMatrix<T>& den_matrix) {
	return den_matrix.Dim();
}
template<class T>
inline DenseMatrix<T> Transpose(const DenseMatrix<T>& den_matrix) {
	DenseMatrix<T> trans_den_matrix(Cols(den_matrix), Rows(den_matrix));
	for (int i = 0; i < Rows(den_matrix); i++) {
		for (int j = 0; j < Cols(den_matrix); j++) {
			trans_den_matrix[j][i] = den_matrix[i][j];
		}
	}
	return trans_den_matrix;
}
template<class T>
inline DenseMatrix<T> Transpose(const DenseVector<T>& den_vec) {
	DenseMatrix<T> trans_den_matrix(den_vec.Dim(),1);
	for (int i = 0; i < den_vec.Dim(); i++) {
			trans_den_matrix[i][0] = den_vec[i];
	}
	return trans_den_matrix;
}
template<class T>
inline void SetTranspose(DenseMatrix<T>& den_matrix) {
	DenseMatrix<T> trans_den_matrix(Cols(den_matrix), Rows(den_matrix));
	for (int i = 0; i < Rows(den_matrix); i++) {
		for (int j = 0; j < Cols(den_matrix); j++) {
			trans_den_matrix[j][i] = den_matrix[i][j];
		}
	}
	den_matrix = trans_den_matrix;
}
template<class T>
inline DenseMatrix<T> Matrix(const DenseVector<T>& den_vec) {
	DenseMatrix<T> trans_den_matrix(1, den_vec.Dim());
	for (int i = 0; i < den_vec.Dim(); i++) {
		trans_den_matrix[0][i] = den_vec[i];
	}
	return trans_den_matrix;
}
template<class T>
inline DenseMatrix<T> ColMatrix(const DenseVector<T>& den_vec) {
	DenseMatrix<T> trans_den_matrix(den_vec.Dim(), 1);
	for (int i = 0; i < den_vec.Dim(); i++) {
		trans_den_matrix[i][0] = den_vec[i];
	}
	return trans_den_matrix;
}
template<class T>
inline DenseVector<T> Vector(const DenseMatrix<T>& den_mat) {
	if (Cols(den_mat) == 1) {
		DenseVector<T> den_vec(Rows(den_mat));
		for (int i = 0; i < den_vec.Dim(); i++) {
			den_vec[i]=trans_den_matrix[i][0];
		}
		return den_vec;
	}
	else if(Rows(den_mat) == 1){
		DenseVector<T> den_vec(Cols(den_mat));
		for (int i = 0; i < den_vec.Dim(); i++) {
			den_vec[i] = trans_den_matrix[0][i];
		}
		return den_vec;
	}
	else {
		throw "matrix with both more that one cols and more than one rows can not be convert to vector";
		return DenseVector<T>(0);
	}
}

template<class T1, class T2>
inline DenseVector<T1>& operator *= (DenseVector<T1>& m1, const T2& val) {
	for (int i = 0; i < m1.Dim(); i++)
		m1.value[i] *= val;
	return m1;
}
template<class T1, class T2>
inline DenseVector<T1> operator * (const DenseVector<T1>& m1, const T2& val) {
	DenseVector<T1> result(m1);
	result *= val;
	return result;
}

template<class T1, class T2>
inline DenseVector<T1> operator * (const T2& val,const DenseVector<T1>& m1 ) {
	DenseVector<T1> result(m1);
	result *= val;
	return result;
}

template<class T1, class T2>
inline DenseMatrix<PROMOTE_T1_T2> operator *(const DenseMatrix<T1>& m1, const DenseMatrix<T2>& m2) {
	if (Cols(m1) != Rows(m1)) {
		throw "error:dot, dimension of two dense vectors doesn't match";
	}
	DenseMatrix<PROMOTE_T1_T2> result_den_matrix(Rows(m1), Cols(m2));
	for (int i = 0; i < Rows(result_den_matrix); i++) {
		for (int j = 0; j < Cols(result_den_matrix); j++) {
			result_den_matrix[i][j] = (PROMOTE_T1_T2)0;
			for (int k = 0; k < Cols(m1); k++) {
				result_den_matrix[i][j] += m1[i][k] * m2[k][j];
			}
		}
	}
	return result_den_matrix;
}
template<class T1, class T2>
inline DenseMatrix<PROMOTE_T1_T2> operator *(const DenseMatrix<T1>& m1, const DenseVector<T2>& v2) {
	return m1 * ColMatrix(v2);
}

template<class T1, class T2>
inline DenseVector<PROMOTE_T1_T2> operator *(const DenseVector<T1>& v2,const DenseMatrix<T2>& m1) {
	return   Vector(Matrix(v2)* m1);
}


template<class T1, class T2> 
inline DenseMatrix<T1>& operator *= (DenseMatrix<T1>& m1, const DenseMatrix<T2>& m2) {
	m1 = m1 * m2;
	return m1;
}

template<class T1, class T2>
inline DenseMatrix<T1>& operator *= (DenseMatrix<T1>& m1, const DenseVector<T2>& v2) {
	m1 = m1 * v2;
	return m1;
}
template<class T1, class T2>
inline DenseVector<T1>& operator *= (const DenseVector<T1>& v2, const DenseMatrix<T2>& m1) {
	v2 = v2 * m1;
	return v2;
}

template<class T1, class T2> 
inline PROMOTE_T1_T2 dot(const DenseVector<T1>& den_vec1, const DenseVector<T2>& den_vec2){
	if( den_vec1.Dim() != den_vec2.Dim() ){
		std::cout << "error:dot, dimension of two dense vectors doesn't match" << std::endl;
		exit(0);
	}
	PROMOTE_T1_T2	sum = 0;
	for(int i=0; i<den_vec1.Dim(); i++)
		sum += den_vec1[i] * den_vec2[i];

	return sum;
}
template<class T>
inline void MatrixSwapCol (DenseMatrix<T>& m,int i,int j) {
	if (i >= Cols(m) || j >= Cols(m)) {
		throw "index out of densematrix range";
	}
	for (int k = 0; k < Rows(m); k++) {
		mySwap(m[k][i], m[k][j]);
	}
}
template<class T>
inline void MatrixSwapRow(DenseMatrix<T>& m, int i, int j) {
	if (i >= Rows(m) || j >= Rows(m)) {
		throw "index out of densematrix range";
	}
	for (int k = 0; k < Cols(m); k++) {
		mySwap(m[i][k], m[j][k]);
	}
}
template<class T>
inline DenseVector<T> MatrixGetCol(const DenseMatrix<T>& m, int i) {
	if (i >= Cols(m)) {
		throw "index out of densematrix range";
	}
	DenseVector<T> result(Rows(m));
	for (int j = 0; j < Rows(m); j++) {
		result[j] = m[j][i];
	}
	return result;
}
template<class T>
inline DenseVector<T> MatrixGetRow(const DenseMatrix<T>& m, int i) {
	if (i >= Rows(m)) {
		throw "index out of densematrix range";
	}
	return m[i];
}
template<class T>
inline void MatrixSetCol(DenseMatrix<T>& m, int i, DenseVector<T> v) {
	if (i >= Cols(m)) {
		throw "index out of densematrix range";
	}
	if (v.Dim() != Rows(m)) {
		throw "mismatch of the dimension";
	}
	for (int j = 0; j < Rows(m); j++) {
		m[j][i]=v[j];
	}
}
template<class T>
inline void MatrixSetRow(DenseMatrix<T>& m, int i,DenseVector<T> v) {
	if (i >= Rows(m)) {
		throw "index out of densematrix range";
	}
	if (v.Dim() != Cols(m)) {
		throw "mismatch of the dimension";
	}
	m[i]=v;
}


///@}

}	

#endif	