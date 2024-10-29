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

#include "biscuit/dependencies_eigen.h"
#include "biscuit/dependencies_units.h"

export module dime.biscuit:classes.Class;
import std;
import biscuit;
import :Basic;
import :Base;
import :util;
import :RecordHolder;
import :Input;
import :Output;
import :Model;

using namespace std::literals;

export namespace dime {

	class dimeClass : public dimeRecordHolder {
		friend class dimeClassesSection;
		friend class dimeModel;
	public:
		using base_t = dimeRecordHolder;
		using this_t = dimeClass;

	public:
		dimeClass() = default;
		dimeClass(dimeClass const&) = default;
		dimeClass(dimeClass&&) = default;
		dimeClass& operator=(dimeClass const&) = default;
		dimeClass& operator=(dimeClass&&) = default;
		virtual ~dimeClass() = default;

		virtual std::unique_ptr<dimeClass> clone() const = 0;

		virtual std::string const& getDxfClassName() const = 0;
		bool read(dimeInput& in) override;
		bool write(dimeOutput& out) override;
		bool isOfType(const int thetypeid) const override;
		size_t countRecords() const override;

		std::string const& getClassName() const { return className; }
		std::string const& getApplicationName() const { return appName; }
		int32 getVersionNumber() const { return versionNumber; }
		int8 getFlag280() const { return flag1; }
		int8 getFlag281() const { return flag2; }

		void setClassName(std::string className) { this->className = std::move(className); }
		void setApplicationName(std::string appname) { this->appName = std::move(appname); }
		void setVersionNumber(const int32 v) { this->versionNumber = v; }
		void setFlag280(const int8 flag) { this->flag1 = flag; }
		void setFlag281(const int8 flag) { this->flag2 = flag; }

	protected:
		bool handleRecord(const int groupcode, const dimeParam& param) override;

	public:
		static std::unique_ptr<dimeClass> createClass(std::string name);
	protected:
		bool copyRecords(dimeClass& newclass) const;

	private:
		std::string className;
		std::string appName;
		int32 versionNumber{};
		int8 flag1{};
		int8 flag2{};

	}; // class dimeClass

} // namespace dime

