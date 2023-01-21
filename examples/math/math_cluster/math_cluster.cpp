/***********************************************************/
/**	
\file
\brief		Example of cluster
\details	test the use of cluster
\author		Zhiqi Li
\date		1/20/2022
*/
/***********************************************************/
#include <iostream>
#include<zqBasicMath/math_distance.h>
#include<zqBasicMath/math_cluster.h>
#include<ResearchM_config.h>

//https://blog.csdn.net/playgoon2/article/details/77579757
zq::real data2D_raw[] = {-1.26, 0.46, -1.15, 0.49, -1.19, 0.36, -1.33, 0.28, -1.06, 0.22, -1.27, 0.03, -1.28, 0.15, -1.06, 0.08, -1.00, 0.38, -0.44, 0.29, -0.37, 0.45, -0.22, 0.36, -0.34, 0.18, -0.42, 0.06, -0.11, 0.12, -0.17, 0.32,
 -0.27, 0.08, -0.49, -0.34, -0.39, -0.28, -0.40, -0.45, -0.15, -0.33, -0.15, -0.21, -0.33, -0.30, -0.23, -0.45, -0.27, -0.59, -0.61, -0.65, -0.61, -0.53, -0.52, -0.53, -0.42, -0.56, -1.39, -0.26};
zq::Array<zq::Vec<zq::real, 2>> data2D;

int example= 0;

int main() {
	/// First generate data
	for (int i = 0; i < sizeof(data2D_raw) / sizeof(zq::real); i += 2) {
		data2D.push_back(zq::Vec<zq::real, 2>(data2D_raw[i], data2D_raw[i + 1]));
	}
	/// Select data
	printf("Select the exmample:");
	//scanf("%d", &example);
	if (example==0) {
		zq::math::ClusterPara para;
		para.cluster_num = 4;
		zq::Array<int, HOST> clusters;
		zq::Array<zq::Vec<zq::real, 2>, HOST> center;
		zq::math::kMeansCluster<zq::real, 2, HOST>(
			data2D,
			para,
			clusters,
			center
		);

		printf("Center:\n");
		for (int i = 0; i < center.size(); i++) {
			printf("(%f,%f)\t", center[i][0], center[i][1]);
		}

		printf("Cluster:\n");
		for (int i = 0; i < clusters.size(); i++) {
			printf("%d:%d\t", i,clusters[i]);
		}

	}
	return 0;
}