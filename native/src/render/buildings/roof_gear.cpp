// What the player sets on a roof (Build > Roof): panels and skylights lying on its slope, vents, chimneys and tanks, a spire and a
// lighthouse lantern. Each stands on the roof at its own point (roof_height_at); the lantern's proportions are those of a tower top.
#include "render/buildings/facade.h"
#include "sim/buildings/b40/roof_shape.h"

#include <cmath>

using namespace godot;

namespace city::b40 {

namespace {
constexpr float SPIRE_RINGS[4][2] = { { 0.f, 0.78f }, { 0.28f, 0.48f }, { 0.84f, 0.12f }, { 1.f, 0.025f } }; // height share, radius share
constexpr float GLASS_SHARE = 3.69f / 4.02f; // the lantern's glass across its body (the source model: 3.69 of 4.02 m)
constexpr int OCTAGON = 8; // a lantern's and a spire's sides
// The flags (Roadcraft buildingMesh.ts): a spire's 2.2 x 1.25 m at its tip; a lantern's under the mast's tip, 6.6 x 4.3 m on the
// model's 19.4 m lighthouse.

struct Gear {
	Shell &shell;
	const Placed &pl;
	float x, y;
	Lin paint(const char *name) const { return paint_lin(pl.data->color(name)); }
	int8_t finish(const char *name) const { return pl.data->color(name).finish; }
	Vector3 at(float angle, float r, float z) const { return pl.lot.at(x + std::cos(angle) * r, y + std::sin(angle) * r, z); }
	// A ring of `n` faces between two radii and heights.
	void ring(int n, float z0, float z1, float r0, float r1, const char *name, float turn = 0) {
		for (int i = 0; i < n; i++) {
			float a = turn + TAU_F * float(i) / float(n), c = turn + TAU_F * float(i + 1) / float(n), m = (a + c) / 2;
			Vector3 out = pl.lot.e0 * std::cos(m) + pl.lot.e1 * std::sin(m);
			shell.quad(at(a, r0, z0), at(c, r0, z0), at(c, r1, z1), at(a, r1, z1), out, paint(name), finish(name));
		}
	}
	void disc(int n, float z, float r, const char *name, float turn = 0) {
		for (int i = 0; i < n; i++) {
			float a = turn + TAU_F * float(i) / float(n), c = turn + TAU_F * float(i + 1) / float(n);
			shell.tri(pl.lot.at(x, y, z), at(a, r, z), at(c, r, z), pl.lot.e2, paint(name), finish(name));
		}
	}
	void box(float hw, float hd, float z0, float z1, const char *name) { shell.box(pl.lot, x - hw, x + hw, y - hd, y + hd, z0, z1, paint(name), finish(name)); }
	// The mast's flag, w x h metres, its hoist's top at height z.
	void fly(const RoofPart &p, float z, float w, float h) {
		if (p.flag.pattern != FLAG_NONE) shell.flags.push_back({ pl.lot.at(x, y, z), w, h, p.flag });
	}
};

// A flat panel that lies on the roof: its corners each stand on the slope under them.
void panel(Gear &g, const Volume &v, const RoofPart &p, float eaves, const char *top) {
	const RoofGearLook &K = g.pl.look->shapes.roof_gear;
	float c = std::cos(p.turn), s = std::sin(p.turn);
	auto corner = [&](float dx, float dy, float lift) {
		float px = p.x + dx * c - dy * s, py = p.y + dx * s + dy * c;
		return g.pl.lot.at(px, py, eaves + roof_height_at(v, px, py) + lift);
	};
	float hw = p.w / 2, hd = p.d / 2, r = K.panel_rim;
	g.shell.quad(corner(-hw - r, -hd - r, K.panel_lift), corner(hw + r, -hd - r, K.panel_lift), corner(hw + r, hd + r, K.panel_lift), corner(-hw - r, hd + r, K.panel_lift), g.pl.lot.e2,
			g.paint("gear_metal"), g.finish("gear_metal"));
	g.shell.quad(corner(-hw, -hd, K.panel_top), corner(hw, -hd, K.panel_top), corner(hw, hd, K.panel_top), corner(-hw, hd, K.panel_top), g.pl.lot.e2, g.paint(top), g.finish(top));
}

void spire(Gear &g, const RoofPart &p, float z) {
	const RoofGearLook &K = g.pl.look->shapes.roof_gear;
	float radius = std::min(p.w, p.d) * K.spire_share;
	g.box(radius, radius, z - K.spire_foot_sink, z + K.spire_foot_h, "gear_base");
	for (int band = 0; band < 3; band++) {
		const float *lo = SPIRE_RINGS[band], *hi = SPIRE_RINGS[band + 1];
		float z0 = band == 0 ? z + K.spire_foot_h : z + p.h * lo[0];
		g.ring(OCTAGON, z0, z + p.h * hi[0], radius * lo[1], radius * hi[1], band == 0 ? "gear_base" : "gear_metal");
	}
	g.fly(p, z + p.h * K.spire_flag_at, K.spire_flag_w, K.spire_flag_h);
}

void lantern(Gear &g, const RoofPart &p, float z) {
	const RoofGearLook &K = g.pl.look->shapes.roof_gear;
	float k = p.h / K.lantern_h, base_r = std::min(p.w, p.d) / 2, turn = TAU_F / float(OCTAGON * 2), z1 = z + K.base_h * k;
	g.ring(OCTAGON, z, z1, base_r, base_r, "lantern_stone", turn);
	g.disc(OCTAGON, z1, base_r, "lantern_stone", turn);
	float glass_r = base_r * GLASS_SHARE, z2 = z1 + K.glass_h * k;
	g.ring(K.round, z1, z2, glass_r, glass_r, "lantern_glass");
	for (int f = 0; f <= K.bands; f++) {
		float at = z1 + (z2 - z1) * float(f) / float(K.bands);
		g.ring(K.round, std::max(z1, at - K.band_t), std::min(z2, at + K.band_t), glass_r + K.band_out, glass_r + K.band_out, "lantern_frame");
	}
	for (int i = 0; i < K.round; i++) {
		float a = TAU_F * float(i) / float(K.round), cx = g.x + std::cos(a) * glass_r, cy = g.y + std::sin(a) * glass_r;
		g.shell.box(g.pl.lot, cx - K.mullion, cx + K.mullion, cy - K.mullion, cy + K.mullion, z1, z2, g.paint("lantern_frame"), g.finish("lantern_frame"));
	}
	g.ring(K.round, z2, z2 + K.lid_h, glass_r + K.lid_out, glass_r + K.lid_out, "lantern_render");
	g.disc(K.round, z2 + K.lid_h, glass_r + K.lid_out, "lantern_render");
	float dome = z2 + K.lid_h, dome_h = K.dome_h * k;
	for (int s = 0; s < K.dome_steps; s++) {
		float a0 = float(s) / float(K.dome_steps) * TAU_F / 4, a1 = float(s + 1) / float(K.dome_steps) * TAU_F / 4;
		g.ring(K.round, dome + std::sin(a0) * dome_h, dome + std::sin(a1) * dome_h, glass_r * std::cos(a0), std::max(K.dome_top, glass_r * std::cos(a1)), "lantern_dome");
	}
	float top = z + p.h;
	g.ring(OCTAGON, dome + dome_h, top, K.mast_foot, K.mast_top, "lantern_mast");
	g.box(K.beacon, K.beacon, top - K.beacon_low, top - K.beacon_high, "lantern_beacon");
	g.fly(p, top - K.lantern_flag_drop, p.h * K.lantern_flag_w, p.h * K.lantern_flag_w * K.lantern_flag_aspect);
}
} // namespace

void emit_roof_gear(Shell &shell, const Placed &pl, const Volume &v, float eaves) {
	const RoofGearLook &K = pl.look->shapes.roof_gear;
	if (pl.coarse) return;
	for (const RoofPart &p : v.gear) {
		Gear g{ shell, pl, p.x, p.y };
		float z = eaves + roof_height_at(v, p.x, p.y), hw = p.w / 2, hd = p.d / 2;
		switch (p.kind) {
			case GEAR_SOLAR: panel(g, v, p, eaves, "gear_panel"); break;
			case GEAR_SKYLIGHT: panel(g, v, p, eaves, "gear_glazing"); break;
			case GEAR_VENT: g.box(hw, hd, z - K.gear_sink_m, z + K.vent_h, "gear_metal"); break;
			case GEAR_CHIMNEY:
				g.box(hw, hd, z - K.gear_sink_m, z + K.chimney_h, "gear_masonry");
				g.box(hw + K.cap_out, hd + K.cap_out, z + K.chimney_h, z + K.chimney_h + K.cap_h, "gear_cap");
				break;
			case GEAR_TANK: g.box(hw, hd, z - K.gear_sink_m / 2, z + K.tank_h, "gear_metal"); break;
			case GEAR_SPIRE: spire(g, p, z); break;
			default: lantern(g, p, z); break;
		}
	}
}

} // namespace city::b40
