// Roofs of a placed body: flat slabs (membrane or FLAT_ROOFS pick), terrace
// floors, parapets cut per bay (outer wall face, inner face, coping and lip; attic crowns 1.5 m), terrace railings,
// cornices (cornice / deco / slab), lift machine rooms, roof plant and used terraces (roof_terrace.cpp); pitched roofs go to roof_pitched.cpp.
#include "render/buildings/facade_bay.h"
#include "render/buildings/roof_sizes.h"
#include "sim/buildings/b40/gen.h"
#include "render/buildings/roof_util.h"

using namespace godot;

namespace city::b40 {

namespace {

// A volume standing at or over this volume's top storey covers its parapet there.
bool roof_covered(const Body &b, const Volume &v, V2 p) {
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
		for (const V2 &q : v.outline) p.push_back(pl.lot.at(v.x + q.x * v.w, v.y + q.y * v.d, z));
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
	const RoofEmitLook &K = pl.look->shapes.roof_emit; // last: it reads pl
};

void RoofEmit::edge(const Side &sd, bool terrace, Crown crown) {
	const Body &b = *pl.b;
	int n = bays_of(v, sd.side);
	float W = sd.length / float(n), H = crown == CROWN_ATTIC ? ATTIC_H : ROOF_EDGE_H;
	Paint cp = moulding_of(pl);
	Lin coping = paint_lin(cp), roof = paint_lin(b.roof_of(v)) * K.inner_shade, wall = wall_lin(pl, v, v.storeys - 1, sd.side);
	int8_t wf = b.wall_of(v).finish;
	V2 dir{ (sd.p1.x - sd.p0.x) / sd.length, (sd.p1.y - sd.p0.y) / sd.length };
	for (int i = 0; i < n; i++) {
		float a0 = W * float(i), am = a0 + W / 2;
		V2 p{ sd.p0.x + dir.x * am - sd.n.x * ROOF_TEST_OUT, sd.p0.y + dir.y * am - sd.n.y * ROOF_TEST_OUT };
		if (roof_covered(b, v, p)) continue;
		if (terrace) {
			Basis rf{ sd.f.at(a0, eaves, -K.rail_in), sd.f.e0, sd.f.e1, sd.f.e2 };
			part_roof_railing(shell, rf, W, pl.look->guard_h_m, paint_lin(pl.data->color("roof_railing")), FIN_METAL);
			continue;
		}
		shell.grime_floor = -FAR_AWAY;
		shell.rect(Basis{ sd.f.o + sd.f.e1 * eaves, sd.f.e0, sd.f.e1, sd.f.e2 }, a0, a0 + W, 0, H, 0, wall, wf);
		Basis in{ sd.f.at(0, eaves, -K.parapet_t), sd.f.e0, sd.f.e1, sd.f.e2 };
		shell.rect(in, a0, a0 + W, 0, H, 0, roof, b.roof_of(v).finish, true);
		Basis top{ sd.f.at(0, eaves + H, 0), sd.f.e0, -sd.f.e2, sd.f.e1 };
		shell.rect(top, a0, a0 + W, -K.coping_out, COPING_IN, 0, coping, cp.finish);
		shell.box(sd.f, a0, a0 + W, eaves + H - K.lip_h, eaves + H, 0, K.coping_out, coping * K.lip_shade, cp.finish, SKIP_E2_NEG | SKIP_E1_POS);
	}
}

void RoofEmit::crown_bands(const Side &sd, Crown crown) {
	V2 mid{ (sd.p0.x + sd.p1.x) / 2 - sd.n.x * ROOF_TEST_OUT, (sd.p0.y + sd.p1.y) / 2 - sd.n.y * ROOF_TEST_OUT };
	if (roof_covered(*pl.b, v, mid)) return;
	Paint mp = moulding_of(pl);
	Lin m = paint_lin(mp);
	if (crown == CROWN_CORNICE) band(pl, shell, sd, eaves - K.cornice_drop, K.cornice_out, K.cornice_h, m, mp.finish, false);
	else if (crown == CROWN_DECO) {
		band(pl, shell, sd, eaves - K.cornice_drop, K.deco_out, K.deco_h, m, mp.finish, false);
		band(pl, shell, sd, eaves - K.deco2_drop, K.deco2_out, K.deco2_h, m, mp.finish, false);
	} else if (crown == CROWN_SLAB) band(pl, shell, sd, eaves + K.slab_up, K.slab_out, K.slab_h, m, mp.finish, true);
}

void RoofEmit::machine_rooms() {
	const Body &b = *pl.b;
	for (const Core &c : b.cores) {
		float cx = c.x + MACHINE_ROOM_M / 2, cy = c.y + MACHINE_ROOM_M / 2;
		const Volume *best = nullptr;
		for (const Volume &o : b.volumes)
			if (roof_flat(o.roof) && cx > o.x && cx < o.x + o.w && cy > o.y && cy < o.y + o.d && (!best || o.top() > best->top())) best = &o;
		if (best != &v) continue;
		float x0 = std::max(c.x, v.x + K.machine_in), x1 = std::min(c.x + MACHINE_ROOM_M, v.x + v.w - K.machine_in);
		float y0 = std::max(c.y, v.y + K.machine_in), y1 = std::min(c.y + MACHINE_ROOM_M, v.y + v.d - K.machine_in);
		if (x1 - x0 < K.machine_min || y1 - y0 < K.machine_min) continue;
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
