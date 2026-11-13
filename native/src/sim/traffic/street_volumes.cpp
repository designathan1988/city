// The street's pieces in volume (street_volumes.h): the kept furniture of the segments asked about, then the plates of their junctions,
// the player's and the streets' names.
#include "sim/traffic/street_volumes.h"

#include "sim/street/street_spots.h"
#include "sim/traffic/ped_system.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <limits>
#include <set>

namespace city {

namespace {

constexpr float NO_BOUND = std::numeric_limits<float>::max();

struct Box {
	float x0, z0, x1, z1;
	bool has(const V2 &p) const { return p.x >= x0 && p.x <= x1 && p.y >= z0 && p.y <= z1; }
};

// A plate as a piece: its post's foot and its boxes (sign_boxes), its SignType as its station.
StreetPiece plate_piece(const CityState &s, const SignStand &st) {
	const float post = s.catalog.furniture.signs.post_radius_m;
	StreetPiece piece;
	piece.spot.kind = FurnitureKind::SIGN;
	piece.spot.station = int(st.type);
	piece.body = StandingBody{ V2(st.x, st.z), V2(1, 0), post, post };
	sign_boxes(s.catalog.furniture, st.type, piece.body.at, V2(st.face_x, st.face_z), piece.boxes);
	return piece;
}

// The pieces segment `sid` keeps within the box, and its junctions into `nodes`.
void segment_pieces(const CityState &s, int sid, const Box &box, std::vector<FurnitureSpot> &spots, std::set<int> &nodes, std::vector<StreetPiece> &out) {
	const RoadSegment &g = s.roads.segs[size_t(sid)];
	if (!g.alive) return;
	nodes.insert(g.a);
	nodes.insert(g.b);
	spots.clear();
	traffic::street_furniture_of(s, sid, FURNITURE_ALL, spots);
	for (const FurnitureSpot &p : spots) {
		if (p.kind == FurnitureKind::SIGN) continue; // a plate's post: its plate stands below, with the post
		StreetPiece piece{ p, body_of(s.roads, p), {} };
		if (!box.has(piece.body.at)) continue;
		body_boxes(s.roads, s.catalog.furniture, p, piece.boxes);
		out.push_back(std::move(piece));
	}
}

// The plates within the box: the rules' at `nodes`, the player's, the streets' names (in the order of their items).
void plates_in(const CityState &s, const std::set<int> &nodes, const Box &box, std::vector<StreetPiece> &out) {
	std::vector<SignStand> stands;
	for (int n : nodes)
		if (s.roads.nodes[size_t(n)].alive) traffic::junction_signs(s, n, stands);
	for (const StreetItem &it : s.street.items.items)
		if (it.alive && it.kind == ItemKind::SIGN) placed_sign(s.roads, it, stands);
	std::vector<int> named;
	for (const auto &entry : s.street.names.by_item) named.push_back(entry.first);
	std::sort(named.begin(), named.end());
	for (int id : named) {
		const std::vector<SignStand> &plates = s.street.names.by_item.at(id).plates;
		stands.insert(stands.end(), plates.begin(), plates.end());
	}
	for (const SignStand &st : stands)
		if (box.has(V2(st.x, st.z))) out.push_back(plate_piece(s, st));
}

} // namespace

void street_pieces_in(const CityState &s, float x0, float z0, float x1, float z1, std::vector<StreetPiece> &out) {
	const Box box{ x0, z0, x1, z1 };
	const float reach = s.catalog.widest_half(); // a piece stands within its segment's half width of its centre line
	std::vector<FurnitureSpot> spots;
	std::set<int> nodes;
	s.roads.for_segments_near(x0 - reach, z0 - reach, x1 + reach, z1 + reach, [&](int sid) { segment_pieces(s, sid, box, spots, nodes, out); });
	plates_in(s, nodes, box, out);
}

void street_pieces_all(const CityState &s, std::vector<StreetPiece> &out) {
	const Box all{ -NO_BOUND, -NO_BOUND, NO_BOUND, NO_BOUND };
	std::vector<FurnitureSpot> spots;
	std::set<int> nodes;
	for (int sid = 0; sid < int(s.roads.segs.size()); sid++) segment_pieces(s, sid, all, spots, nodes, out);
	for (int n = 0; n < int(s.roads.nodes.size()); n++) nodes.insert(n); // every junction's rules, as the audit has always read them
	plates_in(s, nodes, all, out);
}

} // namespace city
