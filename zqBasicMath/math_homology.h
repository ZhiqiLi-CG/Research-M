/***********************************************************/
/**	\file
	\brief		homologys
	\details	calculate the betti number, persistent homology,...
	\author		Zhiqi Li, Yu Chen
	\date	    12/25/2022

*/
/***********************************************************/
#ifndef __MATH_HOMOLOGY_H__
#define __MATH_HOMOLOGY_H__

#include <assert.h>
#include <iostream>
#include <math.h>
#include<unordered_map>
#include<set>
#include<algorithm>
#include<functional>
#include <zqBasicMath/math_type_promote.h>
#include <zqBasicMath/math_dense.h>
#include <zqBasicMath/math_dense_solver.h>
namespace zq {

	/**
		Homology solver
	*/
	template<typename T> // T should be index or coordinate
	class Simplex {
	public:
		std::vector<T> points;
		int simplex_index = -1; /// for complex to sort simplex
		Simplex() {}
		template<typename TYPE> Simplex(const TYPE* data, int dim_num) {
			points.resize(dim_num);
			points.assign(data, data + dim_num);
		}
	public:
		void FindSubSimplex(std::vector<Simplex<T>>& subsimplex) {
			std::vector<bool> use(subsimplex.size());
			while (AddIndex(use)) {
				subsimplex.push_back(CreateSimplexByIndex(use));
			}
			SortSimplex(subsimplex);
		}
		bool operator< (const Simplex& rhs)  const
		{
			if (points.size() < rhs.points.size()) return true;
			else if (points.size() > rhs.points.size()) return false;
			else {
				if (simplex_index < rhs.simplex_index) return true;
				else if (simplex_index > rhs.simplex_index) return false;
				for (int i = 0; i < points.size(); i++) {
					if (points[i] < rhs.points[i]) return true;
					else if (points[i] > rhs.points[i]) return false;
				}
				return false;
			}
		}
		int Dim() const {
			return points.size();
		}
		void SortSimplex(std::vector<Simplex<T>>& subsimplex) {
			std::sort(subsimplex.begin(), subsimplex.end());
		}
		T index(int i) const {
			if (i >= points.size()) {
				throw "index out of range";
			}
			return points[i];
		}
	public:
		// hash

	private:
		bool AddIndex(std::vector<bool>& index) {
			std::vector<bool> tem_index = index;
			for (int i = 0; i < tem_index.size(); i++) {
				if (tem_index[i] == false) {
					tem_index[i] = true;
					index = tem_index[i];
					return true;
				}
				else {
					tem_index[i] = false;
				}
			}
			return false;
		}
		bool AddIndexWithOrder(std::vector<bool>& index) {
			std::vector<bool> tem_index = index;
			for (int i = 0; i < tem_index.size(); i++) {
				if (tem_index[i] == false) {
					tem_index[i] = true;
					index = tem_index[i];
					return true;
				}
				else {
					tem_index[i] = false;
				}
			}
			return false;
		}
		Simplex<T> CreateSimplexByIndex(std::vector<bool>& index) {
			std::vector<T> new_points;
			for (int i = 0; i < index.size(); i++) {
				if (index[i]) {
					new_points.push_back(points[i]);
				}
			}
			return Simplex<T>(&new_points[0], new_points.size());
		}
	};
}
namespace std {
	template<>
	struct hash<zq::Simplex<int>> {
	public:
		size_t operator()(const zq::Simplex<int>& p) const
		{
			size_t hv = 0;
			for (int i = 0; i < p.Dim(); i++) {
				hv ^= hash<int>()(p.index(i));
			}
			return hv;
		}

	};

