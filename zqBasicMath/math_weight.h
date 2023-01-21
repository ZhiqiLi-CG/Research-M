/***********************************************************/
/**	\file
	\brief		weight class
	\details	weight class
	\author		Zhiqi Li
	\date	    1/20/2022

*/
/***********************************************************/
#ifndef __MATH_WEIGHT_H__
#define __MATH_WEIGHT_H__

#include <assert.h>
#include <iostream>
#include <zqBasicMath/math_vector.h>
#include <zqBasicMath/math_matrix.h>
#include<ResearchM_config.h>
#ifdef  RESEARCHM_ENABLE_CUDA
#include <zqBasicUtils/utils_cuda.h>
#include <zqBasicUtils/utils_array.h>
#endif //  RESEARCHM_ENABLE_CUDA

namespace zq {
	namespace math {
		class WeightBase {
			/// TODO
		};
		
		enum  WeightType /// Weight Type is used to set weight for cluster,...
		{
			CLUSTER
		};

		/// In the weight class, you can set which weight function you will use
		template<int type>
		class Weight {
			/// TODO
			template<class T>
			T weight(T dist) {
				return 0;
			}
		};
	}
}

#endif	//	__ZQ_VECTOR_H__