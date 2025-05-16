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


export module dime.biscuit:sections.BlocksSection;
import std;
import "biscuit/dependencies_eigen.hxx";
import "biscuit/dependencies_units.hxx";
import biscuit;
import :Basic;
import :util;
import :Base;
import :Input;
import :Output;
import :objects;
import :sections.Section;
import :entities.Entity;
import :entities.Block;

using namespace std::literals;

namespace dime {
}

export namespace dime {


	class dimeBlocksSection : public dimeSection {
	public:
		using this_t = dimeBlocksSection;
		using base_t = dimeSection;
		static inline std::string const sectionName{ "BLOCKS" };

	public:
		dimeBlocksSection() = default;
		dimeBlocksSection(dimeBlocksSection const&) = default;
		dimeBlocksSection(dimeBlocksSection&&) = default;
		dimeBlocksSection& operator=(dimeBlocksSection const&) = default;
		dimeBlocksSection& operator=(dimeBlocksSection&&) = default;
		virtual ~dimeBlocksSection() = default;

		std::unique_ptr<dimeSection> clone() const override { return std::make_unique<this_t>(*this); }

		std::string const& getSectionName() const override { return sectionName; }

	public:
		bool read(dimeInput& file) override;
		bool write(dimeOutput& file) override;
		int typeId() const override { return dimeBase::dimeBlocksSectionType; }
		size_t countRecords() const override;

		//void fixReferences(dimeModel* model);

		size_t getNumBlocks() const { return blocks.size(); }
		dimeBlock* getBlock(int idx);
		void removeBlock(int idx);
		void insertBlock(std::unique_ptr<dimeBlock> block, int idx = -1);

		auto& getBlocks() { return blocks; }
		auto const& getBlocks() const { return blocks; }

	private:
		std::vector<tptr_t<dimeBlock, biscuit::TStaticCloner<dimeBlock>>> blocks;

	}; // class dimeBlocksSection

} // namespace dime

