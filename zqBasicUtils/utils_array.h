
/***********************************************************/
/**	\file
	\brief		Array in HOST or DEVICE
	\author		Zhiqi Li
	\date		5/26/2022
*/
/***********************************************************/
#ifndef __UTILS_ARRAY_BASIC_H__
#define __UTILS_ARRAY_BASIC_H__

#include<ResearchM_config.h>
#include<zqBasicUtils/utils_common.h>
#include <iostream>
#ifdef RESEARCHM_ENABLE_CUDA
	#include <cuda.h>
	#include <cuda_runtime_api.h>
	#include <cuda_runtime.h>
	#include "cublas_v2.h"
	#include <thrust/device_vector.h>
	#include <thrust/host_vector.h>
	#include <vector_functions.h>
	#include "driver_types.h"
	#include <thrust/iterator/zip_iterator.h>
	#include <thrust/iterator/counting_iterator.h>
	#include <thrust/device_vector.h>
	#include <thrust/host_vector.h>
	#include <thrust/transform.h>
	#include <thrust/functional.h>
	#include <thrust/inner_product.h>
	#include <thrust/extrema.h>
#endif
namespace zq {
		#define HOST 0
		#define DEVICE 1
#ifdef RESEARCHM_ENABLE_CUDA
		template<class T, int side = 0>
		using Array = typename std::conditional<side == 0, thrust::host_vector<T>, thrust::device_vector<T>>::type;
#elif		
		template<class T, int side = 0> using Array = std::vector<T>;
#endif
		template<class T, int side = 0> 
		using ArrayPtr = std::shared_ptr<Array<T,side> >;
		
		template<class T, int side = 0>
		using ArrayIter = typename Array<T, side>::iterator;
#define counting thrust::counting_iterator<int>
}

#endif	