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

export module dime.biscuit:Model;
import std;
import biscuit;
import :Basic;
import :util;
import :Base;
import :Record;
import :Layer;
//import :entities.Entity;
import :sections.Section;
import :entities.Block;

using namespace std::literals;

export namespace dime {
	class dimeInput;
	class dimeOutput;
	class dimeSection;
	//class dimeEntitiesSection;
	//class dimeBlocksSection;
	//class dimeEntity;
	class dimeRecord;
} // namespace dime

export namespace dime {

	class dimeModel {
	public:
		using this_t = dimeModel;
	public:
		dimeModel() { init(); }
		dimeModel(dimeModel const&) = default;
		dimeModel(dimeModel&&) = default;
		dimeModel& operator = (dimeModel const&) = default;
		dimeModel& operator = (dimeModel&&) = default;
		~dimeModel();

		std::unique_ptr<dimeModel> clone() const { return std::make_unique<this_t>(*this); }

		bool init();
		bool read(dimeInput& in);
		bool write(dimeOutput& out);

		size_t countRecords() const;

		bool traverseEntities(callbackEntity_t callback,
			bool traverseBlocksSection = false,
			bool explodeInserts = true,
			bool traversePolylineVertices = false);

		void addReference(std::string_view name, void* id);
		void* findReference(std::string_view name) const;
		char const* findRefStringPtr(std::string_view name) const;
		void removeReference(std::string_view name);

		size_t getNumLayers() const;
		dimeLayer const* getLayer(int idx) const;
		dimeLayer const* getLayer(std::string_view layername) const;
		dimeLayer const* addLayer(std::string_view layername, int16 colnum = 7, int16 flags = 0);

		std::string const& getDxfVersion() const;

		static std::string const& getVersionString();
		static void getVersion(int& major, int& minor);
		//bool addBlock(std::string const& blockname, std::unique_ptr<dimeBlock> block);
		//dimeBlock* findBlock(std::string const& blockname);
		//dimeBlock const* findBlock(std::string const& blockname) const;

		template < typename T >
		T* findSection(std::string_view name) {
			for (auto& section : sections) {
				if (section->getSectionName() == name)
					return dynamic_cast<T*>(section.get());
			}
			return nullptr;
		}
		template < typename T >
		T const* findSection(std::string_view name) const {
			for (auto const& section : sections) {
				if (section->getSectionName() == name)
					return dynamic_cast<T const*>(section.get());
			}
			return nullptr;
		}
		dimeSection const* findSection(const char* const sectionname) const;

		size_t getNumSections() const { return sections.size(); }
		auto* getSection(int idx) { return sections[idx].get(); }
		void insertSection(std::unique_ptr<dimeSection> section, int idx = -1) {
			if (idx < 0) sections.push_back(std::move(section));
			else {
				ASSERT(idx <= sections.size());
				sections.insert(sections.begin() + idx, std::move(section));
			}
		}
		void removeSection(int idx) {
			ASSERT(idx >= 0 && idx < sections.size());
			sections.erase(sections.begin() + idx);
		}

		void registerHandle(int handle);
		void registerHandle(std::string_view handle);
		int getUniqueHandle();
		std::string getUniqueHandleHexString();
		void addEntity(std::unique_ptr<dimeEntity> entity);

	private:
		int largestHandle{};
		//std::map<std::string, tptr_t<dimeBlock, biscuit::TStaticCloner<dimeBlock>>> blocks;
		std::vector<tptr_t<dimeSection>> sections;
		std::vector<dimeLayer> layers;
		std::vector<dimeRecord> headerComments;
	}; // class dimeModel

} // namespace dime

