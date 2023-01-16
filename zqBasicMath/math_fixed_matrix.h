/***********************************************************/
/**	\file
	\brief		Matrix Utilities
	\details	functions for general matrix, based on yzLib of Dr. Yizhong Zhang
	\author		Zhiqi Li
	\date		12/20/2022
*/
/***********************************************************/
#ifndef __MATH_FIXED_MATRIX_H__
#define __MATH_FIXED_MATRIX_H__

#include <math.h>
#include<ResearchM_config.h>
#ifdef  RESEARCHM_ENABLE_CUDA
#include <zqBasicUtils/utils_cuda.h>
#endif
#include<zqBasicMath/math_utils.h>
namespace zq{
/**
	Set a matrix to be identity matrix

	\param	mat		pointer to a square matrix
	\param	dim		dimension of the matrix
*/
template<typename T>
inline void matrixSetIdentity(T* mat, int dim){
	for( int j=0; j<dim; j++ )
		for( int i=0; i<dim; i++ )
			mat[j*dim+i] = ( i==j ? 1 : 0 );
}

/**
	Set a matrix to its transpose

	\param	mat		pointer to a square matrix
	\param	dim		dimension of the matrix
*/
template<typename T>
inline void matrixSetTranspose(T* mat, int dim){
	for( int j=0; j<dim; j++ )
		for( int i=j+1; i<dim; i++ )
			zq::mySwap( mat[j*dim+i], mat[i*dim+j] );
}

/**
	Set a non-square matrix to its transpose, m*n to n*m

	\param	mat		pointer to a matrix, there should be no space between each row
	\param	m		rows of the original matrix
	\param	n		columes of the original matrix
*/
template<typename T>
#ifdef  RESEARCHP_ENABLE_CUDA
__host__ __device__
#endif
inline void matrixSetTranspose(T* mat, int m, int n){
	T *buf = new T[m*n];
	memcpy(buf, mat, sizeof(T)*m*n);

	for(int j=0; j<n; j++)
		for(int i=0; i<m; i++)
			mat[j*m+i] = buf[i*n+j];

	delete[] buf;
}

/**
	Inverse a matrix

	\param	mat		pointer to a square matrix
	\param	dim		dimension of the matrix
*/
template<typename T>
#ifdef  RESEARCHM_ENABLE_CUDA
__host__ __device__
#endif
inline int matrixSetInverse(T* mat, int dim){

	T* inv = new T[dim*dim];

	//	save old data
	T* mat_old = new T[dim*dim];
#ifdef  RESEARCHM_ENABLE_CUDA
	for (int i = 0; i < dim * dim; i++) {
		mat_old[i] = mat[i];
	}
#else
	memcpy(mat_old, mat, sizeof(T) * dim * dim);
#endif


	//	set up identity matrix
	for( int j=0; j<dim; j++ )	
		for( int i=0; i<dim; i++ )
			inv[j*dim+i] = (i==j ? 1 : 0);

	//	set the matrix to upper triangle
	for( int j=0; j<dim; j++ ){		//	for each row
		//	search the element with biggest absolute value in colume j under row j
		T max_val_abs = abs(mat[j*dim+j]);
		T max_val = mat[j*dim+j];
		int max_val_j = j;
		for( int i=j+1; i<dim; i++ )
			if( abs(mat[i*dim+j]) > max_val_abs ){
				max_val_abs = abs(mat[i*dim+j]);
				max_val = mat[i*dim+j];
				max_val_j = i;
			}
		//	swap make the row with biggest value on top
		if( max_val_j != j ){		//	need to swap
			for( int i=0; i<dim; i++ ){
				mySwap( mat[j*dim+i], mat[max_val_j*dim+i] );
				mySwap( inv[j*dim+i], inv[max_val_j*dim+i] );
			}
		}
		//	check whether the max_val is 0, if so, inverse doesn't exist
		if( max_val > -0.000001 && max_val < 0.000001 ){
#ifdef  RESEARCHM_ENABLE_CUDA
		for (int kk = 0; kk < dim * dim; kk++) {
			mat[kk] = mat_old[kk];
		}
#else
			memcpy(mat, mat_old, sizeof(T)*dim*dim);
#endif
			delete[] mat_old;
			delete[] inv;
			return 0;
		}
		//	make this row to be 1
		for( int i=0; i<dim; i++ ){
			mat[j*dim+i] /= max_val;
			inv[j*dim+i] /= max_val;
		}
		//	make all row in this col below this row zero
		for( int i=j+1; i<dim; i++ ){
			T coef = -mat[i*dim+j];
			for( int k=0; k<dim; k++ ){
				mat[i*dim+k] += coef * mat[j*dim+k];
				inv[i*dim+k] += coef * inv[j*dim+k];
			}
		}
	}

	//	set the matrix to identity
	for( int j=dim-1; j>=0; j-- ){
		for( int i=j-1; i>=0; i-- ){
			T coef = -mat[i*dim+j];
			for( int k=0; k<dim; k++ ){
				mat[i*dim+k] += coef * mat[j*dim+k];
				inv[i*dim+k] += coef * inv[j*dim+k];
			}
		}
	}

	//	set value
#ifdef  RESEARCHM_ENABLE_CUDA
	for (int i = 0; i < dim * dim; i++) {
		mat[i] = inv[i];
	}
#else
	memcpy(mat, inv, sizeof(T) * dim * dim);
#endif


	delete[] mat_old;
	delete[] inv;
	return 1;
}

/**
	Multiply two matrice
	res = A * B,	res: m*n  A: m*k  B: k*n

	\param	res		result matrix
	\param	A		matrix A
	\param	B		matrix B
	\param	m		rows of matrix A
	\param	k		cols of matrix A, rows of matrix B
	\param	n		cols of matrix B
*/
template<typename T>
#ifdef  RESEARCHM_ENABLE_CUDA
__host__ __device__
#endif
inline void matrixMultiplyMatrix(T* res, T* A, T* B, int m, int k, int n){
	T* mul = new T[m*n];
	for( int j=0; j<m; j++ )
		for( int i=0; i<n; i++ ){
			T sum = 0;
			for( int z=0; z<k; z++ )
				sum += A[j*k+z] * B[z*n+i];
			mul[j*n+i] = sum;
		}
#ifdef  RESEARCHM_ENABLE_CUDA
	for (int i = 0; i < m * n;i++) {
		res[i] = mul[i];
	}
#else
	memcpy(res, mul, sizeof(T) * m * n);
#endif

	delete[] mul;
}

///@}

}	

#endif	//	__YZ_MATRIX_UTILS_H__