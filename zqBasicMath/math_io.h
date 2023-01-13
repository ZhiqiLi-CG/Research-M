/***********************************************************/
/**	\file
	\brief		Read and Write of Math Related Materials
	\details	IO can be organised into 2 kinds, display mode and compress mode
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
#include<zqBasicMath/math_utils.h>
#include<zqBasicMath/math_dense.h>
#include<zqBasicUtils/utils_io.h>
namespace zq{
	namespace utils {
		//	========================================
///@{
/**	@name Read & Write Dense Vector, display mode
*/
//	========================================

/**
	Read Dense Vector from file

	\param	vector_file_name	file name of the vector file
	\param	dense_vec			the vector to store the reading data
	\return						whether read succeed
*/
		template<typename T>
		int readDenseVectorFromFile(const char* vector_file_name,
			DenseVector<T>& dense_vec) {
			std::ifstream file;
			file.open(vector_file_name);
			zq::Vec3f a;
			std::cout << a;
			if (!file.is_open()) {
#ifndef BE_QUIET
				std::cout << "error: readDenseVectorFromFile, cannot open " << vector_file_name << std::endl;
#endif
				return 0;
			}

			dense_vec.Reset();

			TYPE_PROMOTE(T, float) val;
			while (file >> val) {
				dense_vec.value.push_back(val);
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
		int writeDenseVectorToFile(const char* vector_file_name,
			DenseVector<T>& dense_vec) {
			std::ofstream file;
			file.open(vector_file_name);

			if (!file.is_open()) {
#ifndef BE_QUIET
				std::cout << "error: writeDenseVectorToFile, cannot open " << vector_file_name << std::endl;
#endif
				return 0;
			}

			for (int i = 0; i < dense_vec.Dim(); i++) {
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
		template<template<class > class Holder, class T>
		int readDenseVectorPointsFromFile(const char* vector_file_name,
			DenseVector<Holder<T>>& dense_vec) {
			DenseVector<T> value_vec;
			std::ifstream file;
			file.open(vector_file_name);
			if (!file.is_open()) {
#ifndef BE_QUIET
				std::cout << "error: readDenseVectorFromFile, cannot open " << vector_file_name << std::endl;
#endif
				return 0;
			}

			value_vec.Reset();

			TYPE_PROMOTE(T, float) val;
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
		template<template<class > class Holder, class T>
		int writeDenseVectorPointsToFile(
			const char* vector_file_name,
			DenseVector<Holder<T>>& dense_vec) {
			std::ofstream file;
			file.open(vector_file_name);
			DenseVector<T> value_vec;
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
			//if (!std::is_same<ContainerType::Type, ValueType>::value) throw "type mismatch";
			for (int i = 0; i < value.Dim(); i += ContainerType::dims) {
				vec.push_back(ContainerType(&value[i]));
			}
		}
		template<typename ContainerType, typename ValueType>
		void WriteVectorToValue(
			const DenseVector<ContainerType>& vec,
			DenseVector<ValueType>& value
		) {
			//if (!std::is_same<ContainerType::Type, ValueType>::value) throw "type mismatch";
			for (int i = 0; i < vec.Dim(); i++) {
				for (int j = 0; j < ContainerType::dims; j++) {
					value.push_back(vec[i][j]);
				}
			}
		}
		///@}

		//	========================================
		///@{
		/**	@name Read & Write array of small vector and matrix, compress mode
		*/
		//	========================================
		//// Declaration of functions

		////Write Vector array to binary, padding to 3D. Memory layout:
		//Bytes [0,4): an integer n, indicating number of vectors (NOT number of bytes)
		//Bytes [4,4+n*3*sizeof(T)): binary value of n 3D vectors.
		//Note: always store 3D vectors in binary file. If the original vectors are less than 3D, we fill it with 0.
		template<class T, int d> bool Write_Vector_Array_3D(const std::string& file_name, const Array<Vec<T, d> >& arr);
		template<class T, int d, class F> bool Write_Vector_Array_3D(const std::string& file_name, F& f, std::uint32_t n);//i-th vector is f(i)
		template<class T, int d> bool Read_Vector_Array_3D(const std::string& file_name, Array<Vec<T, d> >& arr);

		//// Implementation of functions

		template<class T, int d>
		bool Write_Vector_Array_3D(const std::string& file_name, const Array<Vec<T, d>>& arr)
		{
			assert(1 <= d && d <= 3);
			std::ofstream output(file_name, std::ios::binary);
			if (!output) return false;
			std::uint32_t n = (std::uint32_t)arr.size();
			Write_Binary(output, n);
			T* data = new T[n * 3];
			memset(data, 0, n * 3 * sizeof(T));
#pragma omp parallel for
			for (int i = 0; i < (int)n; i++) {
				for (int axis = 0; axis < d; axis++) data[i * 3 + axis] = arr[i][axis];
			}
			Write_Binary_Array(output, data, n * 3);
			delete[] data;
			output.close();
			return true;
		}

		template<class T, int d, class F>
		bool Write_Vector_Array_3D(const std::string& file_name, F& f, std::uint32_t n)
		{
			assert(1 <= d && d <= 3);
			std::ofstream output(file_name, std::ios::binary);
			if (!output) return false;
			Write_Binary(output, n);
			T* data = new T[n * 3];
			memset(data, 0, n * 3 * sizeof(T));
#pragma omp parallel for
			for (int i = 0; i < n; i++) {
				Vec<T, d> vec = f(i);
				for (int axis = 0; axis < d; axis++) data[i * 3 + axis] = vec[axis];
			}
			Write_Binary_Array(output, data, n * 3);
			delete[] data;
			output.close();
			return true;
		}

		template<class T, int d>
		bool Read_Vector_Array_3D(const std::string& file_name, Array<Vec<T, d>>& arr)
		{
			assert(1 <= d && d <= 3);
			std::ifstream input(file_name, std::ios::binary);
			if (!input) return false;
			std::uint32_t n;
			Read_Binary(input, n);
			arr.resize(n);
			T* data = new T[n * 3];
			Read_Binary_Array(input, data, n * 3);
#pragma omp parallel for
			for (int i = 0; i < (int)n; i++) {
				for (int axis = 0; axis < d; axis++) arr[i][axis] = data[i * 3 + axis];
			}
			delete[] data;
			input.close();
			return true;
		}
		///@}
	}
}	//	end namespace zq

#endif	//	__ZQ_MATH_IO_H__