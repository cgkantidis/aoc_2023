#include "utility.hpp"

#include <print> // std::println
#include <ranges> // std::views::zip

namespace
{
void
tests();
u64
get_prod_of_num_ways_to_win(std::vector<std::string> const &lines);
static std::pair<std::vector<u64>, std::vector<u64>>
parse_times_and_distances(std::vector<std::string> const &lines);
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
  ASSERT(get_prod_of_num_ways_to_win(lines) == 288);
}

u64
get_prod_of_num_ways_to_win(std::vector<std::string> const &lines) {
  auto [times, distances] = parse_times_and_distances(lines);
  u64 product{1};
  for (auto const &[time, distance] : std::views::zip(times, distances)) {
    product *= num_ways_to_win(time, distance);
  }
  return product;
}

std::pair<std::vector<u64>, std::vector<u64>>
parse_times_and_distances(std::vector<std::string> const &lines) {
  return {split(split(lines[0], "Time:")[0], " ")
              | std::views::transform(str_to_int<u64>)
              | std::ranges::to<std::vector>(),
          split(split(lines[1], "Distance:")[0], " ")
              | std::views::transform(str_to_int<u64>)
              | std::ranges::to<std::vector>()};
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
