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
  \class dimeText dime/entities/Text.h
  \brief The dimeText class handles a Text \e entity.
*/

#include <dime/entities/Text.h>
#include <dime/records/Record.h>
#include <dime/Input.h>
#include <dime/Output.h>
#include <dime/util/MemHandler.h>
#include <dime/Model.h>
#define _USE_MATH_DEFINES	// PWH.
#include <math.h>
#include <string.h>
#include <stddef.h>

#define CHAR_ASP 0.82

static char entityName[] = "TEXT";
static char entityNameMText[] = "MTEXT";	// PWH.

/*!
  Constructor.
*/

dimeText::dimeText() 
  : origin( 0.0, 0.0, 0.0 ), second( 0.0, 0.0, 0.0 ), haveSecond( false ), height( 0.0 ), width( 0.0 ), rotation( 0.0 ), wScale( 0.0 ), hJust(0), vJust(0), text( NULL )
{
}

//!

void dimeText::setTextString(const char* s)
{
  size_t l;
  l = strlen( s );
  char* t = new char[l+1];
  strcpy( t, s );
  this->text = (char*) t;

  // Set new width.
  this->width = this->height * CHAR_ASP * strlen( this->text );
  if( this->wScale != 0.0 ) 
    this->width = this->width * this->wScale;

  //??? Set new origin or second if hJust is set?
}

//!

dimeEntity *
dimeText::copy(dimeModel * const model) const
{
  dimeText *t = new(model->getMemHandler()) dimeText;
  if (!t) return NULL;
  
  if (!this->copyRecords(t, model)) {
    // check if allocated on heap.
    if (!model->getMemHandler()) delete t;
    t = NULL;
  }
  else {
    t->origin = this->origin;
    t->second = this->second;
    t->haveSecond = this->haveSecond;
    t->height = this->height;
    t->width = this->width;
    t->rotation = this->rotation;
    t->wScale = this->wScale;
    t->hJust = this->hJust;
    t->vJust = this->vJust;
    size_t l;
    l = strlen( this->text );
    char* s = new char[l+1];
    strcpy( s, this->text );
    t->text = (char*) s;
    t->copyExtrusionData(this);
  }
  return t;  
}

//!

bool 
dimeText::write(dimeOutput * const file)
{
  this->preWrite(file);
  
  // Write a text subclass before first controlled record.
  file->writeGroupCode(100);
  file->writeString("AcDbText");

  file->writeGroupCode(1);

  file->writeString(this->text);

  file->writeGroupCode(10);
  file->writeDouble(this->origin[0]);
  file->writeGroupCode(20);
  file->writeDouble(this->origin[1]);
  file->writeGroupCode(30);
  file->writeDouble(this->origin[2]);
  
  file->writeGroupCode(40);
  file->writeDouble(this->height);

  if( this->wScale != 0.0 ) {
    file->writeGroupCode(41);
    file->writeDouble(this->wScale);
  }

  if( this->rotation != 0.0 ) {
    file->writeGroupCode(50);
    file->writeDouble(this->rotation);
  }

  //<< PWH
  if (this->fTextGeneration) {
	  file->writeGroupCode(71);
	  file->writeInt16(this->fTextGeneration);
  }
  //>>

  if( this->hJust != 0 ) {
    file->writeGroupCode(72);
    file->writeInt16( (int16) this->hJust);
  }

  if( haveSecond ) {
    file->writeGroupCode(11);
    file->writeDouble(this->second[0]);
    file->writeGroupCode(21);
    file->writeDouble(this->second[1]);
    file->writeGroupCode(31);
    file->writeDouble(this->second[2]);

  }

  // For some reason a new subclass record is needed here.
  file->writeGroupCode(100);
  file->writeString("AcDbText");

  // The write order appears to be an issue???
  if( this->vJust != 0 ) {
    file->writeGroupCode(73);
    file->writeInt16( (int16) this->vJust);
  }

  return this->writeExtrusionData(file) && dimeEntity::write(file);
}

//!

int 
dimeText::typeId() const
{
  return dimeBase::dimeTextType;
}

