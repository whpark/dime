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

module dime.biscuit:Input;
import std;
import biscuit;
import :Input;
import :Model;

using namespace std::literals;
using namespace biscuit::literals;

namespace dime {
	constexpr auto TMPBUFSIZE = 4096; // temporary buffer used to read floats or integers

	bool dimeInput::init() {
		this->aborted = false;
		this->putBackCode.reset();
		this->line = 0;
		this->binary = false;
		this->binary16bit = false;

		m_stream.seekg(0, std::ios::end);
		this->filesize = m_stream.tellg();
		m_stream.seekg(0);
		this->prevposition = 0.0f;
		this->cbcnt = 0;
		this->prevwashandle = false;

		//this->endianSwap = false;
		this->binary = checkBinary();
		return true;
	}

	/*!
	This method returns whether file input was aborted or not.
	*/

	bool dimeInput::isAborted() const {
		return this->aborted;
	}

	/*!
	This method sets a progress callback that will be called with a
	float in the range between 0 and 1, and void * \a cbdata as arguments.
	*/

	void dimeInput::setProgressCallback(callbackProgress_t callback) {
		this->callbackProgress = callback;
		this->prevposition = 0.0f;
		this->cbcnt = 0;
	}

	/*!
	Returns the relative file position. 0.0 means beginning of file,
	1.0 is at end of file.
	*/

	float dimeInput::relativePosition() {
		ASSERT(this->m_stream.good());
		if (!this->filesize)
			return 0.0f;
		return (((float)(this->m_stream.tellg())) / ((float)(this->filesize)));
	}


	/*!
	Reads a group code from the file. In binary files, group codes
	are represented as a single byte, with the exception of
	extended data which has 255 as the first byte, and then the
	actual group code following as a 16-bit integer.
	*/

	bool dimeInput::readGroupCode(int32& code) {
		bool ret;
		if (putBackCode) {
			code = *this->putBackCode;
			this->putBackCode.reset();
			ret = true;
		}
		else {
			if (callbackProgress && cbcnt++ > 100) {
				cbcnt = 0;
				float pos = relativePosition();
				if (pos > prevposition + 0.01f) {
					prevposition = pos;
					if (!callbackProgress(pos)) {
						aborted = true;
						return false;
					}
				}
			}

			if (this->binary) {
				if (this->binary16bit) {
					uint16 uval{};
					ret = this->readUInt16(uval);
					code = (int32)uval;
				}
				else {
					unsigned char uval{}; // group code is unsigned int8
					ret = this->get(uval);
					code = (int32)uval;
				}
				if (code == 255) {
					int16 val16{};
					ret = this->readInt16(val16);
					code = (int32)val16;
				}
			}
			else {
				//
				// quick fix to ignore comments
				//
				ret = readInt32(code);
				while (ret && code == 999) {
					readString();
					ret = readInt32(code);
				}
			}
		}
		if (code == 5) this->prevwashandle = true;
		else this->prevwashandle = false;
		return ret;
	}

	/*!
	This function is needed when a loader snoops for future group codes.
	It is possible to put back a single group code so that the next time
	dimeInput::readGroupCode() is called, the put back value will be
	returned.
	*/

	void dimeInput::putBackGroupCode(int32 code) {
		ASSERT(!putBackCode);
		putBackCode = code;
	}


	/*!
	Reads an 8 bit integer from the file.
	*/

	bool dimeInput::readInt8(int8& val) {
		if (this->binary) {
			char* ptr = (char*)&val;
			return get(*ptr);
		}

		long tmp;
		bool ok = skipWhiteSpace();
		if (ok && readInteger(tmp) && tmp >= -128 && tmp <= 127) {
			val = (int8)tmp;
			return nextLine();
		}
		return false;
	}

	/*!
	Reads a 16 bit integer from the file.
	*/

	bool dimeInput::readInt16(int16& val) {
		if (this->binary) {
			bool ret;
			char* ptr = (char*)&val;
			if constexpr (this->endianSwap) {
				this->get(ptr[1]);
				ret = this->get(ptr[0]);
			}
			else {
				this->get(ptr[0]);
				ret = this->get(ptr[1]);
			}
			return ret;
		}

		long tmp;
		bool ok = skipWhiteSpace();
		if (ok && readInteger(tmp) && tmp >= -32768 && tmp <= 32767) {
			val = (int16)tmp;
			return nextLine();
		}
		return false;
	}

	/*!
	Reads a 32 bit integer from the file.
	*/

