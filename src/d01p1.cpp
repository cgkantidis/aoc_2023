#include <algorithm> // std::ranges::find_if
#include <print> // std::println
#include <ranges> // std::ranges::reverse_view
#include <string> // std::string

#include "utility.hpp"

namespace
{
void
tests();
std::uint64_t
get_sum_of_calibration_values(std::ranges::range auto &&lines);
} // namespace

int
main(int argc, char const **argv) {
  tests();

  std::vector<std::string> const lines = read_program_input(argc, argv);
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
      "1abc2"sv,
      "pqr3stu8vwx"sv,
      "a1b2c3d4e5f"sv,
      "treb7uchet"sv,
  };
  ASSERT(get_sum_of_calibration_values(lines) == 142);
}

std::uint64_t
get_sum_of_calibration_values(std::ranges::range auto &&lines) {
  std::uint64_t total{};
  for (auto const &line : lines) {
    // find first digit (from left to right)
    auto first = std::ranges::find_if(line, is_digit);
    // find first digit (from right to left)
    auto last = std::ranges::find_if(std::ranges::reverse_view(line), is_digit);
    total += char_to_int(*first) * 10UL + char_to_int(*last);
  }
  return total;
}
} // namespace
