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
import :Base;
import :Layer;
//import :entities.Entity;
import :sections.Section;
//import dime.biscuit.util;

using namespace std::literals;

namespace dime {
	class dimeInput;
	class dimeOutput;
	class dimeSection;
	class dimeEntitiesSection;
	class dimeBlocksSection;
	class dimeBlock;
	class dimeEntity;
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
			void* userdata = nullptr,
			bool traverseBlocksSection = false,
			bool explodeInserts = true,
			bool traversePolylineVertices = false);

		const char* addReference(const char* const name, void* id);
		void* findReference(const char* const name) const;
		const char* findRefStringPtr(const char* const name) const;
		void removeReference(const char* const name);
		dimeMemHandler* getMemHandler();

		int getNumLayers() const;
		const dimeLayer* getLayer(const int idx) const;
		const dimeLayer* getLayer(const char* const layername) const;
		const dimeLayer* addLayer(std::string layername, const int16 colnum = 7, const int16 flags = 0);

		std::string const& getDxfVersion() const;

		static std::string const& getVersionString();
		static void getVersion(int& major, int& minor);
		std::string const& addBlock(std::string_view blockname, dimeBlock* const block);
		class dimeBlock* findBlock(std::string_view blockname);

		template < typename T >
		T* findSection(std::string_view name) {
			for (auto& section : sections) {
				if (section->getSectionName() == name)
					return dynamic_cast<T*>(section.get());
			}
			return nullptr;
		}
		const class dimeSection* findSection(const char* const sectionname) const;

		size_t getNumSections() const { return sections.size(); }
		auto* getSection(const int idx) { return sections[idx].get(); }
		void insertSection(std::unique_ptr<dimeSection> section, const int idx = -1) {
			if (idx < 0) sections.push_back(std::move(section));
			else {
				ASSERT(idx <= sections.size());
				sections.insert(sections.begin() + idx, std::move(section));
			}
		}
		void removeSection(const int idx) {
			ASSERT(idx >= 0 && idx < sections.size());
			sections.erase(sections.begin() + idx);
		}

		void registerHandle(const int handle);
		void registerHandle(std::string const& handle);
		int getUniqueHandle();
		std::string getUniqueHandleHexString();
		void addEntity(std::unique_ptr<dimeEntity> entity);

	private:
		std::vector<tptr_t<dimeSection>> sections;
		biscuit::TContainerMap<std::string, tptr_t<dimeLayer>> layers;
		std::vector<tptr_t<dimeRecord>> headerComments;
	}; // class dimeModel

} // namespace dime



