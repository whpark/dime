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

export module dime.biscuit:entities.Block;
import std;
import biscuit;
import :Basic;
import :util;
import :Base;
import :Input;
import :Output;
//import :Model;
import :entities.Entity;

using namespace std::literals;

namespace dime {
}

export namespace dime {

	class dimeBlock : public dimeEntity {
		friend class dimeBlocksSection;
		friend class dimeEntitiesSection;
		friend class dimeInsert;
	public:
		static inline auto const entityName = "BLOCK"s;
	public:
		BSC__DEFINE_R5(dimeBlock, dimeEntity);
		BSC__DEFINE_CLONE(dimeEntity);

		dimeVec3f const& getBasePoint() const { return this->basePoint; }
		void setBasePoint(const dimeVec3f& v) { this->basePoint = v; }
		size_t getNumEntities() const { return this->entities.size(); }
		dimeEntity* getEntity(int idx) {
			ASSERT(idx >= 0 && idx < this->entities.size());
			return this->entities[idx].get();
		}
		void insertEntity(std::unique_ptr<dimeEntity> const entity, int idx = -1);
		void removeEntity(int idx/*, const bool deleteIt = true*/);
		void fitEntities();

		std::string const& getName() const;
		void setName(std::string name);

		
		virtual bool getRecord(int groupcode,
			dimeParam& param,
			int index = 0) const;
		std::string const& getEntityName() const override { return entityName; }

		bool read(dimeInput& in) override;
		bool write(dimeOutput& out) override;
		int typeId() const override { return dimeBase::dimeBlockType; }
		size_t countRecords() const override;

	protected:
		bool traverse(dimeState const* state, callbackEntity_t callback) override;
		//void fixReferences(dimeModel* model) override;
		bool handleRecord(int groupcode, const dimeParam& param) override;

	private:
		int16 flags;
		std::string name;
		dimeVec3f basePoint;
		std::vector<tptr_t<dimeEntity>> entities;
		tptr_t<dimeEntity> endblock;

	}; // class dimeBlock


} // namespace dime

