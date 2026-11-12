// The street lamps' pools of light (lamp_pools.h): where each lit lamp's falls, and the superchunks that draw them.
#include "render/props/lamp_pools.h"

#include "render/owned_segments.h"
#include "render/props/prop_renderer.h"
#include "render/roads/road_cut.h"
#include "render/roads/road_mesh.h"
#include "sim/roads/street_furniture.h"
#include "sim/street/footway.h"

#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr double GRADE_RUN_M = 1.0; // the street's grade under a pool, read this far either side of its lamp
constexpr float HALF_BOX = 0.5f; // the unit box spans this either way of its middle
constexpr float WHOLE = 2.f; // a box's side over its half-axis
constexpr int ROW_ORIGIN_X = 3, ROW_ORIGIN_Y = 7, ROW_ORIGIN_Z = 11; // a row's origin: the transform's last column, row by row (mm_row_plain)

// A pool `out` from its lamp's foot along `toward` (horizontal, toward the road), on the street's top at `ground`, its along axis the
// street's way `along` at `grade` (rise a metre), its up axis square to both: the three square to each other and turning as x, y, z do
// (the shader reads a point's place along each by itself).
LampPoolPlace pool_place(const Vector3 &foot, float ground, const Vector3 &toward, const Vector3 &along, float grade, const LampPool &pool, float out,
		float slab, const Vector3 &lens) {
	LampPoolPlace p;
	const Vector3 mid = foot + toward * out;
	p.centre = Vector3(mid.x, ground, mid.z);
	Vector3 way = Vector3(along.x, grade, along.z).normalized();
	Vector3 up = way.cross(toward).normalized();
	if (up.y < 0) { // the pool is the same either way along: its axis turned so up is up
		way = -way;
		up = -up;
	}
	p.across = toward * pool.across;
	p.up = up * slab;
	p.along = way * pool.along;
	p.lens = lens;
	return p;
}

// Each whole chunk of the square [x0, x0 + size) x [z0, z0 + size), `fn(chunk key)`.
template <typename F>
void each_chunk_in(float x0, float z0, float size, F &&fn) {
	const int across = int(std::lround(size / CHUNK)), i0 = int(std::floor(x0 / CHUNK)), j0 = int(std::floor(z0 / CHUNK));
	for (int j = 0; j < across; j++)
		for (int i = 0; i < across; i++) fn(cell_key(i0 + i, j0 + j));
}

// The pools of segment `sid`'s lit lamps: a column's across its street from its own face, a pole lamp's under its head, a corner
// pole's toward its junction.
void segment_pools(const CityView &s, const StreetLook &L, const FurnitureLook &F, int sid, std::vector<FurnitureSpot> &spots,
		std::vector<LampPoolPlace> &out) {
	spots.clear();
	furniture_of(s, sid, LIT_FURNITURE, spots);
	for (const FurnitureSpot &p : spots) {
		if (!p.lit) continue;
		const bool column = p.row != 0 || p.kind == FurnitureKind::COLUMN;
		Basis facing;
		const Vector3 foot = spot_world(s, L, p, &facing);
		const Vector3 toward = facing.get_column(2);
		const SegFrame f = seg_frame(s, sid, p.v);
		const bool corner = p.corner >= 0;
		const Vector3 along = corner ? Vector3(-toward.z, 0, toward.x) : f.fwd;
		const float grade = corner ? 0.f : float((s.roads.seg_y(sid, p.v + GRADE_RUN_M) - s.roads.seg_y(sid, p.v - GRADE_RUN_M)) / (2 * GRADE_RUN_M));
		const ArmReach lens = arm_reach(F, column);
		const LampPool pool = lamp_pool(s.roads, sid, column);
		out.push_back(pool_place(foot, f.c.y, toward, along, grade, pool, column ? pool.out : lens.reach, F.pool_slab, lens_at(foot, toward, lens)));
	}
}
} // namespace

void place_lamp_pools(const CityView &s, const StreetLook &L, const FurnitureLook &F, float x0, float z0, float size, std::vector<LampPoolPlace> &out) {
	std::vector<FurnitureSpot> spots;
	each_chunk_in(x0, z0, size, [&](int64_t chunk) { each_owned_segment(s, chunk, [&](int sid) { segment_pools(s, L, F, sid, spots, out); }); });
	const float x1 = x0 + size, z1 = z0 + size;
	s.street.poles_in(x0, z0, x1, z1, [&](int id) {
		const UtilityPole &pole = s.street.poles.items[size_t(id)];
		if (!pole.alive || !pole.lamp || pole.x < x0 || pole.x >= x1 || pole.z < z0 || pole.z >= z1) return;
		const PoleFoot at = pole_foot(s, L, id);
		const ArmReach lens = arm_reach(F, false);
		const Vector3 along(-at.toward.z, 0, at.toward.x);
		out.push_back(pool_place(at.foot, at.foot.y, at.toward, along, 0.f, lamp_pool(s.roads, at.seg, false), lens.reach, F.pool_slab, lens_at(at.foot, at.toward, lens)));
	});
}

namespace {
// The pools as MultiMesh rows: the unit box's basis the pool's whole axes, its origin the pool's middle; and the box round them all.
void lay_rows(const std::vector<LampPoolPlace> &pools, PoolRows &out) {
	for (size_t i = 0; i < pools.size(); i++) {
		const LampPoolPlace &p = pools[i];
		mm_push_plain(out.rows, p.across * WHOLE, p.up * WHOLE, p.along * WHOLE, p.centre, 0, 0, 0, 0);
		const float reach = std::max({ p.across.length(), p.up.length(), p.along.length() });
		const AABB round(p.centre - Vector3(reach, reach, reach), Vector3(reach, reach, reach) * WHOLE);
		out.bounds = i == 0 ? round : out.bounds.merge(round);
	}
}
} // namespace

