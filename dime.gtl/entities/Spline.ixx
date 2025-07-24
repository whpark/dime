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
#include "../Basic.h"

export module dime.gtl:entities.Spline;
import std;
import :Basic;
import :util;
import :Base;
import :Input;
import :Output;
import :entities.Entity;
import :entities.ExtrusionEntity;

using namespace std::literals;

namespace dime {
}

export namespace dime {

	class dimeSpline : public dimeEntity {
		constexpr static inline double const DEFAULT_CP_TOLERANCE      = 1e-7;    // 0.0000001
		constexpr static inline double const DEFAULT_FIT_TOLERANCE     = 1e-10;   // 0.0000000001
		constexpr static inline double const DEFAULT_KNOT_TOLERANCE    = 1e-7;    // 0.0000001

	public:
		static inline std::string const entityName = "SPLINE"s;
		BSC__DEFINE_CTOR_DTOR_DERIVED(dimeSpline, dimeEntity);
		BSC__DEFINE_CLONE_DERIVED(dimeEntity);

		std::string const& getEntityName() const override { return entityName; }

		enum Flags {
			CLOSED   = 0x01,
			PERIODIC = 0x02,
			RATIONAL = 0x04,
			PLANAR   = 0x08,
			LINEAR   = 0x10
		};

		bool hasWeights() const;

		int16 getFlags() const;
		void setFlags(int16 flags);

		int16 getDegree() const;
		void setDegree(int16 degree);

		dxfdouble getControlPointTolerance() const;
		void setControlPointTolerance(const dxfdouble tol);
		dxfdouble getFitPointTolerance() const;
		void setFitPointTolerance(const dxfdouble tol);
		dxfdouble getKnotTolerance() const;
		void setKnotTolerance(const dxfdouble tol);

		size_t getNumKnots() const { return knots.size(); }
		dxfdouble getKnotValue(int idx) const;
		void setKnotValue(int idx, const dxfdouble value);
		void setKnotValues(std::vector<dxfdouble> const& values);

		size_t getNumControlPoints() const { return controlPoints.size(); }
		const dimeVec3f& getControlPoint(int idx) const;
		void setControlPoint(int idx, const dimeVec3f& v);
		void setControlPoints(std::vector<dimeVec3f> const& pts);

		size_t getNumWeights() const { return getNumControlPoints(); }
		dxfdouble getWeight(int idx) const;
		void setWeight(int idx, const dxfdouble w);

		size_t getNumFitPoints() const { return fitPoints.size(); }
		const dimeVec3f& getFitPoint(int idx) const;
		void setFitPoint(int idx, const dimeVec3f& pt);
		void setFitPoints(std::vector<dimeVec3f> const& pts);

		bool getRecord(int groupcode, dimeParam& param, int index) const override;

		//virtual void print() const;
		bool write(dimeOutput& out) override;
		int typeId() const override { return dimeBase::dimeSplineType; }
		size_t countRecords() const override;

	protected:
		bool handleRecord(int groupcode, const dimeParam& param) override;

	private:
		int16 flags{PLANAR};
		int16 degree{3};
		//int16 numKnots{};
		//int16 numControlPoints{};
		//int16 numFitPoints{};
		dxfdouble knotTolerance{DEFAULT_KNOT_TOLERANCE};
		dxfdouble fitTolerance{DEFAULT_FIT_TOLERANCE};
		dxfdouble cpTolerance{DEFAULT_CP_TOLERANCE};
		std::vector<dxfdouble> knots;
		std::vector<dxfdouble> weights;
		std::vector<dimeVec3f> controlPoints;
		std::vector<dimeVec3f> fitPoints;

		// read/handle counters
		//int16 knotCnt{};
		//int16 fitCnt{};
		//int16 cpCnt{};
		//int16 weightCnt{};

	}; // class dimeSpline

	inline int16 dimeSpline::getFlags() const {
		return this->flags;
	}

	inline void dimeSpline::setFlags(int16 f) {
		this->flags = f;
	}

	inline int16 dimeSpline::getDegree() const {
		return this->degree;
	}

	inline void dimeSpline::setDegree(int16 d) {
		this->degree = d;
	}

	inline dxfdouble dimeSpline::getControlPointTolerance() const {
		return this->cpTolerance;
	}

	inline void dimeSpline::setControlPointTolerance(const dxfdouble tol) {
		this->cpTolerance = tol;
	}

	inline dxfdouble dimeSpline::getFitPointTolerance() const {
		return this->fitTolerance;
	}

	inline void dimeSpline::setFitPointTolerance(const dxfdouble tol) {
		this->fitTolerance = tol;
	}

	inline dxfdouble dimeSpline::getKnotTolerance() const {
		return this->knotTolerance;
	}

	inline void dimeSpline::setKnotTolerance(const dxfdouble tol) {
		this->knotTolerance = tol;
	}

	inline dxfdouble dimeSpline::getKnotValue(int idx) const {
		ASSERT(idx >= 0 && idx < this->knots.size());
		return this->knots[idx];
	}

	inline void dimeSpline::setKnotValue(int idx, const dxfdouble value) {
		ASSERT(idx >= 0 && idx < this->knots.size());
		this->knots[idx] = value;
	}

	inline const dimeVec3f& dimeSpline::getControlPoint(int idx) const {
		ASSERT(idx >= 0 && idx < this->controlPoints.size());
		return this->controlPoints[idx];
	}

	inline void dimeSpline::setControlPoint(int idx, const dimeVec3f& v) {
		ASSERT(idx >= 0 && idx < this->controlPoints.size());
		this->controlPoints[idx] = v;
	}


	inline dxfdouble dimeSpline::getWeight(int idx) const {
		if (this->hasWeights()) {
			ASSERT(idx >= 0 && idx < this->controlPoints.size());
			return this->weights[idx];
		}
		return 1.0;
	}

	inline const dimeVec3f& dimeSpline::getFitPoint(int idx) const {
		ASSERT(idx >= 0 && idx < this->fitPoints.size());
		return this->fitPoints[idx];
	}

	inline void dimeSpline::setFitPoint(int idx, const dimeVec3f& pt) {
		ASSERT(idx >= 0 && idx < this->fitPoints.size());
		this->fitPoints[idx] = pt;
	}

} // namespace dime

