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
  \class dimeEntity dime/entities/Entity.h
  \brief The dimeEntity class is the superclass of all \e entity classes.

  If you plan to implement your own entity you should inherit this
  class.  There are some methods you need to implement to create an
  entity, and some steps need to be taken if it's going to work 100%
  with the rest of the library. I recommend looking at the code for
  the dimeArc entity when you read this documentation and before you
  implement your own record. This is a very simple entity. It inherits
  the dimeExtrusionEntity class. The dimeExtrusionEntity class is used
  to store extrusion information for an entity, and you should inherit
  this class if your entity needs all of the following group codes: 39
  (thickness), 210, 220, 230 (extrusion direction vector).

  The getRecord() method should be implemented to enable records to be
  queried using the group code of the record. Usually when you implement
  an entity you store some of the records in class members, and let the
  dimeRecordHolder class store the records you're not interested in.
  So, this method should check if the group code parameter matches
  one of the records stored in one of your class members, and then
  return the value of that member. If the requested group code isn't
  stored by your class, you should return with a call to your parent's
  class getRecord() method, usually dimeEntity::getRecord() or
  dimeExtrusionEntity::getRecord().

  The getEntityName() should simply return the DXF name of the entity.

  The copy() method should make an exact copy of the entity and return
  this. The dimeModel argument to copy() is the model the copied
  entity will be a part of. First you should create a new instance of
  your entity. The model's memory handler should be passed as an
  argument to new(). Then you should call the copyRecords() method to
  copy records stored by dimeRecordHolder (dimeEntity inherits
  dimeRecordHolder). Then you should copy your data members into the
  new instance. If you inherit the dimeExtrusionEntity you should
  class copyExtrusionData() before returning.

  The write() method should write your entity using the dimeOutput
  parameter. Fist you should call the preWrite() method. This will
  take care of writing the entity name, and handle ugly stuff such
  as entity handles and ACAD REACTORS data. Then you should write
  your data members. If you inherit from dimeExtrusionEntity method
  you should call writeExtrusionData(). The last thing to do before
  returning is to call dimeEntity::write() to write records not
  handled by your class.

  The typeId() method should be implemented if your entity is not
  an abstract class. Now you have to edit the include/dime/Base.h,
  and add a unique type name id for your entity. The typeId()
  method should simply return this enum value.

  The countRecords() method is not critical to implement. It should
  return the number of records that will be written by your entity.
  It can be used to create a progress bar while writing a DXF
  file. It is really only useful for _very_ large DXF files. But you
  should implement it since it's not too much work.

  Implement the extractGeometry() method if you feel like it. This
  is just a convenience method so you don't have to do this.

  The handleRecord() should be implemented to support reading entities
  from a file, and to let users set records based on group codes.
  When reading, the handleRecord() method will be called for every
  record found in the entity. If some group code is not supported by
  you, you should call the parent's handleRecord() and it will be
  stored there.  If you need to allocate memory to store the data you
  should check if the dimeMemHandler parameter != NULL and then use it
  to allocate memory.  There is a convenience macro defined in
  include/dime/Basic.h that copies a string, using the memory handler
  if set.

  Well, that's about it I think. Good luck :) Don't hesitate to contact
  us (coin-support@coin3d.org) if you have questions about how to create
  entities.
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

module dime.biscuit:entities.Entity;
import std;
import biscuit;
import :Basic;
import :util;
import :Record;
import :Input;
import :Output;
import :Model;
import :entities.UnknownEntity;
import :entities_3DFace;
import :entities.Polyline;
import :entities.Vertex;
import :entities.Line;
import :entities.Text;
import :entities.Block;
import :entities.Insert;
import :entities.Solid;
import :entities.Trace;
import :entities.Point;
import :entities.Circle;
import :entities.Arc;
import :entities.LWPolyline;
import :entities.Spline;
import :entities.Ellipse;


using namespace std::literals;

namespace dime {

// misc defines
	constexpr int const TMP_BUFFER_LEN = 1024;


	/*!
	  \fn dimeEntity *dimeEntity::copy(dimeModel * const model) const = 0
	  Must be implemented by subclasses to return a copy of the entity.
	  \a model is the model the new entity should belong to.
	*/

	/*!
	  \fn const dimeLayer *dimeEntity::getLayer() const
	  Returns the layer of this entity.
	*/

