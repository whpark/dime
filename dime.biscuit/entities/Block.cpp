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
  \class dimeBlock dime/entities/Block.h
  \brief The dimeBlock class handles a BLOCK \e entity.

  It cannot strictly be called an entity, as you will only find BLOCKs
  in the BLOCKS section, not in the ENTITIES section. But BLOCKs share
  a lot of attributes and functionality with "real" entities, so
  in DXFLIB, a BLOCK is called an entity.
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


module dime.biscuit:entities.Block;
import std;
import "biscuit/dependencies_eigen.hxx";
import "biscuit/dependencies_units.hxx";
import biscuit;
import :Basic;
import :util;
import :Record;
import :Input;
import :Output;
import :entities.Block;
import :Model;

using namespace std::literals;

namespace dime {


	/*!
	  \fn const dimeVec3f &dimeBlock::getBasePoint() const
	  Returns the base point of this block.
	*/

	/*!
	  \fn void dimeBlock::setBasePoint(const dimeVec3f &v)
	  Sets the base point of this entity.
	*/

	/*!
	  \fn int dimeBlock::getNumEntities() const
	  Returns the number of entities in this block.
	*/

	/*!
	  \fn dimeEntity *dimeBlock::getEntity(int idx)
	  Returns the entity at index \a idx.

	  \sa dimeBlock::getNumEntities()
	*/

	/*!
	  \fn const char *dimeBlock::getName() const
	  Returns the name of this block (used by INSERT to reference the block).
	*/

	/*!
	  \fn void dimeBlock::setName(const char * const name)
	  Sets the name of this block. \a name Must be a static char pointer, or
	  some pointer that will not be deleted before the block, as the text string
	  will not be copied. It is best to avoid using this method. Use
	  dimeModel::addBlock() instead.
	*/



	/*!
	  This method reads a BLOCK entity from \a file.
	*/

	bool dimeBlock::read(dimeInput& file) {
		this->name.clear();
		bool ret = dimeEntity::read(file);
		//if (ret && !this->name.empty()) {
		//	name = file.getModel()->addBlock(name, this);
		//}

		// got to do some reading to get all entities in the block
		if (ret) {
			entities.clear();
			entities.reserve(1024);
			ret = dimeEntity::readEntities(file, this->entities, "ENDBLK");
			if (ret) {
				this->endblock = dimeEntity::createEntity("ENDBLK"sv);
				// read the ENDBLOCK entity
				if (!this->endblock || !this->endblock->read(file))
					ret = false;
			}
			this->entities.shrink_to_fit(); // don't waste too much memory
		}

	#ifdef _DEBUG
		dimeParam param;
		if (getRecord(67, param) && std::get<int16>(param) == 1) {
			std::println("paperspace block name: {}",
				((dimeBlock*)this)->getName());
		}
	#endif
		return ret;
	}

	/*!
	  This methods writes a BLOCK entity to \a file.
	*/

	bool dimeBlock::write(dimeOutput& file) {
		this->preWrite(file);

		file.writeGroupCode(2);
		file.writeString(this->name);
		file.writeGroupCode(70);
		file.writeInt16(this->flags);

		file.writeGroupCode(10);
		file.writeDouble(this->basePoint[0]);
		file.writeGroupCode(20);
		file.writeDouble(this->basePoint[1]);
		file.writeGroupCode(30);
		file.writeDouble(this->basePoint[2]);

		// write unknown records.
		bool ret = dimeEntity::write(file);

		if (ret) {
			for (auto const& e : entities) {
				if (!e->write(file))
					return false;
			}
			if (this->endblock) {
				ret = this->endblock->write(file);
			}
			else { // just put a minimal ENDBLK there
				file.writeGroupCode(0);
				file.writeString("ENDBLK");
				file.writeGroupCode(8);
				ret = file.writeString(this->getLayerName());
			}
		}
		return ret;
	}


	//!

	bool dimeBlock::handleRecord(int groupcode, const dimeParam& param) {
		switch (groupcode) {
		case 2:
			{
				std::string str = std::get<std::string>(param);
				if (!str.empty()) {
					name = std::move(str);
				}
				return true;
			}
		  ////<< PWH
		  //case 8 :
		  //    layerNameW = ToWString(std::get<std::string>(param));
		  //  return true;
		  ////>>
		case 70:
			this->flags = std::get<int16>(param);
			return true;
		case 10:
		case 20:
		case 30:
			this->basePoint[groupcode/10-1] = std::get<double>(param);
			return true;
		}
		return dimeEntity::handleRecord(groupcode, param);
	}

	//!

	bool dimeBlock::getRecord(int groupcode, dimeParam& param, int index) const {
		switch (groupcode) {
		case 2:
			param = this->name;
			return true;
		case 70:
			param.emplace<int16>(this->flags);
			return true;
		case 10:
		case 20:
		case 30:
			param.emplace<double>(this->basePoint[groupcode/10-1]);
			return true;
		}
		return dimeEntity::getRecord(groupcode, param, index);
	}

	////!

	//void dimeBlock::fixReferences(dimeModel* model) {
	//	for (auto& e : entities) {
	//		e->fixReferences(model);
	//	}
	//}

	//!

	size_t dimeBlock::countRecords() const {
		size_t cnt = 0;
		cnt += 3; // header
		cnt += 3; // basePoint

		for (auto const& e : entities)
			cnt += e->countRecords();

		return cnt + base_t::countRecords();
	}

	/*!
	  Inserts an entity in this block at position \a idx.
	*/

	void dimeBlock::insertEntity(std::unique_ptr<dimeEntity> entity, int idx) {
		if (idx < 0)
			this->entities.push_back(std::move(entity));
		else {
			ASSERT(idx <= this->entities.size());
			this->entities.insert(entities.begin() + idx, std::move(entity));
		}
	}

	/*!
	  Removes the entity at position \a idx. If \a deleteIt is \e true, and
	  no memory handler is used, the entity will be deleted before
	  returning from this method.
	*/

	void dimeBlock::removeEntity(int idx/*, const bool deleteIt*/) {
		ASSERT(idx >= 0 && idx < this->entities.size());
		this->entities.erase(entities.begin()+idx);
	}

	//!

	bool dimeBlock::traverse(const dimeState* state, callbackEntity_t callback) {
		if (callback(state, this)) {
		  //FIXME: what to do with basePoint?
			for (auto const& e : entities) {
				if (!e->traverse(state, callback)) return false;
			}
		}
		if (this->endblock)
			return callback(state, this->endblock.get());
		return true;
	}

	/*!
	  Since a growable array is used to hold the entities, it might sometimes
	  use more memory than absolutely needed. Call this method after you have
	  finished modifying a block if you want to free that overhead memory.
	*/

	void dimeBlock::fitEntities() {
		this->entities.shrink_to_fit();
	}


} // namespace dime
