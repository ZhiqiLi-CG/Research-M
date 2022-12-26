/***********************************************************/
/**	\file
	\brief		Read and Write of Math Related Materials
	\details	matrix files are stored 1-indexing
	\author		Zhiqi Li, based on yzLib of Dr. Yizhong Zhang
	\date		12/26/2022
*/
/***********************************************************/
#ifndef __MATH_IO_H__
#define __MATH_IO_H__

#include <iostream>
#include <fstream>
#include<zqBasicMath/math_vector.h>
#include<zqBasicMath/math_matrix.h>
#include<zqBasicMath/math_matrix_utils.h>
#include<zqBasicMath/math_dense.h>
namespace zq{
//	========================================
///@{
/**	@name Read & Write Dense Vector
*/
//	========================================

/**
	Read Dense Vector from file

	\param	vector_file_name	file name of the vector file
	\param	dense_vec			the vector to store the reading data
	\return						whether read succeed
*/
template<typename T>
int readDenseVectorFromFile(const char*		vector_file_name,
							DenseVector<T>&	dense_vec){
	std::ifstream file;
	file.open(vector_file_name);
	zq::Vec3f a;
	std::cout << a;
	if( !file.is_open() ){
		#ifndef BE_QUIET
			std::cout << "error: readDenseVectorFromFile, cannot open " << vector_file_name << std::endl;
		#endif
		return 0;
	}

	dense_vec.Reset();

	TYPE_PROMOTE(T, float) val;
	while( file >> val ){
		dense_vec.value.push_back( val );
	}

	file.close();
	return 1;
}

/**
	Write Dense Vector to File

	\param	vector_file_name	file name of the vector file
	\param	dense_vec			the vector 
	\return						whether write succeed
*/
template<typename T>
int writeDenseVectorToFile(const char*		vector_file_name,
						   DenseVector<T>&	dense_vec){
	std::ofstream file;
	file.open(vector_file_name);

	if( !file.is_open() ){
		#ifndef BE_QUIET
			std::cout << "error: writeDenseVectorToFile, cannot open " << vector_file_name << std::endl;
		#endif
		return 0;
	}

	for(int i=0; i<dense_vec.Dim(); i++){
		file << dense_vec[i] << '\t';
	}

	file.close();
	return 1;
}
/**
	Read Dense Vector of points from file

	\param	vector_file_name	file name of the vector file
	\param	dense_vec			the vector to store the reading data
	\return						whether read succeed
*/
template<typename T>
int readDenseVectorPointsFromFile(const char* vector_file_name,
	DenseVector<T>& dense_vec) {
	DenseVector<T::Type> value_vec;
	std::ifstream file;
	file.open(vector_file_name);
	if (!file.is_open()) {
#ifndef BE_QUIET
		std::cout << "error: readDenseVectorFromFile, cannot open " << vector_file_name << std::endl;
#endif
		return 0;
	}

	value_vec.Reset();

	TYPE_PROMOTE(T::Type, float) val;
	while (file >> val) {
		value_vec.value.push_back(val);
	}

	file.close();
	ReadVectorFromValue(dense_vec, value_vec);
	return 1;
}

/**
	Write Dense Vector to File

	\param	vector_file_name	file name of the vector file
	\param	dense_vec			the vector
	\return						whether write succeed
*/
template<typename T>
int writeDenseVectorPointsToFile(
	const char* vector_file_name,
	DenseVector<T>& dense_vec) {
	std::ofstream file;
	file.open(vector_file_name);
	DenseVector<T::Type> value_vec;
	WriteVectorToValue(dense_vec, value_vec);
	if (!file.is_open()) {
#ifndef BE_QUIET
		std::cout << "error: writeDenseVectorToFile, cannot open " << vector_file_name << std::endl;
#endif
		return 0;
	}

	for (int i = 0; i < value_vec.Dim(); i++) {
		file << value_vec[i] << '\t';
	}

	file.close();
	return 1;
}
template<typename ContainerType, typename ValueType>
void ReadVectorFromValue(
	DenseVector<ContainerType>& vec,
	const DenseVector<ValueType>& value
) {
	if (!std::is_same<ContainerType::Type, ValueType>::value) throw "type mismatch";
	for (int i = 0; i < value.Dim(); i+= ContainerType::dims) {
		vec.push_back(ContainerType(&value[i]));
	}
}
template<typename ContainerType, typename ValueType>
void WriteVectorToValue(
	const DenseVector<ContainerType>& vec,
	DenseVector<ValueType>& value
) {
	if (!std::is_same<ContainerType::Type, ValueType>::value) throw "type mismatch";
	for (int i = 0; i< vec.Dim(); i ++) {
		for (int j = 0; j < ContainerType::dims; j++) {
			value.push_back(vec[i][j]);
		}
	}
}

}	//	end namespace yz

#endif	//	__YZ_MATH_IO_H__