	/*!
	  \fn const char *dimeEntity::getEntityName() const = 0
	  Must be implemented by subclasses to return the entity name,
	  e.g. POLYLINE, 3DFACE, etc.
	*/

	/*!
	  \fn void dimeEntity::setColorNumber(int16 c)
	  Sets the color number for this entity.
	  Zero indicates the BYBLOCK (floating) color. 256 indicates BYLAYER.
	  A negative value indicates that the layer is turned off.
	*/

	/*!
	  \fn int16 dimeEntity::getColorNumber() const
	  Returns the color number for this entity.

	  \sa dimeEntity::setColorNumber()
	*/

	/*!
	  \fn void dimeEntity::print() const
	*/


	///*!
	//  Returns if this entity is marked as deleted or not.
	//  \sa dimeEntity::setDeleted().
	//*/

	//bool dimeEntity::isDeleted() const {
	//	return this->entityFlags & FLAG_DELETED;
	//}

	///*!
	//  Sets the deleted state of this entity to \a onOff.
	//  \sa dimeEntity::isDeleted().
	//*/

	//void dimeEntity::setDeleted(const bool onOff) {
	//	if (onOff) {
	//		this->entityFlags |= FLAG_DELETED;
	//	}
	//	else {
	//		this->entityFlags &= ~FLAG_DELETED;
	//	}
	//}

	/*!
	  Useful for developers (at least for me :-).
	  \sa dimeEntity::setTagged()
	*/
	bool dimeEntity::isTagged() const {
		return this->entityFlags & FLAG_TAGGED ? true : false;
	}

	/*!
	  Useful for developers.
	  \sa dimeEntity::isTagged()
	*/
	void dimeEntity::setTagged(const bool onOff) {
		if (onOff) {
			this->entityFlags |= FLAG_TAGGED;
		}
		else {
			this->entityFlags &= ~FLAG_TAGGED;
		}
	}



	//!

	bool dimeEntity::write(dimeOutput& file) {
		return base_t::write(file);
	}

	/*!
	  Static function which creates an entity based on its name.
	*/

	std::unique_ptr<dimeEntity> dimeEntity::createEntity(std::string_view name) {
	#ifndef NDEBUG
	  //fprintf(stderr,"Entity: %s\n", name);
	#endif

	  //
	  // TODO: optimize 
	  //
	  // right now, I just check for the most common (for 3D gfx)
	  // entities first.
	  //
	  // Could be quite easily solved by using a hash list with
	  // pointers to static methods that returns a new instance
	  // of the correct type.
	  //

		if (name == "3DFACE"sv)
			return std::make_unique<dime3DFace>();
		if (name == "VERTEX"sv)
			return std::make_unique<dimeVertex>();
		if (name == "POLYLINE"sv)
			return std::make_unique<dimePolyline>();
		if (name == "LINE"sv)
			return std::make_unique<dimeLine>();
		  //<< PWH
		if (name == "MTEXT"sv)
			return std::make_unique<dimeMText>();
		  //>>
		if (name == "TEXT"sv)
			return std::make_unique<dimeText>();
		if (name == "INSERT"sv)
			return std::make_unique<dimeInsert>();
		if (name == "BLOCK"sv)
			return std::make_unique<dimeBlock>();
		if (name == "SOLID"sv)
			return std::make_unique<dimeSolid>();
		if (name == "TRACE"sv)
			return std::make_unique<dimeTrace>();
		if (name == "POINT"sv)
			return std::make_unique<dimePoint>();
		if (name == "CIRCLE"sv)
			return std::make_unique<dimeCircle>();
		if (name == "LWPOLYLINE"sv)
			return std::make_unique<dimeLWPolyline>();
		if (name == "SPLINE"sv)
			return std::make_unique<dimeSpline>();
		if (name == "ELLIPSE"sv)
			return std::make_unique<dimeEllipse>();
		if (name == "ARC"sv)
			return std::make_unique<dimeArc>();
		return std::make_unique<dimeUnknownEntity(name, memhandler)>();
	}

	/*!
	  Static function that reads all entities until an entity of type
	  \a stopat is found. Returns \e true if all entities were read OK.
	  When reading stops, the group code 0 and the entity name (\a stopat)
	  will already have been read.
	*/

