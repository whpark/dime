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
  \class dimeInsert dime/entities/Insert.h
  \brief The dimeInsert class handles an INSERT \e entity.
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

#include "biscuit/dependencies_eigen.h"
#include "biscuit/dependencies_units.h"

module dime.biscuit:entities.Insert;
import std;
import biscuit;
import :Basic;
import :util;
import :Record;
import :Input;
import :Output;
import :entities.Block;
import :Model;
import :State;

using namespace std::literals;

namespace dime {

	dimeBlock* dimeInsert::getBlock(dimeModel const& model) const {
		if (!block) {
			block = model.findBlock(blockName);
		}
		return this->block;
	}


	/*!
	  Reads an INSERT entity.
	*/

	bool dimeInsert::read(dimeInput& file) {
		  // see handleRecord() to understand what is done with
		  // blockName here... Ugly code, but who cares :-)
		this->blockName.clear();
		bool ret = dimeEntity::read(file);
		if (!ret)
			return false;
		if (!this->blockName.empty()) {
			file.getModel()->addReference(this->blockName, nullptr);
			char* tmp = (char*)this->blockName;
			this->blockName = file.getModel()->findRefStringPtr(tmp);
			if (this->blockName) {
				this->block = (dimeBlock*)file.getModel()->findReference(tmp);
			}
			else {
			  // probably a forward reference, just add as reference
				this->blockName = file.getModel()->addReference(tmp, NULL);
			}
			delete[] tmp;
		}

		if (ret && this->attributesFollow) {
			dimeMemHandler* memhandler = file.getMemHandler();
			// read following entities.
			std::vector<dimeEntity*> array;
			ret = dimeEntity::readEntities(file, array, "SEQEND");
			if (ret) {
				this->seqend = dimeEntity::createEntity("SEQEND", memhandler);
				// read the SEQEND entity
				if (!this->seqend || !this->seqend->read(file)) ret = false;
			}
			int n = array.count();
			if (ret && n) {
				this->entities = ARRAY_NEW(memhandler, dimeEntity*, n);
				if (this->entities) {
					this->numEntities = n;
					for (int i = 0; i < n; i++) {
						this->entities[i] = array[i];
					}
				}
				else ret = false;
			}
		}
		return ret;
	}

	/*!
	  Writes an INSERT entity.
	*/

	bool
		dimeInsert::write(dimeOutput& file) {
		this->preWrite(file);

		if (this->attributesFollow) {
			file.writeGroupCode(66);
			file.writeInt16(1);
		}
		file.writeGroupCode(2);
		file.writeString(this->blockName);

		file.writeGroupCode(10);
		file.writeDouble(insertionPoint[0]);
		file.writeGroupCode(20);
		file.writeDouble(insertionPoint[1]);
		file.writeGroupCode(30);
		file.writeDouble(insertionPoint[2]);

		if (this->scale != dimeVec3f(1, 1, 1)) {
			file.writeGroupCode(41);
			file.writeDouble(this->scale[0]);
			file.writeGroupCode(42);
			file.writeDouble(this->scale[1]);
			file.writeGroupCode(43);
			file.writeDouble(this->scale[2]);
		}
		if (this->rotAngle != 0.0) {
			file.writeGroupCode(50);
			file.writeDouble(this->rotAngle);
		}

		if (this->columnCount != 1) {
			file.writeGroupCode(70);
			file.writeInt16(this->columnCount);
		}
		if (this->rowCount != 1) {
			file.writeGroupCode(71);
		#ifdef DIME_FIXBIG
			file.writeInt32(this->rowCount);
		#else
			file.writeInt16(this->rowCount);
		#endif
		}
		if (this->columnSpacing != 0.0) {
			file.writeGroupCode(44);
			file.writeDouble(this->columnSpacing);
		}
		if (this->rowSpacing != 0.0) {
			file.writeGroupCode(45);
			file.writeDouble(this->rowSpacing);
		}

		bool ret = dimeEntity::write(file); // write unknown records

		if (this->extrusionDir != dimeVec3f(0, 0, 1)) {
			file.writeGroupCode(210);
			file.writeDouble(this->extrusionDir[0]);
			file.writeGroupCode(220);
			file.writeDouble(this->extrusionDir[1]);
			file.writeGroupCode(230);
			ret = file.writeDouble(this->extrusionDir[2]);
		}

		if (this->attributesFollow && this->numEntities) {
			int i;
			for (i = 0; i < this->numEntities; i++) {
				if (!this->entities[i]->write(file)) break;
			}
			if (this->seqend) ret = this->seqend->write(file);
			else {
				file.writeGroupCode(0);
				ret = file.writeString("SEQEND");
			}
		}
		return ret;
	}

	//!

	bool
		dimeInsert::handleRecord(int groupcode,
			const dimeParam& param,
			) {
		switch (groupcode) {
		case 66:
			this->attributesFollow = std::get<int16>(param);
			return true;
		case 2:
			{
			  // will only arrive here during read(). Allocate a temporary buffer
			  // to store the blockname. Will be deleted in dimeInsert::read() 
				const char* str = std::get<std::string>(param);
				if (str) {
					this->blockName = new char[strlen(str)+1];
					if (this->blockName) {
						strcpy((char*)this->blockName, str);
					}
				}
				return true;
			}
		case 10:
		case 20:
		case 30:
			this->insertionPoint[groupcode/10-1] = std::get<double>(param);
			return true;
		case 210:
		case 220:
		case 230:
			this->extrusionDir[(groupcode-210)/10] = std::get<double>(param);
			return true;
		case 41:
		case 42:
		case 43:
			this->scale[groupcode-41] = std::get<double>(param);
			return true;
		case 44:
			this->columnSpacing = std::get<double>(param);
			return true;
		case 45:
			this->rowSpacing = std::get<double>(param);
			return true;
		case 50:
			this->rotAngle = std::get<double>(param);
			return true;
		case 70:
			this->columnCount = std::get<int16>(param);
			return true;
		case 71:
		#ifdef DIME_FIXBIG
			this->rowCount = std::get<int32>(param);
		#else
			this->rowCount = std::get<int16>(param);
		#endif
			return true;
		}
		return dimeEntity::handleRecord(groupcode, param, memhandler);
	}

