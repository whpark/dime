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
  \class dimeLWPolyline dime/entities/LWPolyline.h
  \brief The dimeLWPolyline class handles an LWPOLYLINE \e entity.
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

module dime.biscuit:entities.LWPolyline;
import std;
import biscuit;
import :Basic;
import :util;
import :Record;
import :Input;
import :Output;
import :Model;

using namespace std::literals;

namespace dime {


	// todo:
	// FIXME: after the first vertex is found, I should make sure no more 
	// unknown records are found. If unknown records are found, the LWPOLYLINE
	// will be illegal upon writing. The current code will work for releases
	// up to r14, but might not work for r15+ if additional per-vertex data
	// is added for those future file formats. 
	//

	//!

	bool dimeLWPolyline::write(dimeOutput& file) {
		this->preWrite(file);

		bool ret = true;

		file.writeGroupCode(90);
		ret = file.writeInt16((int16)this->numVertices);

		if (ret && this->flags != 0) {
			file.writeGroupCode(70);
			ret = file.writeInt16(this->flags);
		}
		if (ret && this->elevation != 0.0) {
			file.writeGroupCode(38);
			ret = file.writeDouble(this->elevation);
		}
		if (ret && this->constantWidth != 0.0) {
			file.writeGroupCode(43);
			ret = file.writeDouble(this->constantWidth);
		}

		if (!ret) return false;

		// write extrusion data and unksnown records
		ret = this->writeExtrusionData(file) &&
			dimeEntity::write(file);

		if (ret) {
			int num = this->numVertices;
			for (int i = 0; ret && i < num; i++) {
				auto const& c = this->coords[i];
				file.writeGroupCode(10);
				file.writeDouble(c.x);
				file.writeGroupCode(20);
				ret = file.writeDouble(c.y);

				if (ret) {
					if (c.startingWidth != 0.0) {
						file.writeGroupCode(40);
						file.writeDouble(c.startingWidth);
					}
					if (c.endWidth != 0.0) {
						file.writeGroupCode(41);
						ret = file.writeDouble(c.endWidth);
					}
				}
				if (ret && c.bulge != 0.0) {
					file.writeGroupCode(42);
					ret = file.writeDouble(c.bulge);
				}
			}
		}

		return ret;
	}


	/*!
	  Handles the callback from dimeEntity::readRecords().
	*/

	bool dimeLWPolyline::handleRecord(int groupcode, const dimeParam& param) {
		switch (groupcode) {
		case 10:
		case 20:
		case 40:
		case 41:
		case 42:
			{
				static std::map<int, std::function<double&(sLWPoint&)>> handlers = {
					{ 10, [](sLWPoint& c)->double& { return c.x; } },
					{ 20, [](sLWPoint& c)->double& { return c.y; } },
					{ 40, [](sLWPoint& c)->double& { return c.startingWidth; } },
					{ 41, [](sLWPoint& c)->double& { return c.endWidth; } },
					{ 42, [](sLWPoint& c)->double& { return c.bulge; } },
				};
				if (auto& handler = handlers[groupcode]) {
					if (tmpFlagsVertexPart.contains(groupcode)) {
						tmpFlagsVertexPart.clear();
						coords.push_back(sLWPoint());
					}
					tmpFlagsVertexPart.insert(groupcode);
					handler(coords.back()) = std::get<double>(param);
				}
				return true;
			}
		case 38:
			this->elevation = std::get<double>(param);
			return true;
		case 43:
			this->constantWidth = std::get<double>(param);
			return true;
		case 70:
			this->flags = std::get<int16>(param);
			return true;
		case 90:
			this->numVertices = std::get<int32>(param);
			tmpFlagsVertexPart.insert(10);
			tmpFlagsVertexPart.insert(20);
			tmpFlagsVertexPart.insert(40);
			tmpFlagsVertexPart.insert(41);
			tmpFlagsVertexPart.insert(42);
			coords.clear();
			if (numVertices > 0) {
				coords.reserve(this->numVertices);
			}
			return true;
		}
		return dimeExtrusionEntity::handleRecord(groupcode, param);
	}


	//!

	bool dimeLWPolyline::getRecord(int groupcode, dimeParam& param, int index) const {
		switch (groupcode) {
		case 10:
			if (index >= 0 && index < this->coords.size()) {
				param.emplace<double>(this->coords[index].x);
				return true;
			}
			return false;
		case 20:
			if (index >= 0 && index < this->coords.size()) {
				param.emplace<double>(this->coords[index].y);
				return true;
			}
			return false;
		case 40:
			if (index >= 0 && index < this->coords.size()) {
				param.emplace<double>(this->coords[index].startingWidth);
				return true;
			}
			return false;
		case 41:
			if (index >= 0 && index < this->coords.size()) {
				param.emplace<double>(this->coords[index].endWidth);
				return true;
			}
			return false;
		case 42:
			if (index >= 0 && index < this->coords.size()) {
				param.emplace<double>(this->coords[index].bulge);
				return true;
			}
			return false;
		case 38:
			param.emplace<double>(this->elevation);
			return true;
		case 43:
			param.emplace<double>(this->constantWidth);
			return true;
		case 70:
			param.emplace<int16>(this->flags);
			return true;
		case 90:
			param.emplace<int32>(this->numVertices);
			return true;
		}
		return dimeExtrusionEntity::getRecord(groupcode, param, index);
	}

	////!

	//void dimeLWPolyline::print() const {
	//	fprintf(stderr, "LWPOLYLINE:\n");
	//	for (int i = 0; i < this->numVertices; i++) {
	//		fprintf(stderr, "coord: %f %f\n", xcoord[i], ycoord[i]);
	//	}
	//}

	//!

	dimeEntity::GeometryType dimeLWPolyline::extractGeometry(
		std::vector<dimeVec3f>& verts,
		std::vector<int>&/*indices*/,
		dimeVec3f& extrusionDir_,
		dxfdouble& thickness_)
	{
		thickness_ = this->thickness;
		extrusionDir_ = this->extrusionDir;

		for (auto const& c : coords) {
			verts.emplace_back(c.x, c.y, this->elevation);
		}
		// is POLYLINE closed?
		if ((this->flags & 1) and verts.size() > 1) {	// more than one vertex (not 0)
			verts.push_back(verts.front());
		}
		return dimeEntity::LINES;
	}

	//!

	size_t dimeLWPolyline::countRecords() const {
		size_t cnt = 2; // header + numVertices

		if (this->elevation != 0.0) cnt++;
		if (this->constantWidth != 0.0) cnt++;
		if (this->flags != 0) cnt++;
		cnt += this->numVertices * 2; // x and y coordinates

		// count optional per-vertex records
		for (auto const& c : coords) {
			if (c.bulge != 0.0) cnt++;
			if (c.startingWidth != 0.0) cnt++;
			if (c.endWidth != 0.0) cnt++;
		}

		return cnt + dimeExtrusionEntity::countRecords();
	}

} // namespace dime

