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
  \class dimeText dime/entities/Text.h
  \brief The dimeText class handles a Text \e entity.
*/

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

#include "gtl/gtl.h"

module dime.gtl:entities.Text;
import std;
import :Basic;
import :util;
import :Record;
import :Input;
import :Output;
import :Model;

using namespace std::literals;

namespace dime {


	constexpr double const CHAR_ASP = 0.82;

	//!

	void dimeText::setTextString(std::string str) {
		text = std::move(str);

		// Set new width.
		this->width = this->height * CHAR_ASP * text.size();
		if (this->wScale != 0.0)
			this->width = this->width * this->wScale;

		  //??? Set new origin or second if hJust is set?
	}

	//!

	bool dimeText::write(dimeOutput& file) {
		this->preWrite(file);

		// Write a text subclass before first controlled record.
		file.writeGroupCode(100);
		file.writeString("AcDbText");

		file.writeGroupCode(1);

		file.writeString(this->text);

		file.writeGroupCode(10);
		file.writeDouble(this->origin[0]);
		file.writeGroupCode(20);
		file.writeDouble(this->origin[1]);
		file.writeGroupCode(30);
		file.writeDouble(this->origin[2]);

		file.writeGroupCode(40);
		file.writeDouble(this->height);

		if (this->wScale != 0.0) {
			file.writeGroupCode(41);
			file.writeDouble(this->wScale);
		}

		if (this->rotation != 0.0) {
			file.writeGroupCode(50);
			file.writeDouble(this->rotation);
		}

		//<< PWH
		if (this->fTextGeneration) {
			file.writeGroupCode(71);
			file.writeInt16(this->fTextGeneration);
		}
		//>>

		if (this->hJust != 0) {
			file.writeGroupCode(72);
			file.writeInt16((int16)this->hJust);
		}

		if (haveSecond) {
			file.writeGroupCode(11);
			file.writeDouble(this->second[0]);
			file.writeGroupCode(21);
			file.writeDouble(this->second[1]);
			file.writeGroupCode(31);
			file.writeDouble(this->second[2]);

		}

		// For some reason a new subclass record is needed here.
		file.writeGroupCode(100);
		file.writeString("AcDbText");

		// The write order appears to be an issue???
		if (this->vJust != 0) {
			file.writeGroupCode(73);
			file.writeInt16((int16)this->vJust);
		}

		return this->writeExtrusionData(file) && dimeEntity::write(file);
	}

	//!

	bool dimeText::handleRecord(int groupcode, const dimeParam& param) {

		switch (groupcode) {
		case 1:
			this->setTextString(std::get<std::string>(param));
			if (this->height != 0.0)
				this->width = this->height * CHAR_ASP * this->text.size();
			if (wScale != 0.0)
				this->width = this->width * wScale;
			return true;
		case 10:
		case 20:
		case 30:
			this->origin[(groupcode / 10) - 1] = std::get<double>(param);
			return true;
		case 11:
		case 21:
		case 31:
			this->second[((groupcode - 1) / 10) - 1] = std::get<double>(param);
			this->haveSecond = true;
			return true;
		case 40:
			this->height = std::get<double>(param);
			if (!this->text.empty())
				this->width = this->height * CHAR_ASP * this->text.size();
			if (wScale != 0.0)
				this->width = this->width * wScale;
			return true;
		case 41:
			wScale = std::get<double>(param);
			if (this->width != 0.0)
				this->width = this->width * wScale;
			return true;
		case 50:
			this->rotation = std::get<double>(param);
			return true;
		  //<< PWH
		case 71:
			this->fTextGeneration = std::get<int16>(param);
			return true;
		  //>>
		case 72:
			this->hJust = std::get<int16>(param);	// PWH. int32_data -> int16_data
			return true;
		case 73:
			this->vJust = std::get<int16>(param);	// PWH. int32_data -> int16_data
			return true;
		case 100:
		  // Eat AcDbText records, leave others.
			if (std::get<std::string>(param) == "AcDbText") {
				return true;
			}
			else {
				return dimeExtrusionEntity::handleRecord(groupcode, param);
			}
		}
		return dimeExtrusionEntity::handleRecord(groupcode, param);
	}

	//!

