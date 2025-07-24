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

export module dime.gtl:Base;
import std;
import :Basic;
import :util;

export namespace dime {

	class dimeBase {
	public:
		enum eDIME {
			dimeBaseType = 1,
			dimeRecordType,
			dimeStringRecordType,
			dimeFloatRecordType,
			dimeDoubleRecordType,
			dimeInt8RecordType,
			dimeInt16RecordType,
			dimeInt32RecordType,
			dimeHexRecordType,
			dimeRecordHolderType,
			dimeClassType,
			dimeUnknownClassType,
			dimeObjectType,
			dimeUnknownObjectType,
			dimeEntityType,
			dimeUnknownEntityType,
			dimePolylineType,
			dimeVertexType,
			dimeFaceEntityType,
			dimeExtrusionEntityType,
			dime3DFaceType,
			dimeSolidType,
			dimeTraceType,
			dimeLineType,
			dimeMTextType,	// PWH.
			dimeTextType,
			dimePointType,
			dimeBlockType,
			dimeInsertType,
			dimeCircleType,
			dimeArcType,
			dimeLWPolylineType,
			dimeEllipseType,
			dimeSplineType,
			dimeSectionType,
			dimeUnknownSectionType,
			dimeEntitiesSectionType,
			dimeBlocksSectionType,
			dimeTablesSectionType,
			dimeHeaderSectionType,
			dimeClassesSectionType,
			dimeObjectsSectionType,
			dimeTableType,
			dimeTableEntryType,
			dimeUnknownTableType,
			dimeUCSTableType,
			dimeLayerTableType,

			// this should be last
			dimeLastTypeTag
		};


	public:
		dimeBase() {}
		dimeBase(dimeBase const&) = default;
		dimeBase(dimeBase&&) = default;
		dimeBase& operator=(dimeBase const&) = default;
		dimeBase& operator=(dimeBase&&) = default;
		virtual ~dimeBase() = default;

		//virtual std::unique_ptr<dimeBase> NewClone() const = 0;
		virtual int typeId() const = 0;
		/*!
			Returns \e true if the object is of type \a typeid or is inherited 
			from it. Function in base class checks whether \a thetypeid 
			equals the virtual dimeBase::typeId() value or equals \e dimeBaseType.
			Must be implemented by all subclasses that are superclasses of other
			classes, and should check if \a thetypeid equals its typeId,
			and then call its parent's isOfType function.  Leaf-classes
			do not have to implement this method.
		*/
		virtual bool isOfType(int thetypeid) const {
			return this->typeId() == thetypeid || thetypeid == dimeBaseType;
		}
	public:
		//void* operator new(size_t size, dimeMemHandler* memhandler = nullptr, int alignment = 4) {
		//	if (memhandler)
		//		return memhandler->allocMem(size, alignment);
		//	else return ::operator new(size);
		//}
		//void operator delete(void* ptr) {
		//	// will only get here if we don't use a memory handler
		//	::operator delete(ptr);
		//}

	}; // class dimeBase

}	// namespace dime

