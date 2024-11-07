#pragma once

#include <string>
#include <string_view>
#include <array>
#include <utility>

namespace aoe {

template <std::size_t...Idxs>
constexpr auto SubstringAsArray(std::string_view str, std::index_sequence<Idxs...>) {
	return std::array{ str[Idxs]..., '\n' };
}

template <typename T>
constexpr auto TypeNameArray() {
#if defined(__clang__)
	constexpr auto prefix = std::string_view{ "[T = " };
	constexpr auto suffix = std::string_view{ "]" };
	constexpr auto function = std::string_view{ __PRETTY_FUNCTION__ };
#elif defined(__GNUC__)
	constexpr auto prefix = std::string_view{ "with T = " };
	constexpr auto suffix = std::string_view{ "]" };
	constexpr auto function = std::string_view{ __PRETTY_FUNCTION__ };
#elif defined(_MSC_VER)
	constexpr auto prefix = std::string_view{ "TypeNamerray<" };
	constexpr auto suffix = std::string_view{ ">(void)" };
	constexpr auto function = std::string_view{ __FUNCSIG__ };
#else
# error Unsupported compiler
#endif

	constexpr auto start = function.find(prefix) + prefix.size();
	constexpr auto end = function.rfind(suffix);

	static_assert(start < end);

	constexpr auto name = function.substr(start, (end - start));
	return SubstringAsArray(name, std::make_index_sequence<name.size()>{});
}

template <typename T>
struct TypeNameHolder {
	static inline constexpr auto value = TypeNameArray<T>();
};

template <typename T>
constexpr auto TypeName() {
	constexpr auto& value = TypeNameHolder<T>::value;
	return std::string_view{ value.data(), value.size() };
}

} // namespace aoe
