// Zone colours and congestion map meshes (overlay_renderer.h).
#include "render/overlays/overlay_renderer.h"

#include "core/color.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr int SIDES = 2;
constexpr double MIN_DERIV = 1e-6; // a degenerate curve keeps a finite normal
constexpr float MIN_SPAN = 1e-3f; // free_share == jam_share still divides
constexpr int JAM_STOPS = 3;
constexpr float RGBA_MAX = 255.f;
constexpr int R_SHIFT = 24, G_SHIFT = 16, B_SHIFT = 8;
constexpr uint32_t BYTE = 0xff;

Color rgba(uint32_t c) {
	return Color(float((c >> R_SHIFT) & BYTE) / RGBA_MAX, float((c >> G_SHIFT) & BYTE) / RGBA_MAX, float((c >> B_SHIFT) & BYTE) / RGBA_MAX,
			float(c & BYTE) / RGBA_MAX);
}
} // namespace

bool OverlayRenderer::init(const RenderContext &ctx, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	material_ = ctx.material("overlay", errors);
	Dictionary t = ctx.table("overlays", errors);
	if (!errors.is_empty()) return false;
	zone_depth_ = float(t.get("zone_depth_m", 0));
	lift_ = float(t.get("lift_m", 0));
	sample_ = std::max(1.f, float(t.get("sample_m", 1)));
	refresh_ = float(t.get("refresh_s", 1));
	free_share_ = float(t.get("free_share", 1));
	jam_share_ = float(t.get("jam_share", 0));
	Array cols = t.get("congestion", Array());
	if (cols.size() != JAM_STOPS) {
		errors.push_back("overlays.json: congestion precisa de 3 cores");
		return false;
	}
	for (int i = 0; i < JAM_STOPS; i++) jam_colors_[i] = rgba(parse_hex_color(String(cols[i]).utf8().get_data()));
	return true;
}

void OverlayRenderer::reset() {
	for (MeshDraw &d : draws_) d.free();
	zones_dirty_ = true;
	since_ = refresh_;
}

void OverlayRenderer::set(Overlay o, bool on) {
	on_[int(o)] = on;
	draws_[int(o)].visible(on);
	if (on) {
		zones_dirty_ = zones_dirty_ || o == Overlay::ZONES;
		since_ = refresh_;
	}
}

// A ribbon along segment `seg` between offsets u0 and u1 (left of a -> b positive), on the road or on the ground.
void OverlayRenderer::strip(Ribbon &out, const CityState &s, int seg, float u0, float u1, const Color &col, bool on_road) const {
	const RoadSegment &g = s.roads.segs[seg];
	int n = std::max(1, int(std::ceil(g.len / sample_)));
	Vector3 prev0, prev1;
	for (int i = 0; i <= n; i++) {
		double t = double(i) / n, x, z, dx, dz;
		g.q.at(t, x, z);
		g.q.deriv(t, dx, dz);
		double l = std::max(MIN_DERIV, std::hypot(dx, dz)), nx = dz / l, nz = -dx / l;
		auto at = [&](float u) {
			float px = float(x + nx * u), pz = float(z + nz * u);
			float y = on_road ? float(s.roads.seg_y(seg, g.len * t)) : s.terrain.height_at(px, pz);
			return Vector3(px, y + lift_, pz);
		};
		Vector3 p0 = at(u0), p1 = at(u1);
		if (i > 0) {
			for (const Vector3 &v : { prev0, prev1, p1, prev0, p1, p0 }) {
				out.v.push_back(v);
				out.c.push_back(col);
			}
		}
		prev0 = p0;
		prev1 = p1;
	}
}

void OverlayRenderer::upload(MeshDraw &d, Ribbon &r) {
	d.reset(scenario_, material_, false);
	if (r.v.is_empty()) return;
	Array arrays;
	arrays.resize(RenderingServer::ARRAY_MAX);
	arrays[RenderingServer::ARRAY_VERTEX] = r.v;
	arrays[RenderingServer::ARRAY_COLOR] = r.c;
	RenderingServer::get_singleton()->mesh_add_surface_from_arrays(d.mesh, RenderingServer::PRIMITIVE_TRIANGLES, arrays);
}

void OverlayRenderer::build_zones(const CityState &s) {
	Ribbon r;
	for (int sg = 0; sg < int(s.roads.segs.size()); sg++) {
		const RoadSegment &g = s.roads.segs[sg];
		if (!g.alive) continue;
		const Profile &p = s.roads.profile_of(sg);
		for (int side = 0; side < SIDES; side++) {
			int code = side == 0 ? g.zl : g.zr;
			if (code == 0) continue;
			Color col = rgba(s.catalog.zone(code).color);
			if (side == 0) strip(r, s, sg, p.left, p.left + zone_depth_, col, false);
			else strip(r, s, sg, -p.right - zone_depth_, -p.right, col, false);
		}
	}
	upload(draws_[int(Overlay::ZONES)], r);
	draws_[int(Overlay::ZONES)].visible(on_[int(Overlay::ZONES)]);
}

// Mean speed of the vehicles on each segment against its limit: free (green) ... jammed (red); empty roads are free.
void OverlayRenderer::build_congestion(const CityState &s, const PoseFrame &f) {
	Ribbon r;
	for (int sg = 0; sg < int(s.roads.segs.size()); sg++) {
		if (!s.roads.segs[sg].alive) continue;
		const Profile &p = s.roads.profile_of(sg);
		if (p.rail) continue;
		float share = size_t(sg) < f.seg_speed_share.size() ? f.seg_speed_share[size_t(sg)] : 1.f;
		float k = std::clamp((share - jam_share_) / std::max(MIN_SPAN, free_share_ - jam_share_), 0.f, 1.f);
		Color col = k > 0.5f ? jam_colors_[1].lerp(jam_colors_[0], (k - 0.5f) * 2) : jam_colors_[2].lerp(jam_colors_[1], k * 2);
		strip(r, s, sg, -p.asph_right, p.asph_left, col, true);
	}
	upload(draws_[int(Overlay::CONGESTION)], r);
	draws_[int(Overlay::CONGESTION)].visible(on_[int(Overlay::CONGESTION)]);
}

void OverlayRenderer::update(const CityState &s, const PoseFrame &f, double dt) {
	if (on_[int(Overlay::ZONES)] && zones_dirty_) {
		build_zones(s);
		zones_dirty_ = false;
	}
	since_ += dt;
	if (on_[int(Overlay::CONGESTION)] && since_ >= refresh_) {
		build_congestion(s, f);
		since_ = 0;
	}
}

} // namespace city
