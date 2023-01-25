/***********************************************************/
/**	\file
	\brief		mapper 
	\details	mapper, reproduction of Topological Methods for the Analysis of High Dimensional Data Sets and 3D Object Recognition
	\author		Zhiqi Li
	\date	    1/22/2022

*/
/***********************************************************/
#ifndef __MATH_MAPPER_H__
#define __MATH_MAPPER_H__

#include <assert.h>
#include <iostream>
#include <math.h>
#include <zqBasicMath/math_vector.h>
#include <zqBasicMath/math_matrix.h>
#include <zqBasicMath/math_distance.h>
#include <zqBasicMath/math_cluster.h>
#include <zqBasicMath/math_filter.h>
#include <zqBasicMath/math_vector_utils.h>
#include <zqBasicMath/math_homology.h>
#include<ResearchM_config.h>
#include<unordered_set>
#include <zqBasicUtils/utils_array.h>
#ifdef  RESEARCHM_ENABLE_CUDA
#include <zqBasicUtils/utils_cuda.h>
#include <zqBasicUtils/utils_array.h>
#endif //  RESEARCHM_ENABLE_CUDA

namespace zq {
	namespace math {


		template<class Filter,ClusterType ct= ClusterType::KMEANS>
		class Mapper {
		public:
			Filter filter;

			template<class T, int d, int side>
			void FilterPoints(
				const Array<Vec<T, d>, side>& points,
				const Array<Vec<T, Filter::res_dim>, side>& interval_l,
				const Array<Vec<T, Filter::res_dim>, side>& interval_u,
				Array<Vec<T, Filter::res_dim>, side>& filter_res,
				Array<Array<int, side>>& index_class,
				Array<Array<Vec<T, d>, side>>& ori_points_class
			) {
				filter_res.resize(points.size());
				auto points_ptr = get_ptr<Vec<T, d>, side>(points);
				Filter filter_local = filter;
				auto filter_phi = [points_ptr, filter_local]
#ifdef RESEARCHM_ENABLE_CUDA
					__device__ __host__
#endif
				(const int idx)->Vec<T, Filter::res_dim> {
					return filter_local(points_ptr[idx]);
				};
				zq::utils::Calc_Each<decltype(filter_phi), side>(
					filter_phi
					, filter_res
				);

				/// Because Array could not be used in GPU,
				/// So the parallel of this part only support CPU
				Array<Array<int, HOST>> index_class_host(interval_l.size());
				Array<Array<Vec<T, d>, HOST>> ori_points_class_host(interval_l.size());
				Array<Vec<T, Filter::res_dim>, HOST> filter_res_host = filter_res;
				Array<Vec<T, d>, HOST> points_host = points;
				Array<Vec<T, Filter::res_dim>, HOST> interval_l_host = interval_l;
				Array<Vec<T, Filter::res_dim>, HOST> interval_u_host = interval_u;
				auto group_phi = [&](const int idx) {
					for (int i = 0; i < filter_res_host.size(); i++) {
						bool valid = true;
						for (int j = 0; j < Filter::res_dim; j++) {
							if (filter_res_host[i][j] < interval_l_host[idx][j] ||
								filter_res_host[i][j] > interval_u_host[idx][j]) {
								valid = false;
								break;
							}
						}
						if (valid) {
							ori_points_class_host[idx].push_back(points_host[i]);
							index_class_host[idx].push_back(i);
						}
					}
				};
				zq::utils::Exec_Each<decltype(group_phi), Vec<T, Filter::res_dim>, side>(
					group_phi
					, interval_l_host
				);
				/// Then copy to side
				index_class.resize(index_class_host.size());
				ori_points_class.resize(index_class_host.size());
				for (int i = 0; i < index_class_host.size();i++) {
					index_class[i] = index_class_host[i];
					ori_points_class[i] = ori_points_class_host[i];
				}
			}

			template<class T,int d, int side>
			void ClusterByGroup(
				int total_points_number,
				const Array<Array<Vec<T, d>, side>>& ori_points_class,
				const Array<Array<int, side>>& index_class,
				ClusterPara para,
				Array<Array<int, side>>& clusters_map,
				Array<int, side>& clusters_map_num,
				Array<Vec<T, d>, side>& center
			) {
				clusters_map.resize(Filter::res_dim * 2);
				Array<int*, side> clusters_map_ptr(Filter::res_dim * 2);
				int** clusters_map_ptr_ptr = get_ptr<int*, side>(clusters_map_ptr);
				for (int i = 0; i < clusters_map.size(); i++) {
					clusters_map[i].resize(total_points_number);
					zq::fill<int>(clusters_map[i].begin(), clusters_map[i].end(), -1);
					clusters_map_ptr[i] = get_ptr<int, side>(clusters_map[i]);
				}

				clusters_map_num.resize(total_points_number);
				auto clusters_map_num_ptr = get_ptr<int, side>(clusters_map_num);

				for (int i = 0; i < ori_points_class.size(); i++) {
					Array<int, side> clusters_local;
					Array<Vec<real, d>, side> center_local;
					if constexpr (ct == ClusterType::KMEANS) {
						kMeansClusterGap<zq::real, d, side>(
							ori_points_class[i],
							para,
							clusters_local,
							center_local
						);
					}

					int ori_num = center.size();
					auto index_class_ptr = get_ptr<int, side>(index_class[i]);
					auto clusters_local_ptr = get_ptr<int, side>(clusters_local);
					auto phi = [clusters_map_num_ptr, clusters_map_ptr_ptr, index_class_ptr, clusters_local_ptr, ori_num]
#ifdef RESEARCHM_ENABLE_CUDA
						__device__ __host__
#endif
					(const int idx)->void {
						int area_index = clusters_map_num_ptr[index_class_ptr[idx]];
						clusters_map_ptr_ptr[area_index][index_class_ptr[idx]] = clusters_local_ptr[idx] + ori_num;
						clusters_map_num_ptr[index_class_ptr[idx]]++;
					};
					zq::utils::Exec_Each<decltype(phi),int, side>(
						phi
						, clusters_local
					);
					center.resize(center.size() + center_local.size());
					zq::copy<Vec<T, d>, side>(center_local.begin(), center_local.end(), center.begin() + ori_num);
				}
			}

