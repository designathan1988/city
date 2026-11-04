// Tracks in their layers (ballast, sleepers, rails), a platform with its roof at each station, a shelter at each bus stop, a way down at
// each metro station; a car is a box with its windows and the line's stripe of paint. See transit_renderer.h.
#include "render/transit/transit_renderer.h"

#include "core/color.h"
#include "render/agents/people_renderer.h"
#include "render/mesh_builder.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float HALF = 0.5f, AABB_HALF_H_M = 500.f, FAR_M = 1e9f;
constexpr int RGB_BITS = 8;
constexpr uint32_t OPAQUE = 0xFFu;

Color hex(const Variant &v) {
	const uint32_t c = parse_hex_color(String(v).utf8().get_data());
	return Color::hex(c);
}

Vector3 v3(const Variant &v) {
	const Array a = v;
	return Vector3(float(a[0]), float(a[1]), float(a[2]));
}

struct Ground {
	const CityState &s;
	float at(const V2 &p) const { return s.terrain.height_at(p.x, p.y); }
	Vector3 on(const V2 &p, float up) const { return Vector3(p.x, at(p) + up, p.y); }
};

// A flat strip along a-b, `half` either side, `up` over the ground, facing up.
void strip(MeshBuilder &mb, const Ground &g, const V2 &a, const V2 &b, float half, float up, const Color &c) {
	const V2 d = (b - a).norm(), n(-d.y, d.x);
	mb.quad(g.on(a + n * half, up), g.on(b + n * half, up), g.on(b - n * half, up), g.on(a - n * half, up), c, Vector3(0, 1, 0));
}

void track_mesh(MeshBuilder &mb, const Ground &g, const RailTrack &t, const Dictionary &look) {
	const float gauge = look["gauge_m"], step = look["sleeper_step_m"];
	const Vector3 ballast = v3(look["lay_m"]), sleeper = v3(look["sleeper_m"]), rail = v3(look["rail_m"]); // half width, height, along
	const Color cb = hex(look["ballast"]), cs = hex(look["sleeper"]), cr = hex(look["rail"]);
	for (size_t i = 1; i < t.points.size(); i++) {
		const V2 &a = t.points[i - 1], &b = t.points[i], d = (b - a).norm(), n(-d.y, d.x);
		strip(mb, g, a, b, ballast.x, ballast.y, cb);
		for (int side : { -1, 1 }) strip(mb, g, a + n * (float(side) * gauge * HALF), b + n * (float(side) * gauge * HALF), rail.x, rail.y, cr);
		const float len = (b - a).len();
		for (float at = step * HALF; at < len; at += step) {
			const V2 p = a + d * at;
			strip(mb, g, p - d * (sleeper.z * HALF), p + d * (sleeper.z * HALF), sleeper.x, sleeper.y, cs);
		}
	}
}

// A box on the ground, `size` [long, high, wide], its long side along `dir`.
void block(MeshBuilder &mb, const Ground &g, const V2 &at, const V2 &dir, const Vector3 &size, float up, const Color &c) {
	const V2 d = dir.norm() * (size.x * HALF), n = V2(-dir.norm().y, dir.norm().x) * (size.z * HALF);
	const float y0 = g.at(at) + up, y1 = y0 + size.y;
	const V2 q[4] = { at - d - n, at + d - n, at + d + n, at - d + n };
	for (int i = 0; i < 4; i++) {
		const V2 &p = q[i], &r = q[(i + 1) % 4], e = (r - p), out(e.y, -e.x);
		mb.quad(Vector3(p.x, y0, p.y), Vector3(r.x, y0, r.y), Vector3(r.x, y1, r.y), Vector3(p.x, y1, p.y), c, Vector3(out.x, 0, out.y));
	}
	mb.quad(Vector3(q[0].x, y1, q[0].y), Vector3(q[1].x, y1, q[1].y), Vector3(q[2].x, y1, q[2].y), Vector3(q[3].x, y1, q[3].y), c, Vector3(0, 1, 0));
}

V2 stop_dir(const CityState &s, const TransitStop &q) {
	if (q.track >= 0)
		for (const RailTrack &t : s.transit.tracks) {
			if (t.id != q.track) continue;
			V2 best(1, 0);
			float dist = FAR_M;
			for (size_t i = 1; i < t.points.size(); i++) {
				const float d = ((t.points[i] + t.points[i - 1]) * HALF - q.at).len();
				if (d < dist) {
					dist = d;
					best = (t.points[i] - t.points[i - 1]).norm();
				}
			}
			return best;
		}
	if (q.seg >= 0 && size_t(q.seg) < s.roads.segs.size()) {
		const RoadSegment &r = s.roads.segs[size_t(q.seg)];
		double t, dist, dx, dz;
		closest_t(r.q, q.at.x, q.at.y, t, dist);
		r.q.deriv(t, dx, dz);
		return V2(float(dx), float(dz)).norm();
	}
	return V2(1, 0);
}

