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
  \class dimeTableEntry dime/tables/TableEntry.h
  \brief The dimeTableEntry class is the superclass for all table classes.
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

module dime.biscuit:tables.TableEntry;
import std;
import biscuit;
import :Basic;
import :util;
import :tables.Table;
import :Input;
import :Output;
import :Model;
import :Record;
import :tables.UnknownTable;
import :tables.LayerTable;
import :tables.UCSTable;

using namespace std::literals;

namespace dime {

	/*!
	  Static function that creates a table based on its name.
	*/

	std::unique_ptr<dimeTableEntry> dimeTableEntry::createTableEntry(std::string_view name) {
		if (name == "LAYER"sv) {
			return std::make_unique<dimeLayerTable>();
		}
		//if (!strcmp(name, "UCS")) // UCS is not used for the moment
		//  return new(memhandler) dimeUCSTable;

		return std::make_unique<dimeUnknownTable>(std::string(name));
	}

	//!

	bool dimeTableEntry::isOfType(const int thetypeid) const {
		return thetypeid == dimeBase::dimeTableEntryType ||
			dimeRecordHolder::isOfType(thetypeid);
	}

	//!

	bool dimeTableEntry::preWrite(dimeOutput& file) {
		return file.writeGroupCode(0) &&
			file.writeString(this->getTableName()) &&
			base_t::write(file);
	}

	/*!
	  \fn const char * dimeTableEntry::getTableName() const = 0
	*/

	/*!
	  \fn dimeTableEntry * dimeTableEntry::copy(dimeModel * const model) const = 0
	*/

	/*!
	  \fn int dimeTableEntry::typeId() const = 0
	*/

} // namespace dime