	bool dimeInput::readInt32(int32& val) {
		if (this->binary) {
			bool ret;
			char* ptr = (char*)&val;
			if constexpr (this->endianSwap) {
				this->get(ptr[3]);
				this->get(ptr[2]);
				this->get(ptr[1]);
				ret = this->get(ptr[0]);
			}
			else {
				this->get(ptr[0]);
				this->get(ptr[1]);
				this->get(ptr[2]);
				ret = this->get(ptr[3]);
			}
			return ret;
		}

		long tmp;
		if (skipWhiteSpace() && readInteger(tmp)) {
			val = tmp;
			return nextLine();
		}
		return false;
	}

	/*!
	Reads a single precision floating point number from the file.
	*/

	bool dimeInput::readFloat(float& val) {
		bool ret = false;

		if (this->binary) {
			// binary files only contains doubles
			dxfdouble tmp;
			ret = readDouble(tmp);
			val = (float)tmp;
		}
		else {
			dxfdouble tmp;
			ret = skipWhiteSpace();
			if (ret && readReal(tmp)) {
				val = (float)tmp;
				ret = nextLine();
			}
		}
		if (ret) {
			if (!dime_finite(val)) {
				val = (val > 0) ? std::numeric_limits<float>::max() : -std::numeric_limits<float>::max();
			}
		}
		return ret;
	}

	/*!
	Reads a dxfdouble precision floating point number from the file.
	*/

	bool dimeInput::readDouble(dxfdouble& val) {
		bool ret = false;
		if (this->binary) {
			ret = readDouble(val);
		}
		else {
			if (skipWhiteSpace()) {
				if (readReal(val)) {
					ret = nextLine();
				}
			}
		}

		if (ret) {
			if (!dime_finite(val)) {
				val = (val > 0) ? std::numeric_limits<double>::max() : -std::numeric_limits<double>::max();
			}
		}
		return ret;
	}

	/*!
	Returns a null-terminated string read from the file. The string
	is valid only until the next read operation, so you'd better
	copy it somewhere if you need it.
	*/

	std::string dimeInput::readString() {
		std::string str;
		if (!skipWhiteSpace())
			return str;

		std::getline(m_stream, str);
		if (str.size() && str.back() == '\r') {
			str.resize(str.size() - 1);
		}
		putBack('\n');	// NOT '\r'
		nextLine();

		if (this->prevwashandle) {
			this->prevwashandle = false;
			if (this->model) {
				this->model->registerHandle(str);
			}
		}
		return str;
	}

	/*!
	Special version of readString() that does not skip leading blanks.
	This is used for reading TEXT primary values, group_code = 1.
	*/

	std::string dimeInput::readStringNoSkip() {
		std::string str;

		std::getline(m_stream, str);
		if (str.size() && str[str.size() - 1] == '\r') {
			str.resize(str.size() - 1);
			putBack('\n');	// NOT '\r'
		}
		nextLine();

		if (this->prevwashandle) {
			this->prevwashandle = false;
			if (this->model) {
				this->model->registerHandle(str);
			}
		}
		return str;
	}

	///*!
	//Returns the memory handler used in this model.
	//*/

	//dimeMemHandler* dimeInput::getMemHandler() {
	//	if (model) return model->getMemHandler();
	//	return nullptr;
	//}

	///*!
	//Returns the model for this file.
	//*/

	//dimeModel* dimeInput::getModel() {
	//	return model;
	//}

	/*!
	For ASCII files, it returns the current line number.
	For binary files the file position is returned.
	*/

	int dimeInput::getFilePosition() const {
		return line;
	}

	/*!
	Returns true if this is a binary (DXB) file.
	*/

	bool dimeInput::isBinary() const {
		return binary;
	}

	/*!
	Returns the version of this file (10, 12, 13 or 14).
	*/

	int dimeInput::getVersion() const {
		return version;
	}

	// private funcs ***********************************************************


	//
	// returns the first non white-space char
	//

	bool dimeInput::read(char& c) {
		return (skipWhiteSpace() && get(c));
	}

	//
	// returns the next char in the stream
	//

	bool dimeInput::get(char& c) {
		if (!m_stream.good())
			return false;
		c = (char)m_stream.get();
		if (this->binary) {
			this->line++;
		}
		return true;
	}

	//
	// skip all white spaces
	//

	bool dimeInput::skipWhiteSpace() {
		if (this->binary) return true;
		char c;
		/*register*/ bool gotChar;
		/*register*/ constexpr char endline = 0xa;
		/*register*/ constexpr char endline2 = 0xd;
		while ((gotChar = get(c)) && std::iswspace(c) && c != endline && c != endline2)
			;
		if (!gotChar) return false;

		// step one char back
		putBack(c);
		return true;
	}

