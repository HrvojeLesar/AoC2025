#include "../utils/utils.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <print>
#include <string>
#include <unordered_set>
#include <vector>

#ifdef TEST_INPUT
static const int64_t repeat_times = 10;
#else
static const int64_t repeat_times = 1000;
#endif

class JunctionBox {
  public:
	int64_t x{};
	int64_t y{};
	int64_t z{};

	JunctionBox(std::string &line) {
		auto parts = split_string(line, ',').begin();

		x = std::stol((*parts).data());
		parts = std::next(parts);
		y = std::stol((*parts).data());
		parts = std::next(parts);
		z = std::stol((*parts).data());
	}

	JunctionBox(const JunctionBox &other)
		: x(other.x), y(other.y), z(other.z) {}

	int64_t distance(const JunctionBox &other) const {
		return std::abs(std::sqrt(std::pow(x - other.x, 2) +
								  std::pow(y - other.y, 2) +
								  std::pow(z - other.z, 2)));
	}

	std::string to_string() const {
		return std::to_string(x) + "," + std::to_string(y) + "," +
			   std::to_string(z);
	}
};

template <> struct std::formatter<JunctionBox> {
	constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

	auto format(const JunctionBox &p, std::format_context &ctx) const {
		return std::format_to(ctx.out(), "(x:{}, y:{}, z:{})", p.x, p.y, p.z);
	}
};

class JunctionBoxDistancePair {
  public:
	std::shared_ptr<JunctionBox> first;
	std::shared_ptr<JunctionBox> second;
	int64_t distance;

	JunctionBoxDistancePair(std::shared_ptr<JunctionBox> first,
							std::shared_ptr<JunctionBox> second)
		: first(first), second(second), distance(first->distance(*second)) {}

	bool operator==(const JunctionBoxDistancePair &other) const {
		return first.get() == other.first.get() &&
				   second.get() == other.second.get() ||
			   first.get() == other.second.get() &&
				   second.get() == other.first.get();
	}
};

template <> struct std::hash<JunctionBoxDistancePair> {
	size_t operator()(JunctionBoxDistancePair const &b) const noexcept {
		return std::hash<std::string>()(b.first->to_string()) ^
			   std::hash<std::string>()(b.second->to_string()) ^
			   std::hash<int64_t>()(b.distance);
	}
};

class ConnetedJunctionBoxes {
	std::vector<std::shared_ptr<JunctionBox>> boxes{};

  public:
	void add(const std::shared_ptr<JunctionBox> box) {
		if (has_box(box)) {
			return;
		}

		boxes.push_back(std::move(box));
	}

	void add(ConnetedJunctionBoxes &&connected_boxes) {
		for (const auto &box : connected_boxes.boxes) {
			if (has_box(box)) {
				continue;
			}
			boxes.push_back(std::move(box));
		}
	}

	bool has_box(const std::shared_ptr<JunctionBox> &box) const {
		return std::find(boxes.begin(), boxes.end(), box) != boxes.end();
	}

	constexpr size_t size() const { return boxes.size(); }
};

class JunctionBoxes {
  public:
	std::vector<std::shared_ptr<JunctionBox>> boxes{};
	std::vector<JunctionBoxDistancePair> distances{};

	JunctionBoxes() {
		InputFile file{};

		std::string line{};

		while (std::getline(file.file_handle, line)) {
			boxes.emplace_back(
				std::make_shared<JunctionBox>(JunctionBox{line}));
		}

		std::unordered_set<JunctionBoxDistancePair> distances_set{};
		for (const auto &first : boxes) {
			for (const auto &second : boxes) {
				if (first.get() == second.get()) {
					continue;
				}

				JunctionBoxDistancePair pair{first, second};

				if (!distances_set.contains(pair)) {
					distances_set.insert(std::move(pair));
				}
			}
		}

		distances.insert(distances.begin(), distances_set.begin(),
						 distances_set.end());

		std::sort(distances.begin(), distances.end(),
				  [](const auto &a, const auto &b) {
					  return a.distance < b.distance;
				  });
	}