	bool dimeText::getRecord(int groupcode, dimeParam& param, int index) const {
		switch (groupcode) {
		case 1:
			param.emplace<std::string>(this->text);
			return true;
		case 10:
		case 20:
		case 30:
			param.emplace<double>(this->origin[groupcode / 10 - 1]);
			return true;
		case 11:
		case 21:
		case 31:
			param.emplace<double>(this->second[(groupcode - 1) / 10 - 1]);
			return true;
		case 40:
			param.emplace<double>(this->height);
			return true;
		case 41:
			if (this->wScale == 0) return false;
			param.emplace<double>(this->wScale);
			return true;
		case 50:
			param.emplace<double>(this->rotation);
			return true;
		  //<< PWH
		case 71:
			param.emplace<int16>(this->fTextGeneration);
			return true;
		  //>>
		case 72:
			param.emplace<int16>(this->hJust);	// PWH. int32_data -> int16_data
			return true;
		case 73:
			param.emplace<int16>(this->vJust);	// PWH. int32_data -> int16_data
			return true;
		}
		return dimeExtrusionEntity::getRecord(groupcode, param, index);
	}

	////!

	//void dimeText::print() const {
	//	fprintf(stderr, "Text:\n");
	//	std::println(" origin: {:.3f} {:.3f} {:.3f}",
	//		origin[0], origin[1], origin[2]);
	//	if (haveSecond) {
	//		std::println(" second: {:.3f} {:%.3f} {:%.3f}",
	//			second[0], second[1], second[2]);
	//	}
	//	std::println(" height: {}", height);
	//	std::println(" rotation: {}", rotation);
	//	std::println(" horizJust: {}", hJust);
	//	std::println(" vertJust: {}", vJust);
	//	std::println(" wScale: {}", wScale);	// PWH.
	//	std::println(" textGeneration: {}, {} {}", fTextGeneration, fTextGeneration & 0x02 ? "FlipX" : "", fTextGeneration & 0x04 ? "FlipY" : "");	// PWH.
	//	std::println(" text: {}", (const char*)text);
	//	std::println(" extrusionDir: {} {} {}",
	//		extrusionDir[0], extrusionDir[1], extrusionDir[2]);
	//	std::println(" thickness: {}", thickness);
	//}

	//!

	dimeEntity::GeometryType dimeText::extractGeometry(
		std::vector<dimeVec3f>& verts,
		[[maybe_unused]] std::vector<int>& indices,
		dimeVec3f& extrusionDir_,
		dxfdouble& thickness_)
	{

		thickness_ = this->thickness;
		extrusionDir_ = this->extrusionDir;

		// find points at corners of box around text.
		verts.push_back(origin);
		verts.push_back(dimeVec3f(this->origin.x + this->width, this->origin.y, 0.0));
		verts.push_back(dimeVec3f(this->origin.x + this->width, this->origin.y + this->height, 0.0));
		verts.push_back(dimeVec3f(this->origin.x, this->origin.y + this->height, 0.0));

		// close loop with first point.
		verts.push_back(origin);

		if (this->thickness == 0.0) return dimeEntity::LINES;
		else return dimeEntity::POLYGONS;
	}

	//!

	size_t dimeText::countRecords() const {
		size_t cnt = 1 + 3 + 3 + 1 + 1 + 1 + 1 + 1 + 1 + 1; // header + origin + second + haveSecond + height + rotation + wScale + hJust + vJust + text
		cnt += 1;			// PWH. fTextGeneration
		return cnt + dimeExtrusionEntity::countRecords();
	}



	//=============================================================================
	// MText
	// 2013.02.28. PWH.
	// 2024-11-06.

