#include "../utils/utils.hpp"
#include <cstddef>
#include <print>
#include <string>
#include <vector>

class PaperGrid {
	const static char PAPER_CHARACTER = '@';
	std::vector<std::vector<char>> grid;

	bool is_valid_paper(int64_t x, int64_t y, size_t x_max, size_t y_max) {
		size_t count = 0;

		// Top
		if (y - 1 >= 0 && grid.at(y - 1).at(x) == PAPER_CHARACTER) {
			count += 1;
		}

		// Top right
		if (y - 1 >= 0 && x + 1 < x_max &&
			grid.at(y - 1).at(x + 1) == PAPER_CHARACTER) {
			count += 1;
		}

		// Right
		if (x + 1 < x_max && grid.at(y).at(x + 1) == PAPER_CHARACTER) {
			count += 1;
		}

		// Bottom right
		if (y + 1 < y_max && x + 1 < x_max &&
			grid.at(y + 1).at(x + 1) == PAPER_CHARACTER) {
			count += 1;
		}

		// Bottom
		if (y + 1 < y_max && grid.at(y + 1).at(x) == PAPER_CHARACTER) {
			count += 1;
		}

		// Bottom left
		if (y + 1 < y_max && x - 1 >= 0 &&
			grid.at(y + 1).at(x - 1) == PAPER_CHARACTER) {
			count += 1;
		}

		// Left
		if (x - 1 >= 0 && grid.at(y).at(x - 1) == PAPER_CHARACTER) {
			count += 1;
		}

		// Top left
		if (y - 1 >= 0 && x - 1 >= 0 &&
			grid.at(y - 1).at(x - 1) == PAPER_CHARACTER) {
			count += 1;
		}

		if (count < 4) {
			return true;
		} else {
			return false;
		}
	}

  public:
	PaperGrid() : grid() {
		InputFile file{};

		std::string line;
		while (std::getline(file.file_handle, line)) {
			grid.push_back({line.begin(), line.end()});
		}
	}

	void find_rolls() {
		size_t roll_count = 0;
		const auto y_max = grid.size();
		for (size_t y = 0; y < y_max; y++) {
			const auto x_max = grid.at(y).size();
			for (size_t x = 0; x < x_max; x++) {
				if (grid.at(y).at(x) == PAPER_CHARACTER &&
					is_valid_paper(x, y, x_max, y_max)) {
					roll_count += 1;
				}
			}
		}

		std::println("Part 1: {}", roll_count);
	}

	void remove_rolls() {
		size_t total_rolls_removed = 0;
		while (true) {
			size_t roll_count = 0;
			std::vector<std::pair<size_t, size_t>> papers_to_remove{};
			const auto y_max = grid.size();
			for (size_t y = 0; y < y_max; y++) {
				const auto x_max = grid.at(y).size();
				for (size_t x = 0; x < x_max; x++) {
					if (grid.at(y).at(x) == PAPER_CHARACTER &&
						is_valid_paper(x, y, x_max, y_max)) {
						papers_to_remove.emplace_back(x, y);
						roll_count += 1;
					}
				}
			}

			for (auto coord_pair : papers_to_remove) {
				grid.at(coord_pair.second).at(coord_pair.first) = '.';
			}

			if (roll_count == 0) {
				break;
			} else {
				total_rolls_removed += roll_count;
			}
		}

		std::println("Part 2: {}", total_rolls_removed);
	}
};

int main(int argc, char *argv[]) {
	PaperGrid paper_grid{};
	paper_grid.find_rolls();
	paper_grid.remove_rolls();

	return 0;
}
