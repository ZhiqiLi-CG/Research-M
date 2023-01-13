/***********************************************************/
/**	\file
	\brief		Dense Vector
	\details	dense vector is used to represent vector of any
				dimension. It can operate with dense matrix or 
				sparse matrix. 
	\author		Zhiqi Li, based on yzLib of Dr. Yizhong Zhang
	\date		12/24/2022
*/
/***********************************************************/
#ifndef __MATH_DENSE_H__
#define __MATH_DENSE_H__

#include <assert.h>
#include <iostream>
#include <vector>
#include <math.h>
#ifdef  RESEARCHM_ENABLE_CUDA
#include <zqBasicUtils/utils_cuda.h>
#include <zqBasicUtils/utils_array.h>
#endif //  RESEARCHM_ENABLE_CUDA
#include<zqBasicMath/math_type_promote.h>
#include<zqBasicMath/math_const.h>


namespace zq{

/**
	Dense vector

	dense vector can represent vector of any dimension using 
	std::vector storing all the data. 
*/
template<class T>
class DenseVector{
public:
	std::vector<T>	value;

	typedef PROMOTE_T_TO_FLOAT ZQ_REAL;

public:
	//	constructors
	DenseVector(){}


	DenseVector(int dim_num){
		value.resize(dim_num);
	}
	DenseVector(int dim_num1, int dim_num2) {
		value.resize(dim_num1);
		for (int i = 0; i < dim_num1; i++) {
			value[i] = T(dim_num2);
		}
	}

	template<typename TYPE> DenseVector(const TYPE* data, int dim_num){
		value.resize(dim_num);
		value.assign(data, data+dim_num);
	}

	DenseVector(const DenseVector<T>& vec){
		if( &vec != &(*this) )
			value.assign(vec.value.begin(), vec.value.end());
	}

	template<typename TYPE> DenseVector(const DenseVector<TYPE>& vec){
		value.resize(vec.Dim());
		value.assign(vec.value.begin(), vec.value.end());
	}

	void push_back(const T& v) {
		value.push_back(v);
	}

	/**
		Reset the vector, clear everything
	*/
	inline void Reset(){
		value.clear();
	}

	/**
		Set dimension of the vector
	*/
	inline void SetDimension(int dim_num){
		assert(value.size() == 0);
		assert(dim_num > 0);
		value.resize(dim_num, 0);
	}

	/**
		Set Zero
	*/
	inline void SetZero(){
		//memset(&value[0], 0, sizeof(T)*value.size());
		value.assign(value.size(), 0);
	}

	/**
		Set all elements to the same value
	*/
	template<typename TYPE> inline void SetValue(TYPE val){
		for( int i=0; i<Dim(); i++ )
			value[i] = val;
	}

	/**
		Copy the value from an array to the vector
	*/
	template<typename TYPE> inline void SetValue(TYPE* data){
		value.assign(data, data + Dim());
	}

	//	operators
	inline T& operator[] (int id){
		assert(id>=0 && id<value.size());
		return value[id];
	}

	inline const T& operator[] (int id) const{
		assert(id>=0 && id<value.size());
		return value[id];
	}

	inline T& operator() (int id){
		assert(id>=0 && id<value.size());
		return value[id];
	}

	inline const T& operator() (int id) const{
		assert(id>=0 && id<value.size());
		return value[id];
	}

	inline DenseVector<T> operator - () const{
		DenseVector<T> result(*this);
		for( int i=0; i<result.Dim(); i++ )
			result.value[i] = -result.value[i];
		return result;
	}
	inline DenseVector<T> operator ~ () const {
		DenseVector<T> result(*this);
		for (int i = 0; i < result.Dim(); i++)
			result.value[i] = ~result.value[i];
		return result;
	}
	inline DenseVector& operator = (const DenseVector<T>& vec){
		if( &vec != &(*this) )
			value.assign(vec.value.begin(), vec.value.end());
		return *this;
	}

	template<typename TYPE> inline DenseVector& operator = (const DenseVector<TYPE>& vec){
		value.resize(vec.Dim());
		value.assign(vec.value.begin(), vec.value.end());
		return *this;
	}

	template<typename TYPE> inline DenseVector& operator += (const DenseVector<TYPE>& vec){
		AssureDimLegal(vec);
		for( int i=0; i<Dim(); i++ )
			value[i] += vec.value[i];
		return *this;
	}

	template<typename TYPE> inline DenseVector& operator -= (const DenseVector<TYPE>& vec){
		AssureDimLegal(vec);
		for( int i=0; i<Dim(); i++ )
			value[i] -= vec.value[i];
		return *this;
	}
	
