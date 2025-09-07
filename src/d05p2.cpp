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

struct range
{
  // range from [src, dst)
  u64 src;
  u64 dst;
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
std::vector<range>
get_seed_ranges(std::span<std::string const> lines);
std::vector<std::vector<mapping>>
get_mappings(std::vector<std::span<std::string const>> const &blocks);
std::vector<mapping>
parse_map(std::span<std::string const> lines);
std::vector<range>
tranform_range_by_mapping(range const &r, std::vector<mapping> const &mappings);
bool
is_overlapping(range const &r, mapping const &m);
range
convert(range const &r, mapping const &m);
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
  ASSERT(get_min_location_for_seeds(lines) == 46);
}

u64
get_min_location_for_seeds(std::vector<std::string> const &lines) {
  auto const blocks = get_blocks(lines);
  std::vector<range> seed_ranges = get_seed_ranges(blocks[0]);
  std::vector<std::vector<mapping>> mappings = get_mappings(blocks);

  for (std::vector<mapping> const &mapping_group : mappings) {
    std::vector<range> new_ranges;
    for (range const &r : seed_ranges) {
      new_ranges.append_range(tranform_range_by_mapping(r, mapping_group));
    }
    seed_ranges = std::move(new_ranges);
  }

  return std::ranges::min(seed_ranges, {}, [](range const &r) { return r.src; })
      .src;
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

std::vector<range>
get_seed_ranges(std::span<std::string const> lines) {
  auto str_seeds = split(split(lines[0], "seeds: ")[0]);
  return str_seeds | std::views::transform(str_to_int<u64>)
         | std::views::chunk(2) | std::views::transform([](auto const &chunk) {
             return range{chunk[0], chunk[0] + chunk[1], chunk[1]};
           })
         | std::ranges::to<std::vector<range>>();
}

std::vector<std::vector<mapping>>
get_mappings(std::vector<std::span<std::string const>> const &blocks) {
  return std::views::transform(std::views::drop(blocks, 1), parse_map)
         | std::ranges::to<std::vector<std::vector<mapping>>>();
}

std::vector<mapping>
parse_map(std::span<std::string const> lines) {
  std::vector<mapping> map;
  for (std::string const &line : std::views::drop(lines, 1)) {
    auto tokens = split(line) | std::views::transform(str_to_int<u64>);
    map.emplace_back(tokens[0], tokens[1], tokens[2]);
  }

  std::ranges::sort(map, [](mapping const &left, mapping const &right) {
    return left.src < right.src;
  });

  return map;
}

std::vector<range>
tranform_range_by_mapping(range const &r, std::vector<mapping> const &mappings) {
  auto const overlapping_mappings =
      std::views::filter(mappings,
                         [&r](mapping const &m) { return is_overlapping(r, m); })
      | std::ranges::to<std::vector<mapping>>();

  std::vector<range> new_ranges;
  for (auto const &[idx, m] : std::views::enumerate(overlapping_mappings)) {
    if (idx == 0) {
      // see if there is a left trailing part only for the first
      // any possible right trailing part will be handled later
      if (r.src < m.src) {
        u64 left_sz = m.src - r.src;
        new_ranges.emplace_back(r.src, r.src + left_sz, left_sz);
      }
    }
    new_ranges.emplace_back(convert(r, m));

    if (r.dst <= m.src + m.sz) {
      continue;
    }
    u64 src = m.src + m.sz;
    u64 sz = static_cast<u64>(idx) == overlapping_mappings.size() - 1
                 ? r.dst - src
                 : std::min(r.dst - src,
                            overlapping_mappings[static_cast<u64>(idx) + 1].src
                                - m.src - m.sz);
    new_ranges.emplace_back(src, src + sz, sz);
  }

  if (new_ranges.empty()) {
    return {r};
  }

  return new_ranges;
}

bool
is_overlapping(range const &r, mapping const &m) {
  return r.dst > m.src && r.src < m.src + m.sz;
};

range
convert(range const &r, mapping const &m) {
  u64 src = std::max(r.src, m.src);
  u64 sz = std::min(r.dst, m.src + m.sz) - src;
  return {.src = src + m.dst - m.src, .dst = src + sz + m.dst - m.src, .sz = sz};
}
} // namespace
