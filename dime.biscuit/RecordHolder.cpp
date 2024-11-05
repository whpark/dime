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
  \class dimeRecordHolder dime/RecordHolder.h
  \brief The dimeRecordHolder class is a superclass for objects that store
  records.

  This class makes it very easy to add new classes to dime, as it handles
  all of the reading, error checking and storing of records of no use to the
  subclass.  Subclasses will only need to implement the
  dimeRecordHolder::handleRecord() and dimeRecordHolder::getRecord() methods.
*/

module dime.biscuit:RecordHolder;
import std;
import biscuit;
import :Base;
import :util;
import :Basic;
import :Input;
import :Output;
import :Record;

using namespace std::literals;
using namespace biscuit::literals;

namespace dime {

	/*!
	  Reads records from \a in until the separator group code (specified in
	  constructor) is found. Can be overloaded by subclasses, but in most
	  cases this will not be necessary as dimeRecordHolder::handleRecord()
	  is called for each record found in the stream.

	  \sa dimeEntity::handleRecord().
	*/

	bool dimeRecordHolder::read(dimeInput& file) {
		bool ok{true};
		int32 groupcode;
		std::vector<dimeRecord> array;	// temporary array
		array.reserve(256);

		while (true) {
			if (!file.readGroupCode(groupcode)) {
				ok = false;
				break;
			}
			if (groupcode == 0 /*this->separator*/) {
				file.putBackGroupCode(groupcode);
				break;
			}
			else { // check if subclass will handle this record
				dimeRecord r(groupcode);
				ok = r.read(file);
				if (!ok) {
					std::println("Unable to read record data for groupcode: {}\n", groupcode);
					break;
				}
				if (!this->handleRecord(groupcode, r.param)) {
					array.push_back(std::move(r));
				}
			}
		}
		if (ok)
			this->records = std::move(array);
		return ok;
	}

	/*!
	  Will write the records to \a file.
	*/

	bool dimeRecordHolder::write(dimeOutput& file) {
		for (auto const& r : records) {
			if (!r.writeRecord(file))
				return false;
		}
		return true;
	}

	/*!
	  Must be overloaded by entities that directly support a record
	  type. During dimeRecordHolder::read(), dimeRecordHolder::setRecord
	  and dimeRecordHolder::setRecords, this function is called for every
	  record found, and it is up to the subclass if the record should be
	  stored internally, or if a generic record should be created and
	  stored in this superclass. A subclass should return \e true when it
	  will handle the record, \e false otherwise.  Default function does
	  nothing, and returns \e false.

	  For entities, records with group codes 8 (layer name) and 62 (color
	  number) are automatically handled by the dimeEntity class.

	  \sa dimeRecordHolder::read()
	  \sa dimeRecordHolder::setRecord() */

	bool dimeRecordHolder::handleRecord(int, const dimeParam&) {
		return false;
	}

	/*!
	  Sets the data for the record with group code \a groupcode. If the
	  record already exists, its value will simply be overwritten,
	  otherwise a new record will be created.
	  If the record is handled by a subclass, \a param will be passed on
	  to the subclass (using dimeRecordHolder::handleRecord()), and will be
	  ignored by dimeRecordHolder.

	  For entities, you cannot use this method to set the layer name. Use
	  dimeEntity::setLayer() to do that. Also, you should not use
	  this function to set the block name for a dimeInsert entity,
	  use dimeInsert::setBlock() instead.

	  \sa dimeRecordHolder::handleRecord()
	  \sa dimeRecordHolder::getRecord()
	  \sa dimeRecordHolder::setRecords()
	*/

	void dimeRecordHolder::setRecord(int groupcode, const dimeParam& value) {
		this->setRecordCommon(groupcode, value, 0);
	}

	/*!
	  Basically the same function as setRecord(), but also allows you
	  to specify an index for the record. This is useful if you're going
	  to set several records with the same group code.
	  \sa dimeRecordHolder::setRecord()
	*/

	void dimeRecordHolder::setIndexedRecord(int groupcode, const dimeParam& value, int index) {
		this->setRecordCommon(groupcode, value, index);
	}