	/*
	template<typename TYPE> inline DenseVector& operator *= (TYPE val){
		for( int i=0; i<Dim(); i++ )
			value[i] *= val;
		return *this;
	}
	*/
	template<typename TYPE> inline DenseVector& operator /= (TYPE val){
		for( int i=0; i<Dim(); i++ )
			value[i] /= val;
		return *this;
	}
	template<typename TYPE> inline DenseVector& operator ^= (DenseVector<TYPE> val) {
		for (int i = 0; i < Dim(); i++)
			value[i] ^= val[i];
		return *this;
	}
	template<typename TYPE> inline DenseVector& operator |= (DenseVector<TYPE> val) {
		for (int i = 0; i < Dim(); i++)
			value[i] |= val[i];
		return *this;
	}
	template<typename TYPE> inline DenseVector& operator &= (DenseVector<TYPE> val) {
		for (int i = 0; i < Dim(); i++)
			value[i] &= val[i];
		return *this;
	}
	template<typename TYPE> inline DenseVector<TYPE_PROMOTE(T, TYPE)> operator + (const DenseVector<TYPE>& vec) const{
		DenseVector<TYPE_PROMOTE(T, TYPE)> result(*this);
		result += vec;
		return result;
	}

	template<typename TYPE> inline DenseVector<TYPE_PROMOTE(T, TYPE)> operator - (const DenseVector<TYPE>& vec) const{
		DenseVector<TYPE_PROMOTE(T, TYPE)> result(*this);
		result -= vec;
		return result;
	}

	/*	template<typename TYPE> inline DenseVector<TYPE_PROMOTE(T, TYPE)> operator * (TYPE val) const{
		DenseVector<TYPE_PROMOTE(T, TYPE)> result(*this);
		result *= val;
		return result;
	}*/
	
	template<typename TYPE> inline DenseVector<TYPE_PROMOTE(T, TYPE)> operator / (TYPE val) const{
		DenseVector<TYPE_PROMOTE(T, TYPE)> result(*this);
		result /= val;
		return result;
	}

	template<typename TYPE> inline DenseVector<TYPE_PROMOTE(T, TYPE)> operator ^ (DenseVector<TYPE> val) const {
		DenseVector<TYPE_PROMOTE(T, TYPE)> result(*this);
		result ^= val;
		return result;
	}
	template<typename TYPE> inline DenseVector<TYPE_PROMOTE(T, TYPE)> operator | (DenseVector<TYPE> val) const {
		DenseVector<TYPE_PROMOTE(T, TYPE)> result(*this);
		result |= val;
		return result;
	}
	template<typename TYPE> inline DenseVector<TYPE_PROMOTE(T, TYPE)> operator & (DenseVector<TYPE> val) const {
		DenseVector<TYPE_PROMOTE(T, TYPE)> result(*this);
		result &= val;
		return result;
	}

	/**
		Return normalized vector, but don't change old data
	*/
	inline DenseVector<ZQ_REAL> Normalize() const{
		DenseVector<ZQ_REAL> result(*this);
		result.SetNormalize();
		return result;
	}

	/**
		normalize this vector
	*/
	inline DenseVector& SetNormalize(){
		ZQ_REAL length = Length();
		(*this) /= length;
		return *this;
	}

	//	vector properties
	/**
		return the dimension of the vector
	*/
	inline int Dim() const{
		return value.size();
	}

	/**
		return the sum of all elements
	*/
	inline T Sum() const{
		T sum = 0;
		for( int i=0; i<Dim(); i++ )
			sum += value[i];
		return sum;
	}

	inline ZQ_REAL SquareLength() const{
		T sqr_sum = 0;
		for( int i=0; i<Dim(); i++ )
			sqr_sum += value[i] * value[i];
		return sqr_sum;
	}

	inline ZQ_REAL Length() const{
		return sqrt(SquareLength());
	}
protected:
	/**
		assure dimension of the vector to calculate is the same with the current vector

		\param	vec		the vector to check
	*/
	template<typename TYPE> inline void AssureDimLegal(const DenseVector<TYPE>& vec){
		if( Dim() != vec.Dim() ){
			std::cout << "Dimension of vector illegal, vector dimension don't match" << std::endl;
			exit(0);
		}
	}

};
template<class T> inline std::ostream& operator << (std::ostream& stream, const DenseVector<T> rhs) {
	stream << "(";
	for (int i = 0; i < rhs.Dim(); i++) {
		stream << "," << rhs[i];
	}
	stream << ")"<<std::endl;
	return stream;
}
template<typename type>
using DenseMatrix = typename DenseVector<DenseVector<type>>;
}	

#endif	