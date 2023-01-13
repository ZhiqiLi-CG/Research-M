#include<ResearchM_config.h>
#ifdef RESEARCHM_ENABLE_CUDA
	#include<zqBasicUtils/utils_cuda.h>
	#include<zqBasicUtils/utils_array.h>
	#include<zqBasicUtils/utils_array_func.h>
	#include<zqBasicUtils/utils_array.h>
	#include<zqBasicUtils/utils_array_func.h>

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
			thrust::copy(host_int1.begin(), host_int1.end(), std::ostream_iterator<int>(std::cout, "\n"));
			
			zq::Array<int> device_int1(100), device_int2(100);
			for (int i = 0; i < device_int1.size(); i++) {
				device_int1[i] = i;
			}
			for (int i = 0; i < device_int2.size(); i++) {
				device_int2[i] = i + 1;
			}
			zq::utils::Array_Add<int>(device_int1, device_int2, 2.0);
			thrust::copy(device_int1.begin(), device_int1.end(), std::ostream_iterator<int>(std::cout, "\n"));

		}
		else {
			zq::Array<float,DEVICE> device_float(100);
			device_float[1] = NAN;
			zq::utils::Numerical_Check<float>(device_float,"device_float",false);

			zq::Array<float, HOST> host_float(100);
			host_float[1] = INFINITY;
			zq::utils::Numerical_Check<float>(host_float, "host_float", true);
		}
	}
#endif



