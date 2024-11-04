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

export module dime.biscuit:objects.Object;
import std;
import biscuit;
import :Basic;
import :util;
import :Base;
import :Input;
import :Output;
import :RecordHolder;
//import :Model;

using namespace std::literals;

namespace dime {
	//class dimeObjectsSection;
}

export namespace dime {

	class dimeObject : public dimeRecordHolder {
		friend class dimeObjectsSection;
		friend class dimeModel;
	public:
		using base_t = dimeRecordHolder;
		using this_t = dimeObject;

	public:
		BSC__DEFINE_R5(dimeObject, dimeRecordHolder);

		virtual std::unique_ptr<dimeObject> clone() const = 0;

		virtual std::string const& getObjectName() const = 0;

		bool read(dimeInput& in) override { return base_t::read(in); }
		bool write(dimeOutput& out) override { return base_t::write(out); }
		bool isOfType(int thetypeid) const override {
			return thetypeid == dimeBase::dimeObjectType || base_t::isOfType(thetypeid);
		}
		virtual int typeId() const = 0;
		size_t countRecords() const override { return base_t::countRecords(); }
		virtual void print() const {}

	protected:
		virtual bool handleRecord(int groupcode, const dimeParam& param) { return false; }

	public:
		static std::unique_ptr<dimeObject> createObject(std::string_view name);

	}; // class dimeObject

} // namespace dime