	/*!
	  Will return the value of the record with group code \a groupcode.
	  \e false is returned if the record could not be found.
	  Subclasses should overload this method if one or several records are
	  stored in the class. If the group code queried is not stored internally, the
	  subclass should call its parent's method.
	*/

	bool dimeRecordHolder::getRecord(int groupcode, dimeParam& param, int index) const {
		for (auto& r : records) {
			if (r.groupCode == groupcode) {
				if (index-- == 0) {
					param = r.param;
					return true;
				}
			}
		}
		return false;
	}

	/*!
	  Sets an array of entities. It is much more efficient to use this than
	  using dimeRecordHolder::setRecord() several times if you are going to set
	  the value of more than one record. Otherwise behaves exactly as
	  dimeRecordHolder::setRecord().
	*/

	void dimeRecordHolder::setRecords(int* const groupcodes, const dimeParam* const params, int numrecords) {
		int i;
		std::vector<dimeRecord> newrecords;
		newrecords.reserve(numrecords);

		for (i = 0; i < numrecords; i++) {
			int groupcode = groupcodes[i];
			const dimeParam& param = params[i];

			if (groupcode == 8) {
				throw std::runtime_error("Cannot set layer name in setRecords()!");
			}
			else if (groupcode == 2 && this->typeId() == dimeBase::dimeInsertType) {
				throw std::runtime_error("Cannot set block name for INSERT entities using setRecords()");
			}
			else if (!this->handleRecord(groupcode, param)) {
				dimeRecord* record = this->findRecord(groupcode);
				if (record) {
					record->param = param;
				}
				else {
					newrecords.emplace_back(groupcode, param);
				}
			}
		}
		if (newrecords.size()) {
			// don't forget the old records...
			newrecords.reserve(records.size() + newrecords.size());
			for (auto& r : records)
				newrecords.push_back(std::move(r));
			records = std::move(newrecords);
		}
	}

	/*!
	  Returns the number of records in the record holder. Should be overloaded by
	  subclasses which should count their records, and then call the parent's
	  method. This method is used to precalculate the number of records to be
	  written. Very useful when progress information is needed during write().
	*/

	size_t dimeRecordHolder::countRecords() const {
		return records.size();
	}

	/*!
	  Returns the record with group code \a groupcode. If \a index > 0,
	  the index'th record with group code \a groupcode will be
	  returned. Returns \e NULL if the record is not found or \a index is
	  out of bounds.
	*/

	dimeRecord* dimeRecordHolder::findRecord(int groupcode, int index) {
		for (auto& r : records) {
			if (r.groupCode == groupcode) {
				if (index-- == 0)
					return &r;
			}
		}
		return nullptr;
	}

	/*!
	  Can be overloaded by subclasses that want the record holder to
	  store a record, but handle writing themselves. Default
	  method returns \a true for all group codes.
	*/
	bool dimeRecordHolder::shouldWriteRecord(int /*groupcode*/) const {
		return true;
	}

	void dimeRecordHolder::setRecordCommon(int groupcode, const dimeParam& param, int index) {
		// some safety checks
		if (groupcode == 8 && this->isOfType(dimeBase::dimeEntityType)) {
			throw std::runtime_error("Cannot set layer name in setRecord()!");
		}
		else if (groupcode == 2 && this->typeId() == dimeBase::dimeInsertType) {
			throw std::runtime_error("Cannot set block name for INSERT entities using setRecord()");
		}

		if (!this->handleRecord(groupcode, param)) {
			if (dimeRecord* record = this->findRecord(groupcode, index))
				record->param = param;
			else
				records.emplace_back(groupcode, param);
		}
	}

	/*!
	  Returns the number of records stored in this record holder.
	*/
	size_t dimeRecordHolder::getNumRecordsInRecordHolder(void) const {
		return records.size();
	}

	/*!
	  Returns the \a idx'th record in the record holder.
	  \sa getNumRecordsInRecordHolder().
	*/
	dimeRecord const& dimeRecordHolder::getRecordInRecordHolder(int idx) const {
		ASSERT(idx < records.size() and idx >= 0);
		return records[idx];
	}

} // namespace dime


