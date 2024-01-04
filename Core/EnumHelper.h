#pragma once

#include <type_traits>

#define AOE_DECLARE_ENUM_OPERATORS(T) \
    inline constexpr bool operator!(T arg) { \
		return !static_cast<std::underlying_type_t<T>>(arg); \
	} \
    inline constexpr T operator~(T arg) { \
		return static_cast<T>(~static_cast<std::underlying_type_t<T>>(arg)); \
	} \
    inline constexpr T operator|(T lhs, T rhs) { \
		return static_cast<T>(static_cast<std::underlying_type_t<T>>(lhs) | \
							  static_cast<std::underlying_type_t<T>>(rhs)); \
	} \
    inline constexpr T operator&(T lhs, T rhs) { \
		return static_cast<T>(static_cast<std::underlying_type_t<T>>(lhs) & \
							  static_cast<std::underlying_type_t<T>>(rhs)); \
	} \
    inline constexpr T operator^(T lhs, T rhs) { \
		return static_cast<T>(static_cast<std::underlying_type_t<T>>(lhs) ^ \
							  static_cast<std::underlying_type_t<T>>(rhs)); \
	} \
    inline T& operator|=(T& lhs, T rhs) { \
		return lhs = static_cast<T>(static_cast<std::underlying_type_t<T>>(lhs) | \
									static_cast<std::underlying_type_t<T>>(rhs)); \
	} \
    inline T& operator&=(T& lhs, T rhs) { \
		return lhs = static_cast<T>(static_cast<std::underlying_type_t<T>>(lhs) & \
									static_cast<std::underlying_type_t<T>>(rhs)); \
	} \
    inline T& operator^=(T& lhs, T rhs) { \
		return lhs = static_cast<T>(static_cast<std::underlying_type_t<T>>(lhs) ^ \
									static_cast<std::underlying_type_t<T>>(rhs)); \
	}

namespace aoe {

class EnumHelper {
public:
	EnumHelper() = delete;

	template<typename T>
	static bool HasAnyFlags(T value, T flags)
	{
		return (static_cast<std::underlying_type_t<T>>(value) & 
				static_cast<std::underlying_type_t<T>>(flags)) != 0;
	}

	template<typename T>
	static bool HasNoneFlags(T value, T flags)
	{
		return !HasAnyFlags(value, flags);
	}

	template<typename T>
	static bool HasAllFlags(T value, T flags)
	{
		return (static_cast<std::underlying_type_t<T>>(value) &
				static_cast<std::underlying_type_t<T>>(flags)) == flags;
	}

	template<typename T>
	static T AddFlags(T value, T flags)
	{
		return static_cast<T>(static_cast<std::underlying_type_t<T>>(value) | 
							  static_cast<std::underlying_type_t<T>>(flags));
	}
};

} // namespace aoe