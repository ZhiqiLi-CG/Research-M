/***********************************************************/
/**	\file
	\brief		math basic utils
	\author		Zhiqi Li
	\date	    12/119/2022

*/
/***********************************************************/
#ifndef __MATH_UTILS_H__
#define __MATH_UTILS_H__

#include <assert.h>
#include <iostream>
#include <math.h>

#define ZQ_PI (3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679)
namespace zq{
	template<class T>
	inline T Zero() {
		if constexpr (std::is_same<T, int > ::value) return (T)0;
		else if constexpr (std::is_same<T, short > ::value) return (T)0;
		else if constexpr (std::is_same<T, char > ::value) return (T)0;
		else if constexpr (std::is_same<T, long > ::value) return (T)0;
		else if constexpr (std::is_same<T, long long > ::value) return (T)0;
		else if constexpr (std::is_same<T, unsigned int > ::value) return (T)0;
		else if constexpr (std::is_same<T, unsigned short>::value) return (T)0;
		else if constexpr (std::is_same<T, unsigned char > ::value) return (T)0;
		else if constexpr (std::is_same<T, unsigned long > ::value) return (T)0;
		else if constexpr (std::is_same<T, unsigned long long > ::value) return (T)0;
		else if constexpr (std::is_same<T, float > ::value) return (T)0;
		else if constexpr (std::is_same<T, double > ::value) return (T)0;
		else if constexpr (std::is_same<T, long double > ::value) return (T)0;
		else if constexpr (std::is_same<T, bool > ::value) return (T)false;
		else {
			return T::Zero();
		}
	}

	template<class T>
	inline T One() {
		if constexpr (std::is_same<T, int>::value) return (T)1;
		else if constexpr (std::is_same < T, short>::value) return (T)1;
		else if constexpr (std::is_same < T, char>::value) return (T)1;
		else if constexpr (std::is_same < T, long>::value) return (T)1;
		else if constexpr (std::is_same < T, long long>::value) return (T)1;
		else if constexpr (std::is_same < T, unsigned int>::value) return (T)1;
		else if constexpr (std::is_same < T, unsigned short>::value) return (T)1;
		else if constexpr (std::is_same < T, unsigned char>::value) return (T)1;
		else if constexpr (std::is_same < T, unsigned long>::value) return (T)1;
		else if constexpr (std::is_same < T, unsigned long long>::value) return (T)1;
		else if constexpr (std::is_same < T, float>::value) return (T)1;
		else if constexpr (std::is_same < T, double>::value) return (T)1;
		else if constexpr (std::is_same<T, long double>::value) return (T)1;
		else if constexpr (std::is_same<T, bool>::value) return (T)True;
		else {
			return T::One();
		}
	}
}	//	end namespace zq

#endif