	bool dimeMText::write(dimeOutput& file) {
		this->preWrite(file);

		// Write a text subclass before first controlled record.
		file.writeGroupCode(100);
		file.writeString("AcDbMText");

		file.writeGroupCode(10);
		file.writeDouble(ptOrigin[0]);
		file.writeGroupCode(20);
		file.writeDouble(ptOrigin[1]);
		file.writeGroupCode(30);
		file.writeDouble(ptOrigin[2]);

		file.writeGroupCode(40);
		file.writeDouble(dHeight);
		file.writeGroupCode(41);
		file.writeDouble(dWidth);

		file.writeGroupCode(71);
		file.writeInt16((int16)eAlign);

		file.writeGroupCode(72);
		file.writeInt16((int16)eDrawingDirection);

		// Text.
		// Group 1 : Text string
		//				If the text string is less than 250 characters, all characters appear in group 1. If the text string
		//				is greater than 250 characters, the string is divided into 250-character chunks, which appear
		//				in one or more group 3 codes. If group 3 codes are used, the last group is a group 1 and has
		//				fewer than 250 characters.
		// Group 3 : Additional text (always in 250-character chunks) (optional)
		//
		std::string_view sv(this->text);
		constexpr size_t const nMaxChar = 250;
		while (sv.size() > nMaxChar) {
			file.writeGroupCode(3);
			file.writeString(sv.substr(0, nMaxChar));
			sv = sv.substr(nMaxChar);
		}
		file.writeGroupCode(1);
		file.writeString(sv);

		if (!strTextStyleName.empty() && strTextStyleName.length()) {
			file.writeGroupCode(7);
			file.writeString(strTextStyleName.c_str());
		}

		file.writeGroupCode(11);
		file.writeDouble(vecXDirection[0]);
		file.writeGroupCode(12);
		file.writeDouble(vecXDirection[1]);
		file.writeGroupCode(13);
		file.writeDouble(vecXDirection[2]);

		if (dWidthChar != 0.0) {
			file.writeGroupCode(42);
			file.writeDouble(dWidthChar);
		}

		if (dHeightChar != 0.0) {
			file.writeGroupCode(43);
			file.writeDouble(dHeightChar);
		}

		file.writeGroupCode(50);
		file.writeDouble(dRotationalAngle);

		file.writeGroupCode(73);
		file.writeInt16((int16)eLineSpacingStyle);

		file.writeGroupCode(44);
		file.writeDouble(dLineSpacingFactor);

		// For some reason a new subclass record is needed here.
		file.writeGroupCode(100);
		file.writeString("AcDbMText");

		return this->writeExtrusionData(file) && dimeEntity::write(file);
	}

	bool dimeMText::handleRecord(int groupcode, const dimeParam& param) {
		switch (groupcode) {
		case 3:
			text += std::get<std::string>(param);
			return true;
		case 1:
			text = std::get<std::string>(param);
			return true;
		case 7:
			if (param.index() == std::to_underlying(eDimeParam::str)) {
				strTextStyleName = std::get<std::string>(param);
				//int nLength = strlen(std::get<std::string>(param));
				//rszTextStyleName.Assign(nLength+1);
				//strcpy_s(rszTextStyleName, nLength+1, std::get<std::string>(param));
			}
			else {
				strTextStyleName.clear();
			}
			return true;
		case 10:
		case 20:
		case 30:
			ptOrigin[(groupcode / 10) - 1] = std::get<double>(param);
			return true;
		case 11:
		case 21:
		case 31:
			vecXDirection[((groupcode - 1) / 10) - 1] = std::get<double>(param);
			return true;
		case 40:
			dHeight = std::get<double>(param);
			//if( this->text != NULL ) 
			//	this->width = this->height * CHAR_ASP * strlen( this->text );
			//if( wScale != 0.0 ) 
			//	this->width = this->width * wScale;
			return true;
		case 41:
			dWidth = std::get<double>(param);
			return true;
		case 42:
			dWidthChar = std::get<double>(param);
			return true;
		case 43:
			dHeightChar = std::get<double>(param);
			return true;
		case 44:
			dLineSpacingFactor = std::get<double>(param);
			return true;
		case 50:
			this->dRotationalAngle = std::get<double>(param);	// in radian
			return true;
		case 71:
			eAlign = (eTEXT_ALIGN)std::get<int16>(param);
			if ((eAlign < TA_NONE) || (eAlign >= nTA))
				eAlign = TA_LT;
			return true;
		case 72:
			eDrawingDirection = (eTEXT_DIRECTION)std::get<int16>(param);
			return true;
		case 73:
			eLineSpacingStyle = std::get<int16>(param);
			return true;
		case 100:
			// Eat AcDbText records, leave others.
			if (std::get<std::string>(param) == "AcDbMText") {
				return true;
			}
			else {
				return dimeExtrusionEntity::handleRecord(groupcode, param);
			}
		}
		return dimeExtrusionEntity::handleRecord(groupcode, param);
	}

