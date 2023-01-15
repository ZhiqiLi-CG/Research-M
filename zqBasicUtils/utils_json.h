/***********************************************************/
/**	\file
	\brief		Json adpater
	\author		Zhiqi Li
	\date		12/26/2022
*/
/***********************************************************/
#ifndef __UTILS_JSON_H__
#define __UTILS_JSON_H__

#include <time.h>
#include <json.hpp>
#include <zqBasicUtils/utils_aux_func.h>
#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/ranges.h>
#include <fmt/format.h>
namespace zq{ 
	using json = nlohmann::json;
	template<class T>
	T value(json& j, const std::string key, const T default_value) {
		if (j.contains(key)) {
			T value = j.at(key);
			fmt::print(fg(fmt::color::green), "#     [=] Parse key ");
			fmt::print("{}", key);
			fmt::print(fg(fmt::color::green), " from json: ");
			fmt::print("{}\n", value);
			return value;
		}
		else {
			j[key] = default_value;
			fmt::print(fg(fmt::color::yellow), "#     [+] Can't parse key ");
			fmt::print("{}", key);
			fmt::print(fg(fmt::color::yellow), " in json, set to default value: ");
			fmt::print("{}\n", default_value);
			return default_value;
		}
	}
	template<class T>
	void setNonOverride(json& j, const std::string key, const T value) {
		if (!j.contains(key)) {
			j[key] = value;
		}
	}

	template<class T>
	void setValue(json& j, const std::string key, const T value) {
		j[key] = value;
	}
}	

#endif	