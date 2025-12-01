#include <fstream>
#include <print>

class Dial {
  private:
	std::ifstream fileHandle;
	int32_t at_zero_count;
	int32_t dial_position;
	int32_t at_zero_count_p2;

	void process_line(const std::string &line) {
		auto direction = line.at(0);
		int32_t rotation = std::stoi(line.substr(1));

		auto increment = 1;
		if (direction == 'L') {
			increment = -1;
		}

		for (int32_t i = 0; i < rotation; i++) {
			dial_position += increment;
			if (dial_position == 100) {
				dial_position = 0;
			}
			if (dial_position == -1) {
				dial_position = 99;
			}
			if (dial_position == 0) {
				at_zero_count_p2 += 1;
			}
		}

		if (dial_position == 0) {
			at_zero_count += 1;
		}
	}

  public:
	Dial(const char *filename)
		: fileHandle(filename), at_zero_count(0), dial_position(50),
		  at_zero_count_p2(0) {}

	Dial(const Dial &other) = delete;
	Dial(const Dial &&other) = delete;

	~Dial() { fileHandle.close(); }

	void solve() {
		std::string line;

		while (std::getline(fileHandle, line)) {
			process_line(line);
		}

		std::println("Part 1: {}", at_zero_count);
		std::println("Part 2: {}", at_zero_count_p2);
	}
};

int main() {
	Dial dial("input");

	dial.solve();

	return 0;
}
