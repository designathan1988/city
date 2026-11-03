// Road profiles (cross-sections), Roadcraft src/roads/profiles.ts. Strips are listed from the right edge (negative u)
// to the left edge looking from node a to node b; right-hand traffic, lanes with dir +1 go a -> b on the -u side.
// The definitions live in project/data/catalog/road_profiles.json; derive_profile() computes what every system reads.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace city {

enum class StripKind : uint8_t { SIDEWALK, CURB, PARKING, BIKE, LANE, MEDIAN, SHOULDER, BARRIER, COUNT };
enum class Surface : uint8_t { ASPHALT = 0, CONCRETE = 1, COBBLE = 2, BALLAST = 3 };
enum class Paving : uint8_t { CONCRETE = 0, PAVERS = 1, STONE = 2, GRASS = 3 };
enum class LineKind : uint8_t { NONE = 0, SOLID = 1, DASHED = 2, DOUBLE = 3 };
enum class LineColor : uint8_t { WHITE = 0, YELLOW = 1 };

struct Strip {
	StripKind kind = StripKind::LANE;
	float width = 0;
	int dir = 0; // lanes: +1 a -> b, -1 b -> a
	bool bus = false, flush = false, rail = false, solid = false;
	Paving pave = Paving::CONCRETE;
	uint8_t turns = 0; // TurnBit: left 1, straight 2, right 4; 0 = automatic
};

struct StripEdge {
	float u0 = 0, u1 = 0;
	Strip strip;
};

struct MarkLine {
	float u = 0, width = 0;
	LineKind kind = LineKind::NONE;
	LineColor color = LineColor::WHITE;
};

struct LaneInfo {
	float u = 0, w = 0;
	bool bus = false, rail = false;
	uint8_t turns = 0;
};

struct RoadPrices { // R$ per m2 (data/catalog/road_profiles.json "prices")
	float carriageway = 0, median = 0, footway = 0;
};

struct Profile {
	std::string id, name_key, category, centre; // centre: "dashed" | "solid" | "double"
	std::vector<Strip> strips;
	bool one_way = false, ring = false, custom = false;
	float speed = 0; // km/h
	int priority = 0; // 0 local ... 5 highway, 6 rails
	Surface surface = Surface::ASPHALT;
	// derived (derive_profile)
	std::vector<StripEdge> edges;
	float right = 0, left = 0, asph_right = 0, asph_left = 0;
	std::vector<MarkLine> lines;
	std::vector<LaneInfo> fwd, back; // index 0 = the rightmost lane in that direction
	float walk_right = 0, walk_left = 0;
	bool has_walk_right = false, has_walk_left = false, rail = false;
	float cost_per_m = 0;
	bool walk() const { return has_walk_right || has_walk_left; }
	int lanes_fwd() const { return int(fwd.size()); }
	int lanes_back() const { return int(back.size()); }
	float half() const { return right > left ? right : left; }
};

constexpr int MAX_PROFILE_LINES = 8; // painted lines and lanes per profile

bool is_asphalt(StripKind k);
void derive_profile(Profile &p, const RoadPrices &prices);

} // namespace city
