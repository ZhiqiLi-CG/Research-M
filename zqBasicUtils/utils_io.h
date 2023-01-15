
/***********************************************************/
/**	\file
	\brief		Basic IO
	\author		Zhiqi Li, based on Simplex of Prof. Bo Zhu
	\date		5/26/2022
*/
/***********************************************************/
#ifndef __UTILS_IO_H__
#define __UTILS_IO_H__
#include <ostream>
#include <istream>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cerrno>
#include <climits>
#include <cstdio>
#include <string>
#include <windows.h>
#include <zqBasicUtils/utils_array.h>
namespace zq {
	namespace utils {
		template<class TYPE> class CheckIOFunc {
		private:
			static std::ostream output;
			static std::istream input;

			template<class T> static decltype(std::declval<T>().Write_Binary(output), std::true_type()) check_binary_write(int);
			template<class T> static std::false_type check_binary_write(...);
			template<class T> static decltype(std::declval<T>().Read_Binary(input), std::true_type()) check_binary_read(int);
			template<class T> static std::false_type check_binary_read(...);

			template<class T> static decltype(std::declval<T>().Write_Text(output), std::true_type()) check_text_write(int);
			template<class T> static std::false_type check_text_write(...);
			template<class T> static decltype(std::declval<T>().Read_Text(input), std::true_type()) check_text_read(int);
			template<class T> static std::false_type check_text_read(...);
		public:
			static constexpr bool has_binary_write = std::is_same<decltype(check_binary_write<TYPE>(0)), std::true_type>::value;
			static constexpr bool has_binary_read = std::is_same<decltype(check_binary_read<TYPE>(0)), std::true_type>::value;

			static constexpr bool has_text_write = std::is_same<decltype(check_text_write<TYPE>(0)), std::true_type>::value;
			static constexpr bool has_text_read = std::is_same<decltype(check_text_read<TYPE>(0)), std::true_type>::value;
		};

		//////////////////////////////////////////////////////////////////////////
		////Binary IO
		////Write object without member function Write_Binary(output)
		template<class T> void Write_Binary(std::ostream& output, const T& data, typename std::enable_if<!CheckIOFunc<T>::has_binary_write>::type* = nullptr)
		{
			output.write(reinterpret_cast<const char*>(&data), sizeof(T));
		}

		////Write object with member function Write_Binary(output)
		template<class T> void Write_Binary(std::ostream& output, const T& data, typename std::enable_if<CheckIOFunc<T>::has_binary_write>::type* = nullptr)
		{
			data.Write_Binary(output);
		}

		////Read object without member function Read_Binary(input)
		template<class T> void Read_Binary(std::istream& input, T& data, typename std::enable_if<!CheckIOFunc<T>::has_binary_read>::type* = nullptr)
		{
			input.read(reinterpret_cast<char*>(&data), sizeof(T));
		}

		////Read object with member function Read_Binary(input)
		template<class T> void Read_Binary(std::istream& input, T& data, typename std::enable_if<CheckIOFunc<T>::has_binary_read>::type* = nullptr)
		{
			data.Read_Binary(input);
		}

		template<class T> void Write_Binary_Array(std::ostream& output, const T* array, const std::uint32_t n)
		{
			if (n > 0)output.write(reinterpret_cast<const char*>(array), n * sizeof(T));
		}

		template<class T> void Read_Binary_Array(std::istream& input, T* array, const std::uint32_t n)
		{
			if (n > 0)input.read(reinterpret_cast<char*>(array), n * sizeof(T));
		}

		template<class T> bool Write_Binary_To_File(const std::string& file_name, const T& data)
		{
			std::ofstream output(file_name, std::ios::binary); if (!output)return false; Write_Binary(output, data); return true;
		}

		template<class T> bool Read_Binary_From_File(const std::string& file_name, T& data)
		{
			std::ifstream input(file_name, std::ios::binary); if (!input)return false; Read_Binary(input, data); input.clear(); input.close(); return true;
		}

		template<class T> bool Write_Binary_Array_To_File(const std::string& file_name, T* array, const std::uint32_t n)
		{
			std::ofstream output(file_name, std::ios::binary); if (!output)return false; Write_Binary_Array(output, array, n); return true;
		}

