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

export module dime.gtl:entities.Circle;
import std;
import :Basic;
import :util;
import :entities.ExtrusionEntity;

using namespace std::literals;

export namespace dime {
}

export namespace dime {

	class dimeCircle : public dimeExtrusionEntity {
	public:
		static inline std::string const entityName{ "CIRCLE" };
		BSC__DEFINE_CTOR_DTOR_DERIVED(dimeCircle, dimeExtrusionEntity);
		BSC__DEFINE_CLONE_DERIVED(dimeEntity);

		const dimeVec3f& getCenter() const;
		void setCenter(const dimeVec3f& c);

		void setRadius(const dxfdouble val);
		dxfdouble getRadius() const;

		bool getRecord(int groupcode, dimeParam& param, int index = 0) const override;
		std::string const& getEntityName() const override { return entityName; }
		//virtual void print() const;
		bool write(dimeOutput& out) override;
		int typeId() const override { return dimeBase::dimeCircleType; }
		size_t countRecords() const override;

		GeometryType extractGeometry(std::vector<dimeVec3f>& verts, std::vector<int>& indices, dimeVec3f& extrusionDir, dxfdouble& thickness) override;

	protected:
		virtual bool handleRecord(int groupcode, const dimeParam& param);
	private:
		dimeVec3f center{};
		dxfdouble radius{};

	}; // class dimeCircle

	inline const dimeVec3f& dimeCircle::getCenter() const {
		return this->center;
	}

	inline void dimeCircle::setCenter(const dimeVec3f& c) {
		this->center = c;
	}

	inline void dimeCircle::setRadius(const dxfdouble val) {
		this->radius = val;
	}

	inline dxfdouble dimeCircle::getRadius() const {
		return this->radius;
	}

} // namespace dime

