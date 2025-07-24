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

export module dime.gtl:entities.Insert;
import std;
import :Basic;
import :util;
import :Base;
import :entities.Entity;
import :entities.Block;

using namespace std::literals;

namespace dime {
}

export namespace dime {


	class dimeInsert : public dimeEntity {
		friend class dimeEntitiesSection;
		friend class dimeBlocksSection;

	public:
		static inline std::string const entityName{ "INSERT"s };
		BSC__DEFINE_CTOR_DTOR_DERIVED(dimeInsert, dimeEntity);
		BSC__DEFINE_CLONE_DERIVED(dimeEntity);

		void setBlock(dimeBlock* block_) {
			block = block_;
			blockName = block->getName();
		}
		dimeBlock* getBlock() { return block; }

		bool getRecord(int groupcode, dimeParam& param, int index = 0) const override;
		std::string const& getEntityName() const override { return entityName; }

		bool read(dimeInput& in) override;
		bool write(dimeOutput& out) override;
		int typeId() const { return dimeBase::dimeInsertType; }
		size_t countRecords() const override;

		void setInsertionPoint(const dimeVec3f& v);
		const dimeVec3f& getInsertionPoint() const;

		void setScale(const dimeVec3f& v);
		const dimeVec3f& getScale() const;

		void setRotAngle(dxfdouble angle);
		dxfdouble getRotAngle() const;

		//<< PWH
		int GetColumnCount() const { return columnCount; }
		int GetRowCount() const { return rowCount; }
		dxfdouble GetColumnSpacing() const { return columnSpacing; }
		dxfdouble GetRowSpacing() const { return rowSpacing; }
		//>>

		// FIXME: more set and get methods

	protected:
		//void fixReferences(dimeModel* model) override;
		bool handleRecord(int groupcode, const dimeParam& param) override;
		bool traverse(dimeState const* state, callbackEntity_t callback) override;

	private:
		void makeMatrix(dimeMatrix& m) const;

		int16 attributesFollow{};
		std::string blockName;
		dimeVec3f insertionPoint{};
		dimeVec3f extrusionDir{0., 0., 1.};
		dimeVec3f scale{1.,1.,1.};
		dxfdouble rotAngle{};	// in degrees
		std::vector<tptr_t<dimeEntity>> entities;
	#ifdef DIME_FIXBIG
		int32 rowCount{1};
	#else
		int16 rowCount{1};
	#endif
		int16 columnCount{1};
		dxfdouble rowSpacing{};
		dxfdouble columnSpacing{};
		tptr_t<dimeEntity> seqend;
		dimeBlock* block{nullptr};

	}; // class dimeInsert


	inline void dimeInsert::setInsertionPoint(const dimeVec3f& v) {
		this->insertionPoint = v;
	}

	inline const dimeVec3f& dimeInsert::getInsertionPoint() const {
		return this->insertionPoint;
	}

	inline void dimeInsert::setScale(const dimeVec3f& v) {
		this->scale = v;
	}

	inline const dimeVec3f& dimeInsert::getScale() const {
		return this->scale;
	}

	inline void dimeInsert::setRotAngle(dxfdouble angle) {
		this->rotAngle = angle;
	}

	inline dxfdouble dimeInsert::getRotAngle() const {
		return this->rotAngle;
	}


} // namespace dime

