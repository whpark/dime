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
  \class dimeObjectsSection dime/sections/ObjectsSection.h
  \brief The dimeObjectsSection object handles an OBJECTS \e section.
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


module dime.biscuit:sections.ObjectsSection;
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
import :objects;


using namespace std::literals;

namespace dime {


	//!

	bool dimeObjectsSection::read(dimeInput& file) {
		int32 groupcode{};
		objects.clear();
		objects.reserve(64);

	  //  sim_trace("Reading section: OBJECTS\n");

		while (true) {
			if (!file.readGroupCode(groupcode) || groupcode != 0) {
				std::println("Error reading objects groupcode: {}.\n", groupcode);
				return false;
			}
			auto string = file.readString();
			if (string == "ENDSEC"sv) break;
			auto object = dimeObject::createObject(string);
			if (!object) {
				std::println("error creating object: {}.\n", string);
				return false;
			}
			if (!object->read(file)) {
				std::println("error reading object: {}.\n", string);
				return false;
			}
			objects.push_back(std::move(object));
		}
		return true;
	}

	//!

	bool dimeObjectsSection::write(dimeOutput& file) {
		file.writeGroupCode(2);
		file.writeString(sectionName);

		for (auto const& o : objects) {
			if (!o->write(file))
				return false;
		}
		file.writeGroupCode(0);
		file.writeString("ENDSEC");
		return true;
	}


	//!

	size_t dimeObjectsSection::countRecords() const {
		size_t cnt{};
		for (auto const& o : objects)
			cnt += o->countRecords();
		return cnt + 2; // two additional records are written in write()
	}


	/*!
	  Returns the object at index \a idx.
	*/

	dimeObject* dimeObjectsSection::getObject(int idx) {
		ASSERT(idx >= 0 && idx < this->objects.size());
		return this->objects[idx].get();
	}

	/*!
	  Removes (and deletes if no memory handler is used) the object at index \a idx.
	*/

	void dimeObjectsSection::removeObject(int idx) {
		ASSERT(idx >= 0 && idx < this->objects.size());
		objects.erase(objects.begin() + idx);
	}

	/*!
	  Inserts a new object at index \a idx. If \a idx is negative, the
	  object will be inserted at the end of the list of objects.
	*/

	void dimeObjectsSection::insertObject(std::unique_ptr<dimeObject> object, int idx) {
		if (idx < 0) objects.push_back(std::move(object));
		else {
			ASSERT(idx <= objects.size());
			objects.insert(objects.begin()+idx, object);
		}
	}

} // namespace dime