			template<class T, int d, int side>
			void Interval(
				const Vec<T,d>& ub,
				const Vec<T, d>& lb,
				const Vec<int,d>& slices,
				T overlap,
				Array<Vec<T, d>, side>& interval_u,
				Array<Vec<T, d>, side>& interval_l
			) {
				Vec<T, d> step = ub - lb;
				for (int i = 0; i < d; i++) {
					step[i] /= slices[i];
				}
				int number = slices.Mul();
				interval_l.resize(number);
				interval_u.resize(number);
				auto interval_l_ptr = get_ptr<Vec<T,d>,side>(interval_l);
				auto interval_u_ptr = get_ptr<Vec<T, d>, side>(interval_u);
				Vec<int, d> slices_local = slices;
				Vec<T, d> lb_local = lb;
				auto phi = [interval_l_ptr, interval_u_ptr, slices_local, lb_local, step, overlap]
#ifdef RESEARCHM_ENABLE_CUDA
					__device__ __host__
#endif
				(const int idx) {
					Vec<int, d> coord;
					slices_local.Grid(idx, coord);
					for (int i = 0; i < d; i++) {
						interval_l_ptr[idx][i] =  coord[i] * step[i] - step[i] * (overlap / 2);
						interval_u_ptr[idx][i] = (coord[i] + 1) * step[i] + step[i] * (overlap / 2);
					
					}
					interval_l_ptr[idx] += lb_local;
					interval_u_ptr[idx] += lb_local;
				};
				zq::utils::Exec_Each<decltype(phi), Vec<T, d>, side>(
					phi,
					interval_l
				);
			}

			template<class T,int d,int side>
			void AddComplex(
				const Array<Array<int, side>>& clusters_map,
				const Array<int, side>& clusters_map_num,
				const Array<Vec<T, d>, side>& center,
				Simplical_Complex<Vec<T, d>>& complex
			) {
				complex.points = center;
				std::unordered_set<Simplex<int>> simplex_set;
				for (int i = 0; i < center.size(); i++) {
					complex.simplex.push_back(Simplex<int>(std::vector<int>{i}));
					simplex_set.insert(Simplex<int>(std::vector<int>{i}));
				}
				for (int i = 0; i < clusters_map_num.size(); i++) {
					if constexpr (Filter::res_dim == 1) {
						if (clusters_map_num[i] == 2) {
							complex.AddSimplex(Simplex<int>(std::vector<int>{clusters_map[0][i], clusters_map[1][i]}), simplex_set);
						}
					}
					else if constexpr (Filter::res_dim == 2) {
						if (clusters_map_num[i] == 2) {
							complex.AddSimplex(Simplex<int>(std::vector<int>{clusters_map[0][i], clusters_map[1][i]}), simplex_set);
						}
						else if (clusters_map_num[i] == 3) {
							complex.AddSimplex(Simplex<int>(std::vector<int>{clusters_map[0][i], clusters_map[1][i], clusters_map[2][i]}), simplex_set);
						}
						else if (clusters_map_num[i] == 4) {
							complex.AddSimplex(Simplex<int>(std::vector<int>{clusters_map[0][i], clusters_map[1][i], clusters_map[2][i], clusters_map[3][i]}), simplex_set);
						}
					}
				}
			}
			template<class T,int d, int side>
			void DoMapper(
				const Array<Vec<T, d>, side>& points,
				const Array<Vec<T, Filter::res_dim>, side>& interval_l,
				const Array<Vec<T, Filter::res_dim>, side>& interval_u,
				ClusterPara para,
				Array<Array<int, side>>& clusters_map,
				Array<Vec<T, Filter::res_dim>, side>& filter_res,
				Simplical_Complex<Vec<T, d>>& complex,
				Array<Array<Vec<T, d>, side>>& ori_points_class
			) {
				Array<Array<int, side>> index_class;
				Array<Vec<T, d>, side> center;

				FilterPoints<T, d, side>(
					points,
					interval_l,
					interval_u,
					filter_res,
					index_class,
					ori_points_class
				);

				Array<int, side> clusters_map_num;
				ClusterByGroup<T, d, side>(
					points.size(),
					ori_points_class,
					index_class,
					para,
					clusters_map,
					clusters_map_num,
					center
				);
				
				/// Then add the vertex
			    /// Only res_dim is 1 or 2 is available
				/// 
				AddComplex<T, d, side>(
					clusters_map,
					clusters_map_num,
					center,
					complex
				);
				
			}
		};
	}
}

#endif	//	__ZQ_VECTOR_H__