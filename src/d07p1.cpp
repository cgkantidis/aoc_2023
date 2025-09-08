#include "utility.hpp"

#include <algorithm> // std::ranges::sort
#include <print> // std::println
#include <ranges> // std::views::zip
#include <unordered_map> // std::unordered_map

enum HandType : u8
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
  u64 bid;
  HandType type;
};

static const std::unordered_map<char, u8> CARD_VALUE{{'A', 12},
                                                     {'K', 11},
                                                     {'Q', 10},
                                                     {'J', 9},
                                                     {'T', 8},
                                                     {'9', 7},
                                                     {'8', 6},
                                                     {'7', 5},
                                                     {'6', 4},
                                                     {'5', 3},
                                                     {'4', 2},
                                                     {'3', 1},
                                                     {'2', 0}};

namespace
{
void
tests();
u64
get_total_winnings(std::vector<std::string> const &lines);
HandType
get_hand_type(std::string_view hand);
} // namespace

int
main(int argc, char const **argv) {
  tests();
  std::vector<std::string> lines = read_program_input(argc, argv);
  std::println("{}", get_total_winnings(lines));
  return 0;
}

namespace
{
void
tests() {
  using namespace std::literals::string_view_literals;
  std::vector<std::string> const lines{
      "32T3K 765",
      "T55J5 684",
      "KK677 28",
      "KTJJT 220",
      "QQQJA 483",
  };
  ASSERT(get_total_winnings(lines) == 6440);
}

u64
get_total_winnings(std::vector<std::string> const &lines) {
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
          return CARD_VALUE.at(l) < CARD_VALUE.at(r);
        }
      }
    }
    return lhs.type < rhs.type;
  });

  return std::ranges::fold_left(
      std::views::enumerate(hands),
      0ULL,
      [](auto const &prev, auto const &idx_hand) {
        return prev
               + ((static_cast<u64>(std::get<0>(idx_hand)) + 1)
                  * std::get<1>(idx_hand).bid);
      });
}

HandType
get_hand_type(std::string_view hand) {
  std::unordered_map<char, u8> counter;
  for (char c : hand) {
    auto find_it = counter.find(c);
    if (find_it == counter.end()) {
      counter.emplace(c, 1);
    } else {
      ++find_it->second;
    }
  }

  std::vector<std::pair<char, u8>> counter_vec;
  counter_vec.reserve(counter.size());
  for (auto const &[k, v] : counter) {
    counter_vec.emplace_back(k, v);
  }

  std::ranges::sort(counter_vec, [](auto const &lhs, auto const &rhs) {
    return lhs.second > rhs.second;
  });

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
