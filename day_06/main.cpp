#include "../utils/utils.hpp"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <numeric>
#include <print>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

class CephalopodMathProblem {
	std::vector<int64_t> numbers;
	char sign;

  public:
	CephalopodMathProblem(std::vector<int64_t> numbers, char sign)
		: numbers(numbers), sign(sign) {}

	int64_t get_result() const {
		if (sign == '+') {
			return std::accumulate(numbers.begin(), numbers.end(), (int64_t)0);
		} else if (sign == '*') {
			return std::reduce(numbers.begin(), numbers.end(), (int64_t)1,
							   std::multiplies<int64_t>());
		} else {
			assert(false);
		}
	}

	void print() const {
		std::println("Problem: {} {} = {}", numbers, sign, get_result());
	}
};

class CephalopodMathProblems {
  public:
	std::vector<CephalopodMathProblem> problems;
	CephalopodMathProblems() : problems() {}

	int64_t grand_total() {
		int64_t grand_total = 0;
		for (const auto &problem : problems) {
			grand_total += problem.get_result();
		}

		return grand_total;
	}
};

class CephalopodCalculator {
	CephalopodMathProblems fake_math_problems;
	CephalopodMathProblems real_math_problems;

  public:
	CephalopodCalculator() : fake_math_problems(), real_math_problems() {
		InputFile file{};

		std::vector<std::string> problem_lines;
		std::vector<std::vector<std::string>> math_problems;

		std::string line;
		while (std::getline(file.file_handle, line)) {
			problem_lines.push_back(line);
		}

		auto sign_line = std::move(problem_lines.back());
		problem_lines.pop_back();

		for (int64_t i = 0; i < sign_line.size(); i++) {
			char current_char = sign_line.at(i);
			if (i + 1 == sign_line.size()) {
				for (auto &problem_line : problem_lines) {
					if (problem_line.at(i) == ' ') {
						problem_line.at(i) = '0';
					}
				}
				break;
			}
			char next_char = sign_line.at(i + 1);

			if (current_char == ' ' && (next_char == '+' || next_char == '*')) {
				continue;
			}

			for (auto &problem_line : problem_lines) {
				if (problem_line.at(i) == ' ') {
					problem_line.at(i) = '0';
				}
			}
		}

		problem_lines.push_back(sign_line);

		for (const auto &problem_line : problem_lines) {
			auto split = split_string(problem_line, ' ');
			auto problem_lines = split |
								 std::views::filter([](std::string_view s) {
									 return !s.empty();
								 }) |
								 std::ranges::to<std::vector<std::string>>();

			math_problems.push_back(std::move(problem_lines));
		}

		{
			std::vector<int64_t> numbers{};
			numbers.reserve(math_problems.size());

			char sign = '1';
			for (int64_t i = 0; i < math_problems.at(0).size(); i++) {
				for (int64_t row = 0; row < math_problems.size(); row++) {
					if (row == math_problems.size() - 1) {
						sign = math_problems.at(row).at(i).at(0);
					} else {
						if (numbers.size() < math_problems.size() - 1) {
							auto num = math_problems.at(row).at(i);
							std::replace(num.begin(), num.end(), '0', ' ');
							numbers.push_back(std::stoi(num));
						} else {
							auto num = math_problems.at(row).at(i);
							std::replace(num.begin(), num.end(), '0', ' ');
							numbers.at(row) = std::stoi(num);
						}
					}
				}
				assert(sign != '1');

				this->fake_math_problems.problems.emplace_back(
					CephalopodMathProblem{numbers, sign});
			}
		} // end of fake_math_problems

		{
			std::vector<int64_t> numbers(math_problems.size() - 1, 0);
			std::vector<std::basic_string<char>> numbers_non_parsed(
				math_problems.size() - 1);

			char sign = '1';
			for (int64_t i = 0; i < math_problems.at(0).size(); i++) {
				int64_t max_num_len = 0;
				for (int64_t row = 0; row < math_problems.size(); row++) {
					if (row == math_problems.size() - 1) {
						sign = math_problems.at(row).at(i).at(0);
					} else {
						auto number = math_problems.at(row).at(i);
						if (number.size() > max_num_len) {
							max_num_len = number.size();
						}
						numbers_non_parsed.at(row) = std::move(number);
					}
				}
				assert(sign != '1');

				// add padding at start of shorter numbers
				for (int64_t row = 0; row < numbers_non_parsed.size(); row++) {
					auto number = numbers_non_parsed.at(row);
					auto padding = max_num_len - number.size();
					for (int64_t j = 0; j < padding; j++) {
						number = "0" + number;
					}
					numbers_non_parsed.at(row) = std::move(number);
				}

				// std::println("Numbers non parsed: {}", numbers_non_parsed);

				std::string vertical_number{};
				for (int64_t column = 0; column < max_num_len; column++) {
					vertical_number.clear();
					for (int64_t row = 0; row < math_problems.size() - 1;
						 row++) {
						auto digit = numbers_non_parsed.at(row).at(column);
						if (digit == '0') {
							continue;
						}
						vertical_number += digit;
					}

					numbers.at(column) = std::stoi(vertical_number);
				}

				this->real_math_problems.problems.emplace_back(
					CephalopodMathProblem{
						{numbers.begin(), numbers.begin() + max_num_len},
						sign});

			} // end of real_math_problems
		}
	}

	void solve() {
		std::println("Part 1: {}", fake_math_problems.grand_total());
		std::println("Part 2: {}", real_math_problems.grand_total());
	}
};

int main(int argc, char *argv[]) {
	CephalopodCalculator calculator{};
	calculator.solve();

	return 0;
}
