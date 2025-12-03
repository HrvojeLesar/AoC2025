#pragma once
#include <fstream>
#include <ranges>
#include <string_view>

#ifndef AOC_2025_UTILS
#define AOC_2025_UTILS

class InputFile {
  public:
	std::ifstream file_handle;

	InputFile() {
#ifdef TEST_INPUT
		file_handle.open("test_input");
#else
		file_handle.open("input");
#endif
	}
	InputFile(const char *filename) { file_handle.open(filename); }

	InputFile(const InputFile &other) = delete;
	InputFile(const InputFile &&other) = delete;

	~InputFile() { file_handle.close(); }
};

constexpr auto split_string(std::string_view view, char delimiter) {
	auto split = std::views::split(view, delimiter);
	return std::views::transform(
		split, [](auto &&subrange) -> std::string_view {
			return std::string_view{subrange.begin(), subrange.end()};
		});
}

#endif
