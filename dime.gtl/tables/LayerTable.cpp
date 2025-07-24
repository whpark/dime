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
  \class dimeLayerTable dime/tables/LayerTable.h
  \brief The dimeLayerTable class reads and writes LAYER \e tables.
*/

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

module dime.gtl:tables.LayerTable;
import std;
import :Basic;
import :util;
import :tables.Table;
import :Input;
import :Output;
import :Model;
import :Record;

using namespace std::literals;

namespace dime {

	//!
	bool dimeLayerTable::read(dimeInput& file) {
		bool ret = base_t::read(file);
		if (ret) {
			this->registerLayer(file.getModel());
		}
		return ret;
	}

	//!

	bool dimeLayerTable::write(dimeOutput& file) {
		bool ret = base_t::preWrite(file);

		if (!this->layerName.empty()) {
			ret = file.writeGroupCode(2);
			file.writeString(this->layerName);
		}
		file.writeGroupCode(62);
		file.writeInt16(this->colorNumber);

		ret = dimeTableEntry::write(file);
		return ret;
	}


	//!

	bool dimeLayerTable::handleRecord(int groupcode, const dimeParam& param) {
		switch (groupcode) {
		case 2:
			this->setLayerName(std::get<std::string>(param));
			return true;
		case 62:
			this->setColorNumber(std::get<int16>(param));
			return true;
		}
		return dimeTableEntry::handleRecord(groupcode, param);
	}

	//!

	size_t dimeLayerTable::countRecords() const {
		size_t cnt = 1; // header
		if (this->layerInfo)
			cnt++;
		cnt++; // colorNumber
		return cnt + base_t::countRecords();
	}


	/*!
	  Should be called _once_ after you've finished setting up your
	  layer (name and color number).  Calling this method more than once
	  for a layer might lead to hard-to-find bugs. After calling this
	  method, the layer information (color number) will be available to
	  entities using this layer.
	*/
	// todo: check here
	void dimeLayerTable::registerLayer(dimeModel* model) {
		if (this->layerInfo == nullptr && !this->layerName.empty()) {
			this->layerInfo = (dimeLayer*)
				model->addLayer(this->layerName, std::abs(this->colorNumber));
		}
	}

} // namespace dime

