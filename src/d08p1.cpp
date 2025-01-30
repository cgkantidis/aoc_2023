#include <array> // std::array
#include <cstdint> // std::uint64_t
#include <fmt/core.h> // fmt::print
#include <fstream> // std::ifstream
#include <libassert/assert.hpp> // ASSERT
#include <ranges> // std::views::enumerate
#include <regex> // std::regex
#include <string> // std::string
#include <unordered_map> // std::unordered_map
#include <vector> // std::vector

using u64 = std::uint64_t;
using desert_map_t =
    std::unordered_map<std::string, std::pair<std::string, std::string>>;
using namespace std::literals::string_view_literals;

namespace
{
void
tests();
u64
get_answer(std::ranges::range auto &&lines);
std::pair<std::vector<char>, desert_map_t>
parse_map(std::ranges::range auto &&lines);
u64
get_num_steps(std::vector<char> const &directions,
              desert_map_t const &desert_map);
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
    auto const lines = std::array{"RL"sv,
                                  ""sv,
                                  "AAA = (BBB, CCC)"sv,
                                  "BBB = (DDD, EEE)"sv,
                                  "CCC = (ZZZ, GGG)"sv,
                                  "DDD = (DDD, DDD)"sv,
                                  "EEE = (EEE, EEE)"sv,
                                  "GGG = (GGG, GGG)"sv,
                                  "ZZZ = (ZZZ, ZZZ)"sv};
    ASSERT(get_answer(lines) == 2);
  }
  {
    auto const lines = std::array{"LLR"sv,
                                  ""sv,
                                  "AAA = (BBB, BBB)"sv,
                                  "BBB = (AAA, ZZZ)"sv,
                                  "ZZZ = (ZZZ, ZZZ)"sv};
    ASSERT(get_answer(lines) == 6);
  }
}

u64
get_answer(std::ranges::range auto &&lines) {
  auto [directions, desert_map] = parse_map(lines);
  return get_num_steps(directions, desert_map);
}

std::pair<std::vector<char>, desert_map_t>
parse_map(std::ranges::range auto &&lines) {
  std::vector<char> directions = lines[0] | std::ranges::to<std::vector<char>>();
  desert_map_t desert_map;
  std::regex const re{R"XXX((...) = \((...), (...)\))XXX"};
  std::smatch match;
  for (auto const &line : lines | std::views::drop(2)) {
    std::string str_line(line);
    if (std::regex_match(str_line, match, re)) {
      auto it = match.begin();
      desert_map.emplace(std::piecewise_construct,
                         std::forward_as_tuple(it[1].str()),
                         std::forward_as_tuple(it[2].str(), it[3].str()));
    }
  }
  return {directions, desert_map};
}

u64
get_num_steps(std::vector<char> const &directions,
              desert_map_t const &desert_map) {
  u64 num_steps{};
  auto const dir_size{directions.size()};
  std::string cur_state{"AAA"};
  while (cur_state != "ZZZ") {
    auto find_it = desert_map.find(cur_state);
    if (directions[num_steps % dir_size] == 'L') {
      cur_state = find_it->second.first;
    } else {
      cur_state = find_it->second.second;
    }
    ++num_steps;
  }
  return num_steps;
}
} // namespace
