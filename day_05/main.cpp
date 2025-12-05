#include "../utils/utils.hpp"
#include <charconv>
#include <cstdint>
#include <print>
#include <ranges>
#include <string>
#include <vector>

class Range {
	int64_t from;
	int64_t to;

	Range(int64_t from, int64_t to) : from(from), to(to) {}

  public:
	static Range from_range_string(const std::string &range_string) {
		auto parts = split_string(range_string, '-').begin();

		auto from_string = std::string_view((*parts).data(), (*parts).size());

		parts = std::next(parts);
		auto to_string = std::string_view((*parts).data(), (*parts).size());

		int64_t from;
		std::from_chars(from_string.data(),
						from_string.data() + from_string.size(), from);

		int64_t to;
		std::from_chars(to_string.data(), to_string.data() + to_string.size(),
						to);

		Range range{from, to};

		return range;
	}

	int64_t count_fresh_ids() const { return to - from + 1; }

	bool is_in_range(int64_t value) const {
		return value >= from && value <= to;
	}

	bool intersects(const Range &other) const {
		return !(to < other.from || from > other.to);
	}

	bool equals(const Range &other) const {
		return from == other.from && to == other.to;
	}

	Range intersection(const Range &other) const {
		auto new_from = from < other.from ? from : other.from;
		auto new_to = to > other.to ? to : other.to;

		return Range{new_from, new_to};
	}

	void print() const { std::println("{}-{}", from, to); }
};

class Ranges {
	std::vector<Range> ranges;
	std::string line;
	InputFile file;

  public:
	Ranges() : ranges(std::vector<Range>()), line(), file() {
		while (std::getline(file.file_handle, line)) {
			if (line.empty()) {
				return;
			}

			ranges.push_back(Range::from_range_string(line));
		}
	}

	void count_fresh() {
		int64_t fresh_ingredients = 0;
		while (std::getline(file.file_handle, line)) {
			int64_t ingredient_id = std::stol(line);
			for (const auto &range : ranges) {
				if (range.is_in_range(ingredient_id)) {
					fresh_ingredients += 1;
					break;
				}
			}
		}

		std::println("Part 1: {}", fresh_ingredients);
	};

	void all_possible_fresh_ids_count() {
		auto ranges = this->ranges;
		auto other_ranges = this->ranges;
		other_ranges.clear();

		bool flip = true;

		auto ranges_to_use = flip ? &ranges : &other_ranges;
		auto other_ranges_to_use = !flip ? &ranges : &other_ranges;

		bool found_intersection = false;

		for (int64_t idx = 0; idx < ranges_to_use->size();) {
			auto current_range = ranges_to_use->at(idx);

			bool found_intersection = false;
			for (int64_t i = 0; i < ranges_to_use->size(); i++) {
				if (idx == i) {
					continue;
				}
				auto other_range = ranges_to_use->at(i);
				if (current_range.intersects(other_range)) {
					other_ranges_to_use->push_back(
						current_range.intersection(other_range));
					found_intersection = true;
				} else {
					other_ranges_to_use->push_back(other_range);
				}
			}

			if (found_intersection) {
				idx = 0;
			} else {
				other_ranges_to_use->push_back(current_range);
				idx++;
			}

			flip = !flip;

			ranges_to_use = flip ? &ranges : &other_ranges;
			other_ranges_to_use = !flip ? &ranges : &other_ranges;

			other_ranges_to_use->clear();
		}

		int64_t count = 0;
		for (const Range &range : *ranges_to_use) {
			count += range.count_fresh_ids();
		}

		std::println("Part 2: {}", count);
	}
};

int main(int argc, char *argv[]) {
	Ranges ranges{};
	ranges.count_fresh();
	ranges.all_possible_fresh_ids_count();

	return 0;
}
