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
  \class dimeTrace dime/entities/Trace.h
  \brief The dimeTrace class handles a TRACE \e entity.
*/

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

#include "gtl/gtl.h"

module dime.gtl:entities.Trace;
import std;
import :Basic;
import :util;
import :Base;
import :Record;
import :Input;
import :Output;
import :Model;
import :entities.Entity;
import :entities.FaceEntity;

using namespace std::literals;

namespace dime {



//!
	bool dimeTrace::write(dimeOutput& file) {
		//if (isDeleted())
		//	return true;
		this->preWrite(file);
		this->writeCoords(file);
		if (this->thickness != 0.0f) {
			file.writeGroupCode(39);
			file.writeDouble(this->thickness);
		}
		if (this->extrusionDir != dimeVec3f(0, 0, 1)) {
			file.writeGroupCode(210);
			file.writeDouble(this->extrusionDir[0]);
			file.writeGroupCode(220);
			file.writeDouble(this->extrusionDir[1]);
			file.writeGroupCode(230);
			file.writeDouble(this->extrusionDir[2]);

		}
		return dimeEntity::write(file);
	}

	//!

	bool dimeTrace::handleRecord(int groupcode, const dimeParam& param) {
		switch (groupcode) {
		case 210:
		case 220:
		case 230:
			this->extrusionDir[(groupcode-210)/10] = std::get<double>(param);
			return true;
		case 39:
			this->thickness = std::get<double>(param);
			return true;
		}
		return dimeFaceEntity::handleRecord(groupcode, param);
	}


	//!

	bool dimeTrace::getRecord(int groupcode, dimeParam& param, int index) const {
		switch (groupcode) {
		case 210:
		case 220:
		case 230:
			param.emplace<double>(this->extrusionDir[(groupcode-210)/10]);
			return true;
		case 39:
			param.emplace<double>(this->thickness);
			return true;
		}
		return dimeFaceEntity::getRecord(groupcode, param, index);
	}

	dxfdouble dimeTrace::getThickness() const {
		return this->thickness;
	}

	void dimeTrace::getExtrusionDir(dimeVec3f& ed) const {
		ed = this->extrusionDir;
	}

	bool dimeTrace::swapQuadCoords() const {
		return true;
	}

	void dimeTrace::setThickness(const dxfdouble& thickness_) {
		this->thickness = thickness_;
	}

	void dimeTrace::setExtrusionDir(const dimeVec3f& ed) {
		this->extrusionDir = ed;
	}

	//!

	size_t dimeTrace::countRecords() const {
		size_t cnt = 0;
		//if (isDeleted())
		//	return cnt;
		cnt++; // header
		if (this->thickness != 0.0) cnt++;
		if (this->extrusionDir != dimeVec3f(0, 0, 1)) cnt += 3;
		cnt += dimeFaceEntity::countRecords();
		return cnt;
	}

} // namespace dime

