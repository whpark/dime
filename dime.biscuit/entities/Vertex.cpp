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
  \class dimeVertex dime/entities/Vertex.h
  \brief The dimeVertex class handles a VERTEX \e entity.
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

module dime.biscuit:entities.Vertex;
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


	//!

	bool dimeVertex::write(dimeOutput& file) {
		if (isDeleted())
			return true;
		this->preWrite(file);

		file.writeGroupCode(70);
		file.writeInt16(this->flags);
		file.writeGroupCode(10);
		file.writeDouble(this->coords[0]);
		file.writeGroupCode(20);
		file.writeDouble(this->coords[1]);
		file.writeGroupCode(30);
		file.writeDouble(this->coords[2]);

		//<< PWH
		if (bulge != 0.0) {
			file.writeGroupCode(42);
			file.writeDouble(bulge);
		}
		//>>

		for (int i = 0; i < this->numIndices(); i++) {
			file.writeGroupCode(i+71);
		#ifdef DIME_FIXBIG
			file.writeInt32(this->indices[i]);
		#else
			file.writeInt16(this->indices[i]);
		#endif
		}
		return dimeEntity::write(file);
	}

	/*!
	  Returns the number of indices stored in this vertex.
	*/

	int dimeVertex::numIndices() const {
		int cnt = 0;
		if ((flags & 128) && !(flags & 64)) { // does vertex store indices?
			for (int i = 0; this->indices[i] && i < 4; i++, cnt++)
				;	// empty 
		}
		return cnt;
	}

	/*!
	  Returns index number \a num.
	*/

	int dimeVertex::getIndex(const int num) const {
		return this->indices[num];
	}


	//!

	bool dimeVertex::handleRecord(const int groupcode, const dimeParam& param) {
		switch (groupcode) {
		case 70:
			this->flags = std::get<int16>(param);
			return true;
		case 10:
		case 20:
		case 30:
			this->coords[groupcode/10-1] = std::get<double>(param);
			return true;
		  //<< PWH
		case 42:
			this->bulge = std::get<double>(param);
			return true;
		  //>>
		case 71:
		case 72:
		case 73:
		case 74:
		#ifdef DIME_FIXBIG
			this->indices[groupcode-71] = std::get<int32>(param);
		#else
			this->indices[groupcode-71] = std::get<int16>(param);
		#endif
			return true;
		}
		return dimeEntity::handleRecord(groupcode, param);
	}


	//!

	bool dimeVertex::getRecord(const int groupcode, dimeParam& param, const int index) const {
		switch (groupcode) {
		case 70:
			param.emplace<int16>(this->flags);
			return true;
		case 10:
		case 20:
		case 30:
			param.emplace<double>(this->coords[groupcode/10-1]);
			return true;
		  //<< PWH
		case 40:
			param.emplace<double>(this->bulge);
			return true;
		  //>>
		case 71:
		case 72:
		case 73:
		case 74:
		#ifdef DIME_FIXBIG
			param.emplace<int16>(this->indices[groupcode-71]);
		#else
			param.emplace<int32>(this->indices[groupcode-71]);
		#endif
			return true;
		}
		return dimeEntity::getRecord(groupcode, param, index);
	}

	//!

	size_t dimeVertex::countRecords() const {
		size_t cnt = 0;
		if (!this->isDeleted()) {
			cnt += 5; // header + flags + coords
			cnt += 1;	// PWH. bulge
			cnt += this->numIndices();
			cnt += dimeEntity::countRecords();
		}
		return cnt;
	}

} // namespace dime
