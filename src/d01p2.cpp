#include <fmt/core.h>
#include <fstream>
#include <ranges>
#include <string>

static constexpr auto NUMBERS = std::array<std::pair<char, char const *>, 10>{{
    {'0', "zero"},
    {'1', "one"},
    {'2', "two"},
    {'3', "three"},
    {'4', "four"},
    {'5', "five"},
    {'6', "six"},
    {'7', "seven"},
    {'8', "eight"},
    {'9', "nine"},
}};

std::pair<bool, uint8_t>
str_to_digit(std::string_view sv) {
  for (auto const &[idx, digit_number] : std::views::enumerate(NUMBERS)) {
    if (sv.starts_with(digit_number.first)
        || sv.starts_with(digit_number.second)) {
      return {true, idx};
    }
  }
  return {false, 0};
}

int
main(int argc, char const **argv) {
  auto args = std::span(argv, size_t(argc));
  if (args.size() != 2) {
    fmt::println(stderr, "usage: {} input.txt", args[0]);
    return 1;
  }

  std::ifstream infile(args[1]);
  if (!infile.is_open()) {
    fmt::println(stderr, "couldn't open file {}", args[1]);
    return 2;
  }

  std::uint64_t total{};
  std::string line;
  while (std::getline(infile, line)) {
    auto const line_len = line.size();
    for (std::size_t idx = 0; idx < line_len; ++idx) {
      if (auto const [success, digit] = str_to_digit(line.substr(idx)); success) {
        total += 10 * digit;
        break;
      }
    }
    for (std::size_t idx = 0; idx < line_len; ++idx) {
      if (auto const [success, digit] =
              str_to_digit(line.substr(line_len - idx - 1));
          success) {
        total += digit;
        break;
      }
    }
  }
  fmt::println("{}", total);
  return 0;
}
