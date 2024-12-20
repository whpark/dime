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

#include "biscuit/dependencies_eigen.h"
#include "biscuit/dependencies_units.h"

export module dime.biscuit:Basic;
import std;
import biscuit;

using namespace std::literals;

export namespace dime {

	using dxfdouble = double;

	using int8 = int8_t;
	using uint8 = uint8_t;
	using int16 = int16_t;
	using uint16 = uint16_t;
	using int32 = int32_t;
	using uint32 = uint32_t;
	using int64 = int64_t;
	using uint64 = uint64_t;

	using dimeVec2f = biscuit::sPoint2d;
	using dimeVec3f = biscuit::sPoint3d;
	using dimeVec4f = biscuit::sPoint4d;
	using point2_t = biscuit::sPoint2d;
	using point3_t = biscuit::sPoint3d;
	using point4_t = biscuit::sPoint4d;
	using dimeBox = biscuit::sBounds3d;
	using dimeMatrix = Eigen::Transform<double, 3, Eigen::Affine>;
	using deg_t = biscuit::deg_t;
	using rad_t = biscuit::rad_t;

	template < typename tValue, typename tCloner = biscuit::TCloner<tValue> >
	using tptr_t = biscuit::TCloneablePtr<tValue, tCloner>;
	//using tptr_t = std::unique_ptr<targs...>;

	template < typename tKey, typename tValue>
	using dict_t = biscuit::TContainerMap<tKey, tValue>;

	using callbackEntity_t = std::function<bool(const class dimeState*, class dimeEntity*)>;
	using callbackProgress_t = std::function<bool(float)>;

	//typedef union {
	//	int8  int8_data;
	//	int16 int16_data;
	//	int32 int32_data;
	//	float float_data;
	//	dxfdouble double_data;
	//	const char* string_data;
	//	const char* hex_data;
	//} dimeParam;
	class hex_string_t : public std::string {
	public:
		using base_t = std::string;
		using this_t = hex_string_t;

		using base_t::base_t;
		using base_t::operator=;
	};
	using dimeParam = std::variant<int8, int16, int32, float, dxfdouble, std::string, hex_string_t>;
	enum class eDimeParam { i8, i16, i32, f, d, str, hex_str };

	//int dime_isnan(double value) { return std::isnan(value); }
	//int dime_isinf(double value) { return std::isinf(value); }
	bool dime_finite(double value) { return std::isfinite(value); }

	constexpr bool ASSERT(bool condition, std::string_view msg = ""sv, std::source_location loc = std::source_location::current()) {
		if (!condition)
			throw std::runtime_error(std::string(msg) + " at " + loc.file_name() + ":" + std::to_string(loc.line()));
		return condition;
	}

}	// namespace dime