		template<class T> bool Read_Binary_Array_From_File(const std::string& file_name, T* array, const std::uint32_t n)
		{
			std::ifstream input(file_name, std::ios::binary); if (!input)return false; Read_Binary_Array(input, array, n); input.clear(); input.close(); return true;
		}

		template<class T> bool Write(const std::string& file_name, const T& data)
		{
			return Write_Binary_To_File(file_name, data);
		}

		template<class T> bool Read(const std::string& file_name, T& data)
		{
			return Read_Binary_From_File(file_name, data);
		}

		//////////////////////////////////////////////////////////////////////////
		////Text IO
		////Write object without member function Write_Text(output)
		template<class T> void Write_Text(std::ostream& output, const T& data, typename std::enable_if<!CheckIOFunc<T>::has_text_write>::type* = nullptr)
		{
			output << data;
		}

		////Write object with member function Write_Text(output)
		template<class T> void Write_Text(std::ostream& output, const T& data, typename std::enable_if<CheckIOFunc<T>::has_text_write>::type* = nullptr)
		{
			data.Write_Text(output);
		}

		////Read object without member function Read_Text(input)
		template<class T> void Read_Text(std::istream& input, T& data, typename std::enable_if<!CheckIOFunc<T>::has_text_read>::type* = nullptr)
		{
			input >> data;
		}

		////Read object with member function Read_Text(input)
		template<class T> void Read_Text(std::istream& input, T& data, typename std::enable_if<CheckIOFunc<T>::has_text_read>::type* = nullptr)
		{
			data.Read_Text(input);
		}

		template<class T_ARRAY> void Write_Text_Array(std::ostream& output, const T_ARRAY& array, const int n, char separator = '\n')
		{
			for (int i = 0; i < n; i++)output << array[i] << separator;
		}

		template<class T_ARRAY> void Read_Text_Array(std::istream& input, T_ARRAY& array, const int n, char separator = '\n')
		{
			for (int i = 0; i < n; i++)input >> array[i];
		}

		template<class T> bool Write_Text_To_File(const std::string& file_name, const T& data)
		{
			std::ofstream output(file_name); if (!output)return false; Write_Text(output, data); return true;
		}

		template<class T> bool Read_Text_From_File(const std::string& file_name, T& data)
		{
			std::ifstream input(file_name); if (!input)return false; Read_Text(input, data); return true;
		}

		template<class T_ARRAY> bool Write_Text_Array_To_File(const std::string& file_name, const T_ARRAY& array, const int n, char separator = '\n')
		{
			std::ofstream output(file_name); if (!output)return false; Write_Text_Array(output, array, n, separator); return true;
		}

		template<class T_ARRAY> bool Read_Text_Array_From_File(const std::string& file_name, T_ARRAY& array, const int n)
		{
			std::ifstream input(file_name); if (!input)return false; Read_Text_Array(input, array, n); return true;
		}

		template<class T> bool Append_Text_To_File(const std::string& file_name, const T& data)
		{
			std::ofstream output(file_name, std::ios_base::app); if (!output)return false; Write_Text(output, data); return true;
		}

		inline void Read_Text_To_String(const std::string& file_name, std::string& str)
		{
			std::ifstream file; file.open(file_name); std::stringstream ss; ss << file.rdbuf(); str = ss.str();
		}

		//////////////////////////////////////////////////////////////////////////
		////File operations
		LPWSTR ConvertCharToLPWSTR(const char* szString)
		{
			int dwLen = strlen(szString) + 1;
			int nwLen = MultiByteToWideChar(CP_ACP, 0, szString, dwLen, NULL, 0);//算出合适的长度
			LPWSTR lpszPath = new WCHAR[dwLen];
			MultiByteToWideChar(CP_ACP, 0, szString, dwLen, lpszPath, nwLen);
			return lpszPath;
		}

		inline bool Directory_Exists(const char* dirname)
		{
#ifdef UNICODE
			DWORD attr = GetFileAttributes(ConvertCharToLPWSTR(dirname));
#else
			DWORD attr = GetFileAttributes(dirname);
#endif //  
			return((attr != -1) && (attr & FILE_ATTRIBUTE_DIRECTORY));
		}

