/***********************************************************/
/**	
\file
\brief		Example of Persistent homology
\details	This file illustrates how to use persistent homology 
\author		Zhiqi Li
\date		12/20/2020
*/
/***********************************************************/
#include <iostream>
#include<zqBasicMath/math_vector.h>
#include<zqBasicMath/math_matrix.h>
#include<zqBasicMath/math_quaternion.h>
#include<zqBasicMath/math_homology.h>
#include<zqBasicMath/math_dense.h>
#include<zqBasicMath/math_io.h>
#include<zqBasicUtils/utils_timer.h>
#include<math_data_path.h>

std::vector<std::string> file_list{
	std::string(data_path) + std::string("/persistent_homology_one_cycle.dat"),
	std::string(data_path) + std::string("/persistent_homology_one_cycle_random.dat"),
	std::string(data_path) + std::string("/persistent_homology_two_cycle_random.dat"),
	std::string(data_path) + std::string("/persistent_homology_one_sphere_random.dat"),
};
zq::DenseVector<zq::Vec3f> points;
float epsilon;
int simplex_dimension = 3;
float max_epsilon = 0.67;
int reso_epsilon = 100;

zq::Array<std::pair<float, float>> epsilon_interval;
zq::Array<int> feture_type;
zq::Array<float> epsilon_list;
zq::Array<zq::Simplical_Complex<zq::Vec3f>> complex_list;

int main() {
	// 1. read points
	int cur_epsilon = 0;
	int example = 0;
	printf("the example:");
	scanf("%d", &example);
	zq::utils::readDenseVectorPointsFromFile(file_list[example].c_str(), points);
	// 2. set the epsilon list
	for (int i = 0; i < reso_epsilon; i++) {
		epsilon_list.push_back(max_epsilon / reso_epsilon * i);
	}
	// 3. set the complex list
	for (int i = 0; i < epsilon_list.size(); i++) {
		zq::Array<zq::Array<int>> results;
		zq::VRComplexConstruct<float, zq::Vec3f>(
			epsilon_list[i],
			points.value,
			simplex_dimension,
			results
		);
		complex_list.push_back(zq::Simplical_Complex<zq::Vec3f>(&(points.value[0]), points.Dim(), results));
	}
	// 4. calculate the persistent diagram
	auto save_complex_list = complex_list;
	zq::utils::Timer dense_timeer;
	dense_timeer.Start();
	zq::CalculatePersistentData< zq::Vec3f>(
		epsilon_list,
		max_epsilon,
		complex_list,
		epsilon_interval,
		feture_type
	);
	dense_timeer.Stop();
	std::cout << " before using sparse matrix, time:" << dense_timeer.Elapsed() << "ms"<< std::endl;
	for (int i = 0; i < epsilon_interval.size(); i++) {
		printf("[%f,%f]\t", epsilon_interval[i].first, epsilon_interval[i].second);
	}
	printf("\n");

	epsilon_interval.clear();
	feture_type.clear();

	zq::utils::Timer sparse_timeer;
	sparse_timeer.Start();
	zq::CalculatePersistentDataSparse< zq::Vec3f>(
		epsilon_list,
		max_epsilon,
		save_complex_list,
		epsilon_interval,
		feture_type
	);
	sparse_timeer.Stop();
	std::cout << " after using sparse matrix, time:" << sparse_timeer.Elapsed() <<"ms" << std::endl;
	for (int i = 0; i < epsilon_interval.size(); i++) {
		printf("[%f,%f]\t", epsilon_interval[i].first, epsilon_interval[i].second);
	}
	printf("\n");

	return 0;
}