	//!

	const char*
		dimeInsert::getEntityName() const {
		return entityName;
	}

	//!

	bool
		dimeInsert::getRecord(int groupcode,
			dimeParam& param,
			int index) const {
		switch (groupcode) {
		case 66:
			param.int16_data = this->attributesFollow;
			return true;
		case 2:
			param.string_data = this->blockName;
			return true;
		case 10:
		case 20:
		case 30:
			param.double_data = this->insertionPoint[groupcode/10-1];
			return true;
		case 210:
		case 220:
		case 230:
			param.double_data = this->extrusionDir[(groupcode-210)/10];
			return true;
		case 41:
		case 42:
		case 43:
			param.double_data = this->scale[groupcode-41];
			return true;
		case 44:
			param.double_data = this->columnSpacing;
			return true;
		case 45:
			param.double_data = this->rowSpacing;
			return true;
		case 50:
			param.double_data = this->rotAngle;
			return true;
		case 70:
			param.int16_data = this->columnCount;
			return true;
		case 71:
		#ifdef DIME_FIXBIG
			param.int32_data = this->rowCount;
		#else
			param.int16_data = this->rowCount;
		#endif
			return true;
		}
		return dimeEntity::getRecord(groupcode, param, index);
	}

	//!

	bool
		dimeInsert::traverse(const dimeState* const state,
			dimeCallback callback,
			void* userdata) {
		dimeState newstate = *state;
		newstate.currentInsert = this;

		if (this->block && (state->getFlags() & dimeState::EXPLODE_INSERTS)) {
			for (int i = 0; i < this->rowCount; i++) {
				for (int j = 0; j < this->columnCount; j++) {
					dimeMatrix m = state->getMatrix();
					dimeMatrix m2 = dimeMatrix::identity();
					m2.setTranslate(dimeVec3f(j*this->columnSpacing,
						i*this->rowSpacing,
						0));
					m.multRight(m2);
					this->makeMatrix(m);
					newstate.setMatrix(m);
					if (!block->traverse(&newstate, callback, userdata)) return false;
				}
			}
		}
		else if (!this->isDeleted()) {
			if (!callback(state, this, userdata)) return false;
		}

		dimeMatrix m = state->getMatrix();
		this->makeMatrix(m);
		newstate.setMatrix(m);

		// extract internal INSERT entities
		for (int i = 0; i < this->numEntities; i++) {
			if (!this->entities[i]->traverse(&newstate, callback, userdata)) return false;
		}
		return true;
	}

	//!

	void
		dimeInsert::fixReferences(dimeModel* const model) {
		if (this->block == NULL && this->blockName) {
			this->block = (dimeBlock*)model->findReference(this->blockName);
			if (this->block == NULL) {
				fprintf(stderr, "BLOCK %s not found!\n", blockName);
			}
		}
		for (int i = 0; i < this->numEntities; i++)
			this->entities[i]->fixReferences(model);
	}

	//!

	void
		dimeInsert::makeMatrix(dimeMatrix& m) const {
		if (!this->block) {
			m.makeIdentity();
			return;
		}
		dimeMatrix m2;

		if (this->extrusionDir != dimeVec3f(0, 0, 1)) {
		  // this block has its own coordinate system
		  // generated from one vector (the z-vector)
			dimeEntity::generateUCS(this->extrusionDir, m2);
			m.multRight(m2);
		}

		m2.makeIdentity();
		dimeVec3f tmp = this->insertionPoint;

		// disabled for the moment
		// dimeModel::fixDxfCoords(tmp);

		m2.setTranslate(tmp);
		m.multRight(m2);

		m2.setTransform(dimeVec3f(0, 0, 0),
			this->scale,
			dimeVec3f(0, 0, this->rotAngle));
		m.multRight(m2);

		m2.makeIdentity();
		m2.setTranslate(-block->getBasePoint());
		m.multRight(m2);
	}

	//!

	int
		dimeInsert::countRecords() const {
		size_t cnt = 5; // header + blockName + insertionPoint

		if (this->attributesFollow) cnt++;

		if (this->scale != dimeVec3f(1.0, 1.0, 1.0)) cnt += 3;
		if (this->rotAngle != 0.0) cnt++;
		if (this->columnCount != 1) cnt++;
		if (this->rowCount != 1) cnt++;
		if (this->columnSpacing != 0.0) cnt++;
		if (this->rowSpacing != 0.0) cnt++;
		if (this->extrusionDir != dimeVec3f(0, 0, 1)) cnt += 3;

		if (this->attributesFollow && this->numEntities) {
			int i, n = this->numEntities;
			for (i = 0; i < n; i++) {
				cnt += this->entities[i]->countRecords();
			}
			cnt++; // seqend
		}
		return cnt + dimeEntity::countRecords();
	}

	/*!
	  Sets the block for this INSERT entity. This will change the record
	  with group code 2.
	*/

	void
		dimeInsert::setBlock(dimeBlock* const block) {
		this->block = block;
		this->blockName = block->getName();
	}

