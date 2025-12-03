#include "../utils/utils.hpp"
#include <cassert>
#include <cstdint>
#include <print>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

class Range {
	std::string from;
	std::string to;

	int64_t i_from;
	int64_t i_to;

  public:
	Range(std::string from, std::string to) : from(from), to(to) {
		std::from_chars(from.data(), from.data() + from.size(), i_from);
		std::from_chars(to.data(), to.data() + to.size(), i_to);
	}

	void print() const { std::println("{}-{}", from, to); }

	int64_t count_invalids() const {
		int64_t invalid_count = 0;
		for (int64_t current_id_numeric = i_from; current_id_numeric <= i_to;
			 current_id_numeric++) {
			int32_t view_size{1};
			auto current_id = std::to_string(current_id_numeric);
			auto lenght = current_id.size();
			auto half_length = lenght / 2;
			if (lenght % 2 == 0) {
				auto first_part = std::string_view(
					current_id.begin(), current_id.begin() + half_length);
				auto second_part = std::string_view(
					current_id.begin() + half_length, current_id.end());

				if (first_part == second_part) {
					invalid_count += current_id_numeric;
				}
			}
		}

		return invalid_count;
	}

	int64_t count_invalids_p2() const {
		int64_t invalid_count = 0;
		for (int64_t current_id_numeric = i_from; current_id_numeric <= i_to;
			 current_id_numeric++) {
			int32_t view_size{1};
			auto current_id = std::to_string(current_id_numeric);
			auto lenght = current_id.size();
			auto half_length = lenght / 2;
			for (int32_t current_lenght{1}; current_lenght <= half_length;
				 current_lenght++) {
				auto begin = current_id.begin();
				auto first_part =
					std::string_view(begin, begin + current_lenght);
				auto first_part_lenght = first_part.size();

				bool all_parts_match = true;

				for (int32_t walk{0}; walk < lenght;
					 walk = walk + first_part_lenght) {
					auto second_part = std::string_view(
						begin + walk, begin + walk + current_lenght);

					if (first_part != second_part) {
						all_parts_match = false;
						break;
					}
				}

				if (all_parts_match) {
					invalid_count += current_id_numeric;
                    break;
				}
			}
		}

		return invalid_count;
	}
};

class IdChecker {
	std::vector<Range> ranges;

  public:
	IdChecker() : ranges(std::vector<Range>()) {
		InputFile input_file{};

		std::string ranges_string;
		std::getline(input_file.file_handle, ranges_string);

		for (auto ranges_string_parts : split_string(ranges_string, ',')) {
			auto parts = split_string(ranges_string_parts, '-').begin();
			auto part1 = *parts;
			auto part2 = *std::next(parts);
			ranges.push_back(Range{std::string{part1}, std::string{part2}});
		}
	}

	void solve() {
		int64_t total_invalids = 0;
		int64_t total_invalids_p2 = 0;
		for (const auto &range : ranges) {
			total_invalids += range.count_invalids();
			total_invalids_p2 += range.count_invalids_p2();
		}

		std::println("Part 1: {}", total_invalids);
		std::println("Part 2: {}", total_invalids_p2);
	}
};

int main(int argc, char *argv[]) {
	IdChecker idchecker{};
	idchecker.solve();

	return 0;
}
