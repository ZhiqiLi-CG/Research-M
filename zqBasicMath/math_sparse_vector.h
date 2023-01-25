/***********************************************************/
/**	\file
	\brief		Sparse Vector
	\details	
	\author		Zhiqi Li, based on yzLib of Dr.Yizhong Zhang
	\date		12/27/2022
*/
/***********************************************************/
#ifndef __MATH_SPARSE_VECTOR_H__
#define __MATH_SPARSE_VECTOR_H__

#include <assert.h>
#include <iostream>
#include <vector>
#include<functional>
#include<zqBasicMath/math_utils.h>
#include <zqBasicUtils/utils_array.h>
namespace zq{

//	========================================
//	virtual base for sparse vector
//	========================================
template<class T>
class BaseSparseVector{
public:
	int dim_num;

public:
	BaseSparseVector(int dims = 0){
		assert( dims >= 0 );
		dim_num = dims;
	}

	virtual void SetDimension(int dims) = 0;
};


//	========================================
//	sparse vector stored by index, the index is sorted
//	========================================
template<class T>
class SparseVector : public BaseSparseVector<T>{
public:
	using BaseSparseVector<T>::dim_num;
	typedef PROMOTE_T_TO_FLOAT ZQ_REAL;

	std::vector<T>		value;
	std::vector<int>	index;

public:
	/**
		constructor, if dimension set, setup storage
	*/
	SparseVector(int dims = 0) : BaseSparseVector<T>(dims) {
		assert(dims >= 0);
	}
	SparseVector(const SparseVector<T>& vec) {
		if (&vec != &(*this)) {
			value.assign(vec.value.begin(), vec.value.end());
			index.assign(vec.index.begin(), vec.index.end());
			dim_num = vec.dim_num;
		}
			
	}

	template<typename TYPE> SparseVector(const SparseVector<TYPE>& vec) {
		value.resize(vec.value.size());
		index.resize(vec.index.size());
		value.assign(vec.value.begin(), vec.value.end());
		index.assign(vec.index.begin(), vec.index.end());
		dim_num = vec.dim_num;
	}
	/**
		Set dimension number
	*/
	void SetDimension(int dims){
		assert(dims > 0);
		dim_num = dims;
	}

	/**
		Clear the zero value	
	*/
	void ClearZero() {
		std::vector<T>		tem_value;
		std::vector<int>	tem_index;
		for (int i = 0; i < value.size(); i++) {
			if (value[i] != Zero<T>()) {
				tem_value.push_back(value[i]);
				tem_index.push_back(index[i]);
			}
		}
		index = tem_index;
		value = tem_value;
	}

	/**
		Clear value and this is for float, which is limited to float error
	*/
	void ClearValue(
		const std::function<bool(T)>& filter
	) {
		std::vector<T>		tem_value;
		std::vector<int>	tem_index;
		for (int i = 0; i < value.size(); i++) {
			if (!filter(value[i])) {
				tem_value.push_back(value[i]);
				tem_index.push_back(index[i]);
			}
		}
		index = tem_index;
		value = tem_value;
	}
public:
	/// operator
	inline T& operator[] (int id) {
		assert(id >= 0 && id < dim_num);
		int insert_pos=0;
		for (int i = 0; i < index.size(); i++) {
			if (id == index[i]) return value[i];
			else if ((i==0 || index[i - 1] < id) && id<index[i] ) {
				insert_pos = i;
				break;
			}
			else if(i == index.size()-1){
				insert_pos = index.size();
				break;
			}
		}
		// if not exist, insert it
		value.insert(std::next(value.begin(), insert_pos),0);
		index.insert(std::next(index.begin(), insert_pos), id);
		return value[insert_pos];
	}
	inline const T& operator[] (int id) const {
		assert(id >= 0 && id < dim_num);
		for (int i = 0; i < index.size(); i++) {
			if (id == index[i]) return value[i];
		}
		return 0;
	}

	inline T& operator() (int id) {
		assert(id >= 0 && id < dim_num);
		return value[id];
	}

	inline const T& operator() (int id) const {
		assert(id >= 0 && id < dim_num);
		return value[id];
	}

