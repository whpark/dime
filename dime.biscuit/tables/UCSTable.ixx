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

export module dime.biscuit:tables.UCSTable;
import std;
import biscuit;
import :Basic;
import :util;
import :tables.TableEntry;

using namespace std::literals;

namespace dime {
}

export namespace dime {

	class dimeUCSTable : public dimeTableEntry {
	public:
		using base_t = dimeTableEntry;
		using this_t = dimeUCSTable;
	public:
		dimeUCSTable() {}
		dimeUCSTable(dimeUCSTable const&) = default;
		dimeUCSTable(dimeUCSTable&&) = default;
		dimeUCSTable& operator = (dimeUCSTable const&) = default;
		dimeUCSTable& operator = (dimeUCSTable&&) = default;
		virtual ~dimeUCSTable() {}

		std::unique_ptr<dimeTableEntry> clone() const override { return std::make_unique<this_t>(*this); }

		std::string const& getTableName() const override;

		const dimeVec3f& getOrigin() const { return this->origin; }
		const dimeVec3f& getXaxis() const { return this->xaxis; }
		const dimeVec3f& getYaxis() const { return this->yaxis; }

		void setOrigin(const dimeVec3f& v) { this->origin = v; }
		void setXaxis(const dimeVec3f& v) { this->xaxis = v; }
		void setYaxis(const dimeVec3f& v) { this->yaxis = v; }

		bool write(dimeOutput& out);
		int typeId() const { return dimeBase::dimeUCSTableType; }
		size_t countRecords() const {
			size_t cnt = 1 + 3 + 3 + 3; // header + origin + xaxis + yaxis
			return cnt + dimeTableEntry::countRecords();
		}

	protected:
		virtual bool handleRecord(const int groupcodes, const dimeParam& param);

	private:
		dimeVec3f origin{0., 0., 0.};
		dimeVec3f xaxis{1., 0., 0.};
		dimeVec3f yaxis{0., 1., 0.};

	}; // class dimeUCSTable

} // namespace dime

