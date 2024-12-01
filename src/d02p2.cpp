#include <algorithm>
#include <fmt/core.h>
#include <fstream>
#include <string>
#include <vector>

struct GameSet
{
  std::size_t m_red;
  std::size_t m_green;
  std::size_t m_blue;

  [[nodiscard]] std::size_t
  get_power() const {
    return m_red * m_green * m_blue;
  }
};

struct Game
{
  std::size_t m_id;
  std::vector<GameSet> m_sets;

  [[nodiscard]] GameSet
  get_min_game_set() const {
    GameSet min_game_set{0, 0, 0};
    for (auto const &game_set : m_sets) {
      min_game_set.m_red = std::max(min_game_set.m_red, game_set.m_red);
      min_game_set.m_green = std::max(min_game_set.m_green, game_set.m_green);
      min_game_set.m_blue = std::max(min_game_set.m_blue, game_set.m_blue);
    }
    return min_game_set;
  }
};

std::vector<std::string_view>
split(std::string_view sv, std::string_view delim = " ") {
  std::vector<std::string_view> tokens;
  for (std::size_t right = sv.find(delim); right != std::string_view::npos;
       right = sv.find(delim)) {
    if (right == 0) {
      sv = sv.substr(right + delim.size());
      continue;
    }
    tokens.emplace_back(sv.substr(0, right));
    sv = sv.substr(right + delim.size());
  }
  if (!sv.empty()) {
    tokens.emplace_back(sv);
  }
  return tokens;
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

GameSet
parse_game_set(std::string_view game_set_line) {
  GameSet game_set{0, 0, 0};
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

std::vector<GameSet>
parse_game_sets(std::string_view game_sets_line) {
  std::vector<GameSet> game_sets;
  auto tokens = split(game_sets_line, "; ");
  std::transform(tokens.begin(),
                 tokens.end(),
                 std::back_inserter(game_sets),
                 parse_game_set);
  return game_sets;
}

Game
parse_game(std::string_view line) {
  auto tokens = split(line, ": ");
  return Game{parse_game_id(tokens[0]), parse_game_sets(tokens[1])};
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
    auto game = parse_game(line);
    total += game.get_min_game_set().get_power();
  }
  fmt::println("{}", total);
  return 0;
}
