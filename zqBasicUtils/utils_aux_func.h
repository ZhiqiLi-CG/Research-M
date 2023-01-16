
/***********************************************************/
/**	\file
	\brief		Aux func
	\author		Zhiqi Li
	\date		5/26/2022
*/
/***********************************************************/
#ifndef __UTILS_AUX_FUNC_BASIC_H__
#define __UTILS_AUX_FUNC_BASIC_H__

#include <iostream>
#include <fmt/core.h>
#include <fmt/ranges.h>
#ifdef _DEBUG
	#pragma comment(lib,"fmtd.lib")
#else
	#pragma comment(lib,"fmt.lib")
#endif
namespace zq {
	// Assert
	void Crash_With_Info(const std::string& s, int ret)
	{
		std::cerr << s << "\n";
		exit(ret);
	}
	void Assert(bool flag, const std::string& s, int ret)
	{
		if (!flag) Crash_With_Info(s, ret);
	}

	template <typename... Args>
	void Assert(const bool flg, const char* fmt = "", const Args &...args) {
		if (!flg) {
			std::cerr << "[Error]" << fmt::format(fmt, args...);
			exit(-1);
		}
	}

	// Is valid number
	template<class T> bool Is_Valid_Number(const T& a);
	template<> 
	bool Is_Valid_Number(const float& a) {
		return (!std::isnan(a)) && std::isfinite(a); 
	}
	template<>
	bool Is_Valid_Number(const double& a) {
		return (!std::isnan(a)) && std::isfinite(a);
	}
	template<>
	bool Is_Valid_Number(const long double& a) {
		return (!std::isnan(a)) && std::isfinite(a);
	}


	/// Info
	template<typename ...Args>
	void Info(const char* fmt, const Args&...args) {
		std::cout << "#     ";
		fmt::print(fmt, args...);
		std::cout << "\n";
	}
	void Info(const std::string& str)
	{
		Info(str.c_str());
	}

}

#endif	