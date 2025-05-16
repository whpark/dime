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
  \class dimeHeaderSection dime/sections/HeaderSection.h
  \brief The dimeHeaderSection class handles the HEADER \e section.
*/

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


module dime.biscuit:sections.HeaderSection;
import std;
import "biscuit/dependencies_eigen.hxx";
import "biscuit/dependencies_units.hxx";
import biscuit;
import :Basic;
import :util;
import :Base;
import :Input;
import :Output;
import :Record;
import :sections.Section;
import :RecordHolder;
import :Model;

using namespace std::literals;

namespace dime {

	/*!
	  Returns a header variable.
	  The variable attributes are copied into the \a groupcodes and \a params
	  arrays. No more than \a maxparams attributes are copied.
	  This method returns the number of attributes copied into the arrays,
	  or -1 if the variable could not be found.
	*/

	std::vector<dimeRecord> dimeHeaderSection::getVariable(std::string_view variableName, size_t maxParams) const {
		std::vector<dimeRecord> rs;
		auto idx = findVariable(variableName);
		if (!idx)
			return rs;
		for (size_t i = *idx; i < records.size() and maxParams-- and records[i].groupCode != 9; i++) {
			rs.push_back(this->records[i]);
		}
		return rs;
	}

	/*!
	  Sets a header variable.

	  If the variable already exists in the header section, its value is
	  overwritten.  Otherwise, a new variable is created and appended to the
	  existing variables.
	*/

	void dimeHeaderSection::setVariable(std::string_view variableName, std::vector<dimeRecord> const& records_) {
		auto idx = findVariable(variableName);
		if (!idx) {
			idx = records.size();
			records.emplace_back(9, std::string(variableName));
		}
		size_t i = *idx;
		for (auto&& record : records_) {
			this->records.insert(records_.begin() + (++i), record);
		}
	}

	//!

	bool dimeHeaderSection::read(dimeInput& file) {
		bool ok = true;
		this->records.clear();
		records.reserve(512);

		while (true) {
			dimeRecord record;
			if (!record.readRecord(file)) {
				ok = false;
				break;
			}
			if (record.isEndOfSectionRecord()) {
				break;
			}
			this->records.push_back(std::move(record));
		}
		this->records.shrink_to_fit();
		return ok;
	}

	//!

	bool dimeHeaderSection::write(dimeOutput& file) {
		if (!file.writeGroupCode(2) or !file.writeString(sectionName))
			return false;
		for (auto& record : this->records) {
			if (!record.writeRecord(file))
				return false;
		}

		// don't forget to write EOS record
		file.writeGroupCode(0);
		return file.writeString("ENDSEC");
	}

	std::optional<size_t> dimeHeaderSection::findVariable(std::string_view name) const {
		ASSERT(records.size() >= 0);
		for (auto const& [i, record] : std::views::enumerate(records)) {
			if (record.groupCode == 9
				and record.param.index() == std::to_underlying(eDimeParam::str)
				and std::get<std::string>(record.param) == name) {
				return i;
			}
		}
		return std::nullopt;
	}

} // namespace dime