bool LampPools::init(const RenderContext &ctx, const StreetLook &look, const FurnitureLook &furniture, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	surface_material_ = ctx.material("lamp_pool", errors);
	look_ = &look;
	furniture_ = &furniture;
	if (!errors.is_empty()) return false;
	MeshBuilder box;
	box.box(Vector3(-HALF_BOX, -HALF_BOX, -HALF_BOX), Vector3(HALF_BOX, HALF_BOX, HALF_BOX), Color(1, 1, 1, 1), Vector2(), true);
	box_ = box.bake(surface_material_).shared(); // every superchunk's draw of it is made when it streams in (mesh_draw.h)
	return true;
}

void LampPools::reset() {
	clear_queue();
	for (auto &kv : supers_) kv.second.free();
	supers_.clear();
}

// What a superchunk's pools read, copied under the map's shared lock: the roads of the segments its chunks own and the player's poles in
// it with the poles a span away (a pole's arms turn by its spans), the ground and the footways round them; each owned segment's lamps
// placed anew, nothing else of its furniture.
CityCut LampPools::gather(const CityState &s, int64_t super) const {
	const float x0 = float(key_x(super)) * SUPERCHUNK, z0 = float(key_y(super)) * SUPERCHUNK;
	return cut_city(
			s, CUT_ROADS | CUT_STREET,
			[&](const CityState &live) {
				CutBox box{ x0, z0, x0 + SUPERCHUNK, z0 + SUPERCHUNK };
				each_chunk_in(x0, z0, SUPERCHUNK, [&](int64_t chunk) {
					each_owned_segment(live, chunk, [&](int sid) {
						const RoadSegment &g = live.roads.segs[size_t(sid)];
						box.take(g.minx, g.minz, g.maxx, g.maxz);
					});
				});
				take_pole_reach(box, live.street, x0, z0, x0 + SUPERCHUNK, z0 + SUPERCHUNK);
				const float stand = float(footway_reach(live.roads, live.catalog.furniture.items.stand_reach_m));
				return box.grown(road_reach(live.catalog, *look_) + stand);
			},
			[&](CityCut &cut, const CityState &live) {
				each_chunk_in(x0, z0, SUPERCHUNK, [&](int64_t chunk) { each_owned_segment(live, chunk, [&](int sid) { keep_lamps(cut, live, sid); }); });
			});
}

void LampPools::submit(const CityState &s, int64_t super, JobPool &pool, float priority) {
	submit_cut(pool, JobKey{ JobLane::LAMP_POOLS, super }, super, priority, [this, &s, super]() { return gather(s, super); },
			[this, super](const CityCut &cut, MeshBuilder &, PoolRows &rows) {
				std::vector<LampPoolPlace> pools;
				place_lamp_pools(cut, *look_, *furniture_, float(key_x(super)) * SUPERCHUNK, float(key_y(super)) * SUPERCHUNK, SUPERCHUNK, pools);
				lay_rows(pools, rows);
			});
}

void LampPools::commit_one(Built &b) {
	MultiMeshDraw &d = supers_[b.key];
	d.set(scenario_, box_.rid, b.extra.rows, b.extra.bounds, false, LAYER_FURNITURE, false);
	d.visible(lit_);
}

void LampPools::drop_super(int64_t super) {
	forget(super);
	auto it = supers_.find(super);
	if (it == supers_.end()) return;
	it->second.free();
	supers_.erase(it);
}

void LampPools::lay_by_night(float night) {
	const bool lit = night > 0.f;
	if (lit == lit_) return;
	lit_ = lit;
	for (auto &kv : supers_) kv.second.visible(lit_);
}

void LampPools::drawn_near(float x, float z, float r, PackedVector3Array &out) const {
	if (!lit_) return; // by day none is drawn
	RenderingServer *rs = RenderingServer::get_singleton();
	for (const auto &kv : supers_) {
		const MultiMeshDraw &d = kv.second;
		if (!d.mm.is_valid() || d.count == 0) continue;
		const PackedFloat32Array rows = rs->multimesh_get_buffer(d.mm);
		for (int i = 0; i < d.count; i++) {
			const int64_t at = int64_t(i) * MM_PLAIN_STRIDE;
			const Vector3 middle(rows[at + ROW_ORIGIN_X], rows[at + ROW_ORIGIN_Y], rows[at + ROW_ORIGIN_Z]);
			if (std::hypot(middle.x - x, middle.z - z) <= r) out.push_back(middle);
		}
	}
}

Dictionary lamp_pool_census(const CityState &s, const StreetLook &L, const LampPools &pools, const PropRenderer &props, float x, float z, float r) {
	PackedVector3Array drawn, wanted, heads, lenses;
	std::vector<LampPoolPlace> places;
	{
		MapRead map(s);
		const CityView live(s);
		for_chunks(x - r, z - r, x + r, z + r, [&](int64_t key) {
			place_lamp_pools(live, L, props.furniture_look(), float(key_x(key)) * CHUNK, float(key_y(key)) * CHUNK, CHUNK, places);
		});
	}
	for (const LampPoolPlace &p : places) {
		wanted.push_back(p.centre);
		heads.push_back(p.lens);
	}
	props.lenses_near(x, z, r, lenses);
	pools.drawn_near(x, z, r, drawn);
	Dictionary d;
	d["drawn"] = drawn;
	d["wanted"] = wanted;
	d["heads"] = heads;
	d["lenses"] = lenses;
	return d;
}

} // namespace city
