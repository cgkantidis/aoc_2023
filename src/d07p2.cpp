#include "utility.hpp"
#include <algorithm>
#include <array> // std::array
#include <cstdint> // std::uint64_t
#include <fmt/core.h> // fmt::print
#include <fstream> // std::ifstream
#include <libassert/assert.hpp> // ASSERT
#include <ranges> // std::views::enumerate
#include <string> // std::string
#include <unordered_map>
#include <vector> // std::vector

enum HandType : std::uint8_t
{
  HIGH_CARD,
  ONE_PAIR,
  TWO_PAIR,
  THREE_OF_A_KIND,
  FULL_HOUSE,
  FOUR_OF_A_KIND,
  FIVE_OF_A_KIND,
};

struct Hand
{
  std::string cards;
  long bid;
  HandType type;
};

static std::unordered_map<char, std::uint8_t> CARD_VALUE{{'A', 12},
                                                         {'K', 11},
                                                         {'Q', 10},
                                                         {'T', 9},
                                                         {'9', 8},
                                                         {'8', 7},
                                                         {'7', 6},
                                                         {'6', 5},
                                                         {'5', 4},
                                                         {'4', 3},
                                                         {'3', 2},
                                                         {'2', 1},
                                                         {'J', 0}};

namespace
{
void
tests();
long
get_answer(std::ranges::range auto &&lines);
HandType
get_hand_type(std::string_view hand);
} // namespace

int
main(int argc, char const *const *argv) {
  tests();

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

  std::vector<std::string> lines;
  for (std::string line; std::getline(infile, line);) {
    lines.emplace_back(std::move(line));
  }

  fmt::println("{}", get_answer(lines));
  return 0;
}

namespace
{
void
tests() {
  using namespace std::literals::string_view_literals;
  {
    auto const lines = std::array{
        "32T3K 765"sv,
        "T55J5 684"sv,
        "KK677 28"sv,
        "KTJJT 220"sv,
        "QQQJA 483"sv,
    };
    ASSERT(get_answer(lines) == 5905);
  }
}

long
get_answer(std::ranges::range auto &&lines) {
  std::vector<Hand> hands;
  for (auto const &line : lines) {
    auto tokens = split(line);
    hands.emplace_back(std::string(tokens[0]),
                       str_to_int<long>(tokens[1]),
                       get_hand_type(tokens[0]));
  }

  std::ranges::sort(hands, [](Hand const &lhs, Hand const &rhs) {
    if (lhs.type == rhs.type) {
      for (auto const &[l, r] : std::views::zip(lhs.cards, rhs.cards)) {
        if (l != r) {
          return CARD_VALUE[l] < CARD_VALUE[r];
        }
      }
    }
    return lhs.type < rhs.type;
  });

  return std::ranges::fold_left(
      std::views::enumerate(hands),
      0LL,
      [](auto const &prev, auto const &idx_hand) {
        return prev + ((std::get<0>(idx_hand) + 1) * std::get<1>(idx_hand).bid);
      });
}

HandType
get_hand_type(std::string_view hand) {
  long num_jokers{};
  std::unordered_map<char, std::uint8_t> counter;
  for (char c : hand) {
    if (c == 'J') {
      ++num_jokers;
      continue;
    }

    auto find_it = counter.find(c);
    if (find_it == counter.end()) {
      counter.emplace(c, 1);
    } else {
      ++find_it->second;
    }
  }

  std::vector<std::pair<char, std::uint8_t>> counter_vec;
  counter_vec.reserve(counter.size());
  for (auto const &[k, v] : counter) {
    counter_vec.emplace_back(k, v);
  }

  std::ranges::sort(counter_vec, [](auto const &lhs, auto const &rhs) {
    return lhs.second > rhs.second;
  });

  if (num_jokers == 5) {
    return FIVE_OF_A_KIND;
  }

  counter_vec[0].second += num_jokers;
  if (counter_vec[0].second == 5) {
    return FIVE_OF_A_KIND;
  }
  if (counter_vec[0].second == 4) {
    return FOUR_OF_A_KIND;
  }
  if (counter_vec[0].second == 3 && counter_vec[1].second == 2) {
    return FULL_HOUSE;
  }
  if (counter_vec[0].second == 3 && counter_vec[1].second == 1) {
    return THREE_OF_A_KIND;
  }
  if (counter_vec[0].second == 2 && counter_vec[1].second == 2) {
    return TWO_PAIR;
  }
  if (counter_vec[0].second == 2 && counter_vec[1].second == 1) {
    return ONE_PAIR;
  }
  return HIGH_CARD;
}
} // namespace