	bool dimeMText::getRecord(int groupcode, dimeParam& param, int index) const {
		switch (groupcode) {
		case 1:
			param.emplace<std::string>(text);
			return true;
		case 7:
			param.emplace<std::string>(strTextStyleName);
			return true;
		case 10:
		case 20:
		case 30:
			param.emplace<double>(ptOrigin[groupcode / 10 - 1]);
			return true;
		case 11:
		case 21:
		case 31:
			param.emplace<double>(vecXDirection[(groupcode - 1) / 10 - 1]);
			return true;
		case 40:
			param.emplace<double>(dHeight);
			return true;
		case 41:
			param.emplace<double>(dWidth);
			return true;
		case 42:
			param.emplace<double>(dWidthChar);
			return true;
		case 43:
			param.emplace<double>(dHeightChar);
			return true;
		case 44:
			param.emplace<double>(dLineSpacingFactor);
			return true;
		case 50:
			param.emplace<double>(dRotationalAngle);
			return true;
		case 71:
			param.emplace<int32>(eAlign);
			return true;
		case 72:
			param.emplace<int32>(eDrawingDirection);
			return true;
		case 73:
			param.emplace<int32>(eLineSpacingStyle);
			return true;
		}
		return dimeExtrusionEntity::getRecord(groupcode, param, index);
	}

	//void dimeMText::print() const {
	//	fprintf(stderr, "Text:\n");
	//	std::println(" origin: %.3f %.3f %.3f\n", ptOrigin[0], ptOrigin[1], ptOrigin[2]);
	//	std::println(" vector: %.3f %.3f %.3f\n", vecXDirection[0], vecXDirection[1], vecXDirection[2]);
	//	std::println(" height: %f\n", dHeight);
	//	std::println(" width: %f\n", dWidth);
	//	std::println(" heightChar: %f\n", dHeightChar);
	//	std::println(" widthChar: %f\n", dWidthChar);
	//	std::println(" Align: %d\n", eAlign);
	//	std::println(" DrawingDirection: %d\n", eDrawingDirection);
	//	std::println(" TextStyle: %s\n", strTextStyleName.c_str());
	//	std::println(" rotation: %f\n", (double)dRotationalAngle*180./M_PI);	// in deg
	//	std::println(" lineSpacingStyle: %d\n", eLineSpacingStyle);
	//	std::println(" lineSpacingFactor: %f\n", dLineSpacingFactor);
	//	std::println(" text: %s\n", text.c_str());
	//	std::println(" extrusionDir: %f %f %f\n", extrusionDir[0], extrusionDir[1], extrusionDir[2]);
	//	std::println(" thickness: %f\n", thickness);
	//}

	dimeEntity::GeometryType dimeMText::extractGeometry(
		std::vector<dimeVec3f>& verts,
		[[maybe_unused]] std::vector<int>& indices,
		dimeVec3f& extrusionDir_,
		dxfdouble& thickness_)
	{

		thickness_ = this->thickness;
		extrusionDir_ = this->extrusionDir;

		// find points at corners of box around text.
		verts.push_back(dimeVec3f(ptOrigin.x, ptOrigin.y, ptOrigin.z));
		verts.push_back(dimeVec3f(this->ptOrigin.x + this->dWidth, this->ptOrigin.y, 0.0));
		verts.push_back(dimeVec3f(this->ptOrigin.x + this->dWidth, this->ptOrigin.y + this->dHeight, 0.0));
		verts.push_back(dimeVec3f(this->ptOrigin.x, this->ptOrigin.y + this->dHeight, 0.0));

		// close loop with first point.
		verts.push_back(dimeVec3f(ptOrigin.x, ptOrigin.y, ptOrigin.z));

		if (this->thickness == 0.0) return dimeEntity::LINES;
		else return dimeEntity::POLYGONS;
	}

	size_t dimeMText::countRecords() const {
		size_t cnt =
			1	// header
			+ 3 + 1 + 1		// Origin, Height/Width
			+ 1 + 1 + 1 + 1	// Align, Drawing Direction, textStyleName
			+ 3 + 1 + 1		// Direction Vector, Height/Width of Char
			+ 1 + 1 + 1		// Rotaional Angle, Line Spacing Style/Factor
			;
		return cnt + dimeExtrusionEntity::countRecords();
	}

} // namespace dime
 