	inline SparseVector<T> operator - () const {
		SparseVector<T> result(*this);
		for (int i = 0; i < result.value.size(); i++)
			result.value[i] = -result.value[i];
		return result;
	}
	inline SparseVector<T> operator ~ () const {
		SparseVector<T> result(*this);
		for (int i = 0; i < result.value.size(); i++)
			result.value[i] = ~result.value[i];
		return result;
	}
	inline SparseVector& operator = (const SparseVector<T>& vec) {
		if (&vec != &(*this)) {
			value.assign(vec.value.begin(), vec.value.end());
			index.assign(vec.index.begin(), vec.index.end());
			dim_num = vec.dim_num;
		}
		return *this;
	}

	template<typename TYPE> inline SparseVector& operator = (const SparseVector<TYPE>& vec) {
		value.resize(vec.value.size());
		index.resize(vec.index.size());
		value.assign(vec.value.begin(), vec.value.end());
		index.assign(vec.index.begin(), vec.index.end());
		dim_num = vec.dim_num;
		return *this;
	}

	template<typename TYPE> inline SparseVector& operator += (const SparseVector<TYPE>& vec) {
		AssureDimLegal(vec);
		*this = *this + vec;
		return *this;
	}

	template<typename TYPE> inline SparseVector& operator -= (const SparseVector<TYPE>& vec) {
		AssureDimLegal(vec);
		*this = *this - vec;
		return *this;
	}
	template<typename TYPE> inline SparseVector& operator *= (TYPE val){
		for( int i=0; i<value.size(); i++ )
			value[i] *= val;
		return *this;
	}
	template<typename TYPE> inline SparseVector& operator /= (TYPE val) {
		for (int i = 0; i < value.size(); i++)
			value[i] /= val;
		ClearZero();
		return *this;
	}
	template<typename TYPE> inline SparseVector& operator ^= (SparseVector<TYPE> val) {
		AssureDimLegal(vec);
		*this = *this ^ vec;
		return *this;
	}
	template<typename TYPE> inline SparseVector& operator |= (SparseVector<TYPE> val) {
		AssureDimLegal(vec);
		*this = *this | vec;
		return *this;
	}
	template<typename TYPE> inline SparseVector& operator &= (SparseVector<TYPE> val) {
		AssureDimLegal(vec);
		*this = *this & vec;
		return *this;
	}
	template<typename TYPE> inline SparseVector<TYPE_PROMOTE(T, TYPE)> operator + (const SparseVector<TYPE>& vec) const {
		std::function<TYPE_PROMOTE(T, TYPE)(const T&, const TYPE&)> op = [&](
			const T& v1, const TYPE& v2
			) {
				return v1 + v2;
		};
		return OperatorWith(vec, op);
	}

	template<typename TYPE> inline SparseVector<TYPE_PROMOTE(T, TYPE)> operator - (const SparseVector<TYPE>& vec) const {
		std::function<TYPE_PROMOTE(T, TYPE)(const T&, const TYPE&)> op = [&](
			const T& v1, const TYPE& v2
			) {
				return v1 - v2;
		};
		return OperatorWith(vec, op);
	}
	template<typename TYPE> inline SparseVector<TYPE_PROMOTE(T, TYPE)> operator * (TYPE val) const{
		SparseVector<TYPE_PROMOTE(T, TYPE)> result(*this);
		result *= val;
		return result;
	}
	template<typename TYPE> inline SparseVector<TYPE_PROMOTE(T, TYPE)> operator / (TYPE val) const {
		SparseVector<TYPE_PROMOTE(T, TYPE)> result(*this);
		result /= val;
		return result;
	}

	template<typename TYPE> inline SparseVector<TYPE_PROMOTE(T, TYPE)> operator ^ (SparseVector<TYPE> val) const {
		std::function<TYPE_PROMOTE(T, TYPE)(const T&, const TYPE&)> op = [&](
			const T& v1, const TYPE& v2
			) {
				return v1 ^ v2;
		};
		return OperatorWith(val, op);
	}
	template<typename TYPE> inline SparseVector<TYPE_PROMOTE(T, TYPE)> operator | (SparseVector<TYPE> val) const {
		std::function<TYPE_PROMOTE(T, TYPE)(const T&, const TYPE&)> op = [&](
			const T& v1, const TYPE& v2
			) {
				return v1 | v2;
		};
		return OperatorWith(val, op);
	}
	template<typename TYPE> inline SparseVector<TYPE_PROMOTE(T, TYPE)> operator & (SparseVector<TYPE> val) const {
		std::function<TYPE_PROMOTE(T, TYPE)(const T&, const TYPE&)> op = [&](
			const T& v1, const TYPE& v2
			) {
				return v1 & v2;
		};
		return OperatorWith(val, op);
	}

