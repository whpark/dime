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

#define BSC__DEFINE_R5(ClassName, BaseName) \
	using this_t = ClassName;\
	using base_t = BaseName;\
	ClassName() = default;\
	ClassName(const ClassName&) = default;\
	ClassName(ClassName&&) = default;\
	ClassName& operator=(const ClassName&) = default;\
	ClassName& operator=(ClassName&&) = default;\
	virtual ~ClassName() = default;

#define BSC__DEFINE_CLONE(baseClass) \
	std::unique_ptr<baseClass> clone() const override { return std::make_unique<this_t>(*this); }
