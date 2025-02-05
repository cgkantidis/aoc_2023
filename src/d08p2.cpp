#include "utility.hpp"
#include <algorithm> // std::ranges::all_of
#include <array> // std::array
#include <cstdint> // std::uint64_t
#include <fmt/core.h> // fmt::print
#include <fstream> // std::ifstream
#include <libassert/assert.hpp> // ASSERT
#include <numeric> // std::lcm
#include <ranges> // std::views::enumerate
#include <regex> // std::regex
#include <string> // std::string
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set
#include <vector> // std::vector

using u64 = std::uint64_t;
using desert_map_t =
    std::unordered_map<std::string, std::pair<std::string, std::string>>;
using namespace std::literals::string_view_literals;

struct State
{
  std::string name;
  std::size_t idx;
};

bool
operator==(State const &lhs, State const &rhs) {
  return lhs.name == rhs.name && lhs.idx == rhs.idx;
}

template <>
struct std::hash<State>
{
  std::size_t
  operator()(State const &state) const noexcept {
    std::size_t h1 = std::hash<std::string>{}(state.name);
    std::size_t h2 = std::hash<std::size_t>{}(state.idx);

    std::size_t ret_val = 0;
    hash_combine(ret_val, h1, h2);
    return ret_val;
  }
};

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
std::vector<std::vector<u64>>
get_state_num_steps(std::vector<char> const &directions,
                    desert_map_t const &desert_map,
                    std::vector<std::string> const &states);
bool
advance_indices(std::vector<std::size_t> &indices,
                std::vector<std::size_t> const &sizes);
std::vector<u64>
collect_states(std::vector<std::size_t> const &indices,
               std::vector<std::vector<u64>> const &state_num_steps);
u64
lcm(std::vector<u64> &states);
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
        "LR"sv,
        ""sv,
        "11A = (11B, XXX)"sv,
        "11B = (XXX, 11Z)"sv,
        "11Z = (11B, XXX)"sv,
        "22A = (22B, XXX)"sv,
        "22B = (22C, 22C)"sv,
        "22C = (22Z, 22Z)"sv,
        "22Z = (22B, 22B)"sv,
        "XXX = (XXX, XXX)"sv,
    };
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
  std::unordered_set<std::string> states_set;
  std::vector<std::string> states;
  for (auto const &[k, v] : desert_map) {
    if (k.back() == 'A') {
      if (states_set.find(k) == states_set.end()) {
        states.emplace_back(k);
        states_set.insert(k);
      }
    }
  }

  std::vector<std::vector<u64>> state_num_steps =
      get_state_num_steps(directions, desert_map, states);

  std::vector<std::size_t> indices(state_num_steps.size());
  std::vector<std::size_t> sizes(state_num_steps.size());
  for (std::size_t idx = 0; idx < sizes.size(); ++idx) {
    sizes[idx] = state_num_steps[idx].size();
  }

  u64 min_lcm = std::numeric_limits<u64>::max();
  while (true) {
    auto new_states = collect_states(indices, state_num_steps);
    min_lcm = std::min(min_lcm, lcm(new_states));
    if (!advance_indices(indices, sizes)) {
      break;
    }
  }
  return min_lcm;
}

std::vector<std::vector<u64>>
get_state_num_steps(std::vector<char> const &directions,
                    desert_map_t const &desert_map,
                    std::vector<std::string> const &states) {
  std::size_t const dir_size{directions.size()};
  std::vector<std::vector<u64>> state_num_steps;
  for (auto state : states) {
    std::vector<u64> num_steps_vec;
    u64 num_steps{};
    std::unordered_set<State> visited;
    while (true) {
      auto find_it = desert_map.find(state);
      std::size_t idx = num_steps % dir_size;
      char dir = directions[idx];

      if (visited.contains({state, idx})) {
        break;
      }
      visited.insert({state, idx});

      // if we reached a final state, mark the number of steps
      if (state.back() == 'Z') {
        num_steps_vec.emplace_back(num_steps);
      }

      // advance state
      if (dir == 'L') {
        state = find_it->second.first;
      } else {
        state = find_it->second.second;
      }
      ++num_steps;
    }
    state_num_steps.emplace_back(std::move(num_steps_vec));
  }
  return state_num_steps;
}

bool
advance_indices(std::vector<std::size_t> &indices,
                std::vector<std::size_t> const &sizes) {
  for (std::size_t idx = 0; idx < indices.size(); ++idx) {
    if (indices[idx] + 1 < sizes[idx]) {
      ++indices[idx];
      return true;
    }
    indices[idx] = (indices[idx] + 1) % sizes[idx];
  }
  return false;
}

std::vector<u64>
collect_states(std::vector<std::size_t> const &indices,
               std::vector<std::vector<u64>> const &state_num_steps) {
  std::vector<u64> ret(indices.size());
  for (std::size_t idx = 0; idx < indices.size(); ++idx) {
    ret[idx] = state_num_steps[idx][indices[idx]];
  }
  return ret;
};

u64
lcm(std::vector<u64> &states) {
  u64 ret = std::lcm(states[0], states[1]);
  for (std::size_t idx = 2; idx < states.size(); ++idx) {
    ret = std::lcm(ret, states[idx]);
  }
  return ret;
}
} // namespace
