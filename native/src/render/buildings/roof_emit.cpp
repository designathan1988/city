// Roofs of a placed body: flat slabs (membrane or FLAT_ROOFS pick), terrace
// floors, parapets cut per bay (outer wall face, inner face, coping and lip; attic crowns 1.5 m), terrace railings,
// cornices (cornice / deco / slab), lift machine rooms, roof plant and used terraces (roof_terrace.cpp); pitched roofs go to roof_pitched.cpp.
#include "render/buildings/facade_bay.h"
#include "sim/buildings/b40/gen.h"
#include "render/buildings/roof_util.h"

using namespace godot;

namespace city::b40 {

namespace {
constexpr float ROOF_EDGE_H = 0.85f, ATTIC_H = 1.5f, PARAPET_T = 0.25f, COPING_OUT = 0.05f, COPING_IN = 0.3f, LIP_H = 0.06f;
constexpr float INNER_SHADE = 0.9f, LIP_SHADE = 0.9f, TEST_OUT = 0.03f, CORNICE_DROP = 0.2f, CORNICE_OUT = 0.16f, CORNICE_H = 0.36f;
constexpr float DECO_OUT = 0.55f, DECO_H = 0.55f, DECO2_DROP = 1.05f, DECO2_OUT = 0.28f, DECO2_H = 0.32f, SLAB_UP = 0.73f;
constexpr float SLAB_OUT = 0.45f, SLAB_H = 0.26f, RAIL_IN = 0.12f;
constexpr float MACHINE_H = 3.f, MACHINE_IN = 0.3f;
constexpr float ROOF_MODULE_LEN = 3.f, MACHINE_MIN = 1.f, SLAB_LIFT = 0.002f;

// A volume standing at or over this volume's top storey covers its parapet there.
bool roof_covered(const Body &b, const Volume &v, Point2 p) {
	for (const Volume &o : b.volumes) {
		if (&o == &v || o.base > v.top() || o.top() < v.top()) continue; // the storey under the roof, or a block standing on it
		if (p.x > o.x && p.x < o.x + o.w && p.y > o.y && p.y < o.y + o.d) return true;
	}
	return false;
}

void top_face(Shell &s, const Placed &pl, const Volume &v, float z, const Lin &lin, int8_t fin) {
	std::vector<Vector3> p;
	if (v.outline.empty()) p = { pl.lot.at(v.x, v.y, z), pl.lot.at(v.x + v.w, v.y, z), pl.lot.at(v.x + v.w, v.y + v.d, z), pl.lot.at(v.x, v.y + v.d, z) };
	else
		for (const Point2 &q : v.outline) p.push_back(pl.lot.at(v.x + q.x * v.w, v.y + q.y * v.d, z));
	Vector3 c = pl.lot.at(v.x + v.w / 2, v.y + v.d / 2, z);
	for (size_t i = 0; i < p.size(); i++) s.tri(c, p[i], p[(i + 1) % p.size()], pl.lot.e2, lin, fin);
}


struct RoofEmit {
	Shell &shell, &parts;
	const Placed &pl;
	const Volume &v;
	float eaves;
	void edge(const Side &sd, bool terrace, Crown crown);
	void crown_bands(const Side &sd, Crown crown);
	void machine_rooms();
};

void RoofEmit::edge(const Side &sd, bool terrace, Crown crown) {
	const Body &b = *pl.b;
	int n = bays_of(v, sd.side);
	float W = sd.length / float(n), H = crown == CROWN_ATTIC ? ATTIC_H : ROOF_EDGE_H;
	Paint cp = moulding_of(pl);
	Lin coping = paint_lin(cp), roof = paint_lin(b.roof_of(v)) * INNER_SHADE, wall = wall_lin(pl, v, v.storeys - 1, sd.side);
	int8_t wf = b.wall_of(v).finish;
	Point2 dir{ (sd.p1.x - sd.p0.x) / sd.length, (sd.p1.y - sd.p0.y) / sd.length };
	for (int i = 0; i < n; i++) {
		float a0 = W * float(i), am = a0 + W / 2;
		Point2 p{ sd.p0.x + dir.x * am - sd.n.x * TEST_OUT, sd.p0.y + dir.y * am - sd.n.y * TEST_OUT };
		if (roof_covered(b, v, p)) continue;
		if (terrace) {
			Basis rf{ sd.f.at(a0, eaves, -RAIL_IN), sd.f.e0, sd.f.e1, sd.f.e2 };
			shell.card(rf, 0, W, 0, pl.look->guard_h_m, 0, paint_lin(pl.data->color("roof_railing")), CARD_ROOF_RAILING);
			continue;
		}
		shell.grime_floor = -FAR_AWAY;
		shell.rect(Basis{ sd.f.o + sd.f.e1 * eaves, sd.f.e0, sd.f.e1, sd.f.e2 }, a0, a0 + W, 0, H, 0, wall, wf);
		Basis in{ sd.f.at(0, eaves, -PARAPET_T), sd.f.e0, sd.f.e1, sd.f.e2 };
		shell.rect(in, a0, a0 + W, 0, H, 0, roof, b.roof_of(v).finish, true);
		Basis top{ sd.f.at(0, eaves + H, 0), sd.f.e0, -sd.f.e2, sd.f.e1 };
		shell.rect(top, a0, a0 + W, -COPING_OUT, COPING_IN, 0, coping, cp.finish);
		shell.box(sd.f, a0, a0 + W, eaves + H - LIP_H, eaves + H, 0, COPING_OUT, coping * LIP_SHADE, cp.finish, SKIP_E2_NEG | SKIP_E1_POS);
	}
}

void RoofEmit::crown_bands(const Side &sd, Crown crown) {
	Point2 mid{ (sd.p0.x + sd.p1.x) / 2 - sd.n.x * TEST_OUT, (sd.p0.y + sd.p1.y) / 2 - sd.n.y * TEST_OUT };
	if (roof_covered(*pl.b, v, mid)) return;
	Paint mp = moulding_of(pl);
	Lin m = paint_lin(mp);
	if (crown == CROWN_CORNICE) band(shell, sd, eaves - CORNICE_DROP, CORNICE_OUT, CORNICE_H, m, mp.finish, false);
	else if (crown == CROWN_DECO) {
		band(shell, sd, eaves - CORNICE_DROP, DECO_OUT, DECO_H, m, mp.finish, false);
		band(shell, sd, eaves - DECO2_DROP, DECO2_OUT, DECO2_H, m, mp.finish, false);
	} else if (crown == CROWN_SLAB) band(shell, sd, eaves + SLAB_UP, SLAB_OUT, SLAB_H, m, mp.finish, true);
}

void RoofEmit::machine_rooms() {
	const Body &b = *pl.b;
	for (const Core &c : b.cores) {
		float cx = c.x + ROOF_MODULE_LEN / 2, cy = c.y + ROOF_MODULE_LEN / 2;
		const Volume *best = nullptr;
		for (const Volume &o : b.volumes)
			if (roof_flat(o.roof) && cx > o.x && cx < o.x + o.w && cy > o.y && cy < o.y + o.d && (!best || o.top() > best->top())) best = &o;
		if (best != &v) continue;
		float x0 = std::max(c.x, v.x + MACHINE_IN), x1 = std::min(c.x + ROOF_MODULE_LEN, v.x + v.w - MACHINE_IN);
		float y0 = std::max(c.y, v.y + MACHINE_IN), y1 = std::min(c.y + ROOF_MODULE_LEN, v.y + v.d - MACHINE_IN);
		if (x1 - x0 < MACHINE_MIN || y1 - y0 < MACHINE_MIN) continue;
		bool glass = b.wall_of(v).finish == FIN_GLASS;
		const Paint &wp = glass ? pl.data->color("machine_glass") : b.wall_of(v);
		shell.grime_floor = pl.lot.o.y + eaves;
		shell.box(pl.lot, x0, x1, y0, y1, eaves, eaves + MACHINE_H, paint_lin(wp) * pl.shade, wp.finish, SKIP_E2_NEG | SKIP_E2_POS);
		shell.rect(pl.lot, x0, x1, y0, y1, eaves + MACHINE_H, paint_lin(pl.data->color("machine_top")), FIN_CONCRETE);
	}
}
} // namespace

void emit_roofs(Shell &shell, Shell &parts, const Placed &whole) {
	const Body &b = *whole.b;
	for (size_t i = 0; i < b.volumes.size(); i++) {
		const Volume &v = b.volumes[i];
		Placed raised;
		const Placed &pl = block_frame(whole, v, raised);
		if (b.cut_level >= 0 && v.top() == b.cut_level + 1) continue; // cut open: no roof over the floor seen
		float eaves = b.level_z(v.top());
		emit_roof_gear(shell, pl, v, eaves);
		const bool terrace = v.roof == ROOF_TERRACE;
		if (pl.coarse) {
			if (!roof_flat(v.roof)) continue;
			RoofEmit re{ shell, parts, pl, v, eaves };
			for (const Side &sd : sides_of(pl, v, 0)) {
				re.crown_bands(sd, v.crown);
				if (terrace) re.edge(sd, true, v.crown);
			}
			if (!terrace && v.outline.empty()) emit_roof_plant(shell, parts, pl, v, int(i), eaves);
			if (terrace && !built_on(b, v)) emit_roof_terrace(shell, parts, pl, v, int(i), eaves);
			continue;
		}
		if (!roof_flat(v.roof)) { emit_pitched(shell, pl, v, eaves); continue; }
		Paint rp = terrace ? pl.data->color("terrace") : flat_roof_of(pl, v);
		shell.grime_floor = -FAR_AWAY;
		top_face(shell, pl, v, eaves + SLAB_LIFT, paint_lin(rp), rp.finish);
		RoofEmit re{ shell, parts, pl, v, eaves };
		for (const Side &sd : sides_of(pl, v, 0)) {
			re.edge(sd, terrace, v.crown); // bay by bay, only where the roof really ends: a setback's ledge has its own
			re.crown_bands(sd, v.crown);
		}
		re.machine_rooms();
		if (!terrace && v.outline.empty()) emit_roof_plant(shell, parts, pl, v, int(i), eaves);
		if (terrace && !built_on(b, v)) emit_roof_terrace(shell, parts, pl, v, int(i), eaves);
	}
}

} // namespace city::b40
