#pragma once

#define AOE_NON_COPYABLE_CLASS(ClassName)           \
	ClassName(const ClassName&) = delete;           \
	ClassName& operator=(const ClassName&) = delete;

#define AOE_NON_MOVABLE_CLASS(ClassName)        \
	ClassName(ClassName&&) = delete;            \
	ClassName& operator=(ClassName&&) = delete;

#define AOE_NON_COPYABLE_AND_NON_MOVABLE_CLASS(ClassName) \
	AOE_NON_COPYABLE_CLASS(ClassName)                     \
	AOE_NON_MOVABLE_CLASS(ClassName)