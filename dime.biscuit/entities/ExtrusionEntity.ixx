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
// this dime.biscuit project is a part of the project "biscuit".
// this compiles only inside "biscuit"
// whpark. 2024-10-24
//=============================================================================

#include "biscuit/dependencies_eigen.h"
#include "biscuit/dependencies_units.h"

#include "../Basic.h"

export module dime.biscuit:entities.ExtrusionEntity;
import std;
import biscuit;
import :Basic;
import :util;
import :Base;
import :Input;
import :Output;
import :entities.Entity;

using namespace std::literals;

namespace dime {
}

export namespace dime {

	class dimeExtrusionEntity : public dimeEntity {
	public:
		BSC__DEFINE_CTOR_DTOR_DERIVED(dimeExtrusionEntity, dimeEntity);
		//BSC__DEFINE_CLONE_DERIVED(dimeEntity);

		bool getRecord(int groupcode, dimeParam& param, int index = 0) const override;

		void setExtrusionDir(const dimeVec3f& v);
		const dimeVec3f& getExtrusionDir() const;

		void setThickness(const dxfdouble val);
		dxfdouble getThickness() const;

		int typeId() const override { return dimeBase::dimeExtrusionEntityType; }
		bool isOfType(int thetypeid) const override {
			return thetypeid == dimeExtrusionEntityType || dimeEntity::isOfType(thetypeid);
		}
		size_t countRecords() const override;

	protected:

		bool handleRecord(int groupcode, const dimeParam& param) override;

		void copyExtrusionData(const dimeExtrusionEntity* entity);
		bool writeExtrusionData(dimeOutput& out);

	protected: // should be private :-(
		dimeVec3f extrusionDir {0., 0., 1.};
		dxfdouble thickness {0.};

	}; // class dimeExtrusionEntity

	inline void dimeExtrusionEntity::setExtrusionDir(const dimeVec3f& v) {
		this->extrusionDir = v;
	}

	inline const dimeVec3f& dimeExtrusionEntity::getExtrusionDir() const {
		return this->extrusionDir;
	}

	inline void dimeExtrusionEntity::setThickness(const dxfdouble val) {
		this->thickness = val;
	}

	inline dxfdouble dimeExtrusionEntity::getThickness() const {
		return this->thickness;
	}

} // namespace dime

