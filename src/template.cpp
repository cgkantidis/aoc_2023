#include "utility.hpp"

#include <print> // std::println

namespace
{
void
tests();
u64
get_num_lines(std::vector<std::string> const &lines);
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
  std::vector<std::string> const lines{
      "line1",
      "line2",
  };
  ASSERT(get_num_lines(lines) == 2);
}

u64
get_num_lines(std::vector<std::string> const &lines) {
  return lines.size();
}
} // namespace
