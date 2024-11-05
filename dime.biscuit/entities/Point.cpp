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
\class dimePoint dime/entities/Point.h
\brief The dimePoint class handles a POINT \e entity.
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

module dime.biscuit:entities.Line;
import std;
import biscuit;
import :Basic;
import :util;
import :Record;
import :Input;
import :Output;
import :Model;
import :entities.Entity;
import :entities.ExtrusionEntity;
import :entities.Point;

using namespace std::literals;

namespace dime {


	bool dimePoint::write(dimeOutput& file) {
		if (isDeleted())
			return true;
		this->preWrite(file);

		file.writeGroupCode(10);
		file.writeDouble(this->coords[0]);
		file.writeGroupCode(20);
		file.writeDouble(this->coords[1]);
		file.writeGroupCode(30);
		file.writeDouble(this->coords[2]);

		return this->writeExtrusionData(file) && dimeEntity::write(file);
	}

	//!

	bool dimePoint::handleRecord(int groupcode, const dimeParam& param) {
		switch (groupcode) {
		case 10:
		case 20:
		case 30:
			this->coords[groupcode/10-1] = std::get<double>(param);
			return true;
		}
		return base_t::handleRecord(groupcode, param);
	}


	//!

	bool dimePoint::getRecord(int groupcode, dimeParam& param, int index) const {
		switch (groupcode) {
		case 10:
		case 20:
		case 30:
			param.emplace<double>(this->coords[groupcode/10-1]);
			return true;
		}
		return base_t::getRecord(groupcode, param, index);
	}

	//!

	dimeEntity::GeometryType dimePoint::extractGeometry(
		std::vector<dimeVec3f>& verts_,
		std::vector<int>&/*indices*/,
		dimeVec3f& extrusionDir_,
		dxfdouble& thickness_)
	{
		thickness_ = this->thickness;
		extrusionDir_ = this->extrusionDir;
		verts_.push_back(this->coords);
		return dimeEntity::POINTS;
	}

	//!

	size_t dimePoint::countRecords() const {
		size_t cnt = 0;
		cnt += 4; // header + coordinates
		return cnt + base_t::countRecords();
	}

} // namespace dime
