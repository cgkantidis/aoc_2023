#include "utility.hpp"
#include <print> // std::println
#include <ranges> // std::views::enumerate
#include <string> // std::string

namespace
{
void
tests();
std::uint64_t
get_sum_of_calibration_values(std::ranges::range auto &&lines);
std::pair<bool, uint8_t>
str_to_digit(std::string_view sv);
} // namespace

int
main(int argc, char const **argv) {
  tests();

  std::vector<std::string> lines = read_program_input(argc, argv);
  if (lines.empty()) {
    return 1;
  }

  std::println("{}", get_sum_of_calibration_values(lines));
  return 0;
}

namespace
{
void
tests() {
  using namespace std::literals::string_view_literals;
  auto const lines = std::array{
      "two1nine"sv,
      "eightwothree"sv,
      "abcone2threexyz"sv,
      "xtwone3four"sv,
      "4nineeightseven2"sv,
      "zoneight234"sv,
      "7pqrstsixteen"sv,
  };
  ASSERT(get_sum_of_calibration_values(lines) == 281);
}

std::uint64_t
get_sum_of_calibration_values(std::ranges::range auto &&lines) {
  std::uint64_t total{};
  for (auto const &line : lines) {
    auto const line_len = line.size();
    // get the first number (from left to right)
    for (std::size_t idx = 0; idx < line_len; ++idx) {
      if (auto const [success, digit] = str_to_digit(line.substr(idx)); success) {
        total += 10UL * digit;
        break;
      }
    }
    // get the first digit (from right to left)
    for (std::size_t idx = 0; idx < line_len; ++idx) {
      if (auto const [success, digit] =
              str_to_digit(line.substr(line_len - idx - 1));
          success) {
        total += digit;
        break;
      }
    }
  }
  return total;
}

std::pair<bool, uint8_t>
str_to_digit(std::string_view sv) {
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

  for (auto const &[idx, digit_number] : std::views::enumerate(NUMBERS)) {
    if (sv.starts_with(digit_number.first)
        || sv.starts_with(digit_number.second)) {
      return {true, idx};
    }
  }
  return {false, 0};
}
} // namespace
