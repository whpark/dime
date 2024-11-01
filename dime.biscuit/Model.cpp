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
  \class dimeModel dime/Model.h
  \brief The dimeModel class organizes a model.

  The constructor accepts a boolean value which specifies whether or not a
  memory handler should be used. The special purpose memory handler used
  in Coin can be used if you're just going to read a file and write the
  file, and not do too much dynamic work on the model. The memory handler
  yields very fast allocation/deallocation, and has virtually no overhead
  when allocating. This is important if you have large files with millions
  of records. The disadvantage is that memory will not be freed until the
  model is destructed, so if you modify your model, e.g. remove or replace
  an entity, the memory for the now unused entity will not be freed until
  the model is destructed. Then all used memory will be freed at once.

  Also, if you plan to implement your own entities, it takes a bit of extra
  care to support the memory handler. In short, you should always check
  if a memory allocator should be used before allocating memory, since
  the destructor for entities will never be called when a memory
  handler is used. See the documentation in dimeEntity for more information
  about how to create your own entities and how to support the memory
  handler.
*/

module dime.biscuit:Model;
import std;
import biscuit;
import :Basic;
import :Layer;
import :util;
import :Input;
import :Output;
import :State;
import :Sections;
import :Entities;
import :Record;

using namespace std::literals;

/*!
  This method returns a string saying which version of DIME is used.
*/

namespace dime {

	std::string const& dimeModel::getVersionString() {
		static std::string version = "DIME v0.9 biscuit";
		return version;
	}

	void dimeModel::getVersion(int& major, int& minor) {
		major = 0;
		minor = 9;
	}

	static std::string const SECTIONID = "SECTION"s;
	static std::string const EOFID = "EOF"s;

	/*!
	  Should be called before you start working with the model.  Will
	  be called by read() so if you're reading a model from a file you
	  will not have to worry about this.

	  The method cleans up the old data structures and creates
	  new data structures for the new model.
	*/
	bool dimeModel::init() {
		this->sections.clear();
		this->layers.clear();
		//headerComments.clear();
		return true;
	}

	/*!
	  Reads the model file into the internal structures.
	*/

	bool dimeModel::read(dimeInput& in) {
		//in.model = this; // _very_ important

		this->init();

		int32 groupcode{};
		bool ok = true;

		while (true) {
			ok = false;
			if (!in.readGroupCode(groupcode))
				break;
			if (groupcode != 0 && groupcode != 999)
				break;
			auto string = in.readString();
			if (string.empty())
				break;

			if (groupcode == 999) {
				ok = true;
				this->headerComments.emplace_back(groupcode, std::move(string));
				continue;
			}
			if (string == SECTIONID) {
				ok = in.readGroupCode(groupcode);
				string = in.readString();
				ok &= !string.empty() && groupcode == 2;
				if (!ok)
					break;
				auto section = dimeSection::createSection(string);
				ok = section and section->read(in);
				if (!ok) break;
				this->sections.push_back(std::move(section));
			}
			else if (string == EOFID) {
				ok = true;
				break;
			}
			else
				break; // something unexpected has happened
		}
		if (!ok) {
			if (in.aborted) {
			#ifdef _DEBUG
				std::println("DXF read aborted by user.");
			#endif
			}
			else {
			#ifdef _DEBUG
				std::println("DXF loading failed at line: {}", in.getFilePosition());
			#endif
			}
		}
		else {
			if (auto* bs = this->findSection<dimeBlockSection>("BLOCKS"))
				bs->fixReferences(this);
			if (auto* es = this->findSection<dimeEntitiesSection>("ENTITIES"))
				es->fixReferences(this);
		//#ifndef NDEBUG
		//    fprintf(stderr,"dimeModel::largestHandle: %d\n", this->largestHandle);
		//#endif
		}
		return ok;
	}

	/*!
	  Writes the model to file. Currently only DXF files are supported, but
	  hopefully DWG will be supported soon.
	*/

