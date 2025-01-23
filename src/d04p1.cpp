#include <algorithm>
#include <fmt/core.h>
#include <fstream>
#include <ranges>
#include <set>
#include <string>
#include <vector>

#include "utility.hpp"

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

  std::size_t total{};
  for (std::string line; std::getline(infile, line);) {
    auto card_id_to_numbers = split(line, ": ");
    auto winning_and_have_numbers = split(card_id_to_numbers[1], " | ");
    auto winning_numbers =
        std::views::transform(split(winning_and_have_numbers[0]), str_to_int<std::size_t>);
    auto have_numbers =
        std::views::transform(split(winning_and_have_numbers[1]), str_to_int<std::size_t>);
    auto winning_numbers_set =
        std::set(winning_numbers.begin(), winning_numbers.end());
    auto have_numbers_set = std::set(have_numbers.begin(), have_numbers.end());

    auto count = static_cast<std::size_t>(std::ranges::count_if(
        have_numbers_set,
        [&winning_numbers_set](std::size_t const &have_number) {
          return winning_numbers_set.contains(have_number);
        }));
    if (count > 0) {
      total += 1ULL << (count - 1ULL);
    }
  }
  fmt::println("{}", total);
  return 0;
}
