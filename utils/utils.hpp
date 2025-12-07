#pragma once

#include <format>
#ifndef AOC_2025_UTILS
#define AOC_2025_UTILS

#include <fstream>
#include <ranges>
#include <string_view>
#include <vector>

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

	std::vector<std::string> get_lines() {
		std::vector<std::string> lines;

		std::string line;
		while (std::getline(file_handle, line)) {
			lines.emplace_back(line.begin(), line.end());
		}

		return lines;
	}
};

template <typename T> class Point {
  public:
	T x;
	T y;

	Point<T>() = default;
	Point<T>(T x, T y) : x(x), y(y) {}
	Point<T>(Point<T> &other) : x(other.x), y(other.y) {}

	Point<T>(const Point<T> &other) : x(other.x), y(other.y) {}
	Point<T>(const Point<T> &&other) : x(other.x), y(other.y) {}

	Point<T> &operator=(Point<T> &other) {
		other.x = x;
		other.y = y;

		return other;
	}

	bool operator==(const Point<T> &other) const {
		return x == other.x && y == other.y;
	}
};

template <typename T> struct std::formatter<Point<T>> {
	constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

	auto format(const Point<T> &p, std::format_context &ctx) const {
		return std::format_to(ctx.out(), "({}, {})", p.x, p.y);
	}
};

template <typename T> struct std::hash<Point<T>> {
	size_t operator()(Point<T> const &b) const {
		return std::hash<T>()(b.x) ^ std::hash<T>()(b.y);
	}
};

constexpr auto split_string(std::string_view view, char delimiter) {
	auto split = std::views::split(view, delimiter);
	return std::views::transform(
		split, [](auto &&subrange) -> std::string_view {
			return std::string_view{subrange.begin(), subrange.end()};
		});
}

#endif
