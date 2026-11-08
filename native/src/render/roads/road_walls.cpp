// The retaining walls of a road's cutting (Roadcraft 4.0 render/structures.ts retainingWalls, its behaviour; RoadSegment.cutWalls): a
// wall at each edge of the casing where the road runs in an open cutting, not at grade and not in a bore, with its coping over the
// ground it holds, closed at the ends of a run, and the backfill over the band the cutting was dug back, in the terrain's grass.
#include "render/roads/road_mesh.h"

#include "sim/terrain/road_shape.h"

#include <algorithm>
#include <cmath>
#include <vector>

using namespace godot;

namespace city {

namespace {
constexpr float BACKFILL_IN_M = 0.2f; // the backfill stops this far inside the dug band, where the ground comes back
constexpr float BACKFILL_OVER_M = 0.016f; // and lies this far over the ground it covers
const Vector3 UP(0, 1, 0);

using Section = WallSection;


// The natural ground behind a wall, past what the cutting dug.
float held_ground(const RoadEmit &e, const RoadSegment &g, const SegFrame &f, float side) {
	const Vector3 q = f.c + f.left * (side * (casing_half(e.s.catalog.profile(g.profile)) + cut_wall_reach() - BACKFILL_IN_M));
	return e.s.terrain.height_at(q.x, q.z);
}

// The runs of wall along one side, each as its sections.
std::vector<std::vector<Section>> make_runs(const RoadEmit &e, int sid, float side) {
	const RoadSegment &g = e.s.roads.segs[size_t(sid)];
	std::vector<std::vector<Section>> runs;
	const double v0 = g.retreat_a, v1 = g.len - g.retreat_b;
	const int steps = std::max(1, int(std::ceil((v1 - v0) / e.s.catalog.tune.structure_wall_step_m)));
	bool open = false;
	for (int i = 0; i <= steps; i++) {
		const SegFrame f = seg_frame(e.s, sid, v0 + (v1 - v0) * i / steps);
		if (!walled_at(e, g, f, side)) {
			open = false;
			continue;
		}
		if (!open) runs.emplace_back();
		open = true;
		runs.back().push_back({ f.c + f.left * (side * casing_half(e.s.catalog.profile(g.profile))), f.left * side, f.c.y, held_ground(e, g, f, side) });
	}
	return runs;
}

// The segment's runs of wall on both sides for the city's revision (road_shapes.h): the walls and the backfill read the same runs.
std::shared_ptr<const SegWalls> walls_of(const RoadEmit &e, int sid) {
	return e.shapes.walls.of(e.s.revision, sid, [&]() {
		auto walls = std::make_shared<SegWalls>();
		walls->runs[0] = make_runs(e, sid, -1.f);
		walls->runs[1] = make_runs(e, sid, 1.f);
		return std::shared_ptr<const SegWalls>(std::move(walls));
	}, [](const SegWalls &) { return size_t(1); });
}

const std::vector<std::vector<Section>> &runs_on(const SegWalls &w, float side) { return w.runs[side > 0 ? 1 : 0]; }

Vector3 point(const Section &q, float out, float y) { return Vector3(q.at.x + q.out.x * out, y, q.at.z + q.out.z * out); }

// The end of a run: the backfill closed down to the road.
void close_run(const RoadEmit &e, const Section &q, const Vector3 &away, float foot, float coping) {
	const float back = cut_wall_reach() - BACKFILL_IN_M;
	structure_face(e.mb, point(q, 0, q.deck - foot), point(q, back, q.deck - foot), point(q, back, q.top + BACKFILL_OVER_M), point(q, 0, q.top + coping), structure_concrete(), away);
}
} // namespace

bool walled_at(const RoadEmit &e, const RoadSegment &g, const SegFrame &f, float side) {
	if (!(g.flags & SEG_CUT_WALLS)) return false;
	const Tuning &tn = e.s.catalog.tune;
	const float lift = f.c.y - held_ground(e, g, f, side);
	const float bore = tn.grade_tunnel_headroom_m + tn.grade_tunnel_arch_m + tn.structure_bore_over_m;
	return !is_raised(structure_of(g.flags)) && lift <= -tn.economy_fill_from_m && lift >= -bore;
}

void emit_retaining_walls(const RoadEmit &e, int sid) {
	const Tuning &tn = e.s.catalog.tune;
	const float foot = tn.structure_wall_foot_m, coping = tn.structure_wall_coping_m, thick = tn.structure_wall_thick_m;
	const Color concrete = structure_concrete();
	if (!(e.s.roads.segs[size_t(sid)].flags & SEG_CUT_WALLS)) return;
	const std::shared_ptr<const SegWalls> walls = walls_of(e, sid);
	for (const float side : { 1.f, -1.f })
		for (const std::vector<Section> &run : runs_on(*walls, side)) {
			if (run.size() < 2) continue;
			close_run(e, run.front(), run.front().at - run[1].at, foot, coping);
			close_run(e, run.back(), run.back().at - run[run.size() - 2].at, foot, coping);
			for (size_t k = 0; k + 1 < run.size(); k++) {
				const Section &a = run[k], &b = run[k + 1];
				const Vector3 outward = (a.out + b.out).normalized();
				structure_face(e.mb, point(a, 0, a.deck - foot), point(b, 0, b.deck - foot), point(b, 0, b.top + coping), point(a, 0, a.top + coping), concrete, -outward);
				structure_face(e.mb, point(a, 0, a.top + coping), point(b, 0, b.top + coping), point(b, thick, b.top + coping), point(a, thick, a.top + coping), concrete, UP);
				structure_face(e.mb, point(a, thick, a.top + coping), point(b, thick, b.top + coping), point(b, thick, b.top), point(a, thick, a.top), concrete, outward);
			}
		}
}

void emit_wall_backfill(const RoadEmit &e, int sid) {
	if (!(e.s.roads.segs[size_t(sid)].flags & SEG_CUT_WALLS)) return;
	const float thick = e.s.catalog.tune.structure_wall_thick_m, back = cut_wall_reach() - BACKFILL_IN_M;
	const std::shared_ptr<const SegWalls> walls = walls_of(e, sid);
	for (const float side : { 1.f, -1.f })
		for (const std::vector<Section> &run : runs_on(*walls, side))
			for (size_t k = 0; k + 1 < run.size(); k++) {
				const Section &a = run[k], &b = run[k + 1];
				e.mb.quad(point(a, thick, a.top + BACKFILL_OVER_M), point(b, thick, b.top + BACKFILL_OVER_M), point(b, back, b.top + BACKFILL_OVER_M), point(a, back, a.top + BACKFILL_OVER_M), Color(1, 1, 1), UP);
			}
}

} // namespace city
