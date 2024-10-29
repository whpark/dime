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
  \class dimeRecord dime/records/Record.h
  \brief The dimeRecord class is the superclass of all \e record classes.
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

module dime.biscuit:Record;
import std;
import biscuit;
import :Basic;
import :Base;
import :Model;
import :Input;
import :Output;

using namespace std::literals;

namespace dime {

	bool dimeRecord::read(dimeInput& in) {
		static std::map<int, std::function<bool(dimeRecord& self, dimeInput& in)>> const read_map = {
			{ dimeBase::dimeInt8RecordType,		ReadValue<int8> },
			{ dimeBase::dimeInt16RecordType,	ReadValue<int16> },
			{ dimeBase::dimeInt32RecordType,	ReadValue<int32> },
			{ dimeBase::dimeFloatRecordType,	ReadValue<float> },
			{ dimeBase::dimeDoubleRecordType,	ReadValue<double> },
			{ dimeBase::dimeStringRecordType,	ReadValue<std::string> },
			{ dimeBase::dimeHexRecordType,		ReadValue<hex_string_t> },
		};
		int rt = getRecordType(groupCode);
		if (auto iter = read_map.find(rt); iter != read_map.end()) {
			return iter->second(*this, in);
		}

		return false;
	}

	bool dimeRecord::writeRecord(dimeOutput& out) const {
		if (!out.writeGroupCode(groupCode))
			return false;
		static std::map<int, std::function<bool(dimeRecord const& self, dimeOutput& out)>> const write_map = {
			{ dimeBase::dimeInt8RecordType,		WriteValue<int8> },
			{ dimeBase::dimeInt16RecordType,	WriteValue<int16> },
			{ dimeBase::dimeInt32RecordType,	WriteValue<int32> },
			{ dimeBase::dimeFloatRecordType,	WriteValue<float> },
			{ dimeBase::dimeDoubleRecordType,	WriteValue<double> },
			{ dimeBase::dimeStringRecordType,	WriteValue<std::string> },
			{ dimeBase::dimeHexRecordType,		WriteValue<hex_string_t> },
		};

		int rt = getRecordType(groupCode);
		if (auto iter = write_map.find(rt); iter != write_map.end()) {
			return iter->second(*this, out);
		}
		return false;
	}

	// * static methods *******************************************************

	/*!
	  Reads and returns the next record int file \a in.
	*/

	bool dimeRecord::readRecord(dimeInput& in) {
		param = {};
		if (!in.readGroupCode(groupCode))
			return false;
		return read(in);
	}

	//
	// local function that returns the type based on the group code
	// used to build a look-up table
	//

	constexpr static int get_record_type(const int group_code) {
		int type = dimeBase::dimeStringRecordType;

		if (group_code < 0) {
		  // not normally used in DXF files, but return string record to 
		  // ensure correct read & write
			type = dimeBase::dimeStringRecordType;
		}

		else if (group_code <= 9) {
			type = dimeBase::dimeStringRecordType;
		}
		else if (group_code <= 59) {
		  // double precision floating point values.
			type = dimeBase::dimeDoubleRecordType;
		}
		// FIXME: this is a fix for some illegal files !!!!
	#ifdef DIME_FIXBIG
		else if (group_code <= 70) {
			return dimeBase::dimeInt16RecordType;
		}
		else if (group_code <= 74) {
			return dimeBase::dimeInt32RecordType;
		}
	#endif // DIME_FIXBIG
		else if (group_code <= 79) {
			type = dimeBase::dimeInt16RecordType;
		}
		else if (group_code <= 89) {
		  // not defined yet. Use string.
			type = dimeBase::dimeStringRecordType;
		}
		else if (group_code <= 99) {
			type = dimeBase::dimeInt32RecordType;
		}
		else if (group_code < 140) {
		  // only 100, 102 and 105 are defined. But use string for the rest also. 
			type = dimeBase::dimeStringRecordType;
		}
		else if (group_code <= 147) {
			type = dimeBase::dimeDoubleRecordType;
		}
		else if (group_code < 170) {
		  // not defined. Use string.
			type = dimeBase::dimeStringRecordType;
		}
		else if (group_code <= 178) {
			type = dimeBase::dimeInt16RecordType;
		}
		// XXX: this is not specified in the spec., but...
		else if (group_code == 210 || group_code == 220 || group_code == 230) {
			type = dimeBase::dimeDoubleRecordType;
		}
		else if (group_code < 270) {
		  // not defined. Use string.
			type = dimeBase::dimeStringRecordType;
		}
		else if (group_code <= 275) {
			type = dimeBase::dimeInt8RecordType;
		}
		else if (group_code < 280) {
		  // not defined.
			type = dimeBase::dimeStringRecordType;
		}
		else if (group_code <= 289) {
			type = dimeBase::dimeInt8RecordType;
		}
		else if (group_code < 300) {
		  // not defined.
			type = dimeBase::dimeStringRecordType;
		}
		else if (group_code <= 309) {
			type = dimeBase::dimeStringRecordType;
		}
		else if (group_code <= 319) {
		  // binary chunk of data
			type = dimeBase::dimeHexRecordType;
		}
		else if (group_code <= 329) {
		  // hex handle value
			type = dimeBase::dimeHexRecordType;
		}
		else if (group_code <= 369) {
		  // hexvalue for object ID
			type = dimeBase::dimeHexRecordType;
		}
		else if (group_code < 999) {
		  // not defined.
			type = dimeBase::dimeStringRecordType;
		}
		else if (group_code == 999) {
		  // comment
			type = dimeBase::dimeStringRecordType;
		}
		else if (group_code <= 1009) {
		  // 255 character max string.
			type = dimeBase::dimeStringRecordType;
		}
		else if (group_code <= 1059) {
			type = dimeBase::dimeStringRecordType;
			// should be float according to spec, but I have found
			// _huge_ numbers here that do not fit into a float.
			//    type = dimeBase::dimeFloatRecordType;
		}
		else if (group_code <= 1070) {
			type = dimeBase::dimeInt16RecordType;
		}
		else if (group_code == 1071) {
			type = dimeBase::dimeInt32RecordType;
		}
		else type = dimeBase::dimeStringRecordType;
		return type;
	}

	/*!
	  Static function that returns the record type based on
	  the group code.
	*/

	int dimeRecord::getRecordType(const int group_code) {
		static auto const translation = []{
			std::array<int, 1072> translation;
			for (auto i = 0uz; i < translation.size(); i++) {
				translation[i] = get_record_type(i);
			}
			return translation;
		}();
		if (group_code < 0 || group_code >= translation.size())
			return dimeBase::dimeStringRecordType;
		else return translation[group_code];
	}

} // namespace dime

