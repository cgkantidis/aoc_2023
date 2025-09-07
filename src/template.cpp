#include "utility.hpp"

#include <print> // std::println

namespace
{
void
tests();
u64
get_num_lines(std::ranges::range auto &&lines);
} // namespace

int
main(int argc, char const **argv) {
  tests();
  std::vector<std::string> lines = read_program_input(argc, argv);
  std::println("{}", get_num_lines(lines));
  return 0;
}

namespace
{
void
tests() {
  using namespace std::literals::string_view_literals;
  auto const lines = std::array{
      "line1"sv,
      "line2"sv,
  };
  ASSERT(get_num_lines(lines) == 2);
}

u64
get_num_lines(std::ranges::range auto &&lines) {
  return lines.size();
}
} // namespace
