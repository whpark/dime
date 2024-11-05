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

/*!
  \class dimeCircle dime/entities/Circle.h
  \brief The dimeCircle class handles a CIRCLE \e entity.
*/

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

module dime.biscuit:entities.Circle;
import std;
import biscuit;
import :Basic;
import :util;
import :Record;
import :Input;
import :Output;
import :Model;

using namespace std::literals;

namespace dime {


static char entityName[] = "CIRCLE";

// FIXME: configurable

#define CIRCLE_NUMPTS 16

/*!
  Constructor.
*/

dimeCircle::dimeCircle() 
  : center(0,0,0), radius( 0.0 )
{
}

//!

dimeEntity *
dimeCircle::copy(dimeModel * const model) const
{
  dimeCircle *c = new(model->getMemHandler()) dimeCircle;
  if (!c) return NULL;
  
  if (!this->copyRecords(c, model)) {
    // check if allocated on heap.
    if (!model->getMemHandler()) delete c;
    c = NULL;
  }
  else {
    c->copyExtrusionData(this);
    c->center = this->center;
    c->radius = this->radius;
  }
  return c;  
}

/*!
  This method writes a DXF \e Circle entity to \a file.
*/

bool 
dimeCircle::write(dimeOutput& file)
{
  dimeEntity::preWrite(file);
  
  file.writeGroupCode(10);
  file.writeDouble(this->center[0]);
  file.writeGroupCode(20);
  file.writeDouble(this->center[1]);
  file.writeGroupCode(30);
  file.writeDouble(this->center[2]);
  
  file.writeGroupCode(40);
  file.writeDouble(this->radius);

  this->writeExtrusionData(file);
  
  return dimeEntity::write(file); // write unknown records.
}

//!

int 
dimeCircle::typeId() const
{
  return dimeBase::dimeCircleType;
}

/*!
  Handles the callback from dimeEntity::readRecords().
*/

bool 
dimeCircle::handleRecord(int groupcode,
			const dimeParam &param,
			)
{
  switch(groupcode) {
  case 10:
  case 20:
  case 30:
    this->center[groupcode/10-1] = std::get<double>(param);
    return true;
  case 40:
    this->radius = std::get<double>(param);
    return true;
  }
  return dimeExtrusionEntity::handleRecord(groupcode, param, memhandler);
}

//!

const char *
dimeCircle::getEntityName() const
{
  return entityName;
}

//!

bool 
dimeCircle::getRecord(int groupcode,
		     dimeParam &param,
		     int index) const
{
  switch(groupcode) {
  case 10:
  case 20:
  case 30:
    param.double_data = this->center[groupcode/10-1]; 
    return true;
  case 40:
    param.double_data = this->radius;
    return true;
  }
  return dimeExtrusionEntity::getRecord(groupcode, param, index);  
}

//!

void
dimeCircle::print() const
{
  fprintf(stderr,"CIRCLE:\n");
  fprintf(stderr, " center: %.3f %.3f %.3f\n", center[0], center[1], center[2]);
  fprintf(stderr, " radius: %f\n", radius);
}

//!

dimeEntity::GeometryType 
dimeCircle::extractGeometry(std::vector<dimeVec3f> &verts,
			   std::vector<int> &/*indices*/,
			   dimeVec3f &extrusionDir,
			   dxfdouble &thickness)
{
  thickness = this->thickness;
  extrusionDir = this->extrusionDir;

   // tessellate the circle/cylinder
  dxfdouble inc = (2*M_PI) / CIRCLE_NUMPTS;
  dxfdouble rad = 0.0f;
  int i;
  for (i = 0; i < CIRCLE_NUMPTS; i++) {
    verts.append(dimeVec3f(this->center.x + this->radius * cos(rad),
			   this->center.y + this->radius * sin(rad),
			   this->center.z));
    rad += inc;
  }
  
  dimeVec3f tmp = verts[0];
  verts.append(tmp); // closed line/polygon
  
  if (this->thickness == 0.0) return dimeEntity::LINES;
  else return dimeEntity::POLYGONS;
}

//!

int
dimeCircle::countRecords() const
{
  // header + center point + radius
  return 5 + dimeExtrusionEntity::countRecords();
}


} // namespace dime