	std::optional<ConnetedJunctionBoxes>
	join_boxes(std::vector<ConnetedJunctionBoxes> &connected_boxes_pool,
			   std::shared_ptr<JunctionBox> junction_box) {
		std::vector<int64_t> in_junctions{};
		for (int64_t i = 0; i < connected_boxes_pool.size(); i++) {
			auto &connected_boxes = connected_boxes_pool.at(i);
			if (connected_boxes.has_box(junction_box)) {
				in_junctions.push_back(i);
			}
		}

		if (in_junctions.size() < 2) {
			return std::nullopt;
		}

		std::sort(in_junctions.begin(), in_junctions.end(),
				  [](int64_t a, int64_t b) { return a > b; });

		ConnetedJunctionBoxes new_connected_boxes{};
		for (const auto &junction : in_junctions) {
			auto boxes = std::move(connected_boxes_pool.at(junction));
			new_connected_boxes.add(std::move(boxes));
			connected_boxes_pool.erase(connected_boxes_pool.begin() + junction);
		}

		return new_connected_boxes;
	}

	void connect_boxes() {
		std::vector<ConnetedJunctionBoxes> connected_boxes_pool{};

		for (int64_t i = 0; i < distances.size(); i++) {
			if (i == repeat_times) {
				break;
			}
			auto pair = distances[i];

			std::optional<int64_t> found_connected_boxes_id = std::nullopt;
			for (int64_t box_idx = 0; box_idx < connected_boxes_pool.size();
				 box_idx++) {
				auto &connected_boxes = connected_boxes_pool.at(box_idx);
				if (connected_boxes.has_box(pair.first) ||
					connected_boxes.has_box(pair.second)) {
					found_connected_boxes_id = box_idx;
					break;
				}
			}

			if (found_connected_boxes_id.has_value()) {
				connected_boxes_pool.at(found_connected_boxes_id.value())
					.add(pair.first);
				connected_boxes_pool.at(found_connected_boxes_id.value())
					.add(pair.second);
			} else {
				connected_boxes_pool.emplace_back();
				connected_boxes_pool.back().add(pair.first);
				connected_boxes_pool.back().add(pair.second);
			}

			auto v = join_boxes(connected_boxes_pool, pair.first);
			if (v.has_value()) {
				connected_boxes_pool.push_back(std::move(v.value()));
			}
			v = join_boxes(connected_boxes_pool, pair.second);
			if (v.has_value()) {
				connected_boxes_pool.push_back(std::move(v.value()));
			}
		}

		std::sort(
			connected_boxes_pool.begin(), connected_boxes_pool.end(),
			[](const auto &a, const auto &b) { return a.size() > b.size(); });

		int64_t result = connected_boxes_pool.at(0).size() *
						 connected_boxes_pool.at(1).size() *
						 connected_boxes_pool.at(2).size();

		std::println("Part 1: {}", result);
	}

	void connect_boxes_part_2() {
		std::vector<ConnetedJunctionBoxes> connected_boxes_pool{};

		auto total_connected_boxes =
			[](std::vector<ConnetedJunctionBoxes> &pool) {
				int64_t result = 0;
				for (const auto &box : pool) {
					result += box.size();
				}

				return result;
			};

		for (int64_t i = 0; i < distances.size(); i++) {
			auto pair = distances[i];

			std::optional<int64_t> found_connected_boxes_id = std::nullopt;
			for (int64_t box_idx = 0; box_idx < connected_boxes_pool.size();
				 box_idx++) {
				auto &connected_boxes = connected_boxes_pool.at(box_idx);
				if (connected_boxes.has_box(pair.first) ||
					connected_boxes.has_box(pair.second)) {
					found_connected_boxes_id = box_idx;
					break;
				}
			}

			if (found_connected_boxes_id.has_value()) {
				connected_boxes_pool.at(found_connected_boxes_id.value())
					.add(pair.first);
				connected_boxes_pool.at(found_connected_boxes_id.value())
					.add(pair.second);
			} else {
				connected_boxes_pool.emplace_back();
				connected_boxes_pool.back().add(pair.first);
				connected_boxes_pool.back().add(pair.second);
			}

			auto v = join_boxes(connected_boxes_pool, pair.first);
			if (v.has_value()) {
				connected_boxes_pool.push_back(std::move(v.value()));
			}
			v = join_boxes(connected_boxes_pool, pair.second);
			if (v.has_value()) {
				connected_boxes_pool.push_back(std::move(v.value()));
			}

			if (total_connected_boxes(connected_boxes_pool) == boxes.size() &&
				connected_boxes_pool.size() == 1) {
				std::println("Part 2: {}", (*pair.first).x * (*pair.second).x);
				break;
			}
		}
	}
};

int main(int argc, char *argv[]) {
	JunctionBoxes boxes{};

	boxes.connect_boxes();
	boxes.connect_boxes_part_2();

	return 0;
}