//!

bool 
dimeText::handleRecord(const int groupcode,
		     const dimeParam &param,
		     dimeMemHandler * const memhandler)
{

  //char subclass[80];	// PWH.

  switch(groupcode) {
  case 1:
    this->setTextString( param.string_data );
    if( this->height != 0.0 ) 
      this->width = this->height * CHAR_ASP * strlen( this->text );
    if( wScale != 0.0 ) 
      this->width = this->width * wScale;
    return true;
  case 10:
  case 20:
  case 30:
    this->origin[( groupcode / 10 ) - 1] = param.double_data;
    return true;
  case 11:
  case 21:
  case 31:
    this->second[( ( groupcode - 1 ) / 10 ) - 1] = param.double_data;
    this->haveSecond = true;
    return true;
  case 40:
    this->height = param.double_data;
    if( this->text != NULL ) 
      this->width = this->height * CHAR_ASP * strlen( this->text );
    if( wScale != 0.0 ) 
      this->width = this->width * wScale;
    return true;
  case 41:
    wScale = param.double_data;
    if( this->width != 0.0 ) 
      this->width = this->width * wScale;
    return true;
  case 50:
    this->rotation = param.double_data;
    return true;
  //<< PWH
  case 71 :
    this->fTextGeneration = param.int16_data;
    return true;
  //>>
  case 72:
    this->hJust = param.int16_data;	// PWH. int32_data -> int16_data
    return true;
  case 73:
    this->vJust = param.int16_data;	// PWH. int32_data -> int16_data
    return true;
  case 100:
    // Eat AcDbText records, leave others.
    if( strcmp( param.string_data, "AcDbText" ) == 0 ) {
      return true;
    }
    else {
      return dimeExtrusionEntity::handleRecord(groupcode, param, memhandler);
    }
  }
  return dimeExtrusionEntity::handleRecord(groupcode, param, memhandler);
}

//!

const char *
dimeText::getEntityName() const
{
  return entityName;
}

//!

bool 
dimeText::getRecord(const int groupcode,
		  dimeParam &param,
		  const int index) const
{
  switch(groupcode) {
  case 1:
    param.string_data = this->text;
    return true;
  case 10:
  case 20:
  case 30:
    param.double_data = this->origin[groupcode / 10 - 1];
    return true;
  case 11:
  case 21:
  case 31:
    param.double_data = this->second[( groupcode - 1 ) / 10 - 1];
    return true;
  case 40:
    param.double_data = this->height;
    return true;
  case 41:
    if( this->wScale == 0 ) return false;
    param.double_data = this->wScale;
    return true;
  case 50:
    param.double_data = this->rotation;
    return true;
  //<< PWH
  case 71:
    param.int16_data = this->fTextGeneration;
    return true;
  //>>
  case 72:
    param.int16_data = this->hJust;	// PWH. int32_data -> int16_data
    return true;
  case 73:
    param.int16_data = this->vJust;	// PWH. int32_data -> int16_data
    return true;
  }
  return dimeExtrusionEntity::getRecord(groupcode, param, index);
}

//!

void
dimeText::print() const
{
  fprintf(stderr,"Text:\n");
  fprintf(stderr, " origin: %.3f %.3f %.3f\n", 
	  origin[0], origin[1], origin[2]);
  if( haveSecond ) {
    fprintf(stderr, " second: %.3f %.3f %.3f\n", 
	    second[0], second[1], second[2]);
  }
  fprintf(stderr, " height: %f\n", height);
  fprintf(stderr, " rotation: %f\n", rotation);
  fprintf(stderr, " horizJust: %d\n", hJust);
  fprintf(stderr, " vertJust: %d\n", vJust);
  fprintf(stderr, " wScale: %f\n", wScale);	// PWH.
  fprintf(stderr, " textGeneration: %d, %s %s\n", fTextGeneration, fTextGeneration & 0x02 ? "FlipX" : "", fTextGeneration & 0x04 ? "FlipY" : "");	// PWH.
  fprintf(stderr, " text: %s\n", (const char*)text);
  fprintf(stderr, " extrusionDir: %f %f %f\n", 
	  extrusionDir[0], extrusionDir[1], extrusionDir[2]);
  fprintf(stderr, " thickness: %f\n", thickness);
}

