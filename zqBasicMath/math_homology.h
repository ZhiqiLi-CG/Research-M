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
#include<set>
#include<algorithm>
#include<functional>
#include <zqBasicMath/math_type_promote.h>
#include <zqBasicMath/math_dense.h>
#include <zqBasicMath/math_dense_solver.h>

namespace zq{

/**
	Homology solver
*/
	template<typename T> // T should be index or coordinate
	class Simplex {
	public:
		std::vector<T> points;
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
				for (int i = 0; i < points.size(); i++) {
					if (points[i] < rhs.points[i]) return true;
					else if (points[i] < rhs.points[i]) return false;
				}
				return false;
			}
		}
		int Dim() const{
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

		static bool CheckFace(const Simplex<int>& child, const Simplex<int>& fa) {
			std::set<int> fa_set(fa.points.begin(),fa.points.end());
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
				return _Recusive_SmithNorm(boundary_m,cur+1);
			}
			else {
				//Run out of nonzero entries so done.
				return cur;
			}
		}
		// Persistent Homology — a Survey, Herbert Edelsbrunnerand John Harer
		static DenseMatrix<int> ReduceBoundaryMatrix_PersistancePair(const DenseMatrix<int>& boundary_m) {

		}
	};
	// Calculate Distance Function, which is provided for you to define by yourself
	template<typename Type,typename ReturnType>
	ReturnType Dinstance(const Type& p1,const Type& p2) {
		return (p1 - p2).Length();
	}
	void _FindSubsets(
		std::vector<int>& subset, 
		const std::vector<int>& index_set, 
		int k, 
		int start,
		const std::function<bool(const std::vector<int>&)>& filter,
		std::vector<std::vector<int>>& results
	) {
		if (!filter(subset)) return;
		if (subset.size() == k) {
			results.push_back(subset);
			return;
		}
		for (int i = start; i < index_set.size(); i++) {
			subset.push_back(index_set[i]);
			_FindSubsets(subset, index_set, k, i + 1,filter,results);
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
	
	// Afra Zomorodian algorithm: find the VR complex
	// Note: This function is not suitable for complex with high dimensional
	template<typename ParaType,typename PointType>
	void VRComplexConstruct(
		const ParaType& epsilon,
		const std::vector<PointType>& points,
		int k,
		std::vector<std::vector<int>>& results
	) {
		std::vector<int> index_set(points.size());
		for (int i = 0; i < index_set.size(); i++) index_set[i] = i;
		std::function<bool(const std::vector<int>&)>& filter = [&](const std::vector<int>& index) {
			for (int i = 0; i < index.size(); i++) {
				for (int j = i + 1; j < index.size(); j++) {
					if (Dinstance(points[i], points[j]) > epsilon) return false;
				}
			}
			return true;
		}
		for (int i = 1; i <= k; i++) {
			FilterTuples(index_set, filter, i, results);
		}
	}
}	

#endif	//	__MATH_HOMOLOGY_H__