#include <array> // std::array
#include <cstdint> // std::uint64_t
#include <fmt/core.h> // fmt::print
#include <fstream> // std::ifstream
#include <libassert/assert.hpp> // ASSERT
#include <ranges> // std::views::enumerate
#include <string> // std::string
#include <vector> // std::vector

using u64 = std::uint64_t;

using namespace std::literals::string_view_literals;

namespace
{
void
tests();
u64
get_answer(std::ranges::range auto &&lines);
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
        ""sv,
    };
    ASSERT(get_answer(lines) == 42);
  }
}

u64
get_answer(std::ranges::range auto &&lines) {
  return lines.size();
}
} // namespace

