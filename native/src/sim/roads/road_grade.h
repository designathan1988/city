// The height of every road (Roadcraft 4.0 world/elevation.ts, its behaviour, not its code): each segment a profile of
// stations every STATION_M at most, solved from the natural ground under it (the mean across the casing, smoothed and
// held to the ground grade), flat over its junction plates at the height its node settles on, with ramps, decks and
// bores where it is built raised or sunken; the nodes settle in rounds until none rises. Only what changed since the last
// solve is solved again (the segments and nodes the network marks, the ground under the segments a stroke reached).
// The ground is handed in (sim/roads does not include sim/terrain): the natural ground, water standing as a floor.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

namespace city {

class JobPool;
class RoadNetwork;
struct Profile;
struct Tuning;

// How a segment is built (Roadcraft 4.0 world/structures.ts RoadStructure): saved in its flags.
enum class Structure : uint8_t { GROUND = 0, ELEVATED = 1, BRIDGE = 2, TUNNEL = 3 };
constexpr bool is_raised(Structure s) { return s == Structure::ELEVATED || s == Structure::BRIDGE; }
constexpr bool is_sunken(Structure s) { return s == Structure::TUNNEL; }
constexpr size_t STRUCTURE_COUNT = 4;

// What a structure is (world/structures.ts ROAD_STRUCTURES), the one owner of its figures, read from the tuning (grade):
// clearance, the deck surface over the ground under it (negative: a tunnel's depth under it); deck, the structure under
// the asphalt; supports, whether it stands on bents and piers.
struct StructureSpec {
	float clearance = 0;
	float deck = 0;
	bool supports = false;
};
StructureSpec structure_spec(Structure s, const Tuning &tn);

// The ground a road is laid over at (x, z): the natural height, water standing as a floor.
using GroundAt = std::function<float(float x, float z)>;

// Modelling constants of the solve, the 4.0's (world/elevation.ts): metres, or shares.
constexpr float STATION_M = 1.6f; // STATION: the most a profile's stations stand apart
constexpr int MAX_STATIONS = 400; // MAX_STATIONS: a road of more is read coarser, never longer
constexpr float PLATE_MARGIN_M = 2.f; // PLATE_MARGIN: a station and 0.4 m past the junction's casing
constexpr float CASING_BAND_M = 1.f; // the band past the footway the casing holds
// Half the casing: the road's widest side, footway included, and the casing band past it.
float casing_half(const Profile &p);
constexpr float FOOTWAY_RISE_M = 0.15f; // roadTypes.ts FOOTWAY_RISE: the kerb a footway stands on
constexpr float UNDER_DECK_ROOM_M = 0.4f; // solveRaised: nothing comes closer than this under a deck's structure

// A segment's solved profile: heights at arc i * step from its a node (h), the ground read under it (ceil: the highest
// across the casing, dilated, plus the surface clearance; base: the mean, smoothed and graded, the line it is designed
// on), and the plates flat at each end.
constexpr int CURVE_NUMBERS = 6; // what the curve of a segment is: its two ends and its control point
// How a road's paving ends at a node (render/roads/road_junction.cpp each_shape): met by other roads, cut straight across (a dead
// end), or round a turning bulb.
enum class RoadEndKind : uint8_t { JOINT, CUT, BULB };

struct GradeProfile {
	bool valid = false;
	Structure structure = Structure::GROUND;
	bool manual = false; // a node of it has a height of its own (lift): an authored alignment
	bool walls = false; // its cutting is held by retaining walls
	RoadEndKind ends[2] = { RoadEndKind::JOINT, RoadEndKind::JOINT }; // how its paving ends at a and at b (the ground's shape reads it)
	int a = -1, b = -1;
	float len = 0, step = 0, half = 0, plate_a = 0, plate_b = 0;
	std::vector<float> h, ceil, base, raw; // raw: the mean ground across the casing, before base is made of it
	float ylo = 0, yhi = 0; // the lowest and highest of h, as the solve left it
	// what the ground was read along: kept while the curve, the casing and the ground under it stand
	double q[CURVE_NUMBERS] = {};
	float read_half = -1;
};

// What a node settled on in the last solve: kept whole while none of its roads changed.
struct GradeNode {
	float height = 0; // where it stands
	float start = 0; // where it stood before the rounds (equal: no road carried it up)
	float grade = 0; // its grade height: the mean grade line of its roads at grade at their plates, and its lift
	float cap = 0; // the most a ramp from a land end lets an aloft node rise
	bool aloft = false, capped = false, known = false;
};

class RoadGrade {
public:
	std::vector<GradeProfile> prof; // by segment id
	std::vector<GradeNode> node; // by node id

	// What changed since the last solve (RoadNetwork calls these on every edit).
	void seg_changed(int s) { segs_.push_back(s); }
	void node_changed(int n) { nodes_.push_back(n); }
	void ground_changed(int s) { reground_.push_back(s); }
	void everything() { all_ = true; }
	bool pending() const { return all_ || !segs_.empty() || !nodes_.empty() || !reground_.empty(); }

	// Solves what changed. `moved`: the segments whose heights changed (their surfaces, lanes and ground follow);
	// `gone`: the segments solved before that are no longer alive.
	void solve(RoadNetwork &net, const GroundAt &ground, std::vector<int> &moved, std::vector<int> &gone, JobPool *jobs = nullptr);

	bool has(int s) const { return s >= 0 && s < int(prof.size()) && prof[size_t(s)].valid; }
	// The deck height of segment s at arc `at` from its a node (linear between stations, continuous).
	double at(int s, double arc) const;

	// A solve's working state by segment and by node, kept between solves at its resting values (every entry a solve
	// writes is put back after it), so a solve costs what it reaches, not the size of the network.
	struct Scratch {
		std::vector<char> touched, regrounded, seg_solved, chained, node_solved, aloft;
		std::vector<float> height, start, grade, cap;
		std::vector<int> heighted; // the nodes whose height this solve wrote
		void grow(size_t segs, size_t nodes);
	};

private:
	std::vector<int> segs_, nodes_, reground_;
	bool all_ = false;
	Scratch scratch_;
	// By segment: its heights moved since they were last told (moved), by less than a station's rounding.
	std::vector<char> unshown_;
};

} // namespace city