void stop_mesh(MeshBuilder &mb, const Ground &g, const CityState &s, const TransitStop &q, const Dictionary &look) {
	const V2 dir = stop_dir(s, q), n(-dir.y, dir.x);
	if (q.mode == TransitMode::BUS) {
		const Vector3 shelter = v3(look["shelter_m"]), post = v3(look["shelter_post_m"]);
		block(mb, g, q.at, dir, Vector3(shelter.x, post.y, post.z), 0, hex(look["shelter_wall"]));
		block(mb, g, q.at, dir, shelter, post.y, hex(look[q.terminal ? "terminal_roof" : "shelter_roof"]));
		return;
	}
	if (q.mode == TransitMode::METRO) {
		if (q.has_entrance) block(mb, g, q.entrance, dir, v3(look["entrance_m"]), 0, hex(look["entrance"]));
		return;
	}
	const Vector3 p = v3(look["platform_m"]), roof = v3(look["platform_roof_m"]);
	const float off = float(look["platform_off_m"]);
	for (int side : { -1, 1 }) {
		const V2 at = q.at + n * (float(side) * off);
		block(mb, g, at, dir, p, 0, hex(look["platform"]));
		block(mb, g, at, dir, roof, float(look["platform_roof_up_m"]), hex(look["platform_roof"]));
	}
}

void car_mesh(MeshBuilder &mb, const Dictionary &look) {
	const Vector3 c = v3(look["car_m"]); // long, high, wide
	const float l = c.x * HALF, w = c.z * HALF, y = float(look["car_floor_m"]), band = float(look["car_window_low"]), top = float(look["car_window_high"]);
	const Color glass = hex(look["car_window"]), dark = hex(look["car_under"]);
	mb.box(Vector3(-w, y, -l), Vector3(w, y + c.y, l), Color(1, 1, 1));
	mb.box(Vector3(-w, 0, -l), Vector3(w, y, l), dark);
	const float g0 = y + c.y * band, g1 = y + c.y * top, proud = float(look["car_window_proud_m"]);
	for (int side : { -1, 1 }) mb.box(Vector3(side * (w + proud) - proud * HALF, g0, -l * band), Vector3(side * (w + proud) + proud * HALF, g1, l * band), glass);
}
} // namespace

bool TransitRenderer::init(const RenderContext &ctx, PackedStringArray &errors) {
	reset();
	scenario_ = ctx.scenario;
	material_ = ctx.material("transit", errors);
	look_ = ctx.table("transit", errors);
	if (!errors.is_empty()) return false;
	radius_ = look_["radius_m"];
	MeshBuilder mb;
	car_mesh(mb, look_);
	RenderingServer *rs = RenderingServer::get_singleton();
	car_.reset();
	car_.rid = rs->mesh_create();
	mb.commit_to(car_.rid);
	rs->mesh_surface_set_material(car_.rid, 0, material_);
	return true;
}

void TransitRenderer::reset() {
	RenderingServer *rs = RenderingServer::get_singleton();
	if (instance_.is_valid()) rs->free_rid(instance_);
	instance_ = RID();
	plan_.reset();
	cars_.free();
	plan_seen_ = ~uint64_t(0);
	drawn_cars_ = 0;
}

void TransitRenderer::sync(const CityState &s) {
	if (s.transit_revision == plan_seen_ || !material_.is_valid()) return;
	plan_seen_ = s.transit_revision;
	RenderingServer *rs = RenderingServer::get_singleton();
	MeshBuilder mb;
	const Ground g{ s };
	for (const RailTrack &t : s.transit.tracks)
		if (t.mode == TransitMode::TRAIN) track_mesh(mb, g, t, look_);
	for (const TransitStop &q : s.transit.stops) stop_mesh(mb, g, s, q, look_);
	if (instance_.is_valid()) rs->free_rid(instance_);
	instance_ = RID();
	plan_.reset();
	if (mb.empty()) return;
	plan_.rid = rs->mesh_create();
	mb.commit_to(plan_.rid);
	rs->mesh_surface_set_material(plan_.rid, 0, material_);
	instance_ = rs->instance_create2(plan_.rid, scenario_);
}

void TransitRenderer::update(const PoseFrame &f, const Vector3 &cam, const PoseSample &at) {
	if (!car_.valid()) return;
	PackedFloat32Array buf;
	const float r2 = radius_ * radius_;
	drawn_cars_ = 0;
	for (const AgentRecord &rec : f.trains) {
		if (rec.id != 0) continue; // the metro runs under the ground
		const Pose p = record_pose(rec, at);
		const float dx = p.x - cam.x, dz = p.z - cam.z;
		if (dx * dx + dz * dz > r2) continue;
		const Color line = Color::hex((rec.look << RGB_BITS) | OPAQUE);
		mm_push(buf, Vector3(p.fz, 0, -p.fx), Vector3(0, 1, 0), Vector3(p.fx, 0, p.fz), Vector3(p.x, p.y, p.z), line, 0, 0, 0, 0);
		drawn_cars_++;
	}
	const AABB box(Vector3(cam.x - radius_, cam.y - AABB_HALF_H_M, cam.z - radius_), Vector3(radius_ * 2, AABB_HALF_H_M * 2, radius_ * 2));
	cars_.set(scenario_, car_.rid, RID(), buf, box, true);
}

} // namespace city
