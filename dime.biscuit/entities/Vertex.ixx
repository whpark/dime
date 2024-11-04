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

export module dime.biscuit:entities.Vertex;
import std;
import biscuit;
import :Basic;
import :util;
import :Base;
import :entities.Entity;

using namespace std::literals;

namespace dime {
	class dimePolyline;
}

export namespace dime {

	class dimeVertex : public dimeEntity {
		friend class dimePolyline;
		friend class dimeEntity;

	public:
		enum Flags {
			CURVE_FITTING_VERTEX   = 0x01,
			HAS_CURVE_FIT_TANGENT  = 0x02,
			SPLINE_VERTEX          = 0x08,
			FRAME_CONTROL_POINT    = 0x10,
			POLYLINE_3D_VERTEX     = 0x20,
			POLYGON_MESH_VERTEX    = 0x40,
			POLYFACE_MESH_VERTEX   = 0x80
		};

		static inline std::string const entityName{ "VERTEX"s };
		BSC__DEFINE_R5(dimeVertex, dimeEntity);
		BSC__DEFINE_CLONE(dimeEntity);

		bool getRecord(int groupcode, dimeParam& param, int index = 0) const override;
		std::string const& getEntityName() const override { return entityName; }

		int16 getFlags() const;
		void setFlags(int16 flags);

		void setCoords(const dimeVec3f& v);
		const dimeVec3f& getCoords() const;
		dxfdouble getBulge() const { return bulge; }	// PWH.

		int numIndices() const;
		int getIndex(int idx) const;
		void setIndex(int idx, int val);

		bool write(dimeOutput& out) override;
		int typeId() const override { return dimeBase::dimeVertexType; }
		size_t countRecords() const override;

	protected:
		bool handleRecord(int groupcode, const dimeParam& param) override;

	private:
		int16 flags{};
	#ifdef DIME_FIXBIG
		int32 indices[4] {0, };
	#else
		int16 indices[4] {0, };
	#endif
		dimeVec3f coords{};
		dxfdouble bulge{};// PWH.
		dimePolyline* polyline{}; // link back to polyline...

	}; // class dimeVertex

	inline void dimeVertex::setCoords(const dimeVec3f& v) {
		this->coords = v;
	}

	inline const dimeVec3f& dimeVertex::getCoords() const {
		return this->coords;
	}

	inline void dimeVertex::setIndex(int idx, int val) {
		ASSERT(idx >= 0 && idx < 4);
		this->indices[idx] = val;
	}

	inline int16 dimeVertex::getFlags() const {
		return this->flags;
	}

	inline void dimeVertex::setFlags(int16 flags) {
		this->flags = flags;
	}

} // namespace dime

