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

export module dime.biscuit:sections.ClassesSection;
import std;
import biscuit;
import :Basic;
import :util;
import :Base;
import :Input;
import :Output;
import :classes.Class;
import :sections.Section;

using namespace std::literals;

namespace dime {
}

export namespace dime {

	class dimeClassesSection : public dimeSection {
		friend class dimeModel;

	public:
		static inline std::string const sectionName = "CLASSES";
		BSC__DEFINE_CTOR_DTOR_DERIVED(dimeClassesSection, dimeSection);
		BSC__DEFINE_CLONE_DERIVED(dimeSection);

		std::string const& getSectionName() const override { return sectionName; }

		bool read(dimeInput& file) override;
		bool write(dimeOutput& file) override;
		int typeId() const override { return dimeBase::dimeClassesSectionType; }
		size_t countRecords() const override;

		size_t getNumClasses() const { return classes.size(); }
		dimeClass* getClass(int idx) {
			ASSERT(idx >= 0 && idx < classes.size());
			return classes[idx].get();
		}
		void removeClass(size_t idx) {
			ASSERT(idx < classes.size());
			classes.erase(classes.begin() + idx);
		}
		void insertClass(std::unique_ptr<dimeClass> myclass, int idx = -1) {
			if (idx < 0)
				this->classes.push_back(std::move(myclass));
			else {
				ASSERT(idx <= this->classes.size());
				this->classes.insert(classes.begin() + idx, std::move(myclass));
			}
		}

		auto& getClasses() { return classes; }
		auto const& getClasses() const { return classes; }

	private:
		std::vector<tptr_t<dimeClass>> classes;

	}; // class dimeClassesSection

} // namespace dime

