#include "utility.hpp"

#include <algorithm> // std::ranges::fold_left
#include <numeric> // std::lcm
#include <print> // std::println
#include <regex> // std::regex
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set

using desert_map_t =
    std::unordered_map<std::string, std::pair<std::string, std::string>>;

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
get_num_steps_to_end(std::vector<std::string> const &lines);
std::pair<std::vector<char>, desert_map_t>
parse_map(std::vector<std::string> const &lines);
std::vector<std::vector<u64>>
get_state_num_steps(std::vector<char> const &directions,
                    desert_map_t const &desert_map,
                    std::vector<std::string> const &states);
std::vector<u64>
collect_states(std::vector<std::size_t> const &indices,
               std::vector<std::vector<u64>> const &state_num_steps);
bool
advance_indices(std::vector<std::size_t> &indices,
                std::vector<std::size_t> const &sizes);
u64
lcm(std::vector<u64> const &states);
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
  std::vector<std::string> const lines{
      "LR",
      "",
      "11A = (11B, XXX)",
      "11B = (XXX, 11Z)",
      "11Z = (11B, XXX)",
      "22A = (22B, XXX)",
      "22B = (22C, 22C)",
      "22C = (22Z, 22Z)",
      "22Z = (22B, 22B)",
      "XXX = (XXX, XXX)",
  };
  ASSERT(get_num_steps_to_end(lines) == 6);
}

u64
get_num_steps_to_end(std::vector<std::string> const &lines) {
  auto [directions, desert_map] = parse_map(lines);
  std::unordered_set<std::string> states_set;
  std::vector<std::string> states;
  for (auto const &[k, v] : desert_map) {
    if (k.back() == 'A') {
      if (!states_set.contains(k)) {
        states.emplace_back(k);
        states_set.insert(k);
      }
    }
  }

  std::vector<std::vector<u64>> state_num_steps =
      get_state_num_steps(directions, desert_map, states);

  std::vector<std::size_t> indices(state_num_steps.size());
  auto sizes = state_num_steps | std::views::transform(std::ranges::size)
               | std::ranges::to<std::vector<u64>>();

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
lcm(std::vector<u64> const &states) {
  return std::ranges::fold_left(states, 1ULL, std::lcm<u64, u64>);
}
} // namespace
