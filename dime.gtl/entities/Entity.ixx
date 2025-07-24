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

#include "gtl/gtl.h"

export module dime.gtl:entities.Entity;
import std;
import :Basic;
import :util;
import :Base;
import :RecordHolder;
import :Layer;
//import :Model;

using namespace std::literals;

namespace dime {
}

export namespace dime {


	// misc flag values used in entityFlags.
	constexpr int const FLAG_DELETED          = 0x0001; // used by dimeEntity
	//constexpr int const FLAG_TMP_BUFFER_SET   = 0x0002; // see dimeEntity::read()
	constexpr int const FLAG_VERTICES_FOLLOW  = 0x0004; // used by dimePolyline
	constexpr int const FLAG_TAGGED           = 0x0008; // used by dimeEntity
	constexpr int const FLAG_COLOR_NUMBER     = 0x0010; // signals a color number was read 
	constexpr int const FLAG_SUBCLASS_MARKER  = 0x0020; // will subclass marker need to be written
	constexpr int const FLAG_HANDLE           = 0x0040; // entity has handle in RecordHolder
	constexpr int const FLAG_ACAD_REACTORS    = 0x0080; // ACAD reactors in entity
	constexpr int const FLAG_ACAD_XDICTIONARY = 0x0100; // ACAD xdictionary in entity
	constexpr int const FLAG_PAPERSPACE       = 0x0200; // entity is in paperspace
	constexpr int const FLAG_LINETYPE         = 0x0400; // linetype specified in entity
	constexpr int const FLAG_FIRST_FREE       = 0x0800; // use this if you want to define your own flags


	class dimeEntity : public dimeRecordHolder {
		friend class dimeEntitiesSection;
		friend class dimeModel;
		friend class dimePolyline;
		friend class dimeBlock;
		friend class dimeInsert;

	public:
		using base_t = dimeRecordHolder;
		using this_t = dimeEntity;
	public:
		dimeEntity();
		dimeEntity(dimeEntity const&) = default;
		dimeEntity(dimeEntity&&) = default;
		dimeEntity& operator=(dimeEntity const&) = default;
		dimeEntity& operator=(dimeEntity&&) = default;
		virtual ~dimeEntity();

		virtual std::unique_ptr<dimeEntity> NewClone() const = 0;

		int16 getEntityFlags() const { return this->entityFlags; }
		void setEntityFlags(int16 flags) { this->entityFlags = flags; }

		int16 getColorNumber() const { return this->colorNumber; }
		void setColorNumber(int16 c) { this->colorNumber = c; }

		virtual void setLayer(dimeLayer const* layer);
		virtual std::string const& getEntityName() const = 0;

		const dimeLayer* getLayer() const { return layer; }
		std::string const& getLayerName() const {
			static std::string empty;
			return layer ? layer->getLayerName() : empty;
		}

		bool read(dimeInput& in) override;
		bool write(dimeOutput& out) override;
		virtual bool isOfType(int thetypeid) const;
		size_t countRecords() const override;
		virtual void print() const {}


		//bool isDeleted() const;
		//void setDeleted(const bool onOff = true);

		bool isTagged() const;
		void setTagged(const bool onOff = true);

		virtual bool getRecord(int groupcode, dimeParam& param, int index = 0) const;

		enum GeometryType {
			NONE,
			POLYGONS,
			LINES,
			POINTS
		};

		virtual GeometryType extractGeometry(std::vector<dimeVec3f>& verts,
			std::vector<int>& indices,
			dimeVec3f& extrusionDir,
			dxfdouble& thickness);
	protected:

		bool preWrite(dimeOutput& file);

		virtual bool traverse(dimeState const* state, callbackEntity_t callback);
		//virtual void fixReferences(dimeModel* model);
		virtual bool handleRecord(int groupcode, const dimeParam& param);
		virtual bool shouldWriteRecord(int groupcode) const;

	public:
		static std::unique_ptr<dimeEntity> createEntity(std::string_view name);
		static bool readEntities(dimeInput& file, std::vector<tptr_t<dimeEntity>>& array, std::string_view stopat);

		//static bool copyEntityArray(const dimeEntity* const* const array,
		//	int nument,
		//	dimeModel* const model,
		//	std::vector<dimeEntity*>& destarray);
		//static dimeEntity** copyEntityArray(const dimeEntity* const* const array,
		//	int& nument,
		//	dimeModel* const model);

		static void arbitraryAxis(dimeVec3f const& givenaxis, dimeVec3f& newaxis);
		static void generateUCS(dimeVec3f const& givenaxis, dimeMatrix& ct);

	protected:
		bool copyRecords(dimeEntity* const entity, dimeModel* const model) const;

	private:
		dimeLayer const* layer {};
		mutable std::string tempLayerName;
		int16 entityFlags{};
		int16 colorNumber{};
	}; // class dimeEntity


} // namespace dime

