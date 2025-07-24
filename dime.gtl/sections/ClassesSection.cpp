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
  \class dimeClassesSection dime/sections/ClassesSection.h
  \brief The dimeClassesSection class handles a CLASSES \e section.
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

module dime.gtl:sections.ClassesSection;
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


using namespace std::literals;

namespace dime {


	//!

	bool dimeClassesSection::read(dimeInput& file) {
		int32 groupCode{};
		bool ok = true;
		classes.clear();
		classes.reserve(64);

		while (true) {
			if (!file.readGroupCode(groupCode) or (groupCode != 9 and groupCode != 0)) {
				std::println("Error reading classes groupcode: {}.", groupCode);
				break;
			}
			auto string = file.readString();
			if (string == "ENDSEC")
				break;
			auto myclass = dimeClass::createClass(string);
			if (!myclass) {
				std::println("error creating class: {}.", string);
				ok = false;
				break;
			}
			if (!myclass->read(file)) {
				std::println("error reading class: {}.", string);
				ok = false;
				break;
			}
			this->classes.push_back(std::move(myclass));
		}
		return ok;
	}

	//!

	bool dimeClassesSection::write(dimeOutput& file) {
		file.writeGroupCode(2);
		file.writeString(sectionName);

		for (auto& myclass : this->classes) {
			if (!myclass->write(file))
				return false;
		}
		file.writeGroupCode(0);
		return file.writeString("ENDSEC");
	}

	//!

	size_t dimeClassesSection::countRecords() const {
		size_t cnt = 0;
		for (auto& c : this->classes)
			cnt += c->countRecords();
		return cnt + 2; // two additional records are written in write()
	}

} // namespace dime