//!

dimeEntity::GeometryType 
dimeText::extractGeometry(dimeArray <dimeVec3f> &verts,
			dimeArray <int> &indices,
			dimeVec3f &extrusionDir,
			dxfdouble &thickness)
{

  thickness = this->thickness;
  extrusionDir = this->extrusionDir;

  // find points at corners of box around text.
  verts.append(origin);
  verts.append( dimeVec3f(this->origin.x + this->width, this->origin.y, 0.0) );
  verts.append( dimeVec3f(this->origin.x + this->width, this->origin.y + this->height, 0.0) );
  verts.append( dimeVec3f(this->origin.x, this->origin.y + this->height, 0.0) );

  // close loop with first point.
  verts.append(origin);

  if (this->thickness == 0.0) return dimeEntity::LINES;
  else return dimeEntity::POLYGONS;
}

//!

int
dimeText::countRecords() const
{
  int cnt = 1 + 3 + 3 + 1 + 1 + 1 + 1 + 1 + 1 + 1; // header + origin + second + haveSecond + height + rotation + wScale + hJust + vJust + text
  cnt += 1;			// PWH. fTextGeneration
  return cnt + dimeExtrusionEntity::countRecords();
}



//=============================================================================
// MText
// 2013.02.28. PWH.
//

dimeEntity* dimeMText::copy(dimeModel * const model) const {
	dimeMText *t = new(model->getMemHandler()) dimeMText;
	if (!t) return NULL;

	if (!this->copyRecords(t, model)) {
		// check if allocated on heap.
		if (!model->getMemHandler()) delete t;
		t = NULL;
	} else {
		t->ptOrigin				= ptOrigin;
		t->dHeight				= dHeight;
		t->dWidth				= dWidth;
		t->eAlign				= eAlign;
		t->eDrawingDirection	= eDrawingDirection;
		t->strTextStyleName		= strTextStyleName;
		t->vecXDirection		= vecXDirection;
		t->dWidthChar			= dWidthChar;
		t->dHeightChar			= dHeightChar;
		t->dRotationalAngle		= dRotationalAngle;
		t->eLineSpacingStyle	= eLineSpacingStyle;
		t->dLineSpacingFactor	= dLineSpacingFactor;
		t->strText				= strText;

		t->copyExtrusionData(this);
	}
	return t;  
}

bool dimeMText::write(dimeOutput * const file) {
	this->preWrite(file);

	// Write a text subclass before first controlled record.
	file->writeGroupCode(100);
	file->writeString("AcDbMText");

	file->writeGroupCode(10);
	file->writeDouble(ptOrigin.val[0]);
	file->writeGroupCode(20);
	file->writeDouble(ptOrigin.val[1]);
	file->writeGroupCode(30);
	file->writeDouble(ptOrigin.val[2]);

	file->writeGroupCode(40);
	file->writeDouble(dHeight);
	file->writeGroupCode(41);
	file->writeDouble(dWidth);

	file->writeGroupCode(71);
	file->writeInt16((int16)eAlign);

	file->writeGroupCode(72);
	file->writeInt16((int16)eDrawingDirection);

	// Text.
	// Group 1 : Text string
	//				If the text string is less than 250 characters, all characters appear in group 1. If the text string
	//				is greater than 250 characters, the string is divided into 250-character chunks, which appear
	//				in one or more group 3 codes. If group 3 codes are used, the last group is a group 1 and has
	//				fewer than 250 characters.
	// Group 3 : Additional text (always in 250-character chunks) (optional)
	//
	const char* psz = strText.c_str();
	int nLengthLeft = strlen(psz);
	const int nMaxChar = 250;
	while (nLengthLeft > nMaxChar) {
		char buf[nMaxChar+1];
		memcpy(buf, psz, nMaxChar);
		buf[nMaxChar] = 0;
		psz += nMaxChar;
		nLengthLeft -= nMaxChar;
		file->writeGroupCode(3);
		file->writeString(buf);
	}
	file->writeGroupCode(1);
	file->writeString(psz);

	if (!strTextStyleName.empty() && strTextStyleName.length()) {
		file->writeGroupCode(7);
		file->writeString(strTextStyleName.c_str());
	}

	file->writeGroupCode(11);
	file->writeDouble(vecXDirection.val[0]);
	file->writeGroupCode(12);
	file->writeDouble(vecXDirection.val[1]);
	file->writeGroupCode(13);
	file->writeDouble(vecXDirection.val[2]);

	if (dWidthChar != 0.0) {
		file->writeGroupCode(42);
		file->writeDouble(dWidthChar);
	}

	if (dHeightChar != 0.0) {
		file->writeGroupCode(43);
		file->writeDouble(dHeightChar);
	}

	file->writeGroupCode(50);
	file->writeDouble(dRotationalAngle);

	file->writeGroupCode(73);
	file->writeInt16(eLineSpacingStyle);

	file->writeGroupCode(44);
	file->writeDouble(dLineSpacingFactor);

	// For some reason a new subclass record is needed here.
	file->writeGroupCode(100);
	file->writeString("AcDbMText");

	return this->writeExtrusionData(file) && dimeEntity::write(file);
}

