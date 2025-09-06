#include "utility.hpp"

#include <algorithm> // std::ranges::transform
#include <print> // std::println
#include <string> // std::string
#include <vector> // std::vector

struct GameSet
{
  std::size_t m_red;
  std::size_t m_green;
  std::size_t m_blue;

  [[nodiscard]] std::size_t
  get_power() const;
};

struct Game
{
  std::size_t m_id;
  std::vector<GameSet> m_sets;

  [[nodiscard]] GameSet
  get_min_game_set() const;
};

namespace
{
void
tests();
std::uint64_t
get_sum_of_powers_of_min_game_sets(std::ranges::range auto &&lines);
Game
parse_game(std::string_view line);
std::size_t
parse_game_id(std::string_view game_id_line);
std::vector<GameSet>
parse_game_sets(std::string_view game_sets_line);
GameSet
parse_game_set(std::string_view game_set_line);
} // namespace

int
main(int argc, char const *const *argv) {
  tests();

  std::vector<std::string> lines = read_program_input(argc, argv);
  if (lines.empty()) {
    return 1;
  }

  std::println("{}", get_sum_of_powers_of_min_game_sets(lines));
  return 0;
}

namespace
{
void
tests() {
  using namespace std::literals::string_view_literals;
  auto const lines = std::array{
      "Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green"sv,
      "Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue"sv,
      "Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red"sv,
      "Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red"sv,
      "Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green"sv,
  };
  ASSERT(get_sum_of_powers_of_min_game_sets(lines) == 2286);
}

std::uint64_t
get_sum_of_powers_of_min_game_sets(std::ranges::range auto &&lines) {
  std::uint64_t total{};
  for (auto const &line : lines) {
    auto game = parse_game(line);
    total += game.get_min_game_set().get_power();
  }
  return total;
}

Game
parse_game(std::string_view line) {
  auto tokens = split(line, ": ");
  return Game{.m_id = parse_game_id(tokens[0]),
              .m_sets = parse_game_sets(tokens[1])};
}

std::size_t
parse_game_id(std::string_view game_id_line) {
  static constexpr std::size_t GAME_LEN{5 /*Game */};
  std::size_t game_id{};
  std::from_chars(game_id_line.data() + GAME_LEN,
                  game_id_line.data() + GAME_LEN + game_id_line.size(),
                  game_id);
  return game_id;
}

std::vector<GameSet>
parse_game_sets(std::string_view game_sets_line) {
  std::vector<GameSet> game_sets;
  auto tokens = split(game_sets_line, "; ");
  std::ranges::transform(tokens, std::back_inserter(game_sets), parse_game_set);
  return game_sets;
}

GameSet
parse_game_set(std::string_view game_set_line) {
  GameSet game_set{.m_red = 0, .m_green = 0, .m_blue = 0};
  for (std::string_view pair : split(game_set_line, ", ")) {
    auto tokens = split(pair);
    std::size_t num{};
    std::from_chars(tokens[0].data(), tokens[0].data() + tokens[0].size(), num);
    if (tokens[1] == "red") {
      game_set.m_red = num;
    } else if (tokens[1] == "green") {
      game_set.m_green = num;
    } else {
      game_set.m_blue = num;
    }
  }
  return game_set;
}
} // namespace

std::size_t
GameSet::get_power() const {
  return m_red * m_green * m_blue;
}

GameSet
Game::get_min_game_set() const {
  GameSet min_game_set{.m_red = 0, .m_green = 0, .m_blue = 0};
  for (GameSet const &game_set : m_sets) {
    min_game_set.m_red = std::max(min_game_set.m_red, game_set.m_red);
    min_game_set.m_green = std::max(min_game_set.m_green, game_set.m_green);
    min_game_set.m_blue = std::max(min_game_set.m_blue, game_set.m_blue);
  }
  return min_game_set;
}
