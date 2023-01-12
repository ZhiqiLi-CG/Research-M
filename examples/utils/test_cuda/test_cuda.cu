#include<zqBasicUtils/utils_cuda.h>
#include<zqBasicUtils/utils_array.h>
#include<zqBasicUtils/utils_array_func.h>
int main() {
	int* p;
	cudaNew(p, 100);
	cudaFree(p);
}