	bool dimeModel::write(dimeOutput& out) {
		if (largestHandle > 0) {
			if (auto* hs = this->findSection<dimeHeaderSection>("HEADER")) {
				dimeParam param;
				int groupcode;
				if (hs->getVariable("$HANDSEED", &groupcode, &param, 1) == 1) {
					auto h = this->getUniqueHandleHexString();
					this->largestHandle--; // ok to use this handle next time
					param.string_data = buf;
					hs->setVariable("$HANDSEED", &groupcode, &param, 1, this->getMemHandler());
				}
			}
		}
		(void)out->writeHeader();
		int i, n = this->headerComments.count();
		for (i = 0; i < n; i++) {
			this->headerComments[i]->write(out);
		}

		n = sections.count();
		for (i = 0; i < n; i++) {
			out->writeGroupCode(0);
			out->writeString(SECTIONID);
			if (!sections[i]->write(out)) break;
		}
		if (i == n) {
			return out->writeGroupCode(0) && out->writeString(EOFID);
		}
		return false;
	}

	/*!
	  Adds a reference in this model's dictionary. Used by BLOCK and
	  INSERT entities to resolve references, but can also be used
	  for other purposes.
	*/

	const char*
		dimeModel::addReference(const char* const name, void* id) {
		char* ptr = NULL;
		refDict->enter(name, ptr, id);
		return (const char*)ptr;
	}

	/*!
	  Finds a reference from the dictionary.
	*/

	void*
		dimeModel::findReference(const char* const name) const {
		void* id;
		if (refDict->find(name, id))
			return id;
		return NULL;
	}

	/*!
	  Finds a pointer to a string in the dictionary.
	*/

	const char*
		dimeModel::findRefStringPtr(const char* const name) const {
		return refDict->find(name);
	}

	/*!
	  Removes a reference from the dictionary.
	*/

	void
		dimeModel::removeReference(const char* const name) {
		refDict->remove(name);
	}

	/*!
	  Returns a pointer to the memory handler used for this model.
	*/

	dimeMemHandler*
		dimeModel::getMemHandler() {
		return this->memoryHandler;
	}

	/*!
	  Adds a layer to the list of layers. If the layer already exists, a
	  pointer to the existing layer will be returned.
	*/

	const dimeLayer*
		dimeModel::addLayer(std::string name, const int16 colnum,
			const int16 flags) {
		void* temp = NULL;
		if (!this->layerDict->find(name, temp)) {
		  // default layer has layer-num = 0, hence the + 1
			dimeLayer* layer = new dimeLayer(name, this->layers.count()+1,
				colnum, flags);
			char* ptr;
			layerDict->enter(name, ptr, layer);
			// this is a little hack...
			layer->layerName = ptr; // need a pointer that won't disappear
			this->layers.append(layer);
			return layer;
		}
		return (dimeLayer*)temp;
	}

	/*!
	  Returns the layer at index \a idx.
	  \sa dimeModel::getNumLayers()
	*/

	const dimeLayer*
		dimeModel::getLayer(const int idx) const {
		assert(idx >= 0 && idx <= this->layers.count());
		return this->layers[idx];
	}

	/*!
	  Returns the layer with name \a layername. Returns \e NULL if no layer
	  by that name is found.
	*/

	const dimeLayer*
		dimeModel::getLayer(const char* const layername) const {
		void* ptr = NULL;
		this->layerDict->find(layername, ptr);
		return (const dimeLayer*)ptr;
	}

	/*!
	  Returns the number of layers in the model. A default layer will always
	  be created. It is called "Default DIME layer", and it has layer id
	  number 0. All other layers are assigned running numbers from 1.

	  \sa dimeLayer::getLayerNum()
	  \sa dimeModel::getLayer()
	*/

	int
		dimeModel::getNumLayers() const {
		return layers.count();
	}

	/*!
	  Use this to add a block to the model. Make sure you also add it in the
	  BLOCKS section.
	*/

	const char*
		dimeModel::addBlock(const char* const blockname, dimeBlock* const block) {
		char* ptr = NULL;
		refDict->enter(blockname, ptr, block);
		return (const char*)ptr;
	}

	/*!
	  Returns a pointer to the block with name \a blockname, or \e NULL
	  if no block with that name exists.
	*/

