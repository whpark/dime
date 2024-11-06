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

/*!
  \class dimePolyline dime/entities/Polyline.h
  \brief The dimePolyline class handles a POLYLINE \e entity.
*/

//=============================================================================
// forked from coin3d/dime
//
// This project is a refactoring of the original project "coin3d/dime".
// Significant modifications were made to improve code structure,
// performance, and maintainability while preserving the original project's core
// functionality and license terms.
//
// this dime.biscuit project is a part of the project "biscuit".
// this compiles only inside "biscuit"
// whpark. 2024-10-24
//=============================================================================

#include "biscuit/dependencies_eigen.h"
#include "biscuit/dependencies_units.h"

module dime.biscuit:entities.Polyline;
import std;
import biscuit;
import :Basic;
import :util;
import :Record;
import :Input;
import :Output;
import :Model;
import :entities.Vertex;
import :State;

using namespace std::literals;

namespace dime {



	/*!
	  Reads a POLYLINE entity.
	*/

	bool dimePolyline::read(dimeInput& file) {
		bool ret = dimeEntity::read(file);

		if (ret && this->entityFlags & FLAG_VERTICES_FOLLOW) {
			// read all vertices.
			int32 groupcode{};

			while (true) {
				if (!file.readGroupCode(groupcode) || groupcode != 0) {
					std::println("Error reading groupcode: {}", groupcode);
					ret = false;
					break;
				}
				auto string = file.readString();
				if (string == "SEQEND") {
					this->seqend = dimeEntity::createEntity(string);
					ret = this->seqend && this->seqend->read(file);
					break; // ok, no more vertices.
				}
				if (string != "VERTEX") {
					ret = false;
					break;
				}
				dimeVertex vertex;
				if (!vertex.read(file)) {
					std::println("error reading vertex.");
				//	sim_warning("error reading vertex.\n");
					ret = false;
					break;
				}

				if (vertex.getFlags() & 16) // todo: use a flag instead of magic number
					frameVertices.push_back(vertex);
				else if (vertex.numIndices())
					indexVertices.push_back(vertex);
				else
					coordVertices.push_back(vertex);
			}
		}
		return ret;
	}

	/*!
	  Writes POLYLINE data to \a file.
	*/

	bool dimePolyline::write(dimeOutput& file) {
		//if (this->isDeleted())
		//	return true;

		dimeEntity::preWrite(file);

		if (this->coordVertices.size() || this->indexVertices.size() || this->frameVertices.size()) {
			file.writeGroupCode(66); // vertices follow flag
			file.writeInt16(1);
		}

		file.writeGroupCode(10);
		file.writeDouble(elevation[0]);
		file.writeGroupCode(20);
		file.writeDouble(elevation[1]);
		file.writeGroupCode(30);
		file.writeDouble(elevation[2]);
		file.writeGroupCode(70);
		file.writeInt16(flags);

		this->writeExtrusionData(file);

		if (this->flags & 64) {
		  // FIXME: is this necessary?
			file.writeGroupCode(71);
			file.writeInt16((int16)this->coordVertices.size());
			file.writeGroupCode(72);
			file.writeInt16((int16)this->indexVertices.size());
		}
		else {
			if (this->countM != 0) {
				file.writeGroupCode(71);
				file.writeInt16(this->countM);
			}
			if (this->countN != 0) {
				file.writeGroupCode(72);
				file.writeInt16(this->countN);
			}
			if (this->smoothCountM != 0) {
				file.writeGroupCode(73);
				file.writeInt16(this->smoothCountM);
			}
			if (this->smoothCountN != 0) {
				file.writeGroupCode(74);
				file.writeInt16(this->smoothCountN);
			}
			if (this->surfaceType != 0) {
				file.writeGroupCode(75);
				file.writeInt16(this->surfaceType);
			}
		}
		bool ret = dimeEntity::write(file); // write unknown records
		if (!ret) return false; // too lazy to check every write

		// write all spline frame control points
		for (auto& v : this->frameVertices) {
			if (!v.write(file)) return false;
		}
		// write all coord vertices
		for (auto& v : this->coordVertices) {
			if (!v.write(file)) return false;
		}
		// write all index vertices
		for (auto& v : this->indexVertices) {
			if (!v.write(file)) return false;
		}

		// write end-of-vertex signature...
		if (this->seqend)
			return this->seqend->write(file);
		// ... or just put one there!
		file.writeGroupCode(0);
		file.writeString("SEQEND");
		file.writeGroupCode(8);
		return file.writeString(this->getLayerName());
	}


