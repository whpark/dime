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
  \class dimeExtrusionEntity dime/entities/ExtrusionEntity.h
  \brief The dimeExtrusionEntity class is the superclass of all \e entity classes with extrusion data.
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

module dime.biscuit:entities.ExtrusionEntity;
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


	/*!
	  \fn void dimeExtrusionEntity::setExtrusionDir(const dimeVec3f &v)
	  Sets the extrusion direction. Default value is \e (0,0,1).
	*/

	/*!
	  \fn const dimeVec3f &dimeExtrusionEntity::getExtrusionDir() const
	  Returns the extrusion direction.
	*/

	/*!
	  \fn void dimeExtrusionEntity::setThickness(const dxfdouble val)
	  Sets the extrusion thickness. Default value is \e 0.0.
	*/

	/*!
	  \fn dxfdouble dimeExtrusionEntity::getThickness() const
	  Returns the extrusion thickness.
	*/

	/*!
	  Will write the extrusion and thickness records.
	*/

	bool dimeExtrusionEntity::writeExtrusionData(dimeOutput& file) {
		if (this->thickness != 0.0) {
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
		return true;
	}

	//!

	size_t dimeExtrusionEntity::countRecords() const {
		size_t cnt = 0;
		if (this->thickness != 0.0) cnt++;
		if (this->extrusionDir != dimeVec3f(0, 0, 1)) cnt += 3;
		return cnt + base_t::countRecords();
	}

	/*!
	  Copies all extrusion data from \a entity.
	*/

	void dimeExtrusionEntity::copyExtrusionData(const dimeExtrusionEntity* entity) {
		this->extrusionDir = entity->extrusionDir;
		this->thickness = entity->thickness;
	}

	//!

	bool dimeExtrusionEntity::handleRecord(int groupcode, const dimeParam& param) {
		switch (groupcode) {
		case 39:
			this->thickness = std::get<double>(param);
			return true;
		case 210:
		case 220:
		case 230:
			this->extrusionDir[(groupcode-210)/10] = std::get<double>(param);
			return true;
		}
		return dimeEntity::handleRecord(groupcode, param);
	}

	//!

	bool dimeExtrusionEntity::getRecord(int groupcode, dimeParam& param, int index) const {
		switch (groupcode) {
		case 39:
			param.emplace<double>(this->thickness);
			return true;
		case 210:
		case 220:
		case 230:
			param.emplace<double>(this->extrusionDir[(groupcode-210)/10]);
			return true;
		}
		return dimeEntity::getRecord(groupcode, param, index);
	}


} // namespace dime
