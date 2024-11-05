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

export module dime.biscuit:State;
import std;
import biscuit;
import :Basic;
import :util;
import :Base;

export namespace dime {
	class dimeInsert;
}

export namespace dime {

	class dimeState final {
	public:
		enum {
			TRAVERSE_POLYLINE_VERTICES = 0x1,
			EXPLODE_INSERTS = 0x2,
			// private flags
			PUBLIC_MASK = 0x7fff,
			PRIVATE_MASK = 0x8000,
			INVMATRIX_DIRTY = 0x8000
		};

	public:
		using this_t = dimeState;

	public:
		dimeState() = default;
		dimeState(dimeState const&) = default;
		dimeState(dimeState&&) = default;
		dimeState& operator=(dimeState const&) = default;
		dimeState& operator=(dimeState&&) = default;
		dimeState(const bool traversePolylineVertices, const bool explodeInserts) {
			this->matrix.setIdentity();
			this->invmatrix.setIdentity();
			this->flags = 0;
			if (traversePolylineVertices) {
				this->flags |= TRAVERSE_POLYLINE_VERTICES;
			}
			if (explodeInserts) {
				this->flags |= EXPLODE_INSERTS;
			}
		}

		const dimeMatrix& getMatrix() const { return matrix; }
		const dimeMatrix& getInvMatrix() const {
			if (flags & INVMATRIX_DIRTY) {
				invmatrix = matrix.inverse();
				flags ^= INVMATRIX_DIRTY;
			}
			return invmatrix;
		}

		void getMatrix(dimeMatrix& m) const { m = matrix; }
		void setMatrix(const dimeMatrix& matrix) {
			this->matrix = matrix;
			this->flags |= INVMATRIX_DIRTY;
		}

		void setFlags(const unsigned int flags) {
			this->flags = (this->flags & PRIVATE_MASK) | flags;
		}
		unsigned int getFlags() const {
			return (this->flags & PUBLIC_MASK);
		}

		dimeInsert* getCurrentInsert() { return currentInsert; }

	private:
		friend class dimeInsert;
		dimeMatrix matrix;
		mutable dimeMatrix invmatrix; // to speed up things...
		mutable unsigned int flags;
		mutable dimeInsert* currentInsert{};
	}; // class dimeState

} // namespace dime

