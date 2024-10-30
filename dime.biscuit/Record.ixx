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

export module dime.biscuit:Record;
import std;
import biscuit;
import :Basic;
import :Base;
import :Model;
import :Input;
import :Output;

using namespace std::literals;

export namespace dime {

	class dimeRecord : public dimeBase {
	public:
		dimeRecord(const int group_code = 0) : groupCode(group_code) {}
		dimeRecord(dimeRecord const&) = default;
		dimeRecord(dimeRecord&&) = default;
		dimeRecord& operator = (dimeRecord const&) = default;
		dimeRecord& operator = (dimeRecord&&) = default;
		virtual ~dimeRecord() = default;

		dimeRecord(const int group_code, dimeParam param) : groupCode(group_code), param(std::move(param)) {}

		std::unique_ptr<dimeRecord> clone() const { return std::make_unique<dimeRecord>(*this); }

		void setGroupCode(const int group_code) { groupCode = group_code; }
		int getGroupCode() const { return groupCode; }
		void getValue(dimeParam& param) const { param = this->param; }
		dimeParam const& getValue() const { return param; }
		dimeParam& getValue() { return param; }
		void setValue(const dimeParam& param) { this->param = param; }

	public:
		virtual bool isEndOfSectionRecord() const { return false; }
		virtual bool isEndOfFileRecord() const { return false; }
		virtual int typeId() const override {
			switch ((eDimeParam)param.index()) {
			case eDimeParam::i8: return dimeBase::dimeInt8RecordType;
			case eDimeParam::i16: return dimeBase::dimeInt16RecordType;
			case eDimeParam::i32: return dimeBase::dimeInt32RecordType;
			case eDimeParam::f: return dimeBase::dimeFloatRecordType;
			case eDimeParam::d: return dimeBase::dimeDoubleRecordType;
			case eDimeParam::str: return dimeBase::dimeStringRecordType;
			case eDimeParam::hex_str: return dimeBase::dimeHexRecordType;
			}
			return dimeBase::dimeRecordType;
		}
		bool read(dimeInput& in);
		bool writeRecord(dimeOutput& out) const;
		//virtual void print() const { std::println("rec: {}\n", groupCode); }

		bool readRecord(dimeInput& const in);
	public:
		//static bool readRecordData(dimeInput& const in, const int group_code, dimeParam& param);
		//static std::unique_ptr<dimeRecord> createRecord(const int group_code);
		//static std::unique_ptr<dimeRecord> createRecord(const int group_code, const dimeParam& param);
		static int getRecordType(const int group_code);

	protected:
		bool ReadValue(dimeInput& in, int8& v) { return in.readInt8(v); }
		bool ReadValue(dimeInput& in, int16& v) { return in.readInt16(v); }
		bool ReadValue(dimeInput& in, int32& v) { return in.readInt32(v); }
		bool ReadValue(dimeInput& in, float& v) { return in.readFloat(v); }
		bool ReadValue(dimeInput& in, dxfdouble& v) { return in.readDouble(v); }
		bool ReadValue(dimeInput& in, std::string& v) {
			v = (groupCode == 1) ? in.readStringNoSkip() : in.readString(); return !v.empty();
		}
		bool ReadValue(dimeInput& in, hex_string_t& v) { v = in.readString(); return !v.empty(); }

		template < typename T >
		static bool ReadValue(dimeRecord& record, dimeInput& in) {
			T value;
			if (!record.ReadValue(in, value))
				return false;
			record.param = std::move(value);
			return true;
		}

		static bool WriteValue(dimeOutput& out, int8 v) { return out.writeInt8(v); }
		static bool WriteValue(dimeOutput& out, int16 v) { return out.writeInt16(v); }
		static bool WriteValue(dimeOutput& out, int32 v) { return out.writeInt32(v); }
		static bool WriteValue(dimeOutput& out, float v) { return out.writeFloat(v); }
		static bool WriteValue(dimeOutput& out, dxfdouble v) { return out.writeDouble(v); }
		static bool WriteValue(dimeOutput& out, const std::string& v) { return out.writeString(v); }
		static bool WriteValue(dimeOutput& out, const hex_string_t& v) { return out.writeString(v); }

		template < typename T >
		static bool WriteValue(dimeRecord const& record, dimeOutput& out) {
			out.writeGroupCode(record.groupCode);
			return WriteValue(out, std::get<T>(record.param));
		}
	protected:
		int groupCode{};
		dimeParam param;
	}; // class dimeRecord

}; // namespace dime

