#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

namespace aoe {

enum class ExtensionOption {
	kWithDot,
	kWithoutDot,
};

class FileHelper {
public:
	FileHelper() = delete;

	static std::string GetExtension(const std::wstring& path, ExtensionOption option = ExtensionOption::kWithDot) {
		std::filesystem::path file_path(path);
		std::string extension = file_path.extension().generic_string();

		if (option == ExtensionOption::kWithoutDot && !extension.empty()) {
			extension.erase(0, 1);
		}

		return extension;
	}

	static std::vector<char> ReadAllFile(const std::wstring& path) {
		std::ifstream file_stream(path, std::ios::binary | std::ios::ate);
		std::streamsize size = file_stream.tellg();
		file_stream.seekg(0, std::ios::beg);

		std::vector<char> buffer(size);
		file_stream.read(buffer.data(), size);
		
		if (!file_stream) {
			throw std::runtime_error("Failed to read all file.");
		}

		return buffer;
	}
};

} // namespace aoe