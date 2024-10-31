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
  \class dimeLine dime/entities/Line.h
  \brief The dimeLine class handles a LINE \e entity.
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

using namespace std::literals;

namespace dime {


	/*!
	  Writes a \e Line entity.
	*/

	bool dimeLine::write(dimeOutput& file) {
		this->preWrite(file);

		for (size_t i = 0; i < coords.size(); i++) {
			for (int j = 0; j < 3; j++) {
				file.writeGroupCode((j+1)*10+i);
				file.writeDouble(this->coords[i][j]);
			}
		}
		return this->writeExtrusionData(file) && dimeEntity::write(file);
	}

	/*!
	  Handles the callback from dimeEntity::readRecords().
	*/

	bool dimeLine::handleRecord(const int groupcode, const dimeParam& param) {
		switch (groupcode) {
		case 10:
		case 20:
		case 30:
		case 11:
		case 21:
		case 31:
			this->coords[groupcode % 10][groupcode / 10 - 1] = std::get<double>(param);
			return true;
		}
		return base_t::handleRecord(groupcode, param);
	}

	//!

	bool dimeLine::getRecord(const int groupcode, dimeParam& param, const int index) const {
		switch (groupcode) {
		case 10:
		case 20:
		case 30:
		case 11:
		case 21:
		case 31:
			param.emplace<double>(this->coords[groupcode % 10][groupcode / 10 - 1]);
			return true;
		}
		return base_t::getRecord(groupcode, param, index);
	}

	////!

	//void dimeLine::print() const {
	//	fprintf(stderr, "LINE:\n");
	//	for (int i = 0; i < 2; i++) {
	//		fprintf(stderr, "coord: %f %f %f\n", coords[i][0],
	//			coords[i][1], coords[i][2]);

	//	}
	//}

	//!

	dimeEntity::GeometryType dimeLine::extractGeometry(std::vector<dimeVec3f>& verts,
			std::vector<int>&/*indices*/,
			dimeVec3f& extrusionDir,
			dxfdouble& thickness) {
		thickness = this->thickness;
		extrusionDir = this->extrusionDir;

		verts.push_back(coords[0]);
		verts.push_back(coords[1]);
		return dimeEntity::LINES;
	}

	//!

	size_t dimeLine::countRecords() const {
		size_t cnt = 1; // header
		cnt += 6; // coordinates
		return cnt + base_t::countRecords();
	}


} // namespace dime

