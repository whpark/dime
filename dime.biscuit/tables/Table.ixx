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

export module dime.biscuit:tables.Table;
import std;
import biscuit;
import :Basic;
import :util;
import :Base;
import :Input;
import :Output;
import :tables.TableEntry;
import :Record;

using namespace std::literals;

namespace dime {
}

export namespace dime {

	class dimeTable : public dimeBase {
	public:
		dimeTable();
		dimeTable(dimeTable const&) = default;
		dimeTable(dimeTable&&) = default;
		dimeTable& operator = (dimeTable const&) = default;
		dimeTable& operator = (dimeTable&&) = default;
		virtual ~dimeTable();
		std::unique_ptr<dimeTable> clone() const { return std::make_unique<dimeTable>(*this); }

		virtual bool read(dimeInput& in);
		virtual bool write(dimeOutput& out);

		int typeId() const override { return dimeBase::dimeTableType; }
		virtual size_t countRecords() const;
		virtual int tableType() const {
			if (tableEntries.empty()) return -1;
			return tableEntries.front()->typeId();
		}

		void setTableName(std::string name);
		std::string const& tableName() const;

		size_t getNumTableEntries() const;
		dimeTableEntry* getTableEntry(const int idx);
		void insertTableEntry(std::unique_ptr<dimeTableEntry> tableEntry, const int idx = -1);
		void removeTableEntry(const int idx);

		size_t getNumTableRecords() const;
		dimeRecord& getTableRecord(const int idx);
		dimeRecord const& getTableRecord(const int idx) const;
		void insertTableRecord(dimeRecord record, const int idx = -1);
		void removeTableRecord(const int idx);

	private:
		int16 maxEntries; // dummy variable read from file
		std::string tablename;
		std::vector<biscuit::TCloneablePtr<dimeTableEntry>> tableEntries;
		std::vector<dimeRecord> records;
	}; // class dimeTable

} // namespace dime

