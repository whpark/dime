#pragma once

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


#define BSC__DEFINE_CTOR_DTOR(ClassName) \
public:\
	using this_t = ClassName;\
public:\
	ClassName() = default;\
	ClassName(const ClassName&) = default;\
	ClassName(ClassName&&) = default;\
	ClassName& operator=(const ClassName&) = default;\
	ClassName& operator=(ClassName&&) = default;\
	virtual ~ClassName() = default;

#define BSC__DEFINE_CTOR_DTOR_DERIVED(ClassName, BaseName) \
public:\
	using base_t = BaseName;\
	BSC__DEFINE_CTOR_DTOR(ClassName)

#define BSC__DECLARE_CLONE_PURE(BaseClass) \
	virtual std::unique_ptr<BaseClass> clone() const = 0;

#define BSC__DEFINE_CLONE_DERIVED(BaseClass) \
	std::unique_ptr<BaseClass> clone() const override { return std::make_unique<this_t>(*this); }

#define BSC__DEFINE_CLONE() \
	virtual std::unique_ptr<this_t> clone() const { return std::make_unique<this_t>(*this); }

#define DIME_DLL_API

