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
  \class dimeClass dime/classes/Class.h
  \brief The dimeClass class is the superclass for all the \e class classes.
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


module dime.biscuit:classes.Class;
import std;
import "biscuit/dependencies_eigen.hxx";
import "biscuit/dependencies_units.hxx";
import biscuit;
import :classes.UnknownClass;
import :Input;
import :Output;
import :util;
import :Model;

using namespace std::literals;

namespace dime {

	/*!
	  \fn const char *dimeClass::getDxfClassName() const = 0
	  Must be implemented by subclasses to return the DXF name of the class.
	*/

	/*!
	  \fn dimeClass *dimeClass::copy(dimeModel * const model) const = 0
	  Must be implemented by subclasses to return a copy of the class.
	*/

	/*!
	  \fn const char *dimeClass::getClassName() const
	  Returns the C++ class name.
	*/

	/*!
	  \fn const char *dimeClass::getApplicationName() const
	  Returns the class application name.
	*/

	/*!
	  \fn int32 dimeClass::getVersionNumber() const
	  Returns the version number of this class.
	*/

	/*!
	  \fn int8 dimeClass::getFlag280() const
	  Returns the flag with group code 280.
	*/

	/*!
	  \fn int8 dimeClass::getFlag281() const
	  Returns the flag with group code 281.
	*/

	/*!
	  \fn void dimeClass::setVersionNumber(int32 v)
	  Sets the version number for this class.
	*/

	/*!
	  \fn void dimeClass::setFlag280(int8 flag)
	  Sets the flag for the flag with group code 280.
	*/

	/*!
	  \fn void dimeClass::setFlag281(int8 flag)
	  Sets the group code for the flag with group code 281.
	*/

	/*!
	  Copies the common and unclassified records.
	*/

	bool dimeClass::copyRecords(dimeClass& myclass) const {
		myclass.records = this->records;
		myclass.className = this->className;
		myclass.appName = this->appName;
		myclass.versionNumber = this->versionNumber;
		myclass.flag1 = this->flag1;
		return true;
	}

	/*!
	  Writes common and unknown class records to file.
	*/

	bool dimeClass::write(dimeOutput& file) {
		file.writeGroupCode(1);
		file.writeString(this->className.empty() ? "Default class name" : this->className);
		file.writeGroupCode(2);
		file.writeString(this->appName.empty() ? "default app name" : this->appName);
		file.writeGroupCode(90);
		file.writeInt32(this->versionNumber);
		file.writeGroupCode(280);
		file.writeInt8(this->flag1);
		file.writeGroupCode(281);
		file.writeInt8(this->flag2);
		return base_t::write(file);
	}

	/*!
	  Static function which creates a class based on its name.
	*/

	std::unique_ptr<dimeClass> dimeClass::createClass(std::string name) {
		return std::make_unique<dimeUnknownClass>(std::move(name));
	}

	//!

	size_t dimeClass::countRecords() const {
		return 5 + base_t::countRecords();
	}

	//!

	bool dimeClass::isOfType(int thetypeid) const {
		return thetypeid == dimeClassType || base_t::isOfType(thetypeid);
	}

	/*!
	  Reads a class from \a in. Can be overloaded by subclasses, but in most
	  cases this will not be necessary.

	  \sa dimeClass::handleRecord().
	*/

	bool dimeClass::read(dimeInput& file) {
		return base_t::read(file);
	}

	//!

	bool dimeClass::handleRecord(int groupcode, const dimeParam& param) {
		switch (groupcode) {
		case 1:
			className = std::get<std::string>(param);
			return true;
		case 2:
			appName = std::get<std::string>(param);
			return true;
		case 90:
			this->versionNumber = std::get<int32>(param);
			return true;
		case 280:
			this->flag1 = std::get<int8>(param);
			return true;
		case 281:
			this->flag2 = std::get<int8>(param);
			return true;
		}
		return false;
	}

} // namespace dime

