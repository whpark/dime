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
  \class dime3DFace dime/entities/3DFace.h
  \brief The dime3DFace class handles a 3DFACE \e entity.
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

module dime.biscuit:entities_3DFace;
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


	bool dime3DFace::write(dimeOutput& file) {
		if (isDeleted())
			return true;
		this->preWrite(file);
		this->writeCoords(file);
		if (flags != 0) {
			file.writeGroupCode(70);
			file.writeInt16(flags);
		}
		return dimeEntity::write(file);
	}

	//!

	bool dime3DFace::handleRecord(int groupcode, const dimeParam& param) {
		if (groupcode == 70) {
			this->flags = std::get<int16>(param);
			return true;
		}
		return base_t::handleRecord(groupcode, param);
	}

	//!

	bool dime3DFace::getRecord(int groupcode, dimeParam& param, int index) const {
		if (groupcode == 70) {
			param.emplace<int16>(this->flags);
			return true;
		}
		return dimeFaceEntity::getRecord(groupcode, param, index);
	}

	//!

	//void dime3DFace::print() const {
	//	fprintf(stderr, "3DFACE:\n");
	//	int stop = this->isQuad() ? 4 : 3;
	//	for (int i = 0; i < stop; i++) {
	//		fprintf(stderr, "coord: %f %f %f\n", coords[i][0],
	//			coords[i][1], coords[i][2]);

	//	}
	//}

	//!

	size_t dime3DFace::countRecords() const {
		size_t cnt = 0;
		if (isDeleted())
			return cnt;
		cnt++; // header
		if (this->flags != 0)
			cnt++;

		cnt += base_t::countRecords();
		return cnt;
	}

	void dime3DFace::setFlags(int16 f) {
		this->flags = f;
	}

	int16 dime3DFace::getFlags() const {
		return this->flags;
	}


} // namespace dime

