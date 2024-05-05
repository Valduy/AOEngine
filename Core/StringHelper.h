#pragma once

#include <locale>
#include <codecvt>
#include <string>

namespace aoe {

class StringHelper {
public:
	StringHelper() = delete;

	static std::wstring ToUTF16String(std::string utf8_string) {
		// TODO: more careful solution?
		return std::wstring(utf8_string.begin(), utf8_string.end());
	}
};

} // namespace aoe