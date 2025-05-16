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


#include "../Basic.h"

export module dime.biscuit:entities.LWPolyline;
import std;
import "biscuit/dependencies_eigen.hxx";
import "biscuit/dependencies_units.hxx";
import biscuit;
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

	struct sLWPoint {
		dxfdouble x;
		dxfdouble y;
		dxfdouble startingWidth;
		dxfdouble endWidth;
		dxfdouble bulge;
	};

	class dimeLWPolyline : public dimeExtrusionEntity {
	public:
		static inline std::string const entityName = "LWPOLYLINE";
		BSC__DEFINE_CTOR_DTOR_DERIVED(dimeLWPolyline, dimeExtrusionEntity);
		BSC__DEFINE_CLONE_DERIVED(dimeEntity);

		bool getRecord(int groupcode, dimeParam& param, int index) const override;
		std::string const& getEntityName() const override { return entityName; }

		//virtual void print() const;
		bool write(dimeOutput& out) override;
		int typeId() const override { return dimeBase::dimeLWPolylineType; }
		size_t countRecords() const override;

		GeometryType extractGeometry(std::vector<dimeVec3f>& verts, std::vector<int>& indices, dimeVec3f& extrusionDir, dxfdouble& thickness) override;
		size_t getNumVertices() const;
		std::vector<dxfdouble> const& getXCoords() const;
		std::vector<dxfdouble> const& getYCoords() const;
		std::vector<dxfdouble> const& getStartingWidths() const;
		std::vector<dxfdouble> const& getEndWidths() const;
		std::vector<dxfdouble> const& getBulges() const;

		dxfdouble getElevation() const;
		dxfdouble getConstantWidth() const;
		int16 getFlags() const;

	protected:
		bool handleRecord(int groupcode, const dimeParam& param) override;

	private:
		dxfdouble constantWidth{};
		dxfdouble elevation{};
		int16 flags{};
		int32 numVertices{};
		//mutable int16 tmpCounter{}; // used during loading only
		//mutable uint16 tmpFlagsVertex{};
		//std::vector<dxfdouble> xcoord;
		//std::vector<dxfdouble> ycoord;
		//std::vector<dxfdouble> startingWidth;
		//std::vector<dxfdouble> endWidth;
		//std::vector<dxfdouble> bulge;
		std::vector<sLWPoint> coords;
		mutable std::set<int> tmpFlagsVertexPart;	// used during loading only, vertex parts

	}; // class dimeLWPolyLine


	inline size_t dimeLWPolyline::getNumVertices() const {
		return this->numVertices;
	}

	//inline std::vector<dxfdouble> const& dimeLWPolyline::getXCoords() const {
	//	return this->xcoord;
	//}
	//inline std::vector<dxfdouble> const& dimeLWPolyline::getYCoords() const {
	//	return this->ycoord;
	//}

	//inline std::vector<dxfdouble> const& dimeLWPolyline::getStartingWidths() const {
	//	return this->startingWidth;
	//}

	//inline std::vector<dxfdouble> const& dimeLWPolyline::getEndWidths() const {
	//	return this->endWidth;
	//}

	//inline std::vector<dxfdouble> const& dimeLWPolyline::getBulges() const {
	//	return this->bulge;
	//}

	inline dxfdouble dimeLWPolyline::getElevation() const {
		return this->elevation;
	}

	inline dxfdouble dimeLWPolyline::getConstantWidth() const {
		return this->constantWidth;
	}

	inline int16 dimeLWPolyline::getFlags() const {
		return this->flags;
	}


} // namespace dime

