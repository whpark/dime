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
  \class dimeEllipse dime/entities/Ellipse.h
  \brief The dimeEllipse class handles an ELLIPSE \e entity.
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

module dime.gtl:entities.Ellipse;
import std;
import :Basic;
import :util;
import :Record;
import :Input;
import :Output;
import :Model;

using namespace std::literals;

namespace dime {

/*!
  \fn void dimeEllipse::setCenter(const dimeVec3f &c)
  Sets the center coordinates of the ellipse.
*/

/*!
  \fn const dimeVec3f &dimeEllipse::getCenter() const
  Returns the center coordinates of the ellipse.
*/

/*!
  \fn void dimeEllipse::setMajorAxisEndpoint(const dimeVec3f &v)
  Sets the major axis endpoint of the ellipse.
  \sa dimeEllipse::setRatio()
*/

/*!
  \fn const dimeVec3f &dimeEllipse::getMajorAxisEndpoint() const
  Returns the major axis endpoint of this ellipse.
*/

/*!
  \fn void dimeEllipse::setMinorMajorRatio(const dxfdouble ratio)
  Sets the ratio of the minor axis to the major axis.
*/

/*!
  \fn dxfdouble dimeEllipse::getMinorMajorRatio() const
  Returns the ratio of the minor axis to the major axis.
*/

/*!
  \fn void dimeEllipse::setStartParam(const dxfdouble p)
  Sets the start parameter for this ellipse.
  Possible values range from 0 to 2*pi. Default value is
  0.
*/

/*!
  \fn dxfdouble dimeEllipse::getStartParam() const
  Returns the start parameter for this ellipse.
  \sa dimeEllipse::setStartParam()
*/

/*!
  \fn void dimeEllipse::setEndParam(const dxfdouble p)
  Sets the end parameter for this ellipse.
  Possible values range from 0 to 2*pi, but this value should
  be bigger than the start parameter. Default value is 2*pi.
  \sa dimeEllipse::setStartParam()
*/

/*!
  \fn dxfdouble dimeEllipse::getEndParam() const
  Returns the end parameter for this ellipse.
  \sa dimeEllipse::setEndParam()
*/

// FIXME: should be configurable

	constexpr int const ELLIPSE_NUMPTS = 16;

	//!

	bool dimeEllipse::write(dimeOutput& file) {
		dimeEntity::preWrite(file);

		file.writeGroupCode(10);
		file.writeDouble(this->center[0]);
		file.writeGroupCode(20);
		file.writeDouble(this->center[1]);
		file.writeGroupCode(30);
		file.writeDouble(this->center[2]);

		file.writeGroupCode(11);
		file.writeDouble(this->majorAxisEndpoint[0]);
		file.writeGroupCode(21);
		file.writeDouble(this->majorAxisEndpoint[1]);
		file.writeGroupCode(31);
		file.writeDouble(this->majorAxisEndpoint[2]);

		file.writeGroupCode(40);
		file.writeDouble(this->ratio);

		file.writeGroupCode(41);
		file.writeDouble(this->startParam);

		file.writeGroupCode(42);
		file.writeDouble(this->endParam);

		this->writeExtrusionData(file);

		return dimeExtrusionEntity::write(file);
	}

	//!

	bool dimeEllipse::handleRecord(int groupcode, const dimeParam& param) {
		switch (groupcode) {
		case 10:
		case 20:
		case 30:
			this->center[groupcode/10-1] = std::get<double>(param);
			return true;
		case 11:
		case 21:
		case 31:
			this->majorAxisEndpoint[groupcode/10-1] = std::get<double>(param);
			return true;
		case 40:
			this->ratio = std::get<double>(param);
			return true;
		case 41:
			this->startParam = std::get<double>(param);
			return true;
		case 42:
			this->endParam = std::get<double>(param);
			return true;
		}
		return dimeExtrusionEntity::handleRecord(groupcode, param);
	}

	//!

	bool dimeEllipse::getRecord(int groupcode, dimeParam& param, int index) const {
		switch (groupcode) {
		case 10:
		case 20:
		case 30:
			param.emplace<double>(this->center[groupcode/10-1]);
			return true;
		case 11:
		case 21:
		case 31:
			param.emplace<double>(this->majorAxisEndpoint[groupcode/10-1]);
			return true;
		case 40:
			param.emplace<double>(this->ratio);
			return true;
		case 41:
			param.emplace<double>(this->startParam);
			return true;
		case 42:
			param.emplace<double>(this->endParam);
			return true;
		}

		return dimeExtrusionEntity::getRecord(groupcode, param, index);
	}

	////!

	//void
	//	dimeEllipse::print() const {
	//	std::println("ELLIPSE:");
	//}

	//!

	size_t dimeEllipse::countRecords() const {
		  // header + center point + major endpoint + ratio + start + end
		return 10 + dimeExtrusionEntity::countRecords();
	}


} // namespace dime
