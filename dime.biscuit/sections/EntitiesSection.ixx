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

export module dime.biscuit:sections.EntitiesSection;
import std;
import "biscuit/dependencies_eigen.hxx";
import "biscuit/dependencies_units.hxx";
import biscuit;
import :Basic;
import :util;
import :Base;
import :Input;
import :Output;
//import :Model;
import :entities.Entity;
import :sections.Section;

using namespace std::literals;

namespace dime {
}

export namespace dime {

	class dimeEntitiesSection : public dimeSection {
		friend class dimeModel;
	public:
		static inline std::string const sectionName{ "ENTITIES" };
		BSC__DEFINE_CTOR_DTOR_DERIVED(dimeEntitiesSection, dimeSection);
		BSC__DEFINE_CLONE_DERIVED(dimeSection);

		std::string const& getSectionName() const override { return sectionName; }

		bool read(dimeInput& file) override;
		bool write(dimeOutput& file) override;
		int typeId() const override { return dimeBase::dimeEntitiesSectionType; }
		size_t countRecords() const override;

		//void fixReferences(dimeModel* model);

		/*!
		  Returns the number of entities in this section. Be aware that a POLYLINE
		  entity with attached VERTEX entities will count as a single entity.
		*/
		size_t getNumEntities() const { return entities.size(); }
		dimeEntity* getEntity(size_t idx) { ASSERT(idx >= 0 and idx < entities.size()); return entities[idx].get(); }
		void removeEntity(size_t idx) { ASSERT(idx < entities.size()); entities.erase(entities.begin() + idx); }
		void insertEntity(std::unique_ptr<dimeEntity> entity, int idx = -1) {
			if (idx < 0) entities.push_back(std::move(entity));
			else entities.insert(entities.begin() + idx, std::move(entity));
		}

		auto& getEntities() { return entities; }
		auto const& getEntities() const { return entities; }

	private:
		std::vector<tptr_t<dimeEntity>> entities;

	}; // class dimeEntitiesSection

} // namespace dime

