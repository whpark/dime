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
  \class dimeTable dime/tables/Table.h
  \brief The dimeTable class stores table entries.

  This class will not be allocated by the memory handler, but it will
  store a pointer to the memory handler, and it will be used to allocate
  table entries and records.
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

module dime.biscuit:tables.Table;
import std;
import biscuit;
import :Basic;
import :util;
import :Input;
import :Output;
import :Model;
import :Record;

using namespace std::literals;

namespace dime {

	/*!
	  Reads a table.
	*/

	bool dimeTable::read(dimeInput& file) {
		int32 groupcode;
		bool ok = true;
		this->tableEntries.clear();

		do {
			if (!file.readGroupCode(groupcode)) { ok = false; break; }
			if (groupcode == 70) {
				if (!file.readInt16(this->maxEntries)) { ok = false; break; }
			}
			else if (groupcode == 2) {
				auto s = file.readString();
				if (s.empty()) { ok = false; break; }
				this->setTableName(std::move(s));
			}
			else if (groupcode != 0) {
				records.emplace_back(groupcode);
				if (!records.back().read(file)) { ok = false; break; }
			}
		} while (groupcode != 0);

		if (ok) {
			if (this->maxEntries)
				this->tableEntries.reserve(this->maxEntries);
			while (ok) {
				if (groupcode != 0) { ok = false; break; }
				std::string string = file.readString();
				if (string == "ENDTAB") break; // end of table
				auto entry = dimeTableEntry::createTableEntry(string);
				if (!entry or !entry->read(file)) { ok = false; break; }
				this->tableEntries.push_back(std::move(entry));
				ok = file.readGroupCode(groupcode);
			}
			this->tableEntries.shrink_to_fit();
		}
		return ok;
	}

	/*!
	  Writes the table to \a file.
	*/

	bool dimeTable::write(dimeOutput& file) {
		bool ret = true;
		file.writeGroupCode(0);
		file.writeString("TABLE");
		int i;

		file.writeGroupCode(2);
		ret = file.writeString(this->tableName());

		if (ret) {
			for (auto const& r : records) {
				if (!r.writeRecord(file)) {
					ret = false;
					break;
				}
			}
		}
		if (ret) {
			ret &= file.writeGroupCode(70);
			ret &= file.writeInt16(this->tableEntries.size());	// todo: GroupCode 70 is Int32. not Int16
		}
		if (ret) {
			for (auto const& e : tableEntries) {
				if (!e->write(file)) {
					ret = false;
					break;
				}
			}
			if (ret) {
				file.writeGroupCode(0);
				file.writeString("ENDTAB");
			}
		}
		return ret;
	}


	//!

	std::string const& dimeTable::tableName() const {
		if (!this->tablename.empty()) return this->tablename;
		if (this->tableEntries.size()) return this->tableEntries.front()->getTableName();
		return "";
	}

	void dimeTable::setTableName(std::string name) {
		tablename = std::move(name);	
	}

	/*!
	  Counts the number of records in this table.
	*/

	size_t dimeTable::countRecords() const {
		size_t cnt = 2; // header + maxEntries
		cnt += this->records.size();
		for (auto const& e : this->tableEntries) {
			cnt += e->countRecords();
		}
		cnt++; // ENDTAB
		return cnt;
	}

	/*!
	  Returns the number of table entries in this table.
	*/

	size_t dimeTable::getNumTableEntries() const {
		return this->tableEntries.size();
	}

	/*!
	  Returns the table entry at index \a idx.
	*/

	dimeTableEntry* dimeTable::getTableEntry(int idx) {
		ASSERT(idx >= 0 && idx < this->tableEntries.size());
		return this->tableEntries[idx].get();
	}

	/*!
	  Removes (and deletes if no memory handler is used) the table entry at index
	  \a idx.
	*/

	void dimeTable::removeTableEntry(int idx) {
		ASSERT(idx >= 0 && idx < this->tableEntries.size());
		this->tableEntries.erase(tableEntries.begin() + idx);
	}

	/*!
	  Inserts a new table entry at index \a idx. If \a idx is negative, the
	  table entry will be inserted at the end of the list of table entries.
	*/

	void dimeTable::insertTableEntry(std::unique_ptr<dimeTableEntry> tableEntry, int idx) {
		if (idx < 0)
			this->tableEntries.push_back(std::move(tableEntry));
		else {
			ASSERT(idx <= this->tableEntries.size());
			this->tableEntries.insert(tableEntries.begin()+idx, std::move(tableEntry));
		}
	}

	/*!
	  Returns the number of table records in this table. Table records precedes
	  the table entries, and contain some information about this table.
	  The max. entries record (group code 70), is handled automatically by this
	  class, and should not be set by the user.
	*/

	size_t dimeTable::getNumTableRecords() const {
		return this->records.size();
	}

	/*!
	  Returns the table record at index \a idx.
	*/

	dimeRecord& dimeTable::getTableRecord(int idx) {
		ASSERT(idx >= 0 && idx < this->records.size());
		return this->records[idx];
	}
	dimeRecord const& dimeTable::getTableRecord(int idx) const {
		ASSERT(idx >= 0 && idx < this->records.size());
		return this->records[idx];
	}

	/*!
	  Removes (and deletes if no memory handler is used) the record at index \a idx.
	*/

	void dimeTable::removeTableRecord(int idx) {
		ASSERT(idx >= 0 && idx < this->records.size());
		this->records.erase(records.begin() + idx);
	}

	/*!
	  Inserts a new record at index \a idx. If \a idx is negative, the
	  record will be inserted at the end of the list of records.
	*/

	void dimeTable::insertTableRecord(dimeRecord record, int idx) {
		ASSERT(record.getGroupCode() != 70);
		if (record.getGroupCode() == 2) {
			this->setTableName(std::get<std::string>(record.getValue()));
		}
		size_t i = idx >= 0 ? idx : this->records.size();
		assert(i <= this->records.size());
		this->records.insert(records.begin()+i, std::move(record));
	}

} // namespace dime
