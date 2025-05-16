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


export module dime.biscuit:tables.LayerTable;
import std;
import "biscuit/dependencies_eigen.hxx";
import "biscuit/dependencies_units.hxx";
import biscuit;
import :Basic;
import :util;
import :Base;
import :Input;
import :Output;
import :tables.TableEntry;
import :Layer;
//import :Model;

using namespace std::literals;

export namespace dime {
	class dimeModel;
}

export namespace dime {

	class dimeLayerTable : public dimeTableEntry {
	public:
		using base_t = dimeTableEntry;
		using this_t = dimeLayerTable;

		static inline std::string const tableName = "LAYER";

	public:
		dimeLayerTable() = default;
		dimeLayerTable(dimeLayerTable const&) = default;
		dimeLayerTable(dimeLayerTable&&) = default;
		dimeLayerTable& operator = (dimeLayerTable const&) = default;
		dimeLayerTable& operator = (dimeLayerTable&&) = default;
		virtual ~dimeLayerTable() {}

		void setLayerName(std::string name) { layerName = std::move(name); }
		std::string const& getLayerName(void) const { return layerName; }

		void setColorNumber(int16 colnum) {
			colorNumber = colnum;
			if (layerInfo) {
				layerInfo->setColorNumber(colnum < 0 ? -colnum : colnum);
			}
		}
		int16 getColorNumber(void) const { return colorNumber; }

		void registerLayer(dimeModel* model);

		std::unique_ptr<dimeTableEntry> clone() const override { return std::make_unique<this_t>(*this); }

		std::string const& getTableName() const override { return tableName; }
		bool read(dimeInput& in) override;
		bool write(dimeOutput& out) override;
		int typeId() const override { return dimeBase::dimeLayerTableType; }
		size_t countRecords() const override;

	protected:
		virtual bool handleRecord(int groupcode, const dimeParam& param);

	private:
		int16 colorNumber{};
		std::string layerName;
		dimeLayer* layerInfo{};

	}; // class dimeLayerTable

} // namespace dime

