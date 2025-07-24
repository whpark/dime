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

#include "gtl/gtl.h'

export module dime.gtl:;
import std;
import :Basic;
import :util;

using namespace std::literals;

namespace dime {
}

export namespace dime {


class DIME_DLL_API dxfLayerData {
public:
  dxfLayerData(int colidx);
  ~dxfLayerData();

  void setFillmode(const bool fillmode);
  
  void addLine(const dimeVec3f &v0, const dimeVec3f &v1,
	       const dimeMatrix * const matrix = NULL);

  void addPoint(const dimeVec3f &v,
		const dimeMatrix * const matrix = NULL);

  void addTriangle(const dimeVec3f &v0,
		   const dimeVec3f &v1,
		   const dimeVec3f &v2,
		   const dimeMatrix * const matrix = NULL);
  void addQuad(const dimeVec3f &v0,
	       const dimeVec3f &v1,
	       const dimeVec3f &v2,
	       const dimeVec3f &v3,
	       const dimeMatrix * const matrix = NULL);
  
  void writeWrl(FILE *fp, int indent, const bool vrml1,
                const bool only2d);

//private:
public: // 20011001 thammer - please don't kill me for this ;-)

  friend class dime2So;
  friend class dime2Profit;

  bool fillmode;
  int colidx;
  dimeBSPTree facebsp;
  std::vector<int> faceindices;
  dimeBSPTree linebsp;
  std::vector<int> lineindices;
  std::vector<dimeVec3f> points;
};

} // namespace dime