	bool dimeEntity::readEntities(dimeInput& file, std::vector<tptr_t<dimeEntity>>& array, std::string_view stopat) {
		//int32 groupcode;
		//const char* string;
		//bool ok = true;
		//dimeEntity* entity = NULL;
		//dimeMemHandler* memhandler = file.getMemHandler();

		while (true) {
			int groupCode{};
			if (!file.readGroupCode(groupcode) or groupCode != 0) {
				std::println("Error reading groupcode: {}", groupcode);
				return false;
			}
			auto string = file.readString();
			if (string == stopat)
				break;
			auto entity = dimeEntity::createEntity(string);
			if (!entity) {
				std::println("error creating entity: {}", string);
				return false;
			}
			if (!entity->read(file)) {
				std::println("error reading entity: {}.", string);
				return false;
			}
			array.push_back(std::move(entity));
		}
		return ok;
	}

	///*!
	//  Static function which copies all non-deleted entities from
	//  \a array of length \a nument into a
	//  new array. Will return the number of copied entities in
	//  \a nument. This function returns \e NULL either when out of
	//  memory, or if there was no non-deleted entities.
	//*/

	//dimeEntity**
	//	dimeEntity::copyEntityArray(const dimeEntity* const* const array,
	//		int& nument,
	//		dimeModel* const model) {
	//	int i;
	//	int num = nument;
	//	dimeMemHandler* memh = model->getMemHandler();

	//	nument = 0;
	//	for (i = 0; i < num; i++) {
	//		if (!array[i]->isDeleted()) nument++;
	//	}
	//	if (nument == 0) return NULL;

	//	dimeEntity** newarr = ARRAY_NEW(memh, dimeEntity*, nument);

	//	bool ok = newarr != NULL;
	//	if (ok) {
	//		size_t cnt = 0;
	//		for (i = 0; i < num && ok; i++) {
	//			if (!array[i]->isDeleted()) {
	//				newarr[cnt] = array[i]->copy(model);
	//				if (newarr[cnt] == NULL) {
	//		//          sim_trace("failed to copy entity: %d\n", array[i]->typeId());
	//				}
	//				ok = newarr[cnt++] != NULL;
	//			}
	//		}
	//		if (!ok && !memh) { // free memory
	//			for (i = 0; i < cnt; i++)
	//				delete newarr[i];
	//			delete[] newarr;
	//		}
	//	}
	//	return ok ? newarr : (dimeEntity**)NULL;
	//}

	/*!
	  Static function which copies all non-deleted entities from
	  \a array of length \a nument into \a destarray.
	*/

	//bool
	//	dimeEntity::copyEntityArray(const dimeEntity* const* const array,
	//		int nument,
	//		dimeModel* const model,
	//		std::vector<dimeEntity*>& destarray) {
	//	int i;
	//  //  dimeMemHandler *memh = model->getMemHandler();

	//	int num = 0;
	//	for (i = 0; i < nument; i++) {
	//		if (!array[i]->isDeleted()) num++;
	//	}
	//	if (num == 0) {
	//		destarray.setCount(0);
	//		return true;
	//	}

	//	destarray.makeEmpty(num);

	//	for (i = 0; i < nument; i++) {
	//		if (!array[i]->isDeleted()) {
	//			dimeEntity* entity = array[i]->copy(model);
	//			if (entity == NULL) {
	//				destarray.setCount(0);
	//				return false;
	//			}
	//			destarray.append(entity);
	//		}
	//	}
	//	return true;
	//}




/*!
  Will return an "arbitrary" axis, based on \a givenaxis. Based on code
  from DXF Parser R10, from Autodesk Inc. Is used to find the X-axis
  bases on the given Z-axis.

  \sa dimeEntity::generateUCS()
*/

	void dimeEntity::arbitraryAxis(dimeVec3f const& givenaxis, dimeVec3f& newaxis) {
		constexpr double const ARBBOUND = 0.015625;  /* aka "1/64" */

		dimeVec3f yaxis(0.0, 1.0, 0.0);
		dimeVec3f zaxis(0.0, 0.0, 1.0);

		if (std::abs(givenaxis[0]) < ARBBOUND &&
			std::abs(givenaxis[1]) < ARBBOUND)
			newaxis = yaxis.Cross(givenaxis);
		else
			newaxis = zaxis.Cross(givenaxis);

		newaxis.Normalize();
	}

	/*!
	  Will generate a coordinate system, based on \a givenaxis,
	  which will be the Z-axis in the new coordinate system.

	  \sa dimeEntity::arbitraryAxis()
	*/

