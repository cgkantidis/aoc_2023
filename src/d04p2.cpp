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

  std::vector<std::size_t> winning_cards;
  for (std::string line; std::getline(infile, line);) {
    auto card_id_to_numbers = split(line, ": ");
    auto winning_and_have_numbers = split(card_id_to_numbers[1], " | ");
    auto winning_numbers =
        std::views::transform(split(winning_and_have_numbers[0]),
                              str_to_int<std::size_t>);
    auto have_numbers = std::views::transform(split(winning_and_have_numbers[1]),
                                              str_to_int<std::size_t>);
    auto winning_numbers_set =
        std::set(winning_numbers.begin(), winning_numbers.end());
    auto have_numbers_set = std::set(have_numbers.begin(), have_numbers.end());

    auto count = static_cast<std::size_t>(std::ranges::count_if(
        have_numbers_set,
        [&winning_numbers_set](std::size_t const &have_number) {
          return winning_numbers_set.contains(have_number);
        }));
    winning_cards.emplace_back(count);
  }
  std::vector<std::size_t> total_cards(winning_cards.size(), 1);
  for (std::size_t idx = 0; idx < winning_cards.size(); ++idx) {
    for (std::size_t sub_idx = idx + 1; sub_idx < idx + 1 + winning_cards[idx];
         ++sub_idx) {
      total_cards[sub_idx] += total_cards[idx];
    }
  }

  std::size_t total =
      std::ranges::fold_left(total_cards,
                             0ULL,
                             [](std::size_t const &prev,
                                std::size_t const &val) { return prev + val; });
  fmt::println("{}", total);
  return 0;
}
