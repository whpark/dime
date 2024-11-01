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

#include "biscuit/dependencies_eigen.h"
#include "biscuit/dependencies_units.h"

module dime.biscuit:sections.HeaderSection;
import std;
import biscuit;
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

	/*!
	  Returns a header variable.
	  The variable attributes are copied into the \a groupcodes and \a params
	  arrays. No more than \a maxparams attributes are copied.
	  This method returns the number of attributes copied into the arrays,
	  or -1 if the variable could not be found.
	*/

	int
		dimeHeaderSection::getVariable(const char* const variableName,
			int* const groupcodes,
			dimeParam* const params,
			const int maxparams) const {
		int i = this->findVariable(variableName);
		if (i >= 0) { // yup, found it!
			i++;
			int cnt = 0;
			int n = this->records.count();
			while (i < n && cnt < maxparams && this->records[i]->getGroupCode() != 9) {
				groupcodes[cnt] = this->records[i]->getGroupCode();
				this->records[i]->getValue(params[cnt]);
				cnt++;
				i++;
			}
			return cnt;
		}
		return -1;
	}

	/*!
	  Sets a header variable.

	  If the variable already exists in the header section, its value is
	  overwritten.  Otherwise, a new variable is created and appended to the
	  existing variables.
	*/

	int
		dimeHeaderSection::setVariable(const char* const variableName,
			const int* const groupcodes,
			const dimeParam* const params,
			const int numparams,
			) {
		int i = findVariable(variableName);
		if (i < 0) {
			i = this->records.count();
			dimeStringRecord* sr = (dimeStringRecord*)dimeRecord::createRecord(9, memhandler);
			if (!sr) return false;
			sr->setString(variableName, memhandler);

			this->records.append(sr);
			for (int j = 0; j < numparams; j++) {
				this->records.append(dimeRecord::createRecord(groupcodes[j], memhandler));
			}
		}
		i++;
		size_t cnt = 0;
		for (int j = 0; j < numparams; j++) {
			int k = i;
			int n = this->records.count();
			while (k < n && this->records[k]->getGroupCode() != groupcodes[j] &&
				this->records[k]->getGroupCode() != 9) {
				k++;
			}
			if (k < n && this->records[k]->getGroupCode() == groupcodes[j]) {
				cnt++;
				this->records[k]->setValue(params[j]);
			}
		}
		return cnt;
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
			if (record->isEndOfSectionRecord()) {
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

} // namespace dime