	//!

	bool dimePolyline::handleRecord(int groupcode, const dimeParam& param) {
		switch (groupcode) {
		case 66:
			this->entityFlags |= FLAG_VERTICES_FOLLOW;
			return true;
		case 70:
			this->flags = std::get<int16>(param);
			return true;
		case 71:
			this->countM = std::get<int16>(param);
			return true;
		case 72:
			this->countN = std::get<int16>(param);
			return true;
		case 73:
			this->smoothCountM = std::get<int16>(param);
			return true;
		case 74:
			this->smoothCountN = std::get<int16>(param);
			return true;
		case 75:
			this->surfaceType = std::get<int16>(param);
			return true;
		case 10:
		case 20:
		case 30:
			this->elevation[groupcode / 10 - 1] = std::get<double>(param);
			return true;
		}
		return dimeExtrusionEntity::handleRecord(groupcode, param);
	}


	//!  
	bool dimePolyline::getRecord(int groupcode, dimeParam& param, int index) const {
		switch (groupcode) {
		case 66:
			param.emplace<int16>((this->coordVertices.size() || this->indexVertices.size()) ? 1 : 0);
			return true;
		case 70:
			param.emplace<int16>(this->flags);
			return true;
		case 71:
			param.emplace<int16>(this->countM);
			return true;
		case 72:
			param.emplace<int16>(this->countN);
			return true;
		case 73:
			param.emplace<int16>(this->smoothCountM);
			return true;
		case 74:
			param.emplace<int16>(this->smoothCountN);
			return true;
		case 75:
			param.emplace<int16>(this->surfaceType);
			return true;
		case 10:
		case 20:
		case 30:
			param.emplace<double>(this->elevation[groupcode / 10 - 1]);
			return true;
		}
		return dimeExtrusionEntity::getRecord(groupcode, param, index);
	}

	/*!
	  Convenience function that returns the type of the polyline.
	*/

	int dimePolyline::getType() const {
		if (this->flags & IS_POLYLINE_3D) return POLYLINE;
		if (this->flags & IS_POLYFACE_MESH) return POLYFACE_MESH;
		if (this->flags & IS_POLYMESH_3D) return POLYGON_MESH;

		return this->POLYLINE; // no type flags set => (2D?) POLYLINE
	}

	//!

