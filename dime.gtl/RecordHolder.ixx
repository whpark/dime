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
// this dime.gtl project is a part of the project "gtl".
// this compiles only inside "gtl"
// whpark. 2025-07-24
//=============================================================================

#include "gtl/gtl.h"

export module dime.gtl:RecordHolder;
import std;
import :Basic;
import :util;
import :Base;
import :Input;
import :Output;
import :Record;

export namespace dime {

	class dimeRecordHolder : public dimeBase {
	public:
		using this_t = dimeRecordHolder;
		using base_t = dimeBase;

	public:
		dimeRecordHolder() {}
		dimeRecordHolder(dimeRecordHolder const& rh) = default;
		dimeRecordHolder(dimeRecordHolder&& rh) = default;
		dimeRecordHolder& operator=(dimeRecordHolder const& rh) = default;
		dimeRecordHolder& operator=(dimeRecordHolder&& rh) = default;
		virtual ~dimeRecordHolder() {}

		//std::unique_ptr<dimeBase> NewClone() const override { return std::make_unique<this_t>(*this); }

	public:
		void setRecord(int groupcode, const dimeParam& value);
		void setRecords(int* const groupcodes, const dimeParam* const params, int numrecords);
		void setIndexedRecord(int groupcode, const dimeParam& value, int index);

		virtual bool getRecord(int groupcode, dimeParam& param, int index = 0) const;

		virtual bool read(dimeInput& in);
		virtual bool write(dimeOutput& out);
		virtual bool isOfType(int thetypeid) const override {
			return thetypeid == dimeRecordHolderType || dimeBase::isOfType(thetypeid);
		}
		virtual size_t countRecords() const;

		dimeRecord* findRecord(int groupcode, int index = 0);

		size_t getNumRecordsInRecordHolder(void) const;
		dimeRecord const& getRecordInRecordHolder(int idx) const;

	protected:
		virtual bool handleRecord(int groupcode, const dimeParam& param);

		virtual bool shouldWriteRecord(int groupcode) const;

	protected:
		std::vector<dimeRecord> records;
		// int separator; // not needed ?

	private:
		void setRecordCommon(int groupcode, const dimeParam& param, int index);

	}; // class dimeRecordHolder

}	// namespace dime

