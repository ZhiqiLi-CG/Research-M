#include<ResearchM_config.h>
#ifndef RESEARCHM_ENABLE_CUDA
#include<zqBasicUtils/utils_array.h>
#include<zqBasicUtils/utils_array_func.h>
#include<zqBasicUtils/utils_aux_func.h>

int main() {
	int example;
	scanf("%d", &example);
	if (example == 0) {
		zq::Array<int> host_int1(100), host_int2(100);
		for (int i = 0; i < host_int1.size(); i++) {
			host_int1[i] = i;
		}
		for (int i = 0; i < host_int2.size(); i++) {
			host_int2[i] = i + 1;
		}
		zq::utils::Array_Add<int>(host_int1, host_int2, 2.0);
		std::copy(host_int1.begin(), host_int1.end(), std::ostream_iterator<int>(std::cout, "\n"));
	}
	else {
		zq::Array<float> host_float1(100);
		host_float1[1] = NAN;
		zq::utils::Numerical_Check<float>(host_float1, "host_float1", false);

		zq::Array<float> host_float2(100);
		host_float2[1] = INFINITY;
		zq::utils::Numerical_Check<float>(host_float2, "host_float2", true);
	}
}
#endif