	void dimeEntity::generateUCS(dimeVec3f const& givenaxis, dimeMatrix& m) {
		dimeVec3f newaxis;
		dimeEntity::arbitraryAxis(givenaxis, newaxis);
		newaxis.Normalize();
		dimeVec3f yaxis = givenaxis.Cross(newaxis);
		yaxis.Normalize();
		//m.setRotate(newaxis, yaxis, givenaxis);
		m.matrix() << newaxis.x, yaxis.x, givenaxis.x, 0.0,
			newaxis.y, yaxis.y, givenaxis.y, 0.0,
			newaxis.z, yaxis.z, givenaxis.z, 0.0,
			0.0, 0.0, 0.0, 1.0;
	}

	//!

	size_t dimeEntity::countRecords() const {
		size_t cnt = 0;
		if (this->layer) cnt++;
		if (this->colorNumber != 256) cnt++;
		return cnt + base_t::countRecords();
	}

	/*!
	  The traversal function used when dimeModel::traverseEntities()
	  is called. Most entities use this default method, but some
	  entities (INSERT, BUILD) will need to overload it.
	*/

	bool dimeEntity::traverse(const dimeState* state, callbackEntity_t callback) {
		if (this->isDeleted())
			return true;
		return callback(state, this);
	}

	void dimeEntity::fixReference(dimeModel* model) {
	}

	/*!

	  A special convenience function, included for your pleasure.  Enables
	  the user to ignore the type of entity, and just call this method
	  when extracting geometry.  Very useful for 3D viewers that need DXF
	  support. Check out the dxf2vrml directory for an example on how to
	  convert a DXF file to VRML.

	  Should be overloaded by all subclasses that have geometry, default
	  function returns no geometry. Don't forget to transform vertices by
	  the current transformation matrix if used in a callback from
	  dimeEntity::traverse().

	  If there are coordinates, but no indices, this means running
	  indices for the entire vertex array.

	  Different faces and/or line segments are separated by a \e -1 in
	  the indices array, just as in VRML files.

	  If \a thickness != 0.0, the data should, before transformation,
	  be extruded by that length along the extrusion direction. Hence,
	  a point becomes a line, a line becomes a quad, and a polygon
	  becomes an object with a volume.

	  If \a thickness == 0.0 and \a extrusionDir != (0,0,1) all the
	  vertices should be transformed by a matrix that can be created
	  using dimeEntity::generateUCS(). If you are using
	  dimeModel::traverseEntities() to extract the geometry, simply
	  right-multiply the UCS matrix with the matrix found in dimeState
	  to get the correct transformation.

	*/

	dimeEntity::GeometryType dimeEntity::extractGeometry(std::vector<dimeVec3f>& verts,
			std::vector<int>& indices,
			dimeVec3f& extrusionDir,
			dxfdouble& thickness)
	{
		extrusionDir = dimeVec3f(0, 0, 1);
		verts.clear();
		indices.clear();
		thickness = 0.0f;
		return NONE;
	}

	//!

	bool dimeEntity::isOfType(int thetypeid) const {
		return thetypeid == dimeEntityType ||
			dimeRecordHolder::isOfType(thetypeid);
	}

	/*!
	  Reads an entity from \a in. Can be overloaded by subclasses, but in most
	  cases this will not be necessary.

	  \sa dimeEntity::handleRecord().
	*/

	bool dimeEntity::read(dimeInput& file) {
		if (!dimeRecordHolder::read(file))
			return false;
		if (!tempLayerName.empty()) {
			this->layer = dimeLayer::getDefaultLayer();
		}
		else {
			this->layer = file.getModel()->addLayer(std::move(tempLayerName));
			tempLayerName.clear();	// for sure deleted. ('std::move' may not clear the source string because short string optimization)
		}
		return ok;
	}

	/*!
	  Sets the layer for this entity. This will change the record with
	  group code 8. If \a layer equals \e NULL, the layer will be set
	  to the default layer.
	*/

	void dimeEntity::setLayer(dimeLayer const* layer) {
		this->layer = layer ? layer : dimeLayer::getDefaultLayer();
	}

	//!