	/**
		Return normalized vector, but don't change old data
	*/
	inline SparseVector<ZQ_REAL> Normalize() const {
		SparseVector<ZQ_REAL> result(*this);
		result.SetNormalize();
		return result;
	}

	/**
		normalize this vector
	*/
	inline SparseVector& SetNormalize() {
		ZQ_REAL length = Length();
		(*this) /= length;
		return *this;
	}

	//	vector properties
	/**
		return the dimension of the vector
	*/
	inline int Dim() const {
		return dim_num;
	}

	/**
		return the sum of all elements
	*/
	inline T Sum() const {
		T sum = 0;
		for (int i = 0; i < value.size(); i++)
			sum += value[i];
		return sum;
	}

	inline ZQ_REAL SquareLength() const {
		T sqr_sum = 0;
		for (int i = 0; i < value.size(); i++)
			sqr_sum += value[i] * value[i];
		return sqr_sum;
	}

	inline ZQ_REAL Length() const {
		return sqrt(SquareLength());
	}
protected:
	/**
		assure dimension of the vector to calculate is the same with the current vector

		\param	vec		the vector to check
	*/
	template<typename TYPE> inline void AssureDimLegal(const SparseVector<TYPE>& vec) const{
		if (dim_num != vec.dim_num) {
			std::cout << "Dimension of vector illegal, vector dimension don't match" << std::endl;
			exit(0);
		}
	}
	/**
		merge with others and do the operator,
		this is for helping operator
	*/
	template<typename TYPE> inline SparseVector<TYPE_PROMOTE(T, TYPE)> OperatorWith (
		const SparseVector<TYPE>& vec,
		const std::function<TYPE_PROMOTE(T, TYPE)(const T&,const TYPE&)>& op
	) const {
		AssureDimLegal(vec);
		SparseVector<TYPE_PROMOTE(T, TYPE)> result(vec.dim_num);
		if (vec.value.size() == 0) {
			result = *(this);
		}
		else if (value.size() == 0) {
			result = vec;
		}
		else {
			std::vector< TYPE_PROMOTE(T, TYPE)> new_value;
			std::vector<int> new_index;
			int cur_vec = 0, cur_this = 0;
			while (cur_vec < vec.value.size() && cur_this < value.size()) {
				if (vec.index[cur_vec] == index[cur_this]) {
					new_value.push_back(op(value[cur_this], vec.value[cur_vec]));
					new_index.push_back(index[cur_this]);
					cur_vec++;
					cur_this++;
				}
				else if (vec.index[cur_vec] < index[cur_this]) {
					new_value.push_back(op(0, vec.value[cur_vec]));
					new_index.push_back(vec.index[cur_vec]);
					cur_vec++;
				}
				else {
					new_value.push_back(op(value[cur_this], 0));
					new_index.push_back(index[cur_this]);
					cur_this++;
				}
			}
			if (cur_this == value.size()) {
				for (; cur_vec < vec.value.size(); cur_vec++) {
					new_value.push_back(op(0, vec.value[cur_vec]));
					new_index.push_back(vec.index[cur_vec]);
				}
			}
			if (cur_vec == vec.value.size()){
				for (; cur_this < value.size(); cur_this++) {
					new_value.push_back(op(value[cur_this], 0));
					new_index.push_back(index[cur_this]);
				}
			}
			result.value = new_value;
			result.index = new_index;
		}
		result.ClearZero();
		return result;
	}
};
template<class T> inline std::ostream& operator << (std::ostream& stream, const SparseVector<T> rhs) {
	stream << "(";
	if (rhs.index.size() == 0) {
		for (int i = 0; i < rhs.dim_num; i++) {
			if (i == 0) std::cout << "0";
			else std::cout << "," << "0";
		}

	}
	else {
		for (int i = 0; i < rhs.index[0]; i++) {
			std::cout << "0"<< "," ;
		}
		for (int i = 0; i < rhs.index.size(); i++) {
			stream << rhs.value[i];
			if (i != rhs.index.size() - 1) {
				for (int j = rhs.index[i] + 1; j < rhs.index[i + 1]; j++) {
					stream << "," << Zero<T>();
				}
				stream << ",";
			}
		}
		for (int i = rhs.index[rhs.index.size() - 1] + 1; i < rhs.dim_num; i++) {
			std::cout << "," << "0";
		}
	}

	stream << ")";
	return stream;
}


}

#endif