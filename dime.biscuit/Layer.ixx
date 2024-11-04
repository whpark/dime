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

#include "Basic.h"

export module dime.biscuit:Layer;
import std;
import biscuit;
import :Basic;
import :Base;

using namespace std::literals;

export namespace dime {

	class dimeLayer final {
	public:
		using this_t = dimeLayer;
	public:
		static inline std::string const defaultName{"0"};

		enum Flags {
			FROZEN               = 0x1,
			FROZEN_NEW_VIEWPORTS = 0x2,
			LOCKED               = 0x4
		};

		//dimeLayer() = default;
		dimeLayer(dimeLayer const&) = default;
		dimeLayer(dimeLayer&&) = default;
		dimeLayer& operator = (dimeLayer const&) = default;
		dimeLayer& operator = (dimeLayer&&) = default;
		~dimeLayer() = default;

		std::unique_ptr<dimeLayer> clone() const { return std::make_unique<this_t>(*this); };


		std::string const& getLayerName() const { return layerName; }
		//inline wchar_t const* getLayerNameW() const { return layerNameW.c_str(); }	// PWH.
		int getLayerNum() const;

		int16 getColorNumber() const;
		void setColorNumber(int16 num);

		int16 getFlags() const;
		void setFlags(int16& flags);

		bool isDefaultLayer() const;

		static dimeLayer const* getDefaultLayer();

		static void colorToRGB(int colornum,
			dxfdouble& r, dxfdouble& g, dxfdouble& b);

	private:
		friend class dimeModel;

		dimeLayer();
		dimeLayer(std::string_view name, int num, int16 colnum, int16 flags)
			: layerName( name ), layerNum( num ), colorNum( colnum ), flags( flags )
		{}

		std::string layerName;
		int layerNum{-1};
		int16 colorNum{-1};
		int16 flags{};

	}; // class dimeLayer

	inline int dimeLayer::getLayerNum() const {
		return layerNum;
	}

	inline int16 dimeLayer::getColorNumber() const {
		return colorNum;
	}

	inline void dimeLayer::setColorNumber(int16 num) {
		this->colorNum = num;
	}

	inline int16 dimeLayer::getFlags() const {
		return this->flags;
	}

	inline void dimeLayer::setFlags(int16& flags) {
		this->flags = flags;
	}

	inline bool dimeLayer::isDefaultLayer() const {
		return this == dimeLayer::getDefaultLayer();
	}

}   // namespace dime

namespace dime {