	bool dimeInput::nextLine() {
		if (this->binary) return true;

		for (char ch{}; m_stream.get(ch);)
			if (ch == '\n')
				break;

		this->line++;
		return m_stream.good();
	}

	//
	// read an integer
	//

	bool dimeInput::readInteger(long& l) {
		ASSERT(!this->binary);
		char str[TMPBUFSIZE];
		char* s = str;

		if (readChar(s, '-') || readChar(s, '+'))
			s++;

		if (!readUnsignedIntegerString(s))
			return false;

		l = std::strtol(str, nullptr, 0);

		return true;
	}

	//
	// read an unsigned integer
	//

	bool dimeInput::readUnsignedInteger(unsigned long& l) {
		ASSERT(!this->binary);
		char str[TMPBUFSIZE];
		if (!readUnsignedIntegerString(str))
			return false;

		l = std::strtoul(str, nullptr, 0);

		return true;
	}

	bool dimeInput::readUnsignedIntegerString(char* const str) {
		ASSERT(!this->binary);
		int minSize = 1;
		char* s = str;

		if (readChar(s, '0')) {

			if (readChar(s + 1, 'x')) {
				s += 2 + readHexDigits(s + 2);
				minSize = 3;
			}

			else
				s += 1 + readDigits(s + 1);
		}

		else
			s += readDigits(s);

		if (s - str < minSize)
			return false;

		*s = '\0';

		return true;
	}

	int dimeInput::readDigits(char* const string) {
		ASSERT(!this->binary);
		char c, * s = string;

		while (get(c)) {
			if (std::isdigit(c))
				*s++ = c;
			else {
				putBack(c);
				break;
			}
		}

		return s - string;
	}

	//
	// check if a character is next in stream. Writes character into
	// string if found, puts character back otherwise.
	//

	int dimeInput::readChar(char* const string, char charToRead) {
		ASSERT(!this->binary);

		char c;
		int ret;

		if (!get(c))
			ret = 0;

		else if (c == charToRead) {
			*string = c;
			ret = 1;
		}

		else {
			putBack(c);
			ret = 0;
		}

		return ret;
	}

	int dimeInput::readHexDigits(char* const string) {
		ASSERT(!this->binary);

		char c, * s = string;

		while (get(c)) {

			if (std::isxdigit(c))
				*s++ = c;

			else {
				putBack(c);
				break;
			}
		}

		return s - string;
	}

	//
	// reads a floating point number
	//

	bool dimeInput::readReal(dxfdouble& d) {
		ASSERT(!this->binary);

		char str[TMPBUFSIZE];
		int n;
		char* s = str;
		bool gotNum = false;

		n = readChar(s, '-');
		if (n == 0)
			n = readChar(s, '+');
		s += n;

		if ((n = readDigits(s)) > 0) {
			gotNum = true;
			s += n;
		}

		if (readChar(s, '.') > 0) {
			s++;

			if ((n = readDigits(s)) > 0) {
				gotNum = true;
				s += n;
			}
		}

		if (!gotNum)
			return false;

		n = readChar(s, 'e');
		if (n == 0)
			n = readChar(s, 'E');

		if (n > 0) {
			s += n;

			n = readChar(s, '-');
			if (n == 0)
				n = readChar(s, '+');
			s += n;

			if ((n = readDigits(s)) > 0)
				s += n;

			else
				return false;
		}

		*s = '\0';

		d = std::atof(str);
		return true;
	}

	bool dimeInput::checkBinary() {
		//{ // perform endian-test
		//	uint16 val;
		//	char* ptr = (char*)&val;
		//	ptr[0] = 1;
		//	ptr[1] = 0;
		//	if (val == 0x0001) this->endianSwap = false;
		//	else {
		//		ASSERT(val == 0x0100);
		//		this->endianSwap = true;
		//	}
		//}

		static std::string const binaryid{"AutoCAD Binary DXF"s};
		char buf[64];
		size_t i;
		for (i = 0; i < binaryid.size(); i++) {
			if (!this->get(buf[i])) break;
			if (buf[i] != binaryid[i]) break;
		}
		if (i < binaryid.size()) { // probably ascii
			m_stream.seekg(0);
			this->line = 0;
			return false;
		}
		else {
			// skip next 4 bytes
			for (i = 0; i < 4; i++) this->get(buf[0]);
			this->line = 22; // byte position in file

			char test16;
			this->get(test16);
			ASSERT(test16 == 0);
			this->get(test16);
			if (test16 == 0) {
				this->binary16bit = true;
				this->putBack((char)0);
				this->putBack((char)0);
			}
			else {
				this->binary16bit = false;
				this->putBack(test16);
				this->putBack((char)0);
			}
			return true;
		}
	}

}

