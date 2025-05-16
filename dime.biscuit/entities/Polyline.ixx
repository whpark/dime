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

export module dime.biscuit:entities.Polyline;
import std;
import "biscuit/dependencies_eigen.hxx";
import "biscuit/dependencies_units.hxx";
import biscuit;
import :Basic;
import :util;
import :entities.Entity;
import :entities.ExtrusionEntity;
import :entities.Vertex;

using namespace std::literals;

export namespace dime {
}

export namespace dime {


	class dimePolyline : public dimeExtrusionEntity {
	public:
		static inline std::string const entityName = "POLYLINE"s;
		BSC__DEFINE_CTOR_DTOR_DERIVED(dimePolyline, dimeExtrusionEntity);
		BSC__DEFINE_CLONE_DERIVED(dimeEntity);

		std::string const& getEntityName() const override { return entityName; }
		enum Type {
			POLYLINE,
			POLYFACE_MESH,
			POLYGON_MESH
		};

		enum Flags {
			CLOSED            = 0x01,
			POLYMESH_CLOSED_M = 0x01,
			CURVE_FIT         = 0x02,
			SPLINE_FIT        = 0x04,
			IS_POLYLINE_3D    = 0x08,
			IS_POLYMESH_3D    = 0x10,
			POLYMESH_CLOSED_N = 0x20,
			IS_POLYFACE_MESH  = 0x40,
			CONTINOUS_PATTERN = 0x80
		};

		enum SurfaceType {
			NONE            = 0,
			QUADRIC_BSPLINE = 5,
			CUBIC_BSPLINE   = 6,
			BEZIER          = 8
		};

		int16 getFlags() const;
		void setFlags(int16 flags);

		int getType() const;

		dimeVec3f const& getElevation() const;
		void setElevation(const dimeVec3f& e);

		int16 getPolymeshCountN() const;
		int16 getPolymeshCountM() const;
		int16 getSmoothSurfaceMdensity() const;
		int16 getSmoothSurfaceNdensity() const;

		size_t getNumCoordVertices() const;
		size_t getNumIndexVertices() const;
		size_t getNumSplineFrameControlPoints() const;

		int16 getSurfaceType() const;
		void setSurfaceType(int16 type);

		dimeVertex& getCoordVertex(int index);
		dimeVertex& getIndexVertex(int index);
		dimeVertex& getSplineFrameControlPoint(int index);

		void setCoordVertices(std::vector<dimeVertex> vertices) { this->coordVertices = std::move(vertices); }
		void setIndexVertices(std::vector<dimeVertex> vertices) { this->indexVertices = std::move(vertices); }
		void setSplineFrameControlPoints(std::vector<dimeVertex> vertices) { this->frameVertices = std::move(vertices); }

// KRF, 02-16-2006, added to enable ::copy of new polyline
		void setSeqend(std::unique_ptr<dimeEntity> ent) {
			this->seqend = std::move(ent);
		}

		bool getRecord(int groupcode, dimeParam& param, int index = 0) const override;

		void setLayer(dimeLayer const* layer) override;

		bool read(dimeInput& in) override;
		bool write(dimeOutput& out) override;
		int typeId() const override { return dimeBase::dimePolylineType; }
		size_t countRecords() const override;

		GeometryType extractGeometry(std::vector<dimeVec3f>& verts, std::vector<int>& indices, dimeVec3f& extrusionDir, dxfdouble& thickness) override;

		void clearSurfaceData();

	protected:
		bool handleRecord(int groupcode, const dimeParam& param) override;
		bool traverse(const dimeState* const state, callbackEntity_t callback) override;

	private:
		int numCoordVertices() const;
		int numIndexVertices() const;

		int16 flags{};

	#ifdef DIME_FIXBIG
		int32 countM{};
		int32 countN{};
		int32 smoothCountM{};
		int32 smoothCountN{};
	#else
		int16 countM{};
		int16 countN{};
		int16 smoothCountM{};
		int16 smoothCountN{};
	#endif

		int16 surfaceType{};

		//int32 coordCnt{};  // real # of coordinate vertices
		//int32 indexCnt{};  // real # of index vertices
		//int32 frameCnt{};

		std::vector<dimeVertex> coordVertices;
		std::vector<dimeVertex> indexVertices;
		std::vector<dimeVertex> frameVertices;
		tptr_t<dimeEntity> seqend;
		dimeVec3f elevation;
	}; // class dimePolyline

	inline int16 dimePolyline::getFlags() const {
		return this->flags;
	}

	inline void dimePolyline::setFlags(int16 f) {
		this->flags = f;
	}

	inline const dimeVec3f& dimePolyline::getElevation() const {
		return elevation;
	}

	inline void dimePolyline::setElevation(const dimeVec3f& e) {
		this->elevation = e;
	}

	inline int16 dimePolyline::getPolymeshCountN() const {
		return this->countN;
	}

	inline int16 dimePolyline::getPolymeshCountM() const {
		return this->countM;
	}

	inline int16 dimePolyline::getSmoothSurfaceMdensity() const {
		return this->smoothCountM;
	}

	inline int16 dimePolyline::getSmoothSurfaceNdensity() const {
		return this->smoothCountN;
	}

	inline size_t dimePolyline::getNumCoordVertices() const {
		return this->coordVertices.size();
	}

	inline size_t dimePolyline::getNumIndexVertices() const {
		return this->indexVertices.size();
	}

	inline size_t dimePolyline::getNumSplineFrameControlPoints() const {
		return this->frameVertices.size();
	}

	inline dimeVertex& dimePolyline::getCoordVertex(int index) {
		return this->coordVertices[index];
	}

	inline dimeVertex& dimePolyline::getIndexVertex(int index) {
		return this->indexVertices[index];
	}

	inline dimeVertex& dimePolyline::getSplineFrameControlPoint(int index) {
		return this->frameVertices[index];
	}

	inline int16 dimePolyline::getSurfaceType() const {
		return this->surfaceType;
	}

	inline void dimePolyline::setSurfaceType(int16 type) {
		this->surfaceType = type;
	}


} // namespace dime