	dimeEntity::GeometryType dimePolyline::extractGeometry(
		std::vector<dimeVec3f>& verts,
		std::vector<int>& indices,
		dimeVec3f& extrusionDir_,
		dxfdouble& thickness_)
	{
		verts.clear();
		indices.clear();

		thickness_ = this->thickness;
		extrusionDir_ = this->extrusionDir;

		if ( (((this->flags & 0x58) == 0) || (this->flags & 0x8))
			&& this->coordVertices.size() > 1)
		{
			// this is a polyline 
			for (auto const& v : this->coordVertices) {
				verts.push_back(v.getCoords());
			}
			if (this->flags & 0x1) { // closed polyline 
				if (coordVertices.size() > 1)	// if there are more than one vertex
					verts.push_back(coordVertices.front().getCoords());
			}
			return dimeEntity::LINES;
		}

		// now we know POLYLINE contains polygonal data
		for (auto const& v : coordVertices) {
			verts.push_back(v.getCoords());
		}

		if ((this->flags & 0x10) && coordVertices.size() > 1) {
		  // this is a polygon mesh
			int m = this->countM;
			int n = this->countN;

			int m2 = 0;
			int n2 = 0;

			if (this->surfaceType && this->smoothCountM && this->smoothCountN) {
				m2 = this->smoothCountM;
				n2 = this->smoothCountN;
			}

			if (m*n + m2*n2 != this->coordVertices.size()) {
			  // FIXME: quick bugfix for stehlen.dxf... weird !
				if ((m-1)*n + m2*n2 == this->coordVertices.size()) m--;
				else {
					if (m*n == this->coordVertices.size()) {
						m2 = n2 = 0;
					}
					else if (m2*n2 == this->coordVertices.size()) {
						m = n = 0;
					}
					else { // give up
						std::println("vertices and faces do no add up: {} * {} + {} * {} != {}.",
							m, n, m2, n2, this->coordVertices.size());

						std::println("polyline: {} {}\n", flags, surfaceType);

						verts.clear();
						return dimeEntity::NONE;
					}
				}
			}


			int idx;
			int idxadd = m2*n2;
			int nexti, nextj;
			int endm = (this->flags & 1) ? m : m-1;
			int endn = (this->flags & 32) ? n : n-1;

			for (int i = 0; i < endm; i++) {
				nexti = i+1;
				if (nexti == m) nexti = 0;
				for (int j = 0; j < endn; j++) {
					nextj = j+1;
					if (nextj == n) nextj = 0;
					idx = i*n + j;
					indices.push_back(idxadd+idx);

					idx = i*n + nextj;
					indices.push_back(idxadd+idx);

					idx = nexti*n + nextj;
					indices.push_back(idxadd+idx);

					idx = nexti*n + j;
					indices.push_back(idxadd+idx);
					indices.push_back(-1);
				}
			}

			idxadd = 0;
			// copied code from above. I'm too lazy to write a loop or
			// a separate function :)
			m = m2;
			n = n2;
			endm = (this->flags & 1) ? m : m-1;
			endn = (this->flags & 32) ? n : n-1;

			for (int i = 0; i < endm; i++) {
				nexti = i+1;
				if (nexti == m) nexti = 0;
				for (int j = 0; j < endn; j++) {
					nextj = j+1;
					if (nextj == n) nextj = 0;
					idx = i*n + j;
					indices.push_back(idxadd+idx);

					idx = i*n + nextj;
					indices.push_back(idxadd+idx);

					idx = nexti*n + nextj;
					indices.push_back(idxadd+idx);

					idx = nexti*n + j;
					indices.push_back(idxadd+idx);
					indices.push_back(-1);
				}
			}
			return dimeEntity::POLYGONS;
		}

		// this must be a polyface mesh
		if (this->indexVertices.empty() || this->coordVertices.empty()) {
			verts.clear();
			return dimeEntity::NONE;
		}
		for (int i = 0; i < (int)this->indexVertices.size(); i++) {
			dimeVertex const& v = this->indexVertices[i];
			int num = v.numIndices();
			int idx{};
			for (int j = 0; j < num; j++) {
				idx = v.getIndex(j);
				if (idx < 0) { // negative means hidden edge
					idx = -idx;
				}
				indices.push_back(idx-1);
			}
			indices.push_back(-1);
		}
		return dimeEntity::POLYGONS;
		// phew, should probably have spilt this function into several 
		// smaller ones, but I'm not a coward so...
	}

	//!

	size_t dimePolyline::countRecords() const {
		size_t cnt = 5; // header + elevation + flags

		if (this->coordVertices.size() || this->indexVertices.size()) cnt++; // vertices follow flag

		if (this->flags & 64) cnt += 2;
		else {
			if (this->countM != 0) cnt++;
			if (this->countN != 0) cnt++;
			if (this->smoothCountM != 0) cnt++;
			if (this->smoothCountN != 0) cnt++;
			if (this->surfaceType != 0) cnt++;
		}

		for (auto const& v : coordVertices)
			cnt += v.countRecords();
		for (auto const& v : indexVertices)
			cnt += v.countRecords();
		for (auto const& v : frameVertices)
			cnt += v.countRecords();
		if (this->seqend) cnt+= this->seqend->countRecords();
		else cnt++; // endseq
		return cnt;
	}

	/*!
	  Overloaded from dimeEntity. Will first do a callback for this entity,
	  then for all vertices. Each vertex will have a pointer to its
	  polyline "parent".
	*/

	bool dimePolyline::traverse(dimeState const* state, callbackEntity_t callback) {
		callback(state, this);
		if (state->getFlags() & dimeState::TRAVERSE_POLYLINE_VERTICES) {
			for (auto& v : frameVertices) {
				if (!v.traverse(state, callback)) return false;
			}
			for (auto& v : coordVertices) {
				if (!v.traverse(state, callback)) return false;
			}
			for (auto& v : indexVertices) {
				if (!v.traverse(state, callback)) return false;
			}
		}
		return true;
	}

	//!

	void dimePolyline::setLayer(dimeLayer const* layer_) {
		dimeEntity::setLayer(layer_);
		for (auto& v : frameVertices)
			v.setLayer(layer_);
		for (auto& v : coordVertices)
			v.setLayer(layer_);
		for (auto& v : indexVertices)
			v.setLayer(layer_);
	}

	void dimePolyline::clearSurfaceData() {
		this->smoothCountN = this->smoothCountM = 0;
		this->surfaceType = 0;
	}

} // namespace dime

