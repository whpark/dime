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

export module dime.biscuit:convert.convert_funcs;
import std;
import biscuit;
import :Basic;

using namespace std::literals;

namespace dime {
	class dimeEntity;
	class dimeState;
	class dxfLayerData;
	class dxfConverter;
}

export namespace dime {


void convert_3dface(const dimeEntity *, const dimeState *, 
		    dxfLayerData *, dxfConverter *);
void convert_line(const dimeEntity *, const dimeState *, 
		  dxfLayerData *, dxfConverter *);
void convert_point(const dimeEntity *, const dimeState *, 
		   dxfLayerData *, dxfConverter *);
void convert_circle(const dimeEntity *, const dimeState *, 
		    dxfLayerData *, dxfConverter *);
void convert_ellipse(const dimeEntity *, const dimeState *, 
		     dxfLayerData *, dxfConverter *);
void convert_arc(const dimeEntity *, const dimeState *, 
		 dxfLayerData *, dxfConverter *);
void convert_solid(const dimeEntity *, const dimeState *, 
		   dxfLayerData *, dxfConverter *);
void convert_trace(const dimeEntity *, const dimeState *, 
		   dxfLayerData *, dxfConverter *);
void convert_polyline(const dimeEntity *, const dimeState *, 
		      dxfLayerData *, dxfConverter *);
void convert_lwpolyline(const dimeEntity *, const dimeState *, 
			dxfLayerData *, dxfConverter *);

} // namespace dime

