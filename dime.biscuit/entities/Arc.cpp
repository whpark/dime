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
  \class dimeArc dime/entities/Arc.h
  \brief The dimeArc class handles an ARC \e entity.
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

module dime.biscuit:entities.Arc;
import std;
import biscuit;
import :Basic;
import :util;
import :Record;
import :Input;
import :Output;
//import :Model;

using namespace std::literals;

namespace dime {


// FIXME: should be configurable

	constexpr int const ARC_NUMPTS = 20; // num pts for a 2PI arc

	//!

	bool dimeArc::write(dimeOutput& file) {
		this->preWrite(file);

		file.writeGroupCode(10);
		file.writeDouble(this->center[0]);
		file.writeGroupCode(20);
		file.writeDouble(this->center[1]);
		file.writeGroupCode(30);
		file.writeDouble(this->center[2]);

		file.writeGroupCode(40);
		file.writeDouble(this->radius);

		// For some reason a 73 record needs a new subclass record.
		file.writeGroupCode(100);
		file.writeString("AcDbArc");

		file.writeGroupCode(50);
		file.writeDouble(this->startAngle);
		file.writeGroupCode(51);
		file.writeDouble(this->endAngle);

		return this->writeExtrusionData(file) && dimeEntity::write(file);
	}

	//!

	bool dimeArc::handleRecord(int groupcode, const dimeParam& param) {
		switch (groupcode) {
		case 10:
		case 20:
		case 30:
			this->center[groupcode / 10 - 1] = std::get<double>(param);
			return true;
		case 40:
			this->radius = std::get<double>(param);
			return true;
		case 50:
			this->startAngle = std::get<double>(param);
			return true;
		case 51:
			this->endAngle = std::get<double>(param);
			return true;
		case 100:
		  // Eat AcDbArc records, leave others.
			if (std::get<std::string>(param) == "AcDbArc") {
				return true;
			}
			else {
				return dimeExtrusionEntity::handleRecord(groupcode, param);
			}
		}
		return dimeExtrusionEntity::handleRecord(groupcode, param);
	}

	//!

	bool dimeArc::getRecord(int groupcode, dimeParam& param, int index) const {
		switch (groupcode) {
		case 10:
		case 20:
		case 30:
			param.emplace<double>(this->center[groupcode / 10 - 1]);
			return true;
		case 40:
			param.emplace<double>(this->radius);
			return true;
		case 50:
			param.emplace<double>(this->startAngle);
			return true;
		case 51:
			param.emplace<double>(this->endAngle);
			return true;
		}
		return dimeExtrusionEntity::getRecord(groupcode, param, index);
	}

	////!

	//void dimeArc::print() const {
	//	std::println("ARC:\n");
	//	std::println(" center: {:.3f} {:.3f} {:.3f}",
	//		center[0], center[1], center[2]);
	//	std::println(" radius: {}", radius);
	//	std::println(" start: {}, end: {}", startAngle, endAngle);
	//	std::println(" extrusionDir: {] {} {}",
	//		extrusionDir[0], extrusionDir[1], extrusionDir[2]);
	//	std::println(" thickness: {}", thickness);
	//}

	//!

	dimeEntity::GeometryType dimeArc::extractGeometry(std::vector<dimeVec3f>& verts, std::vector<int>& indices, dimeVec3f& extrusionDir_, dxfdouble& thickness_) {

		verts.clear();
		indices.clear();

		thickness_ = this->thickness;
		extrusionDir_ = this->extrusionDir;

		// split the arc into lines

		double end = this->endAngle;
		if (end < this->startAngle) end += 360.0;

		rad_t delta = deg_t(end - this->startAngle);
		if (delta == 0.0_rad) {
		#ifndef NDEBUG
			std::println("ARC with startAngle == endAngle!");
		#endif
			end += 2 * std::numbers::pi;
			//return dimeEntity::NONE;
		}

		// find the number of this ARC that fits inside 2PI
		int parts = (int)std::abs((2.*std::numbers::pi)/delta.value());

		// find # pts to use for arc
		// add one to avoid arcs with 0 line segments
		int numpts = ARC_NUMPTS / parts + 1;
		if (numpts > ARC_NUMPTS) numpts = ARC_NUMPTS;

		auto inc = delta / numpts;
		rad_t rad = deg_t(this->startAngle);
		int i;
		for (i = 0; i < numpts; i++) {
			verts.push_back(dimeVec3f(this->center.x + this->radius * units::math::cos(rad),
				this->center.y + this->radius * units::math::sin(rad),
				this->center.z));
			rad += inc;
		}
		rad = deg_t(end);

		verts.push_back(dimeVec3f(this->center.x + this->radius * units::math::cos(rad),
			this->center.y + this->radius * units::math::sin(rad),
			this->center.z));

		return dimeEntity::LINES;
	}

	//!

	size_t dimeArc::countRecords() const {
		size_t cnt = 1 + 3 + 1 + 2; // header + center point + radius + angles

		return cnt + dimeExtrusionEntity::countRecords();
	}


} // namespace dime
