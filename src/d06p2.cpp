#include "utility.hpp"

#include <print> // std::println
#include <ranges> // std::views::zip

namespace
{
void
tests();
u64
get_prod_of_num_ways_to_win(std::vector<std::string> const &lines);
static std::pair<u64, u64>
parse_time_and_distance(std::vector<std::string> const &lines);
u64
num_ways_to_win(u64 const time, u64 const distance);
} // namespace

int
main(int argc, char const **argv) {
  tests();
  std::vector<std::string> lines = read_program_input(argc, argv);
  std::println("{}", get_prod_of_num_ways_to_win(lines));
  return 0;
}

namespace
{
void
tests() {
  using namespace std::literals::string_view_literals;
  std::vector<std::string> const lines{
      "Time:      7  15   30",
      "Distance:  9  40  200",
  };
  ASSERT(get_prod_of_num_ways_to_win(lines) == 71503);
}

u64
get_prod_of_num_ways_to_win(std::vector<std::string> const &lines) {
  auto [time, distance] = parse_time_and_distance(lines);
  return num_ways_to_win(time, distance);
}

std::pair<std::uint64_t, std::uint64_t>
parse_time_and_distance(std::vector<std::string> const &lines) {
  std::vector<std::uint64_t> times = split(split(lines[0], "Time:")[0], " ")
                                     | std::views::transform(str_to_int<u64>)
                                     | std::ranges::to<std::vector>();

  std::vector<std::uint64_t> distances =
      split(split(lines[1], "Distance:")[0], " ")
      | std::views::transform(str_to_int<u64>) | std::ranges::to<std::vector>();

  std::uint64_t time{times[0]};
  std::uint64_t distance{distances[0]};
  for (std::size_t idx{1}; idx < times.size(); ++idx) {
    time = time * pow10(get_num_digits(times[idx])) + times[idx];
    distance = distance * pow10(get_num_digits(distances[idx])) + distances[idx];
  }
  return {time, distance};
}

u64
num_ways_to_win(u64 const time, u64 const distance) {
  u64 num_ways{};
  for (u64 t{1}; t < time; ++t) {
    if (t * (time - t) > distance) {
      ++num_ways;
    }
  }
  return num_ways;
}
} // namespace
