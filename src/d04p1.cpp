#include "utility.hpp"

#include <algorithm> // std::ranges::count_if
#include <print> // std::println
#include <ranges> // std::views::transform
#include <set> // std::set

namespace
{
void
tests();
std::uint64_t
get_sum_of_points(std::ranges::range auto &&lines);
} // namespace

int
main(int argc, char const **argv) {
  tests();
  std::vector<std::string> lines = read_program_input(argc, argv);
  std::println("{}", get_sum_of_points(lines));
  return 0;
}

namespace
{
void
tests() {
  using namespace std::literals::string_view_literals;
  auto const lines = std::array{
      "Card 1: 41 48 83 86 17 | 83 86  6 31 17  9 48 53"sv,
      "Card 2: 13 32 20 16 61 | 61 30 68 82 17 32 24 19"sv,
      "Card 3:  1 21 53 59 44 | 69 82 63 72 16 21 14  1"sv,
      "Card 4: 41 92 73 84 69 | 59 84 76 51 58  5 54 83"sv,
      "Card 5: 87 83 26 28 32 | 88 30 70 12 93 22 82 36"sv,
      "Card 6: 31 18 13 56 72 | 74 77 10 23 35 67 36 11"sv,
  };
  ASSERT(get_sum_of_points(lines) == 13);
}

std::uint64_t
get_sum_of_points(std::ranges::range auto &&lines) {
  std::uint64_t total{};
  for (auto const &line : lines) {
    auto card_id_to_numbers = split(line, ": ");
    auto winning_and_have_numbers = split(card_id_to_numbers[1], " | ");
    auto const winning_numbers =
        std::views::transform(split(winning_and_have_numbers[0]),
                              str_to_int<std::size_t>)
        | std::ranges::to<std::set>();
    auto const have_numbers =
        std::views::transform(split(winning_and_have_numbers[1]),
                              str_to_int<std::size_t>)
        | std::ranges::to<std::set>();
    auto count = static_cast<std::size_t>(std::ranges::count_if(
        have_numbers,
        [&winning_numbers](std::size_t const &have_number) {
          return winning_numbers.contains(have_number);
        }));
    if (count > 0) {
      total += 1ULL << (count - 1ULL);
    }
  }
  return total;
}
} // namespace