	template<>
	struct equal_to<zq::Simplex<int>> {//等比的模板定制
	public:
		bool operator()(const zq::Simplex<int>& p1, const zq::Simplex<int>& p2) const
		{
			if (p1.points.size() != p2.points.size()) return false;
			for (int i = 0; i < p1.points.size(); i++) {
				if (p1.points[i] != p2.points[i]) return false;
			}
			return true;
		}

	};
}
namespace zq{
	//Simplical_Complex is orgamized by index
	template<typename T> 
	class Simplical_Complex {
	public:
		std::vector<T> points;
		std::vector<Simplex<int>> simplex;
		Simplical_Complex() {}
		template<typename TYPE> Simplical_Complex(const TYPE* data, int dim_num) {
			points.resize(dim_num);
			points.assign(data, data + dim_num);
		}
		template<typename TYPE> Simplical_Complex(const TYPE* data, int dim_num,const std::vector<Simplex<int>>& simplex) {
			points.resize(dim_num);
			points.assign(data, data + dim_num);
			this->simplex = simplex;
		}
		template<typename TYPE> Simplical_Complex(const TYPE* data, int dim_num, const std::vector<std::vector<int>>& simplex) {
			points.resize(dim_num);
			points.assign(data, data + dim_num);
			this->simplex.resize(simplex.size());
			std::vector<std::vector<int>> tem_simplex = simplex;
			for (int i = 0; i < simplex.size(); i++) {
				this->simplex[i] = Simplex<int>(&tem_simplex[i][0], simplex[i].size());
			}
		}
	public:
		int SimplexNumber() const{
			return simplex.size();
		}
		int Dim()  const {
			return points.size();
		}

