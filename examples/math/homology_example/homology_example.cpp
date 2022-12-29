/***********************************************************/
/**
\file
\brief		Example of ios
\details	Generate example for persistent homology and test the io
\author		Zhiqi Li
\date		12/26/2020
*/
/***********************************************************/
#include <iostream>
#include<zqBasicMath/math_vector.h>
#include<zqBasicMath/math_matrix.h>
#include<zqBasicMath/math_io.h>
#include<math_data_path.h>

bool compare_read_write(
	std::string file_name,
	const zq::DenseVector<zq::Vec3f>& points
) {
	zq::DenseVector<zq::Vec3f> test_points;
	zq::readDenseVectorPointsFromFile(file_name.c_str(), test_points);
	if (points.Dim() != test_points.Dim()) return false;
	else {
		for (int i = 0; i < points.Dim(); i++) {
			if ((points[i] - test_points[i]).Length() > 1e-5) {
				return false;
			}
		}
		return true;
	}
}
int main() {
	int example = 0;
	printf("the example:");
	scanf("%d", &example);
	if (example == 0) {
		zq::DenseVector<zq::Vec3f> points;
		float reso = 20, r = 0.6, ran = 0;
		for (int i = 0; i < reso; i++) {
			points.push_back(
				zq::Vec3f(
					(r + zq::randFloatingPointNumber(0.0f, ran)) * cos(2 * ZQ_PI * i / reso),
					(r + zq::randFloatingPointNumber(0.0f, ran)) * sin(2 * ZQ_PI * i / reso),
					0));
		}
		zq::writeDenseVectorPointsToFile((std::string(data_path) + std::string("/persistent_homology_one_cycle.dat")).c_str(), points);
		if (1) {
			if (compare_read_write((std::string(data_path) + std::string("/persistent_homology_one_cycle.dat")).c_str(), points)) printf("\ninput and output match\n");
			else printf("input and output mismatch\n");
		}
	}
	else if (example == 1) {
		zq::DenseVector<zq::Vec3f> points;
		float reso = 20, r = 0.6, ran = 0.3;
		for (int i = 0; i < reso; i++) {
			points.push_back(
				zq::Vec3f(
					(r + zq::randFloatingPointNumber(-ran / 2, ran / 2)) * cos(2 * ZQ_PI * i / reso),
					(r + zq::randFloatingPointNumber(-ran / 2, ran / 2)) * sin(2 * ZQ_PI * i / reso),
					0));
		}
		zq::writeDenseVectorPointsToFile((std::string(data_path) + std::string("/persistent_homology_one_cycle_random.dat")).c_str(), points);
		if (1) {
			if (compare_read_write((std::string(data_path) + std::string("/persistent_homology_one_cycle_random.dat")).c_str(), points)) printf("input and output match\n");
			else printf("input and output mismatch\n");
		}
	}
	else if (example == 2) {
		zq::DenseVector<zq::Vec3f> points;
		float reso = 20, r = 0.6, ran = 0.3;
		for (int i = 0; i < reso; i++) {
			points.push_back(
				zq::Vec3f(
					(r + zq::randFloatingPointNumber(0.0f, ran)) * cos(2 * ZQ_PI * i / reso),
					(r + zq::randFloatingPointNumber(0.0f, ran)) * sin(2 * ZQ_PI * i / reso),
					0));
		}
		for (int i = 0; i < reso; i++) {
			points.push_back(
				zq::Vec3f(
					r + (r + zq::randFloatingPointNumber(0.0f, ran)) * cos(2 * ZQ_PI * i / reso),
					0,
					(r + zq::randFloatingPointNumber(0.0f, ran)) * sin(2 * ZQ_PI * i / reso)));
		}
		zq::writeDenseVectorPointsToFile((std::string(data_path) + std::string("/persistent_homology_two_cycle_random.dat")).c_str(), points);
		if (1) {
			if (compare_read_write((std::string(data_path) + std::string("/persistent_homology_two_cycle_random.dat")).c_str(), points)) printf("input and output match\n");
			else printf("input and output mismatch\n");
		}
	}
	else if (example == 3) {
		zq::DenseVector<zq::Vec3f> points;
		float reso = 1000, r = 0.6, ran = 0;
		float sqrt_reso = sqrt(reso);
		for (int j = 0; j < sqrt_reso; j++) {
			for (int i = 0; i < sqrt_reso * sin(ZQ_PI * j / sqrt_reso); i++) {
				points.push_back(
					zq::Vec3f(
						(r + zq::randFloatingPointNumber(0.0f, ran)) * cos(2 * ZQ_PI * i / sqrt_reso / sin(ZQ_PI * j / sqrt_reso)) * sin(ZQ_PI * j / sqrt_reso),
						(r + zq::randFloatingPointNumber(0.0f, ran)) * sin(2 * ZQ_PI * i / sqrt_reso / sin(ZQ_PI * j / sqrt_reso)) * sin(ZQ_PI * j / sqrt_reso),
						(r + zq::randFloatingPointNumber(0.0f, ran)) * cos(ZQ_PI * j / sqrt_reso)
					));
			}
		}
		zq::writeDenseVectorPointsToFile((std::string(data_path) + std::string("/persistent_homology_one_sphere_random.dat")).c_str(), points);
		if (1) {
			if (compare_read_write((std::string(data_path) + std::string("/persistent_homology_one_sphere_random.dat")).c_str(), points)) printf("input and output match\n");
			else printf("input and output mismatch\n");
		}
	}
	return 0;
}