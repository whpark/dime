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
  \class dimeBlocksSection dime/sections/BlocksSection.h
  \brief The dimeBlocksSection class handles a BLOCKS \e section.
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


module dime.biscuit:sections.BlocksSection;
import std;
import "biscuit/dependencies_eigen.hxx";
import "biscuit/dependencies_units.hxx";
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
import :entities.Block;
import :entities.Entity;


using namespace std::literals;

namespace dime {


	/*!
	  This method reads a DXF BLOCKS section.
	*/

	bool dimeBlocksSection::read(dimeInput& file) {
		while (true) {
			int32 groupCode;
			if (!file.readGroupCode(groupCode) or groupCode != 0) {
				std::println("Error reading groupCode: {}", groupCode);
				return false;
			}
			auto string = file.readString();
			if (string == "ENDSEC"sv)
				break;
			if (string != "BLOCK"sv) {
				std::println("Unexpected string.");
				return false;
			}
			auto block = std::make_unique<dimeBlock>();
			if (!block) {
				std::println("error creating block: {}", string);
				return false;
			}
			if (!block->read(file)) {
				std::println("error reading block: {}.", string);
				return false;
			}
			blocks.push_back(std::move(block));
		}
		return true;
	}

	/*!
	  This method writes a DXF BLOCKS section.
	*/

	bool dimeBlocksSection::write(dimeOutput& file) {
		if (!file.writeGroupCode(2) or !file.writeString(sectionName))
			return false;
		for (auto const& block : blocks) {
			if (!block->write(file))
				return false;
		}
		return file.writeGroupCode(0) and file.writeString("ENDSEC");
	}


	///*!
	//  This function should be called after loading has ended, and will
	//  find all forward BLOCK references.
	//*/

	//void dimeBlocksSection::fixReferences(dimeModel* model) {
	//	for (auto& block : blocks) {
	//		block->fixReferences(model);
	//	}
	//}

	//!

	size_t dimeBlocksSection::countRecords() const {
		size_t cnt = 0;
		for (auto const& block : blocks)
			cnt += block->countRecords();
		return cnt + 2; // two records are written in write() 
	}

	/*!
	  Returns the block at index \a idx.
	*/

	dimeBlock* dimeBlocksSection::getBlock(int idx) {
		ASSERT(idx >= 0 && idx < this->blocks.size());
		return this->blocks[idx].get();
	}

	/*!
	  Removes (and deletes if no memory handler is used) the block at index \a idx.
	*/

	void dimeBlocksSection::removeBlock(int idx) {
		ASSERT(idx >= 0 && idx < this->blocks.size());
		blocks.erase(blocks.begin() + idx);
	}

	/*!
	  Inserts a new block at index \a idx. If \a idx is negative, the
	  block will be inserted at the end of the list of blocks.
	*/

	void dimeBlocksSection::insertBlock(std::unique_ptr<dimeBlock> block, int idx) {
		if (idx < 0)
			this->blocks.push_back(std::move(block));
		else {
			ASSERT(idx <= this->blocks.size());
			this->blocks.insert(this->blocks.begin()+idx, std::move(block));
		}
	}

} // namespace dime