int dimeMText::typeId() const {
	return dimeBase::dimeMTextType;
}

bool dimeMText::handleRecord(const int groupcode, const dimeParam &param, dimeMemHandler * const memhandler) {
	switch(groupcode) {
	case 3 :
		AddText(param.string_data);
		return true;
	case 1:
		AddText(param.string_data);
		//if( this->height != 0.0 ) 
		//	this->width = this->height * CHAR_ASP * strlen( this->text );
		//if( wScale != 0.0 ) 
		//	this->width = this->width * wScale;
		return true;
	case 7 :
		if (param.string_data) {
			strTextStyleName = param.string_data;
			//int nLength = strlen(param.string_data);
			//rszTextStyleName.Assign(nLength+1);
			//strcpy_s(rszTextStyleName, nLength+1, param.string_data);
		} else {
			strTextStyleName.clear();
		}
		return true;
	case 10:
	case 20:
	case 30:
		ptOrigin.val[( groupcode / 10 ) - 1] = param.double_data;
		return true;
	case 11:
	case 21:
	case 31:
		vecXDirection.val[( ( groupcode - 1 ) / 10 ) - 1] = param.double_data;
		return true;
	case 40:
		dHeight = param.double_data;
		//if( this->text != NULL ) 
		//	this->width = this->height * CHAR_ASP * strlen( this->text );
		//if( wScale != 0.0 ) 
		//	this->width = this->width * wScale;
		return true;
	case 41:
		dWidth = param.double_data;
		return true;
	case 42 :
		dWidthChar = param.double_data;
		return true;
	case 43 :
		dHeightChar = param.double_data;
		return true;
	case 44 :
		dLineSpacingFactor = param.double_data;
		return true;
	case 50:
		this->dRotationalAngle = param.double_data;	// in radian
		return true;
	case 71 :
		eAlign = (eTEXT_ALIGN)param.int16_data;
		if ( (eAlign < TA_NONE) || (eAlign >= nTA) )
			eAlign = TA_LT;
		return true;
	case 72:
		eDrawingDirection = (eTEXT_DIRECTION)param.int32_data;
		return true;
	case 73 :
		eLineSpacingStyle = param.int16_data;
		return true;
	case 100:
		// Eat AcDbText records, leave others.
		if( strcmp( param.string_data, "AcDbMText" ) == 0 ) {
			return true;
		}
		else {
			return dimeExtrusionEntity::handleRecord(groupcode, param, memhandler);
		}
	}
	return dimeExtrusionEntity::handleRecord(groupcode, param, memhandler);
}

const char * dimeMText::getEntityName() const { return entityNameMText; }

