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
  \class dimeSpline dime/entities/Spline.h
  \brief The dimeSpline class handles a SPLINE \e entity.
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

module dime.biscuit:entities.Spline;
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
	  Returns \e true if the spline has control point weights.
	*/

	bool dimeSpline::hasWeights() const {
		if (weights.empty())
			return false;
		for (auto w : weights) {
			if (w != 1.0)
				return true;
		}
	}

	//!

	bool dimeSpline::write(dimeOutput& file) {
		dimeEntity::preWrite(file);

		bool ret = dimeEntity::write(file);

		// write counters first
		file.writeGroupCode(70);
		file.writeInt16(this->flags);
		file.writeGroupCode(71);
		file.writeInt16(this->degree);
		file.writeGroupCode(72);
		file.writeInt16((int16)this->knots.size());
		file.writeGroupCode(73);
		file.writeInt16((int16)this->controlPoints.size());
		file.writeGroupCode(74);
		file.writeInt16((int16)this->fitPoints.size());

		file.writeGroupCode(42);
		file.writeDouble(this->knotTolerance);
		file.writeGroupCode(43);
		file.writeDouble(this->cpTolerance);

		if (this->fitPoints.size()) {
			file.writeGroupCode(44);
			file.writeDouble(this->fitTolerance);
		}

		for (auto const k : this->knots) {
			file.writeGroupCode(40);
			file.writeDouble(k);
		}

		if (this->hasWeights()) {
			for (auto w : weights) {
				file.writeGroupCode(41);
				file.writeDouble(w);
			}
		}

		for (auto const& pt : controlPoints) {
			file.writeGroupCode(10);
			file.writeDouble(pt[0]);
			file.writeGroupCode(20);
			file.writeDouble(pt[1]);
			file.writeGroupCode(30);
			file.writeDouble(pt[2]);
		}
		for (auto const& pt : fitPoints) {
			file.writeGroupCode(11);
			file.writeDouble(pt[0]);
			file.writeGroupCode(21);
			file.writeDouble(pt[1]);
			file.writeGroupCode(31);
			file.writeDouble(pt[2]);
		}

		return ret;
	}


	//!

	bool dimeSpline::handleRecord(int groupcode, const dimeParam& param) {
		switch (groupcode) {
		case 10: controlPoints.emplace_back();// control points
		case 20:
		case 30:
			controlPoints.back()[groupcode/10-1] = std::get<double>(param);
			return true;
		case 11: fitPoints.emplace_back();// fit points
		case 21:
		case 31:
			fitPoints.back()[groupcode/10-1] = std::get<double>(param);
			return true;
		case 40: // knot value, multiple values
			this->knots.push_back(std::get<double>(param));
			return true;
		case 41: // weight, multiple values, presents if not all 1
			this->weights.resize(controlPoints.size(), 1.0);
			this->weights.back() = std::get<double>(param);
			return true;
		case 42: // knot tol
			this->knotTolerance = std::get<double>(param);
			return true;
		case 43: // cp tol
			this->cpTolerance = std::get<double>(param);
			return true;
		case 44: // fit tol
			this->fitTolerance = std::get<double>(param);
			return true;
		case 70: // flags
			this->flags = std::get<int16>(param);
			return true;
		case 71: // degree
			this->degree = std::get<int16>(param);
			return true;
		case 72: // num knots
			{
				auto n = std::get<int16>(param);
				this->knots.reserve(n);
			}
			return true;
		case 73: // num ctrl points
			{
				auto n = std::get<int16>(param);
				this->controlPoints.reserve(n);
				this->weights.reserve(n);
			}
			return true;
		case 74: // num fit points
			{
				auto n = std::get<int16>(param);
				this->fitPoints.reserve(n);
			}
			return true;
		}
		return dimeEntity::handleRecord(groupcode, param);
	}


	//!

	bool dimeSpline::getRecord(int groupcode, dimeParam& param, int index) const {
		switch (groupcode) {
		case 10: // control points
		case 20:
		case 30:
			if (this->controlPoints.size() && index < this->controlPoints.size()) {
				param.emplace<double>(this->controlPoints[index][groupcode/10-1]);
			}
			return true;
		case 11: // fit points
		case 21:
		case 31:
			if (this->fitPoints.size() && index < this->fitPoints.size()) {
				param.emplace<double>(this->fitPoints[index][groupcode/10-1]);
			}
			return true;
		case 40: // knot value, multiple values
			if (this->knots.size() && index < this->knots.size()) {
				param.emplace<double>(this->knots[index]);
			}
			return true;
		case 41: // weight, multiple values, present if not all 1
			if (this->weights.size() && this->controlPoints.size()) {
				param.emplace<double>(this->weights[index]);
			}
			else param.emplace<double>(1.0); // default
			return true;
		case 42: // knot tol
			param.emplace<double>(this->knotTolerance);
			return true;
		case 43: // cp tol
			param.emplace<double>(this->cpTolerance);
			return true;
		case 44: // fit tol
			param.emplace<double>(this->fitTolerance);
			return true;
		case 70: // flags
			param.emplace<int16>(this->flags);
			return true;
		case 71: // degree
			param.emplace<int16>(this->degree);
			return true;
		case 72: // num knots
			param.emplace<int16>((int16)this->knots.size());
			return true;
		case 73: // num ctrl points
			param.emplace<int16>((int16)this->controlPoints.size());
			return true;
		case 74: // num fit points
			param.emplace<int16>((int16)this->fitPoints.size());
			return true;
		}
		return dimeEntity::getRecord(groupcode, param, index);
	}

	////!

	//void
	//	dimeSpline::print() const {
	//	fprintf(stderr, "SPLINE:\n");
	//	// FIXME
	//}

	//!

	size_t dimeSpline::countRecords() const {
		size_t cnt = 7; // name + flags + degree + numKnots + numCP + knotTol + cpTol
		cnt += this->controlPoints.size()*3;
		if (this->hasWeights()) cnt += this->controlPoints.size();
		cnt += this->fitPoints.size()*3;
		if (this->fitPoints.size()) cnt++; // fitTolerance
		cnt += this->knots.size();

		return cnt + dimeEntity::countRecords();
	}

	void dimeSpline::setKnotValues(std::vector<dxfdouble> const& values) {
		knots = values;
	}

	/*!
	  Sets new control points for this spline. It is the user's
	  responsibility to update the weights whenever the control
	  point are changed.
	*/

	void dimeSpline::setControlPoints(std::vector<dimeVec3f> const& pts) {
// update weights array (if present)
		if (this->hasWeights()) {
			weights.resize(pts.size(), 1.0);
		}
		controlPoints = pts;
	}

	/*!
	  Sets the weight of control point \a idx.
	*/

	void dimeSpline::setWeight(int idx, const dxfdouble w) {
		ASSERT(idx >= 0 && idx < this->controlPoints.size());
		if (w != 1.0) {
			weights.resize(controlPoints.size(), 1.0);
			this->weights[idx] = w;
		}
	}

	void dimeSpline::setFitPoints(std::vector<dimeVec3f> const& pts) {
		fitPoints = pts;
	}


} // namespace dime
