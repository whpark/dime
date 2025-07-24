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

/*!
  \class dimeTablesSection dime/sections/TablesSection.h
  \brief The dimeTablesSection class handles a TABLES \e section.
*/

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

module dime.gtl:sections.TablesSection;
import std;
import :Basic;
import :util;
import :Base;
import :Input;
import :Output;
import :sections.Section;
import :Record;
import :RecordHolder;
import :Model;
import :tables.Table;


using namespace std::literals;

namespace dime {



	/*!
	  Will read a DXF TABLES section.
	*/

	bool dimeTablesSection::read(dimeInput& file) {
		int32 groupcode{};
		bool ok = true;

	  //  sim_trace("Reading section: TABLES\n");

		while (true) {
			if (!file.readGroupCode(groupcode) || groupcode != 0) {
				std::println("Error reading groupcode: {}", groupcode);
				ok = false;
				break;
			}
			auto string = file.readString();
			if (string == "ENDSEC")
				break;
			if (string != "TABLE") {
				std::println("unexpected string.");
				ok = false;
				break;
			}

			auto table = std::make_unique<dimeTable>();
			if (!table) {
				std::println("error creating table: {}", string);
				ok = false;
				break;
			}
			if (!table->read(file)) {
				std::println("error reading table: {}.", string);
				ok = false;
				break;
			}
			this->tables.push_back(std::move(table));
		}
		return ok;
	}

	//!

	bool dimeTablesSection::write(dimeOutput& file) {
		if (!file.writeGroupCode(2) or !file.writeString(sectionName))
			return false;
		for (auto& table : this->tables) {
			if (!table->write(file))
				return false;
		}
		return file.writeGroupCode(0) && file.writeString("ENDSEC");
	}


	//!

	size_t dimeTablesSection::countRecords() const {
		size_t cnt = 0;
		for (auto& table : tables)
			cnt += table->countRecords();
		return cnt + 2; // two records are written in write()
	}


} // namespace dime

