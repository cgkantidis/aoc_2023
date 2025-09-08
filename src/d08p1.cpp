#include "utility.hpp"

#include <print> // std::println
#include <regex> // std::regex
#include <unordered_map> // std::unordered_map

using desert_map_t =
    std::unordered_map<std::string, std::pair<std::string, std::string>>;

namespace
{
void
tests();
u64
get_num_steps_to_end(std::vector<std::string> const &lines);
std::pair<std::vector<char>, desert_map_t>
parse_map(std::vector<std::string> const &lines);
} // namespace

int
main(int argc, char const **argv) {
  tests();
  std::vector<std::string> lines = read_program_input(argc, argv);
  std::println("{}", get_num_steps_to_end(lines));
  return 0;
}

namespace
{
void
tests() {
  {
    std::vector<std::string> const lines{
        "RL",
        "",
        "AAA = (BBB, CCC)",
        "BBB = (DDD, EEE)",
        "CCC = (ZZZ, GGG)",
        "DDD = (DDD, DDD)",
        "EEE = (EEE, EEE)",
        "GGG = (GGG, GGG)",
        "ZZZ = (ZZZ, ZZZ)",
    };
    ASSERT(get_num_steps_to_end(lines) == 2);
  }
  {
    std::vector<std::string> const lines{
        "LLR",
        "",
        "AAA = (BBB, BBB)",
        "BBB = (AAA, ZZZ)",
        "ZZZ = (ZZZ, ZZZ)",
    };
    ASSERT(get_num_steps_to_end(lines) == 6);
  }
}

u64
get_num_steps_to_end(std::vector<std::string> const &lines) {
  auto [directions, desert_map] = parse_map(lines);
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

std::pair<std::vector<char>, desert_map_t>
parse_map(std::vector<std::string> const &lines) {
  std::vector<char> directions = lines[0] | std::ranges::to<std::vector<char>>();
  desert_map_t desert_map;
  std::regex const re{R"XXX((...) = \((...), (...)\))XXX"};
  std::smatch match;
  for (auto const &line : lines | std::views::drop(2)) {
    if (std::regex_match(line, match, re)) {
      auto it = match.begin();
      desert_map.emplace(std::piecewise_construct,
                         std::forward_as_tuple(it[1].str()),
                         std::forward_as_tuple(it[2].str(), it[3].str()));
    }
  }
  return {directions, desert_map};
}
} // namespace