	dimeBlock*
		dimeModel::findBlock(const char* const blockname) {
		void* tmp = NULL;
		this->refDict->find(blockname, tmp);
		return (dimeBlock*)tmp;
	}

	/*!
	  Returns the AutoCAD drawing database version number. This function
	  return \e NULL if no version number is found in the file.
	  Currently (directly) supported versions are: r10, r11/r12, r13 and r14.
	*/

	const char*
		dimeModel::getDxfVersion() const {
		const dimeHeaderSection* header =
			(const dimeHeaderSection*)this->findSection("HEADER");

		if (!header) {
			return NULL;
		}

		int groupcode;
		dimeParam param;

		if (header->getVariable("$ACADVER", &groupcode, &param, 1) != 1 ||
			groupcode != 1) {
			return NULL;
		}
		if (!strcmp(std::get<std::string>(param), "AC1006")) return "r10";
		if (!strcmp(std::get<std::string>(param), "AC1009")) return "r11/r12";
		if (!strcmp(std::get<std::string>(param), "AC1012")) return "r13";
		if (!strcmp(std::get<std::string>(param), "AC1013")) return "r14";

		return NULL;
	}

	/*!
	  Counts the number of records in the file. Useful if you need progress
	  information while writing the file to disk.

	  \sa dimeOutput::setCallback()
	*/

	int
		dimeModel::countRecords() const {
		size_t cnt = 0;
		int i, n = sections.count();
		for (i = 0; i < n; i++) {
			cnt += 1 + this->sections[i]->countRecords();
		}
		cnt++; // EOF
		return cnt;
	}

	/*
	  Stupid function to reset the z-value of some rare DXF files that have
	  some of their z-coordinates set to -999999. I have no clue what this
	  is all about... Somebody FIXME please!
	*/

	// void
	// dimeModel::fixDxfCoords(dimeVec3f &c)
	// {
	//   const dxfdouble dummy_dxf_val = -999998.0;
	//   if (c[0] <= dummy_dxf_val) c[0] = 0.0;
	//   if (c[1] <= dummy_dxf_val) c[1] = 0.0;
	//   if (c[2] <= dummy_dxf_val) c[2] = 0.0;
	// }

	/*!
	  Traverses all entities in the model.
	*/

	bool dimeModel::traverseEntities(callbackEntity_t callback,
			bool traverseBlocksSection,
			bool explodeInserts,
			bool traversePolylineVertices)
	{
		int i, n;
		dimeState state(traversePolylineVertices, explodeInserts);
		if (traverseBlocksSection) {
			if (auto* bs = this->findSection<dimeBlocksSection>("BLOCKS")) {
				for (auto& block : bs->getBlocks()) {
					if (!block->traverse(&state, callback, userdata))
						return false;
				}
			}
		}
		if (auto* es = this->findSection<dimeEntitiesSection>("ENTITIES")) {
			for (auto& entity : es->getEntities()) {
				if (!entity->traverse(&state, callback, userdata))
					return false;
			}
		}

		return true;
	}

	/*!
	  Newer DXF files has stupid handles (group code 5) for all
	  entities, tables etc. I can't understand they have no real purpose,
	  but all handles must be unique when the file is loaded back into
	  AutoCAD...
	*/
	void dimeModel::registerHandle(const int handle) {
		if (handle >= this->largestHandle) {
			this->largestHandle = handle;
		}
	}

	/*!
	  \overload
	*/
	void dimeModel::registerHandle(std::string_view handle) {
		auto num = biscuit::tszto<int>(handle, nullptr, 16);
		registerHandle(num);
	}

	int dimeModel::getUniqueHandle() {
		return ++this->largestHandle;
	}

	std::string dimeModel::getUniqueHandleHexString() {
		return std::format("{:x}", getUniqueHandle());
	}

	/*!
	  Convenience function
	*/
	void dimeModel::addEntity(std::unique_ptr<dimeEntity> entity) {
		if (auto* es = this->findSection<dimeEntitiesSection>("ENTITIES")) 
			es->insertEntity(std::move(entity));
	}

} // namespace dime


