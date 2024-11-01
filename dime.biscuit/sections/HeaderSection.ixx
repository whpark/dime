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

export module dime.biscuit:sections.HeaderSection;
import std;
import biscuit;
import :Basic;
import :util;
import :Base;
import :Input;
import :Output;
import :Model;
import :Record;
import :sections.Section;

using namespace std::literals;

namespace dime {
}

export namespace dime {

	class dimeHeaderSection : public dimeSection {
	public:
		static inline std::string const sectionName{ "HEADER" };

		BSC__DEFINE_R5(dimeHeaderSection, dimeSection);
		BSC__DEFINE_CLONE(dimeSection);

		std::string const& getSectionName() const override { return sectionName; }

		std::vector<dimeRecord> getVariable(std::string_view variableName, size_t maxParams) const;
		size_t setVariable(std::string_view variableName, std::vector<dimeRecord> const& records);


		bool read(dimeInput& file) override;
		bool write(dimeOutput& file) override;
		int typeId() const override { return dimeBase::dimeHeaderSectionType; }
		size_t countRecords() const override {
			return this->records.count() + 2; // numrecords + SECTIONNAME + EOS
		}

	private:
		int findVariable(std::string_view name) const {
			ASSERT((int)records.size() >= 0);
			const int n = (int)this->records.size();
			for (int i = 0; i < n; i++) {
				if (this->records[i].groupCode == 9
					and this->records[i].param.index() == eDimeParam::str
					and std::get<std::string>(this->records[i].param) == name)
				{
					return i;
				}
			}
			return -1;
		}

		std::vector<dimeRecord> records;

	}; // class dimeHeaderSection

} // namespace dime

