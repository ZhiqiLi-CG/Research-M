
/***********************************************************/
/**	\file
	\brief		Common of utils
	\author		Zhiqi Li
	\date		5/26/2022
*/
/***********************************************************/
#ifndef __UTILS_COMMON_BASIC_H__
#define __UTILS_COMMON_BASIC_H__

#include<ResearchM_config.h>
#include<math_data_path.h>
#include <iostream>
namespace zq {
	#define Declare_REAL_Types(my_type)  \
		using real=my_type;                     
	#ifdef USE_FLOAT
		Declare_REAL_Types(float)
	#else
		Declare_REAL_Types(double)
	#endif
	
}

#endif	