	bool dimeEntity::handleRecord(int groupcode, const dimeParam& param) {
		if (groupcode == 8) {
			// Should only arrive her during read(). this->layer is then used
			// as a temporary storage for the character string.
			// Checking flag just to be safe...
			tempLayerName = std::get<std::string>(param);
			return true;
		}
		else if (groupcode == 62) {
			this->entityFlags |= FLAG_COLOR_NUMBER;
			this->colorNumber = std::get<int16>(param);
			return true;
		}
		else if (groupcode == 67) {
			this->entityFlags |= FLAG_PAPERSPACE;
			return true;
		}
		else if (groupcode == 100) {
			this->entityFlags |= FLAG_SUBCLASS_MARKER;
			if (std::get<std::string>(param) != "AcDbEntity") {
				return false; // store in record-holder
			}
			return true;
		}
		else if (groupcode == 102) {
			// ignore these, don't store
			return true;
		}
		else if (groupcode == 330) {
			this->entityFlags |= FLAG_ACAD_REACTORS;
			return false; // store in recordholder
		}
		else if (groupcode == 360) {
			this->entityFlags |= FLAG_ACAD_XDICTIONARY;
			return false; // store in recordholder
		}
		else if (groupcode == 5) {
			this->entityFlags |= FLAG_HANDLE;
			return false; // store in recordholder
		}
		else if (groupcode == 6) {
			this->entityFlags |= FLAG_LINETYPE;
			return false; // store in recordholder
		}
		return false;
	}

	//!

	bool dimeEntity::getRecord(int groupcode, dimeParam& param, int index) const {
		switch (groupcode) {
		case 8:
			param.emplace<std::string>(this->layer->getLayerName());
			return true;
		case 62:
			param.emplace<int16>(this->colorNumber);
			return true;
		}
		return dimeRecordHolder::getRecord(groupcode, param, index);
	}

	//
	// FIXME: getRecord
	//

	/*!
	  Writes the group code 0, layer name, and some other stuff.
	*/
	bool dimeEntity::preWrite(dimeOutput& file) {
		file.writeGroupCode(0);
		bool ret = file.writeString(this->getEntityName());

		// write stupid handle?
		if (this->entityFlags & FLAG_HANDLE) {
			dimeParam param;
			if (this->getRecord(5, param)) {
				file.writeGroupCode(5);
				file.writeString(std::get<std::string>(param));
			}
		}

	#if 1
		if (this->entityFlags & FLAG_ACAD_REACTORS) {
			file.writeGroupCode(102);
			file.writeString("{ACAD_REACTORS");
			dimeParam param;
			if (this->getRecord(330, param)) {
				file.writeGroupCode(330);
				file.writeString(std::get<std::string>(param));
			}
			file.writeGroupCode(102);
			file.writeString("}"sv);
		}
		if (this->entityFlags & FLAG_ACAD_XDICTIONARY) {
			file.writeGroupCode(102);
			file.writeString("{ACAD_XDICTIONARY");
			dimeParam param;
			if (this->getRecord(360, param)) {
				file.writeGroupCode(360);
				file.writeString(std::get<std::string>(param));
			}
			file.writeGroupCode(102);
			file.writeString("}");
		}
	#endif

	  // write stupid subclass marker data?
		if (this->entityFlags & FLAG_SUBCLASS_MARKER) {
			file.writeGroupCode(100);
			file.writeString("AcDbEntity");
		}
		if (this->entityFlags & FLAG_PAPERSPACE) {
			file.writeGroupCode(67);
			file.writeInt16(1);
		}

		// write layer
		file.writeGroupCode(8);
		ret = file.writeString(this->layer->getLayerName());

		// write linetype
		if (this->entityFlags & FLAG_LINETYPE) {
			dimeParam param;
			if (this->getRecord(6, param)) {
				file.writeGroupCode(6);
				file.writeString(std::get<std::string>(param));
			}
		}

		// write color number
		if (this->colorNumber != 256) {
			file.writeGroupCode(62);
			ret = file.writeInt16(this->colorNumber);
		}

	#if 1
		if ((this->entityFlags & FLAG_SUBCLASS_MARKER)) {
			dimeParam param;
			int idx = 0;
			while (this->getRecord(100, param, idx)) {
				file.writeGroupCode(100);
				file.writeString(std::get<std::string>(param));
				idx++;
			}
		}
	#endif
		return ret;
	}

	//!
	bool dimeEntity::shouldWriteRecord(int groupcode) const {
		switch (groupcode) {
		case 5:
		case 6:
		case 100:
		case 330:
		case 360:
			return false;
		default:
			return true;
		}
	}



} // namespace dime