		void NChain(int n,std::vector<Simplex<int>>& result) {
			for (int i = 0; i < simplex.size(); i++) {
				if(simplex[i].Dim()==n+1){
					result.push_back(simplex[i]);
				}
			}
		}
		std::vector<int> BettiNumber(int n = -1) {
			std::vector<int> betti;
			std::vector<std::vector<Simplex<int>>> chains;
			chains.push_back(std::vector<Simplex<int>>());
			for (int i = 0;; i++) {
				std::vector<Simplex<int>> tem;
				NChain(i, tem);
				if (tem.size() != 0) {
					chains.push_back(tem);
				}
				else {
					break;
				}
			}
			if (n == -1) n = chains.size()-1;
			std::vector<int> rank(n+1); // 0,1,2,...,n-2
			std::vector<int> nullity(n+1);// 1,2,...,n-1
			for (int j = 0; j < n; j++) {
				if (j == chains.size() - 1) {
					rank[j] = nullity[j] = 0;
				}
				else {
					DenseMatrix<int> boundary_m =
						BoundaryMatrix(
							chains[j],
							chains[j + 1]
						);
					ReduceBoundaryMatrix_SmithNorm(
						boundary_m,
						rank[j],
						nullity[j]
					);
				}
			}
			betti.resize(n);
			for (int i = 0; i < n - 1; i++) {
				betti[i] = nullity[i] - rank[i + 1];
			}
			return betti;
		}
		DenseMatrix<int> BoundaryMatrix(int p,int n) {
			std::vector<Simplex<int>> pchain, nchain;
			NChain(p, pchain);
			NChain(n, nchain);
			return BoundaryMatrix(pchain,nchain);
		}
		DenseMatrix<int> ReducedBoundaryMatrix(
			int p, 
			int n,
			int& rank,
			int& nullity
		) {
			std::vector<Simplex<int>> pchain, nchain;
			NChain(p, pchain);
			NChain(n, nchain);
			DenseMatrix<int> boundary_m=BoundaryMatrix(pchain, nchain);
			return ReduceBoundaryMatrix_SmithNorm(
				boundary_m,
				rank,
				nullity
			);
		}
		DenseMatrix<int> BoundaryMatrix() {
			return BoundaryMatrix(simplex, simplex);
		}
		DenseMatrix<int> ReducedBoundaryMatrix(
			DenseMatrix<int>& memory_m
		) {
			DenseMatrix<int> boundary_m=BoundaryMatrix();
			//std::cout << boundary_m << std::endl;
			//printf("???? %d %d\n", Cols(boundary_m), Rows(boundary_m));
			return ReduceBoundaryMatrix_Echelon(
				boundary_m,
				memory_m
			);
		}
		static bool CheckReduced(const DenseMatrix<int>& boundary_m) {
			std::set<int> low_set;
			for (int i = 0; i < Cols(boundary_m); i++) {
				int low_i = Low(boundary_m, i);
				if (low_i != -1) {
					if (low_set.find(Low(boundary_m, i)) != low_set.end()) {
						throw "";
						return false;
					}
					low_set.insert(low_i);
				}
			}
			return true;
		}
		template<typename T>
		static void AssignSimplexIndex(
				std::vector<zq::Simplical_Complex<T>>& complex_list
			) {
			for (int i = 0; i < complex_list[0].simplex.size(); i++) {
				complex_list[0].simplex[i].simplex_index = 0;
			}
			for (int i = 1; i < complex_list.size(); i++) {
				std::unordered_map<Simplex<int>, int> simplex_map;
				for (int j = 0; j < complex_list[i-1].simplex.size(); j++) {
					simplex_map.insert(std::pair<Simplex<int>, int>(complex_list[i - 1].simplex[j],j));
				}
				for (int j = 0; j < complex_list[i].simplex.size(); j++) {
					auto iter = simplex_map.find(complex_list[i].simplex[j]);
					if ( iter== simplex_map.end()) {
						complex_list[i].simplex[j].simplex_index = i;
					}
					else {
						complex_list[i].simplex[j].simplex_index = complex_list[i - 1].simplex[iter->second].simplex_index;
					}
				}
			}
		}
		template<typename T>
		static void AssignSimplexIndexSort(
			std::vector<zq::Simplical_Complex<T>>& complex_list
		) {
			AssignSimplexIndex(complex_list);
			for (int i = 0; i < complex_list.size(); i++) {
				std::sort(complex_list[i].simplex.begin(), complex_list[i].simplex.end());
				//printf("----------------------------------------------------------------\n");
				//printf("complex_list index %d\n",i);
				//for (int j = 0; j < complex_list[i].SimplexNumber(); j++) {
					//printf("(");
					//for (int k = 0; k < complex_list[i].simplex[j].Dim(); k++) {
						//printf("%d ", complex_list[i].simplex[j].index(k));
					//}
					//printf(")--index:%d\t\t", complex_list[i].simplex[j].simplex_index);
				//}
			}
		}
		static void ReadIntervals(
			DenseMatrix<int> boundary_m,
			std::vector<std::pair<int, int>>& interval
		){
			std::unordered_map<int, int> start_map;
			for (int i = 0; i < Cols(boundary_m); i++) {
				int low_i=Low( boundary_m,i);
				if (low_i == -1) {
					interval.push_back(std::pair<int, int>(i, -1));
					start_map.insert(std::pair<int, int>(i, interval.size()-1));
				}
				else {
					auto iter=start_map.find(low_i);
					if (iter == start_map.end()) {
						interval.push_back(std::pair<int, int>(low_i, i));
						start_map.insert(std::pair<int, int>(low_i, interval.size() - 1));
					}
					else {
						interval[iter->second].second = i;
					}
				}
			}
		}
		static bool CheckUpperTriangleMatrix(const DenseMatrix<int>& boundary_m) {
			for (int i = 0; i < Rows(boundary_m); i++) {
				for (int j = 0; j < Cols(boundary_m); j++) {
					if (boundary_m[i][j] == 1 && i >= j) {
						return false;
					}
				}
			}
			return true;
		}
		static DenseMatrix<int> ReduceBoundaryMatrix_Echelon(
			const DenseMatrix<int>& boundary_m,
			DenseMatrix<int>& memory_m
		) {
			DenseMatrix<int> result_boundary_m = boundary_m;
			memory_m=IndentityMatrix<int>(Rows(result_boundary_m), Cols(result_boundary_m));
			for (int i = 0; i < Cols(result_boundary_m); i++) {
				bool end_loop = false;
				while (!end_loop) {
					end_loop = true;
					for (int j = 0; j < i; j++) {
						int low_i = Low(result_boundary_m, i);
						int low_j = Low(result_boundary_m, j);
						if (low_i==low_j && low_i!=-1) {
							MatrixSetCol(
								result_boundary_m,
								i,
								MatrixGetCol(result_boundary_m, i) ^ MatrixGetCol(result_boundary_m, j)
							);
							MatrixSetCol(
								memory_m,
								i,
								MatrixGetCol(memory_m, i) ^ MatrixGetCol(memory_m, j)
							);
							end_loop = false;
						}
					}

				}
			}
			return result_boundary_m;
		}
		static int Low(const DenseMatrix<int>& matrix, int index) {
			if (index >= Cols(matrix)) {
				throw "index out of range";
			}
			for (int i = Rows(matrix) - 1; i >= 0; i--) {
				if (matrix[i][index] == 1) return i;
			}
			return -1;
		}
		static bool CheckFace(const Simplex<int>& child, const Simplex<int>& fa) {
			std::set<int> fa_set(fa.points.begin(),fa.points.end());
			if (fa.Dim() != child.Dim() + 1) return false;
			for (int i = 0; i < child.Dim(); i++) {
				if (fa_set.find(child.index(i)) == fa_set.end()) {
					return false;
				}
			}
			return true;
		}
		static DenseMatrix<int> BoundaryMatrix(
			const std::vector<Simplex<int>>& pchain,
			const std::vector<Simplex<int>>& nchain
		) {
			//special -1
			if (pchain.size() == 0) {
				return DenseMatrix<int>(1, nchain.size());
			}
			DenseMatrix<int> boudary_m(pchain.size(), nchain.size());
			for (int i = 0; i < pchain.size(); i++) {
				for (int j = 0; j < nchain.size(); j++) {
					if (CheckFace(pchain[i], nchain[j])) {
						boudary_m[i][j] = 1;
					}
					else boudary_m[i][j] = 0;
				}
			}
			return boudary_m;
		}
		// https://zhuanlan.zhihu.com/p/41264363
		//https://triangleinequality.wordpress.com/2014/01/23/computing-homology/ 
		static DenseMatrix<int> ReduceBoundaryMatrix_SmithNorm( 
			const DenseMatrix<int>& boundary_m,
			int& rank,
			int& nullity
		) {
			DenseMatrix<int> result_boundary_m = boundary_m;
			if (Cols(boundary_m)==0 || Rows(boundary_m)==0) {
				rank = nullity = 0;
				return boundary_m;
			}
			rank=_Recusive_SmithNorm(result_boundary_m, 0);
			nullity = Cols(result_boundary_m) - rank;
			return result_boundary_m;
		}
		static int _Recusive_SmithNorm(
			DenseMatrix<int>& boundary_m,
			int cur
		) {
			//Searching for a nonzero entry then moving it to the diagonal.
			bool none_zero = false;
			for (int i = cur; i < Rows(boundary_m); i++) {
				for (int j = cur; j < Cols(boundary_m); j++) {
					if (boundary_m[i][j] == 1) {
						MatrixSwapRow(boundary_m, i, cur);
						MatrixSwapCol(boundary_m, j, cur);
						none_zero = true;
						break;
					}
				}
				if (none_zero) {
					break;
				}
			}
			//std::cout << "None zero:" << none_zero << std::endl;
			if (none_zero) {
				for (int i = cur + 1; i < Rows(boundary_m); i++) {
					if (boundary_m[i][cur] == 1) {
						//std::cout << "before cur:" << MatrixGetRow(boundary_m, cur) << std::endl;
						//std::cout << "before i:" << MatrixGetRow(boundary_m, i) << std::endl;
						//std::cout << "after  xor i:" << (MatrixGetRow(boundary_m, cur) ^ MatrixGetRow(boundary_m, i)) << std::endl;
						MatrixSetRow(
							boundary_m,
							i,
							MatrixGetRow(boundary_m, cur) ^ MatrixGetRow(boundary_m, i)
						);

//std::cout << "after i:" << MatrixGetRow(boundary_m, i) << std::endl;
					}
				}
				for (int i = cur + 1; i < Cols(boundary_m); i++) {
					if (boundary_m[cur][i] == 1) {
						MatrixSetCol(
							boundary_m,
							i,
							MatrixGetCol(boundary_m, cur) ^ MatrixGetCol(boundary_m, i)
						);
					}
				}
				return _Recusive_SmithNorm(boundary_m, cur + 1);
			}
			else {
				//Run out of nonzero entries so done.
				return cur;
			}
		}
		// Persistent Homology — a Survey, Herbert Edelsbrunnerand John Harer
	};
	void _FindSubsets(
		std::vector<int>& subset,
		const std::vector<int>& index_set,
		int k,
		int start,
		const std::function<bool(const std::vector<int>&)>& filter,
		std::vector<std::vector<int>>& results
	) {
		//printf("_FindSubsets, subset.size():%d,k:%d,start:%d\n", subset.size(), k, start);
		if (!filter(subset)) return;
		if (subset.size() == k) {
			results.push_back(subset);
			return;
		}
		for (int i = start; i < index_set.size(); i++) {
			subset.push_back(index_set[i]);
			_FindSubsets(subset, index_set, k, i + 1, filter, results);
			subset.pop_back();
		}
	}
	void FilterTuples(
		const std::vector<int>& index_set,
		const std::function<bool(const std::vector<int>&)>& filter,
		int k,
		std::vector<std::vector<int>>& results
	) {
		std::vector<int> subset;
		_FindSubsets(subset, index_set, k, 0, filter, results);
	}
	// Calculate Distance Function, which is provided for you to define by yourself
	template<typename Type>
	float Distance(const Type& p1, const Type& p2) {
		return (p1 - p2).Length();
	}
	// Afra Zomorodian algorithm: find the VR complex
	// Note: This function is not suitable for complex with high dimensional
	template<typename ParaType, typename PointType>
	void VRComplexConstruct(
		const ParaType& epsilon,
		const std::vector<PointType>& points,
		int k,
		std::vector<std::vector<int>>& results
	) {
		std::vector<int> index_set(points.size());
		for (int i = 0; i < index_set.size(); i++) index_set[i] = i;
		std::function<bool(const std::vector<int>&)> filter = [&](const std::vector<int>& index) {
			for (int i = 0; i < index.size(); i++) {
				for (int j = i + 1; j < index.size(); j++) {
					//std::cout<<points[i] << ","<< points[j] << "," << epsilon<<"\t";
					//printf("%f,%f\t", Distance(points[index[i]], points[index[j]]), epsilon);
					if (Distance(points[index[i]], points[index[j]]) > epsilon) return false;
				}
			}
			return true;
		};
		//printf("points.size():%d\n", points.size());
		for (int i = 1; i <= k; i++) {
			FilterTuples(index_set, filter, i, results);
		}
		//printf("results.size():%d\n", results.size());
	}
	template<typename Type>
	void CalculatePersistentData(
		const std::vector<float>& epsilon_list,
		float max_epsilon,
		std::vector<Simplical_Complex<Type>>& complex_list,
		std::vector<std::pair<float, float>>& epsilon_interval,
		std::vector<int>& feture_type
	) {
		Simplical_Complex<Type>::AssignSimplexIndexSort(complex_list);
		DenseMatrix<int> memory_m;
		int final_index = complex_list.size() - 1;
		DenseMatrix<int> boundary_m = complex_list[final_index].ReducedBoundaryMatrix(memory_m);

		//std::cout << "boundary_m:" << std::endl;
		//std::cout << boundary_m << std::endl;
		std::vector<std::pair<int, int>> interval;
		Simplical_Complex<Type>::ReadIntervals(boundary_m, interval);
		if(!Simplical_Complex<Type>::CheckReduced(boundary_m)){
			throw "bouandry_m is not redueced";
		}
		
		for (int i = 0; i < interval.size(); i++) {
			//printf("interval:%d %d\n", interval[i].first, interval[i].second);
			feture_type.push_back(complex_list[final_index].simplex[interval[i].first].Dim());
		}
		for (int i = 0; i < interval.size(); i++) {
			
			int start_index = complex_list[final_index].simplex[interval[i].first].simplex_index;
			//printf("start_index:%d,", start_index);
			float start_epsilon = epsilon_list[start_index];
			float end_epsilon;
			if (interval[i].second != -1) {
				int end_index = complex_list[final_index].simplex[interval[i].second].simplex_index;
				end_epsilon = epsilon_list[end_index];
			}
			else end_epsilon = max_epsilon;			
			
			//printf("%d,start_epsilon, end_epsilon:%f %f\n", feture_type[i], start_epsilon, end_epsilon);
			epsilon_interval.push_back(std::pair<float, float>(start_epsilon, end_epsilon));
		}
	}
}	

#endif	//	__MATH_HOMOLOGY_H__