// A body fitted to a lot that is no rectangle (lot_outline.h).
#include "sim/buildings/b40/lot_outline.h"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace city::b40 {

namespace {
constexpr float WHOLE_SHARE = 0.97f; // a four-cornered ring that fills this share of its box is its rectangle
constexpr float FRONT_BAND_M = 1.2f; // a boundary piece along the front, this near it, is the front's: it stays
constexpr float PIECE_MOST_M = 20.f; // a run along a side is laid in pieces of this at most
constexpr float SIDE_LEAST_M = 0.5f; // a side shorter than this, past the boundary's thickness, takes no piece
constexpr float HALF = 0.5f;
constexpr uint64_t STAMP_START = 0xcbf29ce484222325ull, STAMP_MUL = 0x100000001b3ull; // FNV-1a, 64 bits

bool is_boundary(Element k) { return k == EL_WALL || k == EL_FENCE || k == EL_HEDGE || k == EL_RAILING; }
bool is_gate(Element k) { return k == EL_GATE || k == EL_CAR_GATE; }
// A piece that closes the lot's front: laid along it (wider than deep), within the band of it.
bool on_front(const LotElement &e) { return e.angle == 0 && e.w >= e.d && e.y < FRONT_BAND_M; }

// The ring is the rectangle it stands in: four corners that fill their box.
bool is_rectangle(const std::vector<Point2> &ring) {
	if (ring.size() != 4) return false;
	float x0 = ring[0].x, x1 = x0, y0 = ring[0].y, y1 = y0, twice = 0;
	for (size_t i = 0; i < ring.size(); i++) {
		const Point2 &p = ring[i], &q = ring[(i + 1) % ring.size()];
		x0 = std::min(x0, p.x); x1 = std::max(x1, p.x); y0 = std::min(y0, p.y); y1 = std::max(y1, p.y);
		twice += p.x * q.y - q.x * p.y;
	}
	return std::fabs(twice) * HALF > (x1 - x0) * (y1 - y0) * WHOLE_SHARE;
}

// The boundary along one side of the ring, a to c, as the side and back runs it replaces were: `like`'s kind, height, paint and
// thickness, in equal pieces, each short of the side's ends by half its thickness and set that far inside the line.
void run_along(std::vector<LotElement> &out, const LotElement &like, const Point2 &a, const Point2 &c, float inward) {
	const float thick = std::min(like.w, like.d), full = std::hypot(c.x - a.x, c.y - a.y);
	if (full < SIDE_LEAST_M + thick) return;
	const float ux = (c.x - a.x) / full, uy = (c.y - a.y) / full, len = full - thick;
	const int pieces = std::max(1, int(std::ceil(len / PIECE_MOST_M)));
	for (int k = 0; k < pieces; k++) {
		const float at = thick * HALF + (float(k) + HALF) / float(pieces) * len;
		LotElement e = like;
		e.x = a.x + ux * at - uy * inward * thick * HALF;
		e.y = a.y + uy * at + ux * inward * thick * HALF;
		e.w = len / float(pieces);
		e.d = thick;
		e.angle = std::atan2(uy, ux);
		e.facing = 0;
		out.push_back(e);
	}
}
} // namespace

bool in_ring(const std::vector<Point2> &ring, float x, float y) {
	bool in = false;
	for (size_t i = 0, j = ring.size() - 1; i < ring.size(); j = i++) {
		const Point2 &p = ring[i], &q = ring[j];
		if ((p.y > y) != (q.y > y) && x < (q.x - p.x) * (y - p.y) / (q.y - p.y) + p.x) in = !in;
	}
	return in;
}

uint64_t ring_stamp(const std::vector<Point2> &ring) {
	uint64_t h = STAMP_START;
	for (const Point2 &p : ring)
		for (float v : { p.x, p.y }) {
			uint32_t bits;
			std::memcpy(&bits, &v, sizeof bits);
			h = (h ^ bits) * STAMP_MUL;
		}
	return ring.empty() ? 0 : (h != 0 ? h : 1);
}

void fit_outline(Body &b, const std::vector<Point2> &ring) {
	if (ring.size() < 3 || is_rectangle(ring)) return;
	// the side and back runs of the plan's rectangle go; one of them says what the lot's own are made of
	const LotElement *like = nullptr;
	std::vector<LotElement> kept;
	for (const LotElement &e : b.elements) {
		const bool boundary = is_boundary(e.kind);
		if (boundary && !on_front(e)) {
			if (!like) like = &e;
			continue;
		}
		// the front's pieces and its gates stay where the plan set them; anything else stays only on the lot
		if (boundary || (is_gate(e.kind) && e.y < FRONT_BAND_M) || in_ring(ring, e.x, e.y)) kept.push_back(e);
	}
	if (like) {
		float twice = 0;
		for (size_t i = 0; i < ring.size(); i++) twice += ring[i].x * ring[(i + 1) % ring.size()].y - ring[(i + 1) % ring.size()].x * ring[i].y;
		const float inward = twice >= 0 ? 1.f : -1.f; // the lot lies on the left of its sides, or on their right
		const LotElement made = *like; // `like` points into the list replaced below
		for (size_t i = 1; i < ring.size(); i++) run_along(kept, made, ring[i], ring[(i + 1) % ring.size()], inward);
	}
	b.elements = std::move(kept);
	b.lot_ring = ring;
}

} // namespace city::b40
