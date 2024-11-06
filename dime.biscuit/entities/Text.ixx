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

#include "../Basic.h"

export module dime.biscuit:entities.Text;
import std;
import biscuit;
import :Basic;
import :util;
import :Base;
import :Input;
import :Output;
import :entities.Entity;
import :entities.ExtrusionEntity;

using namespace std::literals;

namespace dime {
}

export namespace dime {


	class dimeText : public dimeExtrusionEntity {
	public:
		static inline std::string const entityName = "TEXT"s;
		BSC__DEFINE_CTOR_DTOR_DERIVED(dimeText, dimeExtrusionEntity);
		BSC__DEFINE_CLONE_DERIVED(dimeEntity);

		void setOrigin(const dimeVec3f& o);
		void getOrigin(dimeVec3f& o) const;
		void setSecond(const dimeVec3f& s);
		bool getSecond(dimeVec3f& s) const;
		void setHeight(const dxfdouble h);
		dxfdouble getHeight() const;
		void setWidth(const dxfdouble w);
		dxfdouble getWidth() const;
		void setRotation(const dxfdouble a);
		dxfdouble getRotation() const;
		void setHJust(int16 h);
		int32 getHJust() const;
		void setVJust(int16 v);
		int32 getVJust() const;
		void setTextString(std::string str);

		//<< PWH.
		dxfdouble getWScale() const { return wScale; }
		int32 getTextGeneration() const { return fTextGeneration; }
		void setTextGeneration(bool bFlipX, bool bFlipY) { fTextGeneration = (bFlipX ? 2 : 0) | (bFlipY ? 4 : 0); }
		void setTextGeneration(int16 f) { this->fTextGeneration = f; }
		//>>

		//<< PWH.
		std::string const& getTextString() const { return text; }
		//>>

		bool getRecord(int groupcode, dimeParam& param, int index = 0) const override;
		std::string const& getEntityName() const override { return entityName; }

		//virtual void print() const;
		bool write(dimeOutput& out) override;
		int typeId() const override { return dimeBase::dimeTextType; }
		size_t countRecords() const override;

		GeometryType extractGeometry(
			std::vector<dimeVec3f>& verts,
			std::vector<int>& indices,
			dimeVec3f& extrusionDir,
			dxfdouble& thickness) override;

	protected:
		bool handleRecord(int groupcode, const dimeParam& param) override;

	private:
		dimeVec3f origin{};
		dimeVec3f second{};
		bool haveSecond{};
		dxfdouble height{};
		dxfdouble width{};
		dxfdouble rotation{};
		dxfdouble wScale{};
		int16 hJust{};
		int16 vJust{};
		int16 fTextGeneration{};
		std::string text;

	}; // class dimeText

	//
	// inline methods
	//


	inline void dimeText::setOrigin(const dimeVec3f& o) {
		this->origin = o;
	}

	inline void dimeText::getOrigin(dimeVec3f& o) const {
		o = this->origin;
	}

	inline void dimeText::setSecond(const dimeVec3f& s) {
		this->second = s;
	}

	inline bool dimeText::getSecond(dimeVec3f& s) const {
		if (haveSecond) {
			s = this->second;
			return true;
		}
		else {
			return false;
		}
	}

	inline void dimeText::setHeight(const dxfdouble h) {
		this->height = h;
	}

	inline dxfdouble dimeText::getHeight() const {
		return this->height;
	}

	inline void dimeText::setWidth(const dxfdouble w) {
		this->width = w;
	}

	inline dxfdouble dimeText::getWidth() const {
		return this->width;
	}

	inline void dimeText::setRotation(const dxfdouble a) {
		this->rotation = a;
	}

	inline dxfdouble dimeText::getRotation() const {
		return this->rotation;
	}

	inline void dimeText::setHJust(int16 h) {
		this->hJust = h;
	}

	inline int32 dimeText::getHJust() const {
		return this->hJust;
	}

	inline void dimeText::setVJust(int16 v) {
		this->vJust = v;
	}

	inline int32 dimeText::getVJust() const {
		return this->vJust;
	}

	//inline void 
	//dimeText::setTextString(const char* s)
	//{
	  //this->text = s;
	//}

	//=============================================================================
	// dimeMText
	// PWH.

	struct sMText {	// sMText 이거는.... 여기말고, 공통 헤더.... 어딘가로 옮겨놔야 되는데,....
	public:
		enum eTEXT_ALIGN {
			TA_NONE = 0,			// +---+---+---+
			TA_LT, TA_CT, TA_RT,	// | 1 | 2 | 3 |
			TA_LC, TA_CC, TA_RC,	// | 4 | 5 | 6 |
			TA_LB, TA_CB, TA_RB,	// | 7 | 8 | 9 |
			nTA						// +---+---+---+
		};
		enum eTEXT_DIRECTION {
			TD_NONE = 0,
			TD_L2R = 1, TD_T2B = 3, TD_BY_STYLE = 5,
		};

	public:
		biscuit::sPoint3d	ptOrigin{};
		double				dHeight = 0.0;	// Box Height
		double				dWidth = 0.0;		// Box Width
		eTEXT_ALIGN			eAlign = TA_LT;
		eTEXT_DIRECTION		eDrawingDirection = TD_L2R;
		std::string			strTextStyleName;
		biscuit::sPoint3d	vecXDirection{};
		double				dWidthChar = 0.0;
		double				dHeightChar = 0.0;
		double				dRotationalAngle = 0.0;		// in rad.
		double				dLineSpacingFactor = 1.6/3.;	// line spacing factor, Percentage of default (3-on-5) line spacing to be applied. Valid values range from (0.25 (<==(0.25)/3) to 4.00 (<==4/3) )
		int					eLineSpacingStyle = 1;			// 1 : At least (taller characters will override)
															// 2 : Exact (taller characters will not override)
		int					eBackgroundFillSetting;			// 0 : Background fill off, 1 : Use background fill color, 2 : Use drawing window color as background fill color
		uint32_t			crBackground;
	};

	class dimeMText : public dimeExtrusionEntity, public sMText {
	public:
		std::string	text;

	public:
		static inline std::string const entityName = "MTEXT"s;
		BSC__DEFINE_CTOR_DTOR_DERIVED(dimeMText, dimeExtrusionEntity);
		BSC__DEFINE_CLONE_DERIVED(dimeEntity);

		std::string const& getEntityName() const override { return entityName; }
		bool getRecord(int groupcode, dimeParam& param, int index = 0) const override;

		//virtual void print() const;
		bool write(dimeOutput& out) override;
		int typeId() const override { return dimeBase::dimeMTextType; }
		size_t countRecords() const override;

		GeometryType extractGeometry(
			std::vector<dimeVec3f>& verts,
			std::vector<int>& indices,
			dimeVec3f& extrusionDir,
			dxfdouble& thickness) override;

	protected:
		bool handleRecord(int groupcode, const dimeParam& param) override;
	}; // class dimeText

} // namespace dime

