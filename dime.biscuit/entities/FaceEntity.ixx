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

export module dime.biscuit:entities.FaceEntity;
import std;
import biscuit;
import :Basic;
import :util;
import :Base;
import :entities.Entity;
import :Input;
import :Output;
import :Record;

using namespace std::literals;

namespace dime {
}

export namespace dime {

	class dimeFaceEntity : public dimeEntity {
	private:
		BSC__DEFINE_CTOR_DTOR_DERIVED(dimeFaceEntity, dimeEntity);
	public:
		//BSC__DEFINE_CLONE_DERIVED(dimeEntity);

		bool isQuad() const;

		bool getRecord(int groupcode, dimeParam& param, int index = 0) const override;

		void setVertex(int idx, const dimeVec3f& v);
		void setTriangle(const dimeVec3f& v0, const dimeVec3f& v1, const dimeVec3f& v2);
		void setQuad(const dimeVec3f& v0, const dimeVec3f& v1, const dimeVec3f& v2, const dimeVec3f& v3);
		const dimeVec3f& getVertex(int idx) const;
		void getVertices(dimeVec3f& v0, dimeVec3f& v1, dimeVec3f& v2, dimeVec3f& v3) const;

		virtual dxfdouble getThickness() const;
		virtual void getExtrusionDir(dimeVec3f& ed) const;

		GeometryType extractGeometry(std::vector<dimeVec3f>& verts,
			std::vector<int>& indices,
			dimeVec3f& extrusionDir,
			dxfdouble& thickness);

		int typeId() const override { return dimeFaceEntityType; }
		bool isOfType(int thetypeid) const override {
			return thetypeid == dimeFaceEntityType || dimeEntity::isOfType(thetypeid);
		}
		size_t countRecords() const override { return 12 + base_t::countRecords(); }

	protected:
		virtual bool swapQuadCoords() const;

		bool handleRecord(int groupcode, const dimeParam& param) override;
		void copyCoords(const dimeFaceEntity* entity);
		bool writeCoords(dimeOutput& file);

		//dimeFaceEntity();
		std::array<dimeVec3f, 4> coords;

	}; // class dimeFaceEntity

	inline const dimeVec3f& dimeFaceEntity::getVertex(int idx) const {
		ASSERT(idx >= 0 && idx < 4);
		return this->coords[idx];
	}

	inline bool dimeFaceEntity::isQuad() const {
		return (coords[2] != coords[3]);
	}

	inline void dimeFaceEntity::setVertex(int idx, const dimeVec3f& v) {
		ASSERT(idx >= 0 && idx < 4);
		this->coords[idx] = v;
	}

} // namespace dime

