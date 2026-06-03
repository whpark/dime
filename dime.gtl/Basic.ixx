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
// this dime.gtl project is a part of the project "gtl".
// this compiles only inside "gtl"
// whpark. 2025-07-24
//=============================================================================

export module dime.gtl:Basic;
import std;
import "default.hxx";

using namespace std::literals;

export namespace dime {
	//-------------------------------------------------------------------------
	template < typename T >
	struct TCloner {
		std::unique_ptr<T> operator () (T const& self) const { return self.clone(); }
	};
	template < typename T >
	struct TStaticCloner {
		std::unique_ptr<T> operator () (T const& self) const { return std::make_unique<T>(self); }
	};

	template < /*concepts::cloneable */typename T, class CLONER = TCloner<T> >
	class TCloneablePtr : public std::unique_ptr<T> {
	public:
		using base_t = std::unique_ptr<T>;
		using this_t = TCloneablePtr;

		using base_t::base_t;
		//TCloneablePtr(this_t&& other) : base_t(std::move(other)) {}
		using base_t::operator =;
		using base_t::operator *;
		using base_t::operator ->;
		using base_t::operator bool;

		static inline CLONER cloner;

		TCloneablePtr() = default;
		TCloneablePtr(this_t const& other) : base_t(other ? cloner(*other) : nullptr) {}
		TCloneablePtr(this_t&& other) = default;
		TCloneablePtr& operator = (this_t const& other) { base_t::operator = (cloner(*other)); return *this; }
		TCloneablePtr& operator = (this_t&& other) = default;

		// move from unique_ptr
		template < typename U > requires std::is_base_of_v<T, U>
		TCloneablePtr(std::unique_ptr<U>&& other) : base_t(std::move(other)) {}
		template < typename U > requires std::is_base_of_v<T, U>
		TCloneablePtr& operator = (std::unique_ptr<U>&& other) { base_t::operator = (std::move(other)); return *this; }

		// copy from unique_ptr
		template < typename U > requires std::is_base_of_v<T, U>
		TCloneablePtr(std::unique_ptr<U> const& other) : base_t(other ? cloner(*other) : nullptr) {}
		template < typename U > requires std::is_base_of_v<T, U>
		TCloneablePtr& operator = (std::unique_ptr<U> const& other) { base_t::operator = (cloner(*other)); return *this; }

		template < typename U, class CLONER2 > requires std::is_base_of_v<T, U>
		TCloneablePtr& operator = (TCloneablePtr<U, CLONER2>&& other) {
			this->reset(other.release());
			return*this;
		}
		template < typename U, class CLONER2 > requires std::is_base_of_v<T, U>
		TCloneablePtr& operator = (TCloneablePtr<U, CLONER2> const& other) {
			static CLONER2 cloner2;
			this->reset(other ? cloner2(*other).release() : nullptr);
			return*this;
		}

		std::partial_ordering operator <=> (TCloneablePtr const& other) const {
			bool bEmptyA = !(*this);
			bool bEmptyB = !other;
			if (bEmptyA and bEmptyB) return std::partial_ordering::equivalent;
			else if (bEmptyA) return std::partial_ordering::less;
			else if (bEmptyB) return std::partial_ordering::greater;
			if constexpr (requires (T const& a, T const& b) { a.Compare(b); }) {
				return (*this)->Compare(*other);
			}
			else {
				return (**this) <=> (*other);
			}
		}
		bool operator == (TCloneablePtr const& other) const {
			T const* pA = this->get();
			T const* pB = other.get();
			if (pA == pB)
				return true;
			if (!pA || !pB)
				return false;
			if constexpr (requires (T const& a, T const& b) { a.Equals(b); }) {
				return pA->Equals(*pB);
			}
			else {
				return (*pA) == (*pB);
			}
		}
	};

}	// namespace dime

export namespace dime {

	using dxfdouble = double;

	using int8 = std::int8_t;
	using uint8 = std::uint8_t;
	using int16 = std::int16_t;
	using uint16 = std::uint16_t;
	using int32 = std::int32_t;
	using uint32 = std::uint32_t;
	using int64 = std::int64_t;
	using uint64 = std::uint64_t;

	using dimeVec2f = Eigen::Vector2d;
	using dimeVec3f = Eigen::Vector3d;
	using dimeVec4f = Eigen::Vector4d;
	using point2_t = Eigen::Vector2d;
	using point3_t = Eigen::Vector3d;
	using point4_t = Eigen::Vector4d;
	using dimeBox = Eigen::AlignedBox3d;
	using dimeMatrix = Eigen::Affine3d;

	inline double deg2rad(double deg) { return deg * std::numbers::pi / 180.0; }
	inline double rad2deg(double rad) { return rad * 180.0 / std::numbers::pi; }

	template < typename tValue, typename tCloner = TCloner<tValue>>
	using tptr_t = TCloneablePtr<tValue, tCloner>;
	//using tptr_t = std::unique_ptr<targs...>;

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

