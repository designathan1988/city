// What the player sets on a roof (Build > Roof): panels and skylights lying on its slope, vents, chimneys and tanks, a spire and a
// lighthouse lantern. Each stands on the roof at its own point (roof_height_at); the lantern's proportions are those of a tower top.
#include "render/buildings/facade.h"
#include "sim/buildings/b40/roof_shape.h"

#include <cmath>

using namespace godot;

namespace city::b40 {

namespace {
constexpr float PANEL_LIFT = 0.18f, PANEL_TOP = 0.24f, PANEL_RIM = 0.15f, VENT_H = 1.f, TANK_H = 1.6f, CHIMNEY_H = 2.1f, CAP_H = 0.15f, CAP_OUT = 0.1f;
constexpr float GEAR_SINK_M = 0.2f, SPIRE_FOOT_H = 0.75f, SPIRE_FOOT_SINK = 0.15f, SPIRE_SHARE = 0.47f;
constexpr float SPIRE_RINGS[4][2] = { { 0.f, 0.78f }, { 0.28f, 0.48f }, { 0.84f, 0.12f }, { 1.f, 0.025f } }; // height share, radius share
constexpr float LANTERN_H = 19.4f, BASE_H = 1.03f, GLASS_H = 7.47f, GLASS_SHARE = 3.69f / 4.02f, BAND_T = 0.07f, BAND_OUT = 0.04f, MULLION = 0.035f;
constexpr float LID_OUT = 0.12f, LID_H = 0.25f, DOME_H = 1.1f, DOME_TOP = 0.16f, MAST_FOOT = 0.15f, MAST_TOP = 0.1f, BEACON = 0.2f, BEACON_LOW = 0.45f, BEACON_HIGH = 0.1f;
constexpr int OCTAGON = 8, ROUND = 24, BANDS = 5, DOME_STEPS = 6;

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
};

// A flat panel that lies on the roof: its corners each stand on the slope under them.
void panel(Gear &g, const Volume &v, const RoofPart &p, float eaves, const char *top) {
	float c = std::cos(p.turn), s = std::sin(p.turn);
	auto corner = [&](float dx, float dy, float lift) {
		float px = p.x + dx * c - dy * s, py = p.y + dx * s + dy * c;
		return g.pl.lot.at(px, py, eaves + roof_height_at(v, px, py) + lift);
	};
	float hw = p.w / 2, hd = p.d / 2, r = PANEL_RIM;
	g.shell.quad(corner(-hw - r, -hd - r, PANEL_LIFT), corner(hw + r, -hd - r, PANEL_LIFT), corner(hw + r, hd + r, PANEL_LIFT), corner(-hw - r, hd + r, PANEL_LIFT), g.pl.lot.e2,
			g.paint("gear_metal"), g.finish("gear_metal"));
	g.shell.quad(corner(-hw, -hd, PANEL_TOP), corner(hw, -hd, PANEL_TOP), corner(hw, hd, PANEL_TOP), corner(-hw, hd, PANEL_TOP), g.pl.lot.e2, g.paint(top), g.finish(top));
}

void spire(Gear &g, const RoofPart &p, float z) {
	float radius = std::min(p.w, p.d) * SPIRE_SHARE;
	g.box(radius, radius, z - SPIRE_FOOT_SINK, z + SPIRE_FOOT_H, "gear_base");
	for (int band = 0; band < 3; band++) {
		const float *lo = SPIRE_RINGS[band], *hi = SPIRE_RINGS[band + 1];
		float z0 = band == 0 ? z + SPIRE_FOOT_H : z + p.h * lo[0];
		g.ring(OCTAGON, z0, z + p.h * hi[0], radius * lo[1], radius * hi[1], band == 0 ? "gear_base" : "gear_metal");
	}
}

void lantern(Gear &g, const RoofPart &p, float z) {
	float k = p.h / LANTERN_H, base_r = std::min(p.w, p.d) / 2, turn = TAU_F / float(OCTAGON * 2), z1 = z + BASE_H * k;
	g.ring(OCTAGON, z, z1, base_r, base_r, "lantern_stone", turn);
	g.disc(OCTAGON, z1, base_r, "lantern_stone", turn);
	float glass_r = base_r * GLASS_SHARE, z2 = z1 + GLASS_H * k;
	g.ring(ROUND, z1, z2, glass_r, glass_r, "lantern_glass");
	for (int f = 0; f <= BANDS; f++) {
		float at = z1 + (z2 - z1) * float(f) / float(BANDS);
		g.ring(ROUND, std::max(z1, at - BAND_T), std::min(z2, at + BAND_T), glass_r + BAND_OUT, glass_r + BAND_OUT, "lantern_frame");
	}
	for (int i = 0; i < ROUND; i++) {
		float a = TAU_F * float(i) / float(ROUND), cx = g.x + std::cos(a) * glass_r, cy = g.y + std::sin(a) * glass_r;
		g.shell.box(g.pl.lot, cx - MULLION, cx + MULLION, cy - MULLION, cy + MULLION, z1, z2, g.paint("lantern_frame"), g.finish("lantern_frame"));
	}
	g.ring(ROUND, z2, z2 + LID_H, glass_r + LID_OUT, glass_r + LID_OUT, "lantern_render");
	g.disc(ROUND, z2 + LID_H, glass_r + LID_OUT, "lantern_render");
	float dome = z2 + LID_H, dome_h = DOME_H * k;
	for (int s = 0; s < DOME_STEPS; s++) {
		float a0 = float(s) / float(DOME_STEPS) * TAU_F / 4, a1 = float(s + 1) / float(DOME_STEPS) * TAU_F / 4;
		g.ring(ROUND, dome + std::sin(a0) * dome_h, dome + std::sin(a1) * dome_h, glass_r * std::cos(a0), std::max(DOME_TOP, glass_r * std::cos(a1)), "lantern_dome");
	}
	float top = z + p.h;
	g.ring(OCTAGON, dome + dome_h, top, MAST_FOOT, MAST_TOP, "lantern_mast");
	g.box(BEACON, BEACON, top - BEACON_LOW, top - BEACON_HIGH, "lantern_beacon");
}
} // namespace

void emit_roof_gear(Shell &shell, const Placed &pl, const Volume &v, float eaves) {
	if (pl.coarse) return;
	for (const RoofPart &p : v.gear) {
		Gear g{ shell, pl, p.x, p.y };
		float z = eaves + roof_height_at(v, p.x, p.y), hw = p.w / 2, hd = p.d / 2;
		switch (p.kind) {
			case GEAR_SOLAR: panel(g, v, p, eaves, "gear_panel"); break;
			case GEAR_SKYLIGHT: panel(g, v, p, eaves, "gear_glazing"); break;
			case GEAR_VENT: g.box(hw, hd, z - GEAR_SINK_M, z + VENT_H, "gear_metal"); break;
			case GEAR_CHIMNEY:
				g.box(hw, hd, z - GEAR_SINK_M, z + CHIMNEY_H, "gear_masonry");
				g.box(hw + CAP_OUT, hd + CAP_OUT, z + CHIMNEY_H, z + CHIMNEY_H + CAP_H, "gear_cap");
				break;
			case GEAR_TANK: g.box(hw, hd, z - GEAR_SINK_M / 2, z + TANK_H, "gear_metal"); break;
			case GEAR_SPIRE: spire(g, p, z); break;
			default: lantern(g, p, z); break;
		}
	}
}

} // namespace city::b40
