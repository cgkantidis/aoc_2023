#include "utility.hpp"

#include <algorithm> // std::ranges::all_of
#include <print> // std::println
#include <ranges> // std::views::transform
#include <stack> // std::stack

using i64 = std::int64_t;

namespace
{
void
tests();
i64
get_sum_of_extrapolated_values(std::vector<std::string> const &lines);
i64
extrapolate_last_value(std::vector<i64> const &values);
std::vector<i64>
get_diffs(std::vector<i64> const &values);
bool
all_zeros(std::vector<i64> const &values);
} // namespace

int
main(int argc, char const **argv) {
  tests();
  std::vector<std::string> lines = read_program_input(argc, argv);
  std::println("{}", get_sum_of_extrapolated_values(lines));
  return 0;
}

namespace
{
void
tests() {
  std::vector<std::string> const lines{
      "0 3 6 9 12 15",
      "1 3 6 10 15 21",
      "10 13 16 21 30 45",
  };
  ASSERT(get_sum_of_extrapolated_values(lines) == 114);
}

i64
get_sum_of_extrapolated_values(std::vector<std::string> const &lines) {
  i64 total = 0;
  for (auto const &line : lines) {
    auto values = split(line) | std::views::transform(str_to_int<i64>)
                  | std::ranges::to<std::vector<i64>>();
    total += extrapolate_last_value(values);
  }
  return total;
}

i64
extrapolate_last_value(std::vector<i64> const &values) {
  std::stack<std::vector<i64>> values_stack;
  values_stack.push(values);

  while (!all_zeros(values_stack.top())) {
    values_stack.push(get_diffs(values_stack.top()));
  }
  while (values_stack.size() > 1) {
    auto old_top = std::move(values_stack.top());
    values_stack.pop();
    std::vector<i64> &top = values_stack.top();
    top.emplace_back(top.back() + old_top.back());
  }
  return values_stack.top().back();
}

std::vector<i64>
get_diffs(std::vector<i64> const &values) {
  std::vector<i64> diffs(values.size() - 1);
  for (std::size_t idx = 0, end = values.size() - 1; idx < end; ++idx) {
    diffs[idx] = values[idx + 1] - values[idx];
  }
  return diffs;
}

bool
all_zeros(std::vector<i64> const &values) {
  return std::ranges::all_of(values, [](i64 const &val) { return val == 0; });
}

} // namespace
