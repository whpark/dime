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

module dime.biscuit:convert.convert;
import std;
import biscuit;
import :convert.convert_funcs;
import :convert.layerdata;
import :entities.Circle;
import :State;

using namespace std::literals;

namespace dime {

//
// find intersection between circle and the line x=r-maxerr,
// and return the number of circle subdivisions necessary
// to respect the maxerr parameter.
//
static int 
calc_num_sub(dxfdouble maxerr, dxfdouble radius)
{
  if (maxerr >= radius || maxerr <= 0.0) maxerr = radius/40.0f;

  dxfdouble x = radius - maxerr;
  dxfdouble y = sqrt(radius*radius - x*x);

  dxfdouble rad = atan(y/x);
  
  return int(M_PI/fabs(rad)) + 1;
}


void 
convert_circle(const dimeEntity *entity, const dimeState *state, 
	       dxfLayerData *layerData, dxfConverter *converter)
{
  dimeCircle *circle = (dimeCircle*) entity;

  dimeMatrix matrix;
  state->getMatrix(matrix);

  dimeVec3f e = circle->getExtrusionDir();
  dxfdouble thickness = circle->getThickness();
  
  if (e != dimeVec3f(0,0,1)) {
    dimeMatrix m;
    dimeEntity::generateUCS(e, m);
    matrix.multRight(m);
  }
  e = dimeVec3f(0,0,1) * thickness;

  dimeVec3f center = circle->getCenter();
  dxfdouble radius = circle->getRadius();

  dimeParam param;
  if (circle->getRecord(38, param)) {
    center[2] = std::get<double>(param);
  }
  
  int numpts = converter->getNumSub();
  if (numpts <= 0) { // use maxerr
    numpts = calc_num_sub(converter->getMaxerr(), radius);
  }
  dxfdouble inc = (2*M_PI) / numpts;  
  dxfdouble rad = inc;
  int i;
  
  dimeVec3f v;
  dimeVec3f prev(center[0] + radius,
		 center[1],
		 center[2]);
  
  for (i = 1; i < numpts; i++) {
    v = dimeVec3f(center[0] + radius * cos(rad),
		  center[1] + radius * sin(rad),
		  center[2]);
    
    if (thickness == 0.0) {
      layerData->addLine(prev, v, &matrix);
    }
    else {
      layerData->addQuad(prev, v, v + e, prev + e,
			 &matrix);
    }
    prev = v;
    rad += inc;
  }

  v = dimeVec3f(center[0] + radius,
		center[1],
		center[2]);
  
  if (thickness == 0.0) {
    layerData->addLine(prev, v, &matrix);
  }
  else {
    layerData->addQuad(prev, v, v + e, prev + e,
		       &matrix);
  }
  // FIXME: code to close cylinder?
}

} // namespace dime