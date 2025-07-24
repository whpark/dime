module;

/**************************************************************************\
 * Copyright (c) Kongsberg Oil & Gas Technologies AS
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
\**************************************************************************/


//=============================================================================
// forked from coin3d/dime
//
// This project is a refactoring of the original project "coin3d/dime".
// Significant modifications were made to improve code structure,
// performance, and maintainability while preserving the original project's core
// functionality and license terms.
//
// this dime.gtl project is a part of the project "gtl".
// this compiles only inside "gtl"
// whpark. 2025-07-24
//=============================================================================

export module dime.gtl:util.BSPTree;
import std;
import :Basic;

export namespace dime {

	class dime_bspnode {
	public:
		dime_bspnode(std::vector<dimeVec3f>& array) : pointsArray(array) {}
		~dime_bspnode() {}

		void clear() {
			left.reset();
			right.reset();
			dimension = DIM_NONE;
			position = {};
			indices.clear();
		}

		int addPoint(const dimeVec3f& pt, int maxpts);
		int findPoint(const dimeVec3f& pt) const;
		int removePoint(const dimeVec3f& pt);

	private:
		void sort();
		void split();
		bool leftOf(const dimeVec3f& pt) const {
			return double(pt[this->dimension]) < this->position;
		}

		enum DIM {
			// do not change these values!
			DIM_YZ = 0,
			DIM_XZ = 1,
			DIM_XY = 2,
			DIM_NONE
		};

		std::unique_ptr<dime_bspnode> left, right;
		DIM dimension{DIM_NONE};   // which dimension?
		// position in dimension (use double to avoid floating point
		// precision problems)
		double position{};
		std::vector<int> indices;
		std::vector<dimeVec3f>& pointsArray;
	};


	class dimeBSPTree {
	public:

		/*!
		Constructor. Will create an empty BSP tree with one node.
		\a maxnodepts is the maximum number of points in a BSP
		node. \a initsize is the initial size of the arrays that 
		holds the coordinates and user data.
		*/
		dimeBSPTree(int maxnodepts = 64, int initsize = 4)
			: pointsArray(initsize), userdataArray(initsize), topnode(pointsArray), maxnodepoints(maxnodepts)
		{ }
		~dimeBSPTree() {}

		/*!
		Returns the number of points in the BSP tree.
		*/
		int numPoints() const { return this->pointsArray.size(); }

		/*!
		Returns the coordinates for the point at index \a idx.
		\sa dimeBSPTree::numPoints()
		*/
		void getPoint(int idx, dimeVec3f& pt) {
			ASSERT(idx < pointsArray.size());
			pt = this->pointsArray[idx];
		}

		/*!
		Returns the user data for the point at index \a idx.
		*/
		void* getUserData(int idx) const {
			ASSERT(idx < this->userdataArray.size());
			return this->userdataArray[idx];
		}

		/*!
		Sets the user data for the point with index \a idx.
		*/
		void setUserData(int idx, void* const data) {
			ASSERT(idx < this->userdataArray.size());
			this->userdataArray[idx] = data;
		}

		/*!
		Attempts to add a new point to the BSP tree. If a point
		with the same coordinates as \a pt already is in the tree,
		the index to that point will be returned. Otherwise, the
		point is appended at the end of the list of points, the user data 
		is set, and the new index is returned.
		*/
		int addPoint(const dimeVec3f& pt, void* const userdata = nullptr) {
			this->boundingBox.UpdateBoundary(pt);
			int ret = this->topnode.addPoint(pt, this->maxnodepoints);
			if (ret == this->userdataArray.size()) {
				this->userdataArray.push_back(userdata);
			}
			return ret;
		}
		/*!
		\overload
		*/
		int removePoint(const dimeVec3f& pt) {
			return topnode.removePoint(pt);
		}

		/*!
		Removes the point at \a index. The BSP tree will not be
		restructured, no matter how many points you remove.
		*/
		void removePoint(int idx) {
			ASSERT(idx < this->pointsArray.size());
			this->removePoint(this->pointsArray[idx]);
		}

		/*!
		Searches for a point with coordinates \a pos. Returns 
		the index if found, -1 otherwise.
		*/
		int findPoint(const dimeVec3f& pos) const {
			return topnode.findPoint(pos);
		}
		void clear(int initsize = 4) {
			topnode.clear();
			this->pointsArray.clear();
			this->pointsArray.reserve(initsize);
			this->userdataArray.clear();
			this->userdataArray.reserve(initsize);
			this->boundingBox.SetRectEmptyForMinMax();
		}

		const dimeBox& getBBox() const {
			return this->boundingBox;
		}

	private:
		friend class dime_bspnode;
		std::vector<dimeVec3f> pointsArray;
		std::vector<void*> userdataArray;
		dime_bspnode topnode;
		int maxnodepoints;
		dimeBox boundingBox{};
	}; // class dimeBSPTree

}	// namespace dime

