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

export module dime.biscuit:Input;
import std;
import biscuit;
import :Basic;
import :Model;
import :util;

using namespace std::literals;
using namespace biscuit::literals;

export namespace dime {

	constexpr auto DXF_MAXLINELEN = 4096;

	class dimeInput {
		friend class dimeModel;

	protected:
		std::istream& m_stream;

	public:
		dimeInput(std::istream& stream) : m_stream(stream) {}
		~dimeInput() {}

		bool eof() const { return m_stream.eof(); }
		void setProgressCallback(callbackProgress_t callback);
		float relativePosition();

		void putBackGroupCode(const int32 code);
		bool readGroupCode(int32& code);
		bool readInt8(int8& val);
		bool readInt16(int16& val);
		bool readInt32(int32& val);
		bool readUInt8(uint8& val) { return readInt8((int8&)val); }
		bool readUInt16(uint16& val) { return readInt16((int16&)val); }
		bool readUInt32(uint32& val) { return readInt32((int32&)val); }
		bool readFloat(float& val);
		bool readDouble(dxfdouble& val);
		std::string readString();
		std::string readStringNoSkip();

		dimeModel* getModel();
		dimeMemHandler* getMemHandler();

		int getFilePosition() const;

		bool isBinary() const;
		int getVersion() const;
		bool isAborted() const;

	private:
		mutable dimeModel* model{};						// set by the dimeModel class.
		int line{};
		bool binary{};
		bool binary16bit{};
		int version{};

		std::streamsize filesize{};
		std::string lineBuf;
		std::optional<int32> putBackCode{};
		callbackProgress_t callbackProgress;
		float prevposition{};
		int cbcnt{};
		bool aborted{};
		bool prevwashandle{};
		constexpr static inline bool endianSwap = std::endian::native != std::endian::little;

	private:
		bool init();
		bool doBufferRead();
		void putBack(char c) { m_stream.putback(c); }
		void putBack(std::string_view sv) { for (auto c : std::ranges::reverse_view(sv)) putBack(c); }
		bool get(char& c);
		bool get(unsigned char& c) { return get((char&)c); }
		bool read(char& c);
		bool skipWhiteSpace();
		bool nextLine();
		bool readInteger(long& l);
		bool readUnsignedInteger(unsigned long& l);
		bool readUnsignedIntegerString(char* const str);
		int readDigits(char* const string);
		int readHexDigits(char* const string);
		int readChar(char* const string, char charToRead);
		bool readReal(dxfdouble& d);
		bool checkBinary();
	}; // class dimeInput

} // namespace dime
