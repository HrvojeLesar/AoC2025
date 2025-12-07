#include "../utils/utils.hpp"
#include <cassert>
#include <cstdint>
#include <format>
#include <print>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Beam {
	bool has_split = false;

  public:
	Point<int64_t> starting_position;
	Point<int64_t> current_position;
	bool on_splitter{false};
	int64_t value{0};

	Beam(int64_t x, int64_t y)
		: starting_position(x, y), current_position(starting_position) {}
	Beam(Point<int64_t> &p)
		: starting_position(p), current_position(starting_position) {}
	Beam(int64_t x, int64_t y, int64_t value)
		: starting_position(x, y), current_position(starting_position),
		  value(value) {}

	bool operator==(const Beam &other) const {
		return starting_position == other.starting_position;
	}

	const Point<int64_t> &down() {
		if (on_splitter) {
			return current_position;
		}

		current_position.y += 1;

		return current_position;
	}

	bool is_on_splitter(const std::vector<std::string> &lines) {
		if (on_splitter) {
			return on_splitter;
		}

		if (current_position.y >= lines.size()) {
			return on_splitter;
		}

		on_splitter =
			lines.at(current_position.y).at(current_position.x) == '^';

		return on_splitter;
	}

	void split_beam(const std::vector<std::string> &lines,
					std::vector<Beam> &new_beams) {
		if (has_split) {
			return;
		}

		if ((current_position.x - 1) >= 0) {
			new_beams.emplace_back(current_position.x - 1, current_position.y,
								   value);
		}
		if ((current_position.x + 1) < lines.at(current_position.y).size()) {
			new_beams.emplace_back(current_position.x + 1, current_position.y,
								   value);
		}

		has_split = true;
	}
};

template <> struct std::formatter<Beam> {
	constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }
	auto format(const Beam &b, std::format_context &ctx) const {
		return std::format_to(
			ctx.out(), "Start pos: {} | Current pos: {} | On splitter: {}",
			b.starting_position, b.current_position, b.on_splitter);
	}
};

class Beams {
	std::vector<std::string> lines;
	Point<int64_t> start_pos;
	std::unordered_map<Point<int64_t>, Beam> beams{};

  public:
	Beams() {
		InputFile file{};
		lines = std::move(file.get_lines());
		start_pos.x = lines.at(0).find('S');
		start_pos.y = 0;
		Beam starting_beam{start_pos};
		beams.emplace(start_pos, starting_beam);
	}

	void shoot() {
		std::vector<Beam> new_beams{};
		std::unordered_set<Point<int64_t>> visited_splitters{};

		for (const auto &line : lines) {
			for (auto &pair : beams) {
				Beam &beam = pair.second;
				beam.down();

				if (beam.is_on_splitter(lines)) {
					visited_splitters.emplace(beam.current_position);
					beam.split_beam(lines, new_beams);
				}
			}

			for (int i = 0; i < new_beams.size(); i++) {
				const auto beam = std::move(new_beams.at(i));
				if (!beams.contains(beam.starting_position)) {
					beams.emplace(beam.starting_position, std::move(beam));
				}
			}
			new_beams.clear();
		}

		std::println("Part 1, split count: {}", visited_splitters.size());
	}

	std::optional<Beam> find_duplicate(Beam &beam, std::vector<Beam> &beams) {
		for (const auto &b : beams) {
			if (b.current_position == beam.current_position) {
				return b;
			}
		}

		return std::nullopt;
	}

	void quantum_timelines() {
		std::vector<Beam> beams{start_pos};
		beams.at(0).value = 1;

		for (const auto &line : lines) {
			std::vector<Beam> new_beams{};
			std::unordered_map<Point<int64_t>, Beam> unique_beams{};
			for (auto &beam : beams) {
				beam.down();
				if (beam.is_on_splitter(lines)) {
					beam.split_beam(lines, new_beams);
				}
			}
			new_beams.append_range(beams);
			beams.clear();

			for (auto &beam : new_beams) {
				if (beam.on_splitter) {
					continue;
				}

				if (unique_beams.contains(beam.current_position)) {
					auto &other_beam = unique_beams.at(beam.current_position);
					other_beam.value += beam.value;
				} else {
					unique_beams.emplace(beam.current_position, beam);
				}
			}

			for (auto &pair : unique_beams) {
				beams.push_back(std::move(pair.second));
			}
		}

		int64_t timelines{0};
		for (auto &beam : beams) {
			timelines += beam.value;
		}

		std::println("Part 2, timelines count: {}", timelines);
	}
};

int main(int argc, char *argv[]) {
	Beams beams{};
	beams.shoot();
	beams.quantum_timelines();

	return 0;
}