	// palette for color indices 1-255
	static dxfdouble colortable[256*3] = {
		1,0,0, // 1
		1,1,0,
		0,1,0,
		0,1,1,
		0,0,1,
		1,0,1,
		1,1,1,          
		0.5,0.5,0.5,  //0.5,0.5,0.5,     
		0.75,0.75,0.75,  //0.75,0.75,0.75, 
		1,0,0,              
		1,0.5,0.5,
		0.65,0,0,
		0.65,0.325,0.325,
		0.5,0,0,
		0.5,0.25,0.25,
		0.3,0,0,
		0.3,0.15,0.15,
		0.15,0,0,
		0.15,0.075,0.075,
		1,0.25,0,   // 20        
		1,0.625,0.5,
		0.65,0.1625,0,
		0.65,0.4063,0.325,
		0.5,0.125,0,
		0.5,0.3125,0.25,
		0.3,0.075,0,
		0.3,0.1875,0.15,
		0.15,0.0375,0,
		0.15,0.0938,0.075,
		1,0.5,0,
		1,0.75,0.5,
		0.65,0.325,0,
		0.65,0.4875,0.325,
		0.5,0.25,0,
		0.5,0.375,0.25,
		0.3,0.15,0,
		0.3,0.225,0.15,
		0.15,0.075,0,
		0.15,0.1125,0.075,
		1,0.75,0,
		1,0.875,0.5,
		0.65,0.4875,0,
		0.65,0.5688,0.325,
		0.5,0.375,0,
		0.5,0.4375,0.25,
		0.3,0.225,0,
		0.3,0.2625,0.15,
		0.15,0.1125,0,
		0.15,0.1313,0.075,
		1,1,0,
		1,1,0.5,
		0.65,0.65,0,
		0.65,0.65,0.325,
		0.5,0.5,0,
		0.5,0.5,0.25,
		0.3,0.3,0,
		0.3,0.3,0.15,
		0.15,0.15,0,
		0.15,0.15,0.075,
		0.75,1,0,
		0.875,1,0.5,
		0.4875,0.65,0,
		0.5688,0.65,0.325,
		0.375,0.5,0,
		0.4375,0.5,0.25,
		0.225,0.3,0,
		0.2625,0.3,0.15,
		0.1125,0.15,0,
		0.1313,0.15,0.075,
		0.5,1,0,
		0.75,1,0.5,
		0.325,0.65,0,
		0.4875,0.65,0.325,
		0.25,0.5,0,
		0.375,0.5,0.25,
		0.15,0.3,0,
		0.225,0.3,0.15,
		0.075,0.15,0,
		0.1125,0.15,0.075,
		0.25,1,0,
		0.625,1,0.5,
		0.1625,0.65,0,
		0.4063,0.65,0.325,
		0.125,0.5,0,
		0.3125,0.5,0.25,
		0.075,0.3,0,
		0.1875,0.3,0.15,
		0.0375,0.15,0,
		0.0938,0.15,0.075,
		0,1,0,
		0.5,1,0.5,
		0,0.65,0,
		0.325,0.65,0.325,
		0,0.5,0,
		0.25,0.5,0.25,
		0,0.3,0,
		0.15,0.3,0.15,
		0,0.15,0,
		0.075,0.15,0.075,
		0,1,0.25,
		0.5,1,0.625,
		0,0.65,0.1625,
		0.325,0.65,0.4063,
		0,0.5,0.125,
		0.25,0.5,0.3125,
		0,0.3,0.075,
		0.15,0.3,0.1875,
		0,0.15,0.0375,
		0.075,0.15,0.0938,
		0,1,0.5,
		0.5,1,0.75,
		0,0.65,0.325,
		0.325,0.65,0.4875,
		0,0.5,0.25,
		0.25,0.5,0.375,
		0,0.3,0.15,
		0.15,0.3,0.225,
		0,0.15,0.075,
		0.075,0.15,0.1125,
		0,1,0.75,
		0.5,1,0.875,
		0,0.65,0.4875,
		0.325,0.65,0.5688,
		0,0.5,0.375,
		0.25,0.5,0.4375,
		0,0.3,0.225,
		0.15,0.3,0.2625,
		0,0.15,0.1125,
		0.075,0.15,0.1313,
		0,1,1,
		0.5,1,1,
		0,0.65,0.65,
		0.325,0.65,0.65,
		0,0.5,0.5,
		0.25,0.5,0.5,
		0,0.3,0.3,
		0.15,0.3,0.3,
		0,0.15,0.15,
		0.075,0.15,0.15,
		0,0.75,1,
		0.5,0.875,1,
		0,0.4875,0.65,
		0.325,0.5688,0.65,
		0,0.375,0.5,
		0.25,0.4375,0.5,
		0,0.225,0.3,
		0.15,0.2625,0.3,
		0,0.1125,0.15,
		0.075,0.1313,0.15,
		0,0.5,1,
		0.5,0.75,1,
		0,0.325,0.65,
		0.325,0.4875,0.65,
		0,0.25,0.5,
		0.25,0.375,0.5,
		0,0.15,0.3,
		0.15,0.225,0.3,
		0,0.075,0.15,
		0.075,0.1125,0.15,
		0,0.25,1,
		0.5,0.625,1,
		0,0.1625,0.65,
		0.325,0.4063,0.65,
		0,0.125,0.5,
		0.25,0.3125,0.5,
		0,0.075,0.3,
		0.15,0.1875,0.3,
		0,0.0375,0.15,
		0.075,0.0938,0.15,
		0,0,1,
		0.5,0.5,1,
		0,0,0.65,
		0.325,0.325,0.65,
		0,0,0.5,
		0.25,0.25,0.5,
		0,0,0.3,
		0.15,0.15,0.3,
		0,0,0.15,
		0.075,0.075,0.15,
		0.25,0,1,
		0.625,0.5,1,
		0.1625,0,0.65,
		0.4063,0.325,0.65,
		0.125,0,0.5,
		0.3125,0.25,0.5,
		0.075,0,0.3,
		0.1875,0.15,0.3,
		0.0375,0,0.15,
		0.0938,0.075,0.15,
		0.5,0,1,
		0.75,0.5,1,
		0.325,0,0.65,
		0.4875,0.325,0.65,
		0.25,0,0.5,
		0.375,0.25,0.5,
		0.15,0,0.3,
		0.225,0.15,0.3,
		0.075,0,0.15,
		0.1125,0.075,0.15,
		0.75,0,1,
		0.875,0.5,1,
		0.4875,0,0.65,
		0.5688,0.325,0.65,
		0.375,0,0.5,
		0.4375,0.25,0.5,
		0.225,0,0.3,
		0.2625,0.15,0.3,
		0.1125,0,0.15,
		0.1313,0.075,0.15,
		1,0,1,
		1,0.5,1,
		0.65,0,0.65,
		0.65,0.325,0.65,
		0.5,0,0.5,
		0.5,0.25,0.5,
		0.3,0,0.3,
		0.3,0.15,0.3,
		0.15,0,0.15,
		0.15,0.075,0.15,
		1,0,0.75,
		1,0.5,0.875,
		0.65,0,0.4875,
		0.65,0.325,0.5688,
		0.5,0,0.375,
		0.5,0.25,0.4375,
		0.3,0,0.225,
		0.3,0.15,0.2625,
		0.15,0,0.1125,
		0.15,0.075,0.1313,
		1,0,0.5,
		1,0.5,0.75,
		0.65,0,0.325,
		0.65,0.325,0.4875,
		0.5,0,0.25,
		0.5,0.25,0.375,
		0.3,0,0.15,
		0.3,0.15,0.225,
		0.15,0,0.075,
		0.15,0.075,0.1125,
		1,0,0.25,
		1,0.5,0.625,
		0.65,0,0.1625,
		0.65,0.325,0.4063,
		0.5,0,0.125,
		0.5,0.25,0.3125,
		0.3,0,0.075,
		0.3,0.15,0.1875,
		0.15,0,0.0375,
		0.15,0.075,0.0938,
		0.33,0.33,0.33,
		0.464,0.464,0.464,
		0.598,0.598,0.598,
		0.732,0.732,0.732,
		0.866,0.866,0.866,
		1,1,1
	};

	/*!
	\fn static void colorToRGB(int colornum, 
	float &r, float &g, float &b)
	Returns the RGB values based on the color index. Legal color 
	numbers range from 1 through 255.
	*/

	void dimeLayer::colorToRGB(int colornum, 
			dxfdouble &r, dxfdouble &g, dxfdouble &b)
	{
		int idx = 7*3; // default white
		if (colornum >= 1 && colornum <= 255) idx = colornum*3;
		idx -= 3;
		r = colortable[idx];
		g = colortable[idx+1];
		b = colortable[idx+2];
	}

	dimeLayer const* dimeLayer::getDefaultLayer() {
		static std::unique_ptr<dimeLayer> const defaultLayer = [] {
			dimeLayer* layer = new dimeLayer();
			layer->layerName = defaultName;
			layer->layerNum = 0;
			layer->colorNum = 7; // white...
			return std::unique_ptr<dimeLayer>(layer);
		}();
		return defaultLayer.get();
	}

} // namespace dime