		inline bool Create_Directory(const std::string& dirname)
		{
			if (!Directory_Exists(dirname.c_str())) {
				size_t pos = 0;
				do {
					pos = dirname.find_first_of("\\/", pos + 1);
					if (!Directory_Exists(dirname.substr(0, pos).c_str())) {
#ifdef UNICODE
						if (CreateDirectory(ConvertCharToLPWSTR(dirname.substr(0, pos).c_str()), NULL) == 0 && ERROR_ALREADY_EXISTS != GetLastError()) {
#else
						if (CreateDirectory(dirname.substr(0, pos).c_str(), NULL) == 0 && ERROR_ALREADY_EXISTS != GetLastError()) {
#endif //  
							std::cerr << "Error: [File] Create directory " << dirname << " failed!" << std::endl; return false;
						}
					}
				} while (pos != std::string::npos);
			}
			return true;
		}

		inline bool File_Exists(const std::string& file_name)
		{
			return !std::ifstream(file_name.c_str()).fail();
		}

		inline std::string File_Extension_Name(const std::string& file_name)
		{
			size_t p = file_name.rfind('.'); if (p != std::string::npos)return file_name.substr(p + 1); else return "";
		}

		//	========================================
		///@{
		/**	@name Read & Write array of small vector and matrix, compress mode
		*/
		//	========================================
		//// Declaration of functions
				////No padding, just write memory.
		//Bytes [0,4): an integer n, indicating number of scalars (NOT number of bytes)
		//Bytes [4,4+n*sizeof(T)): binary value of n elements
		//Note that for Eigen::Vector or Eigen::Matrix, they're just the flattened element array.

		//Stream Style
		void Write_Array_Stream_Content(std::ostream& output, const Array<bool>& arr)
		{
			std::uint32_t n = (std::uint32_t)arr.size();
			bool* data = new bool[n];
#pragma omp parallel for
			for (int i = 0; i < (int)n; i++) data[i] = arr[i];
			Write_Binary_Array<bool>(output, data, n);
			delete[] data;
		}
		template<class T> void Write_Array_Stream_Content(std::ostream& output, const Array<T>& arr) {
			const T* data = (const T*)(arr.data());
			Write_Binary_Array<T>(output, data, arr.size());
		}
		template<class T> void Write_Array_Stream(std::ostream& output, const Array<T>& arr) {
			std::uint32_t n = (std::uint32_t)arr.size();
			Write_Binary<std::uint32_t>(output, n);
			Write_Array_Stream_Content<T>(output, arr);
		}
		template<class T, class F> void Write_Array_Stream(std::ostream& output, F& f, std::uint32_t n) {
			Array<T> arr(n);
			for (int i = 0; i < n; i++) arr[i] = f(i);
			Write_Array_Stream<T>(output, arr);
		}
		void Read_Array_Stream_Content(std::istream& input, Array<bool>& arr, const std::uint32_t& n) {
			arr.resize(n);
			bool* data = new bool[n];
			Read_Binary_Array<bool>(input, data, (int)n);
#pragma omp parallel for
			for (int i = 0; i < (int)n; i++) arr[i] = data[i];
			delete[] data;
		}
		template<class T> void Read_Array_Stream_Content(std::istream& input, Array<T>& arr, const std::uint32_t& n) {
			arr.resize(n);
			T* data = (T*)(arr.data());
			Read_Binary_Array<T>(input, data, n);
		}
		template<class T> void Read_Array_Stream(std::istream& input, Array<T>& arr) {
			std::uint32_t n;
			Read_Binary<std::uint32_t>(input, n);
			Read_Array_Stream_Content<T>(input, arr, n);
		}

		//File Style
		template<class T> bool Write_Array(const std::string& file_name, const Array<T>& arr) {
			std::ofstream output(file_name, std::ios::binary);
			if (!output) return false;
			Write_Array_Stream<T>(output, arr);
			output.close();
			return true;
		}
		template<class T, class F> bool Write_Array(const std::string& file_name, F& f, std::uint32_t n) {
			Array<T> arr(n);
			for (int i = 0; i < n; i++) arr[i] = f(i);
			return Write_Array<T>(file_name, arr);
		}
		template<class T> bool Read_Array(const std::string& file_name, Array<T>& arr) {
			std::ifstream input(file_name, std::ios::binary);
			if (!input) return false;
			Read_Array_Stream<T>(input, arr);
			input.close();
			return true;
		}
		///@}
	}
}

#endif	