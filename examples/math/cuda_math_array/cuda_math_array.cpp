#include<ResearchM_config.h>
#include<iostream>
#ifndef RESEARCHM_ENABLE_CUDA
template<class T>
class B {
public:
	typedef T Type;
	};
template<class T>
void test_template(T t) {
	T::Type b = 10.9;
	printf("*******%f*******", b);
}
int main() {
	test_template(B<float>());
	test_template(B<int>());
}
#endif