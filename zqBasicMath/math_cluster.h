/***********************************************************/
/**	\file
	\brief		cluster algorithm
	\details	cluster
	\author		Zhiqi Li
	\date	    1/20/2022

*/
/***********************************************************/
#ifndef __MATH_CLUSTER_H__
#define __MATH_CLUSTER_H__

#include <assert.h>
#include <iostream>
#include <math.h>
#include <zqBasicMath/math_vector.h>
#include <zqBasicMath/math_matrix.h>
#include <zqBasicMath/math_distance.h>
#include <zqBasicMath/math_vector_utils.h>
#include<ResearchM_config.h>
#ifdef  RESEARCHM_ENABLE_CUDA
#include <zqBasicUtils/utils_cuda.h>
#include <zqBasicUtils/utils_array.h>
#endif //  RESEARCHM_ENABLE_CUDA

namespace zq {
	namespace math {
		//https://zhuanlan.zhihu.com/p/104355127
		enum CenterType {
			RANDCENT
		};
		class CenterChoose {
		public:
			CenterType center_type = CenterType::RANDCENT;
			CenterChoose() {}
			CenterChoose(CenterType center_type):center_type(center_type){}
		public:
			template<class T,int side>
			void findCenter(
				const Array<T*, side>& data,
				int d,
				int k,
				Array<T*,side>& center
			) {
				center.resize(k);
				for (int i = 0; i < k; i++) {
					if constexpr (side == HOST) {
						center[i] = new T[d];
					}
					else if constexpr (side == DEVICE) {
						cudaNew(center[i], sizeof(T) * d);
					}
				}
#ifdef RESEARCHM_ENABLE_CUDA
				T** data_ptr = thrust::raw_pointer_cast(&data[0]);
				Array<T*, side> host_center(k);
				if constexpr (side == DEVICE) {
					for (int i = 0; i < k; i++) {
						host_center[i] = new T[d];
					}
				}
#else
				T** data_ptr = &data[0];
#endif
				for(int i=0;i<d;i++){
					Array<T, side> dim_data(data.size());
					auto phi= [data_ptr] 
#ifdef RESEARCHM_ENABLE_CUDA
						__device__ __host__ 
#endif
					(int idx)->T {
						return data_ptr[idx][i];
					}
					zq::utils::Calc_Each<decltype(phi),side>(
						phi
						,dim_data
					);
					T min_val, max_val;
					zq::utils::Array_Min<T,side>(dim_data,min_val);
					zq::utils::Array_Max<T, side>(dim_data, max_val);
					for (int j = 0; j < k; j++) {
						if constexpr (side == DEVICE) {
							host_center[j][i] = zq::randNumber<T, T>(min_val, max_val);
						}
						else if constexpr (side == HOST) {
							center[j][i] = zq::randNumber<T, T>(min_val, max_val);
						}
					}
				}
				if constexpr (side == DEVICE) {
					for (int i = 0; i < k; i++) {
						cpyh2d(center[i], host_center[i], sizeof(T) * d);
					}
				}
#ifdef RESEARCHM_ENABLE_CUDA
				if constexpr (side == DEVICE) {
					for (int i = 0; i < k; i++) {
						delete[] host_center[i];;
					}
				}
#endif
			}

			template<class T,int d,int side>
			void findCenter(
				const Array<Vec<T,d>, side>& data,
				int k,
				Array<Vec<T, d>, side>& center
			) {
				center.resize(k);
				auto data_ptr = get_ptr<Vec<T,d>,side>(data);
				Array<Vec<T,d>, HOST> host_center(k);
				for (int i = 0; i < d; i++) {
					Array<T, side> dim_data(data.size());
					auto phi = [data_ptr,i]
#ifdef RESEARCHM_ENABLE_CUDA
						__device__ __host__
#endif
						(int idx)->T {
						return data_ptr[idx][i];
					};
					zq::utils::Calc_Each<decltype(phi), side>(
						phi
						, dim_data
						);
					T min_val, max_val;
					zq::utils::Array_Min<T, side>(dim_data, min_val);
					zq::utils::Array_Max<T, side>(dim_data, max_val);
					for (int j = 0; j < k; j++) {
						host_center[j][i] = zq::randNumber<T, T>(min_val, max_val);
					}
				}
				center = host_center;
			}
		};

		struct ClusterPara {
			int cluster_num=1;
			CenterChoose center_choose= CenterChoose();
			DistType dist_type=DistType::EUCLIDEAN;
			ClusterDistType cluster_dist_type=ClusterDistType::SINGLELINK;
			int p=2;
			int max_iter = 1000;
		};
		