bool dimeMText::getRecord(const int groupcode, dimeParam &param, const int index) const {
	switch(groupcode) {
	case 1:
		param.string_data = strText.c_str();
		return true;
	case 7 :
		param.string_data = strTextStyleName.c_str();
		return true;
	case 10:
	case 20:
	case 30:
		param.double_data = ptOrigin.val[groupcode / 10 - 1];
		return true;
	case 11:
	case 21:
	case 31:
		param.double_data = vecXDirection.val[( groupcode - 1 ) / 10 - 1];
		return true;
	case 40:
		param.double_data = dHeight;
		return true;
	case 41:
		param.double_data = dWidth;
		return true;
	case 42:
		param.double_data = dWidthChar;
		return true;
	case 43:
		param.double_data = dHeightChar;
		return true;
	case 44:
		param.double_data = dLineSpacingFactor;
		return true;
	case 50:
		param.double_data = dRotationalAngle;
		return true;
	case 71:
		param.int32_data = eAlign;
		return true;
	case 72:
		param.int32_data = eDrawingDirection;
		return true;
	case 73:
		param.int32_data = eLineSpacingStyle;
		return true;
	}
	return dimeExtrusionEntity::getRecord(groupcode, param, index);
}

void dimeMText::print() const {
	fprintf(stderr,"Text:\n");
	fprintf(stderr, " origin: %.3f %.3f %.3f\n", ptOrigin.val[0], ptOrigin.val[1], ptOrigin.val[2]);
	fprintf(stderr, " vector: %.3f %.3f %.3f\n", vecXDirection.val[0], vecXDirection.val[1], vecXDirection.val[2]);
	fprintf(stderr, " height: %f\n", dHeight);
	fprintf(stderr, " width: %f\n", dWidth);
	fprintf(stderr, " heightChar: %f\n", dHeightChar);
	fprintf(stderr, " widthChar: %f\n", dWidthChar);
	fprintf(stderr, " Align: %d\n", eAlign);
	fprintf(stderr, " DrawingDirection: %d\n", eDrawingDirection);
	fprintf(stderr, " TextStyle: %s\n", strTextStyleName.c_str());
	fprintf(stderr, " rotation: %f\n", (double)dRotationalAngle*180./M_PI);	// in deg
	fprintf(stderr, " lineSpacingStyle: %d\n", eLineSpacingStyle);
	fprintf(stderr, " lineSpacingFactor: %f\n", dLineSpacingFactor);
	fprintf(stderr, " text: %s\n", strText.c_str());
	fprintf(stderr, " extrusionDir: %f %f %f\n", extrusionDir[0], extrusionDir[1], extrusionDir[2]);
	fprintf(stderr, " thickness: %f\n", thickness);
}

dimeEntity::GeometryType dimeMText::extractGeometry(dimeArray <dimeVec3f> &verts, dimeArray <int> &indices, dimeVec3f &extrusionDir, dxfdouble &thickness) {

	thickness = this->thickness;
	extrusionDir = this->extrusionDir;

	// find points at corners of box around text.
	verts.append(dimeVec3f(ptOrigin.x, ptOrigin.y, ptOrigin.z));
	verts.append( dimeVec3f(this->ptOrigin.x + this->dWidth, this->ptOrigin.y, 0.0) );
	verts.append( dimeVec3f(this->ptOrigin.x + this->dWidth, this->ptOrigin.y + this->dHeight, 0.0) );
	verts.append( dimeVec3f(this->ptOrigin.x, this->ptOrigin.y + this->dHeight, 0.0) );

	// close loop with first point.
	verts.append(dimeVec3f(ptOrigin.x, ptOrigin.y, ptOrigin.z));

	if (this->thickness == 0.0) return dimeEntity::LINES;
	else return dimeEntity::POLYGONS;
}

int dimeMText::countRecords() const {
	int cnt =
		1	// header
		+ 3 + 1 + 1		// Origin, Height/Width
		+ 1 + 1 + 1 + 1	// Align, Drawing Direction, textStyleName
		+ 3 + 1 + 1		// Direction Vector, Height/Width of Char
		+ 1 + 1 + 1		// Rotaional Angle, Line Spacing Style/Factor
		;
	return cnt + dimeExtrusionEntity::countRecords();
}

