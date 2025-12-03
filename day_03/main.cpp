#include "../utils/utils.hpp"
#include <cassert>
#include <cstdint>
#include <print>
#include <vector>

class Battery {
  private:
	InputFile input_file;

	int64_t chars_to_int(const std::vector<char> &chars) {
		int64_t carry = 0;
		if (chars.empty()) {
			return carry;
		}

		for (int32_t i = 0; i < chars.size(); i++) {
			auto c = chars[i];
			carry = carry * 10 + (c - '0');
		}

		return carry;
	}

	int64_t find_max_joltage(const std::string &battery) {
		auto max_joltage = chars_to_int({'1', '1'});

		for (int32_t i = 0; i < battery.size(); i++) {
			const char first = battery.at(i);

			for (int32_t j = i + 1; j < battery.size(); j++) {
				const char second = battery.at(j);
				auto calculated_joltage = chars_to_int({first, second});

				if (calculated_joltage > max_joltage) {
					max_joltage = calculated_joltage;
				}
			}
		}

		std::println("Max joltage for battery: {} is {}", battery, max_joltage);

		return max_joltage;
	}

	int64_t find_max_joltage12(const std::string &battery) {
		std::vector<char> joltages{battery.at(0)};
		joltages.reserve(12);

		for (int32_t i = 1; i < battery.size(); i++) {
			char current_digit = battery.at(i);
			while (!joltages.empty() && joltages.back() < current_digit &&
				   (battery.size() - i) > (12 - joltages.size())) {
				joltages.pop_back();
			}

			if (joltages.size() < 12) {
				joltages.push_back(current_digit);
			}
		}

		assert(joltages.size() == 12);

		return chars_to_int(joltages);
	}

  public:
	Battery() : input_file{} {}

	void jolts_p1() {
		std::string line;

		int64_t jolts2 = 0;
		int64_t jolts12 = 0;

		while (std::getline(input_file.file_handle, line)) {
			jolts2 += find_max_joltage(line);
			jolts12 += find_max_joltage12(line);
		}

		std::println("Part 1 jolts: {}", jolts2);
		std::println("Part 2 jolts: {}", jolts12);
	}
};

int main(int argc, char *argv[]) {
	Battery battery{};
	battery.jolts_p1();

	return 0;
}