		template<class T,int side>
		void kMeansCluster(
			Array<T*,side> data,
			int d,
			ClusterPara para,
			Array<int ,side>& clusters,
			Array<T*, side>& center
		) {
			clusters.resize(data.size());
			/// find center
			center_choose.findCenter(data,d,para.cluster_num,center);
#ifdef RESEARCHM_ENABLE_CUDA
			T** center_ptr = thrust::raw_pointer_cast(&center[0]);
			T** data_ptr = thrust::raw_pointer_cast(&datas[0]);
#else
			T** center_ptr = &center[0];
			T** data_ptr = &data[0];
#endif
			bool cluster_change = true;s
			while (cluster_change) {
				cluster_change = false;
				auto phi = [center_ptr, data_ptr](const int idx) {
					int index = -1;
					T dist;
					for (int i = 0; i < k; i++) {
						T tem=distance(center_ptr[i],data_ptr[idx]);
						if (index == -1 && tem < dist) {
							dist = tem;
							index = i;
						}
					}
					return indexs;
				};
				zq::utils::Calc_Each<decltype(phi),side>(
					phi
					,clusters
				);
				/// Then calculate the center and find if changeged 
				/// TOFIX, here could not be run on DEVICE
				for (int i = 0; i < data.size(); i++) {

				}
			}
		}

		template<int side>
		bool clusterChange(
			const Array<int, side>& clusters,
			const Array<int, side>& clusters_save
		) {
			Array<int, side> cluster_changes(clusters.size());
			auto clusters_ptr = get_ptr<int,side>(clusters);
			auto clusters_save_ptr = get_ptr<int,side>(clusters_save);
			auto change_phi = [clusters_save_ptr,clusters_ptr]
#ifdef RESEARCHM_ENABLE_CUDA
				__device__ __host__
#endif
			(const int idx)->int {
				if (clusters_save_ptr[idx] == clusters_ptr[idx]) return 0;
				return 1;
			};
			zq::utils::Calc_Each<decltype(change_phi), side>(
				change_phi,
				cluster_changes
			);
			int change;
			zq::utils::Array_Sum<int, side>(cluster_changes, change);
			return change != 0 ? true : false;
		}

		/// Please note, for this function, 
		/// the cluster will be emerged in HOST side
		template<class T, int d, int side>
		void clusterCenter(
			const Array<Vec<T, d>, side>& data,
			const Array<int, side>& clusters,
			int k,
			Array<Vec<T, d>, side>& center			
		) {
			Array<Vec<T, d>, HOST> center_host(k);
			auto clusters_ptr = get_ptr<int,side>(clusters);
			auto data_ptr = get_ptr<Vec<T,d>,side>(data);
			for (int i = 0; i < k; i++) {
				Array<Vec<T, d>, side> groups(data.size());
				Array<int, side> groups_count(data.size());
				auto count = [clusters_ptr,i]
#ifdef RESEARCHM_ENABLE_CUDA
					__device__ __host__
#endif
				(const int idx)->int {
					return clusters_ptr[idx] == i ? 1: 0;
				};
				auto phi = [clusters_ptr,data_ptr,i]
#ifdef RESEARCHM_ENABLE_CUDA
					__device__ __host__
#endif
				(const int idx)->Vec<T,d> {
					return clusters_ptr[idx] == i ? data_ptr[idx] : Vec<T,d>();
				};
				zq::utils::Calc_Each(
					phi,
					groups
				);
				zq::utils::Calc_Each(
					count,
					groups_count
				);
				int n;
				zq::utils::Array_Sum<Vec<T,d>,side>(groups, center_host[i]);
				zq::utils::Array_Sum<int, side>(groups_count, n);
				center_host[i] /= n * 1.0;
			}
			center = center_host;
		}

		template<class T, int d, int side>
		void clusterNearCenter(
			const Array<Vec<T, d>, side>& center,
			const Array<Vec<T, d>, side>& data,
			const ClusterPara& para,
			Array<int, side>& clusters
		) {
			auto center_ptr = get_ptr<Vec<T, d>,side>(center);
			auto data_ptr = get_ptr<Vec<T, d>,side>(data);

			auto phi = [center_ptr, data_ptr,para]
#ifdef RESEARCHM_ENABLE_CUDA
				__device__ __host__
#endif
			(const int idx)->int {
				int index = -1;
				T dist;
				for (int i = 0; i < para.cluster_num; i++) {
					T tem = distance<T,d>(center_ptr[i], data_ptr[idx], para.dist_type);
					if (index == -1 ||  tem < dist) {
						dist = tem;
						index = i;
					}
				}
				return index;
			};
			zq::utils::Calc_Each<decltype(phi), side>(
				phi
				, clusters
			);
		}


		template<class T,int d, int side>
		void kMeansCluster(
			Array<Vec<T,d>, side> data,
			ClusterPara para,
			Array<int, side>& clusters,
			Array<Vec<T, d>, side>& center
		) {
			Array<int, side> save_clusters;
			clusters.resize(data.size());
			save_clusters.resize(data.size());
			para.center_choose.findCenter<T,d,side>(data,para.cluster_num, center);			
			auto center_ptr = get_ptr<Vec<T,d>,side>(center);
			auto data_ptr = get_ptr<Vec<T, d>,side>(data);

			bool cluster_change = true;
			for(int i=0;i<para.max_iter && cluster_change;i++) {
				cluster_change = false;
				clusterNearCenter<real,d,side>(
					center,
					data,
					para,
					clusters
				);
				cluster_change=clusterChange<side>(clusters, save_clusters);
				clusters = save_clusters;
				clusterCenter<real,d,side>(
					data,
					clusters,
					para.cluster_num,
					center
				);
			}
		}
	}
}

#endif	//	__ZQ_VECTOR_H__