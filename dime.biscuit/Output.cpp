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
  \class dimeOutput dime/Output.h
  \brief The dimeOutput class handles writing of DXF and DXB files.
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

module dime.biscuit:Output;
import std;
import biscuit;
import :Basic;
import :Base;
//import :Model;

using namespace std::literals;

namespace dime {

	/*!
	  \fn bool dimeOutput::writeHeader()
	  This method does nothing now, but if binary files are supported in the
	  future, it must be called.
	*/

	/*!
	  This method sets a callback function that is called with progress
	  information.  The first argument of the callback is a float in the
	  range between 0 and 1.  The second argument of the callback is the
	  void * \a cbdata argument.
	*/

	void dimeOutput::setCallback(int const num_records, callbackProgress_t callback) {
		this->callbackProgress = callback;
		this->numwrites = 0;
		this->numrecords = num_records;
	}

	/*!
	  Sets binary (DXB) or ASCII (DXF) format. Currently only ASCII
	  is supported.
	*/

	void dimeOutput::setBinary(const bool state) {
		this->binary = state;
	}

	/*!
	  Returns if binary or ASCII will be used when writing.
	*/

	bool dimeOutput::isBinary() const {
		return this->binary;
	}

	/*!
	  Writes a record group code to the file.
	*/

	bool dimeOutput::writeGroupCode(int groupcode) {
		if (this->aborted) return false;
		if (this->callbackProgress && this->numrecords) {
			if ((this->numwrites & 255) == 0) {
				float val = float(this->numwrites) / float(this->numrecords);
				if (val > 1.0f) val = 1.0f;
				this->aborted = !(bool)callbackProgress(val);
			}
			this->numwrites++;
		}
		std::println(os, "{:3d}", groupcode);
		return os.good();
	}

	/*!
	  Writes an 8 bit integer to the file.
	*/

	bool dimeOutput::writeInt8(int8 val) {
		std::println(os, "{:6d}", val);
		return os.good();
	}

	/*!
	  Writes a 16 bit integer to the file.
	*/

	bool dimeOutput::writeInt16(int16 val) {
		std::println(os, "{:6d}", (int)val);
		return os.good();
	}

	/*!
	  Writes a 32 bit integer to the file.
	*/

	bool dimeOutput::writeInt32(int32 val) {
		std::println(os, "{:6d}", val);
		return os.good();
	}

	/*!
	  Writes a single precision floating point number to the file.
	*/

	bool dimeOutput::writeFloat(const float val) {
		  // Check for integer value, force decimal and one zero.
		if (std::abs(val) < 1000000.0 && std::floor(val) == val) {
			std::println(os, "{:.1f}", val);
		}
		else {
			std::println(os, "{:g}", val);
		//    return fprintf(this->fp, "%#f\n", val);
		}
		return os.good();
	}

	/*!
	  Writes a double precision floating point number to the file.
	*/

	bool dimeOutput::writeDouble(const dxfdouble val) {
		  // Check for integer value, force decimal and one zero.
		if (std::abs(val) < 1000000.0 && std::floor(val) == val) {
			std::println(os, "{:.1f}", val);
		}
		else {
			std::println(os, "{:g}", val);
		//    return fprintf(this->fp,"%#f\n", val) > 0;
		}
		return os.good();
	}

	/*!
	  Writes a null-terminated string to the file.
	*/

	bool dimeOutput::writeString(std::string_view sv) {
		std::println(os, "{}", sv);
		return os.good();
	}

	int dimeOutput::getUniqueHandleId() {
		static std::atomic<int> handle_id{ 1030 };
		  // FIXME
		return ++handle_id;
	}

} // namespace dime