namespace dime {


	int dime_bspnode::addPoint(const dimeVec3f& pt, int maxpts) {
		if (this->left) { // node has been split
			if (this->leftOf(pt)) return this->left->addPoint(pt, maxpts);
			else return this->right->addPoint(pt, maxpts);
		}
		else if (this->indices.size() >= maxpts) {
			split();
			return this->addPoint(pt, maxpts);
		}
		else {
			int n = this->indices.size();
			int i;
			dimeVec3f tmp;
			for (i = 0; i < n; i++) {
				tmp = pointsArray[this->indices[i]];
				if (pt == tmp) break;
			}
			if (i == n) {
				int idx = this->pointsArray.size();
				this->pointsArray.push_back(pt);
				this->indices.push_back(idx);
				return idx;
			}
			return this->indices[i];
		}
	}

	int dime_bspnode::findPoint(const dimeVec3f& pt) const {
		if (this->left) {
			if (this->leftOf(pt)) return this->left->findPoint(pt);
			else return this->right->findPoint(pt);
		}
		else {
			int i, n = this->indices.size();
			for (i = 0; i < n; i++) {
				dimeVec3f arrpt = pointsArray[this->indices[i]];
				if (pt == arrpt) return this->indices[i];
			}
		}
		return -1;
	}

	int dime_bspnode::removePoint(const dimeVec3f& pt) {
		if (this->left) {
			if (this->leftOf(pt)) return this->left->removePoint(pt);
			else return this->right->removePoint(pt);
		}
		else {
			int i, n = this->indices.size();
			for (i = 0; i < n; i++) {
				dimeVec3f arrpt = pointsArray[this->indices[i]];
				if (pt == arrpt) {
					int idx = this->indices[i];
					//this->indices->removeElemFast(i);
					indices[i] = indices.back();
					indices.resize(indices.size()-1);
					return idx;
				}
			}
		}
		return -1;
	}

	void dime_bspnode::split() {
		ASSERT(!this->left && !this->right);
		this->left = std::make_unique<dime_bspnode>(this->pointsArray);
		this->right = std::make_unique<dime_bspnode>(this->pointsArray);

		dimeBox box;
		//int i, n = this->indices.count();
		//for (i = 0; i < n; i++) {
		//	box.grow(this->pointsArray->getElem(this->indices[i]));
		//}
		box.SetRectEmptyForMinMax();
		for (auto const& i : indices)
			box.UpdateBoundary(pointsArray[i]);
		dimeVec3f diag = box.pt1() - box.pt0();
		DIM dim = DIM_NONE;
		double pos {};

		if (diag[0] > diag[1]) {
			if (diag[0] > diag[2]) dim = DIM_YZ;
			else dim = DIM_XY;
		}
		else {
			if (diag[1] > diag[2]) dim = DIM_XZ;
			else dim = DIM_XY;
		}

		this->dimension = dim; // set the dimension

		dxfdouble mid = (box.pt0()[dim] + box.pt1()[dim]) / 2.0f;
	#ifdef BSP_SORTED_SPLIT  
		this->sort(); // sort vertices on ascending dimension values

		int splitidx = n / 2;
		pos = (this->pointsArray->getElem(this->indices[splitidx-1])[dim]+
			this->pointsArray->getElem(this->indices[splitidx])[dim])/ 2.0f;

		// got to check and adjust for special cases
		if (pos == box.min[dim] || pos == box.max[dim]) {
			pos = (pos + mid) / 2.0f;
		}

	#else
		pos = (double(box.pt0()[this->dimension])+double(box.pt1()[this->dimension])) / 2.0;
	#endif // BSP_SORTED_SPLIT

		this->position = pos;

		for (auto idx : indices) {
			if (leftOf(pointsArray[idx]))
				left->indices.push_back(idx);
			else
				right->indices.push_back(idx);
		}
		ASSERT(this->left->indices.size() && this->right->indices.size());

		// will never be used anymore
		this->indices.clear();
	}

	//
	// an implementation of the shellsort algorithm
	//
	void dime_bspnode::sort() {
		int* idxarray = this->indices.data();
		int num = this->indices.size();
		int dim = this->dimension;
		dimeVec3f* points = this->pointsArray.data();
		int distance{1};
		int idx;
		for (distance = 1; distance <= num/9; distance = 3*distance + 1);
		for (; distance > 0; distance /= 3) {
			for (int i = distance; i < num; i++) {
				idx = idxarray[i];
				int j = i;
				while (j >= distance &&
					points[idxarray[j-distance]][dim] > points[idx][dim]) {
					idxarray[j] = idxarray[j-distance];
					j -= distance;
				}
				idxarray[j] = idx;
			}
		}
	}

}

