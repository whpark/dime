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

export module dime.gtl:convert.convert;
import std;
import :Basic;

using namespace std::literals;

namespace dime {
  class dimeModel;
  class dxfLayerData;
  class dimeState;
  class dimeEntity;
}

export namespace dime {



class DIME_DLL_API dxfConverter
{
public:
  dxfConverter();
  ~dxfConverter();
  
  void setMaxerr(const dxfdouble maxerr) {
    this->maxerr = maxerr;
  }
  void findHeaderVariables(dimeModel &model);
  bool doConvert(dimeModel &model);
  bool writeVrml(const char * filename, const bool vrml1 = false,
                 const bool only2d = false);
  bool writeVrml(FILE *out, const bool vrml1 = false,
                 const bool only2d = false);

  void setNumSub(int num) {
    this->numsub = num;
  }
  int getNumSub() const {
    return numsub;
  }
  dxfdouble getMaxerr() const {
    return this->maxerr;
  }

  void setFillmode(const bool fill) {
    this->fillmode = fill;
  }
  bool getFillmode() const {
    return this->fillmode;
  }

  bool getLayercol() const {
    return this->layercol;
  }
  
  void setLayercol(const bool v) {
    this->layercol = v;
  }

  dxfLayerData *getLayerData(int colidx);
  dxfLayerData *getLayerData(const dimeEntity *entity);
  dxfLayerData ** getLayerData();
  int getColorIndex(const dimeEntity *entity);
  int getCurrentInsertColorIndex() const {
    return currentInsertColorIndex;
  }

private:
  friend class dime2Profit;
  friend class dime2So;

  dxfLayerData *layerData[255];
  int dummy[4];
  dxfdouble maxerr;
  int currentInsertColorIndex;
  dimeEntity *currentPolyline;
  int numsub;
  bool fillmode;
  bool layercol;
  
  bool private_callback(const dimeState * const state, 
			dimeEntity *entity);
  static bool dime_callback(const dimeState * const state, 
			    dimeEntity *entity, void *);

};

} // namespace dime
