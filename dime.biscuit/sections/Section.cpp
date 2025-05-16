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
  \class dimeSection dime/sections/Section.h
  \brief The dimeSection class is the superclass for all \e section classes.

  Currently supported sections are:
  - Header   (dimeHeaderSection)
  - Classes  (dimeClassSection)
  - Tables   (dimeTableSection)
  - Blocks   (dimeBlockSection)
  - Entities (dimeEntititySection)
  - Objects  (dimeObjectSection)
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


module dime.biscuit:sections.Section;
import std;
import "biscuit/dependencies_eigen.hxx";
import "biscuit/dependencies_units.hxx";
import biscuit;
import :Basic;
import :util;
import :Base;
import :Input;
import :Output;
import :Record;
import :RecordHolder;
import :Model;
import :sections.UnknownSection;
import :sections.EntitiesSection;
import :sections.HeaderSection;
import :sections.BlocksSection;
import :sections.TablesSection;
//import :sections.ClassesSection;
//import :sections.ObjectsSection;


using namespace std::literals;

namespace dime {


	/*!
	  \fn int dimeSection::countRecords() const
	  Returns the number of records in this section.
	*/


	/*!
	  Static function used to create the correct section object
	  from a text string.
	*/

	std::unique_ptr<dimeSection> dimeSection::createSection(std::string_view sectionname) {
		if (sectionname == "HEADER"sv)
			return std::make_unique<dimeHeaderSection>();
	#if 0 // passthrough for the moment. I can't imaging anybody is using them 
		if (sectionname == "CLASSES"sv)
			return std::make_unique<dimeClassesSection>();
		if (sectionname == "OBJECTS"sv)
			return std::make_unique<dimeObjectsSection>();
	#endif
		if (sectionname == "TABLES"sv)
			return std::make_unique<dimeTablesSection>();
		if (sectionname == "BLOCKS"sv)
			return std::make_unique<dimeBlocksSection>();
		if (sectionname == "ENTITIES"sv)
			return std::make_unique<dimeEntitiesSection>();
		return std::make_unique<dimeUnknownSection>(std::string(sectionname));
	}


	/*!
	  \fn const char * dimeSection::getSectionName() const = 0
	*/

	/*!
	  \fn dimeSection * dimeSection::copy(dimeModel * const model) const = 0
	*/

	/*!
	  \fn bool dimeSection::read(dimeInput& file) = 0
	*/

	/*!
	  \fn bool dimeSection::write(dimeOutput& file) = 0
	*/

	/*!
	  \fn int dimeSection::typeId() const = 0
	*/

} // namespace dime

