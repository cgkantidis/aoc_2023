#include <algorithm> // std::ranges::sort
#include <print> // std::println
#include <ranges> // std::views::enumerate

#include "utility.hpp"

struct mapping
{
  u64 dst;
  u64 src;
  u64 sz;
};

namespace
{
void
tests();
u64
get_min_location_for_seeds(std::vector<std::string> const &lines);
std::vector<std::span<std::string const>>
get_blocks(std::vector<std::string> const &lines);
std::vector<u64>
parse_seeds(std::span<std::string const> lines);
std::vector<u64>
parse_and_map(std::span<std::string const> lines,
              std::vector<u64> const &values);
u64
convert(u64 value, std::vector<mapping> const &map);
} // namespace

int
main(int argc, char const **argv) {
  tests();
  std::vector<std::string> lines = read_program_input(argc, argv);
  std::println("{}", get_min_location_for_seeds(lines));
  return 0;
}

namespace
{
void
tests() {
  std::vector<std::string> const lines{
      "seeds: 79 14 55 13",
      "",
      "seed-to-soil map:",
      "50 98 2",
      "52 50 48",
      "",
      "soil-to-fertilizer map:",
      "0 15 37",
      "37 52 2",
      "39 0 15",
      "",
      "fertilizer-to-water map:",
      "49 53 8",
      "0 11 42",
      "42 0 7",
      "57 7 4",
      "",
      "water-to-light map:",
      "88 18 7",
      "18 25 70",
      "",
      "light-to-temperature map:",
      "45 77 23",
      "81 45 19",
      "68 64 13",
      "",
      "temperature-to-humidity map:",
      "0 69 1",
      "1 0 69",
      "",
      "humidity-to-location map:",
      "60 56 37",
      "56 93 4",
  };
  ASSERT(get_min_location_for_seeds(lines) == 35);
}

u64
get_min_location_for_seeds(std::vector<std::string> const &lines) {
  auto const blocks = get_blocks(lines);
  std::vector<u64> const seeds = parse_seeds(blocks[0]);
  auto soil = parse_and_map(blocks[1], seeds);
  auto fertilizer = parse_and_map(blocks[2], soil);
  auto water = parse_and_map(blocks[3], fertilizer);
  auto light = parse_and_map(blocks[4], water);
  auto temperature = parse_and_map(blocks[5], light);
  auto humidity = parse_and_map(blocks[6], temperature);
  auto location = parse_and_map(blocks[7], humidity);
  return std::ranges::min(location);
}

std::vector<std::span<std::string const>>
get_blocks(std::vector<std::string> const &lines) {
  std::vector<std::span<std::string const>> spans;
  auto beg_it = lines.begin();
  while (true) {
    auto find_it =
        std::find_if(beg_it, lines.end(), [](std::string const &line) {
          return line.empty();
        });
    spans.emplace_back(beg_it, find_it);
    if (find_it == lines.end()) {
      break;
    }
    beg_it = std::next(find_it);
  }
  return spans;
}

std::vector<u64>
parse_seeds(std::span<std::string const> lines) {
  auto str_seeds = split(split(lines[0], "seeds: ")[0]);
  return std::views::transform(str_seeds, str_to_int<u64>)
         | std::ranges::to<std::vector<u64>>();
}

std::vector<u64>
parse_and_map(std::span<std::string const> lines,
              std::vector<u64> const &values) {
  std::vector<mapping> map;
  for (std::string const &line : lines) {
    auto tokens = split(line) | std::views::transform(str_to_int<u64>);
    map.emplace_back(tokens[0], tokens[1], tokens[2]);
  }

  std::ranges::sort(map, [](mapping const &left, mapping const &right) {
    return left.src < right.src;
  });

  return std::views::transform(
             values,
             [&map](u64 const &value) { return convert(value, map); })
         | std::ranges::to<std::vector<u64>>();
}

u64
convert(u64 value, std::vector<mapping> const &map) {
  for (mapping const &m : map) {
    if (value < m.src) {
      // value is not mapped
      return value;
    }
    if (value < m.src + m.sz) {
      return m.dst + value - m.src;
    }
  }
  return value;
}
} // namespace
