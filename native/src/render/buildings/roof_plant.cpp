// Roof plant on flat roofs: the 12 kinds in catalogue
// order with their sizes, caps and chances by use, packed on a 0.5 m occupancy grid inside a 0.8 m margin, with a
// two-cell clear ring round every piece and the machine rooms. Stream = mulberry32(id*2654435761 ^ (vol+7)*40503 ^
// 0x5f3759df). Drains, masts and dish posts are fine parts.
#include "render/buildings/facade.h"
#include "render/buildings/roof_sizes.h"

#include "core/math.h"
#include "core/seeded.h"

#include <cmath>

using namespace godot;

namespace city::b40 {

namespace {
enum PlantKind { LIFT_ROOM, TANK_BOX, TANK, HATCH, AIR_HANDLER, CONDENSER, SOLAR, MAST, DISH, SKYLIGHT, VENT, CHIMNEY, PLANT_KINDS };
struct PlantDef {
	float w, d, h;
	int max;
	float per_area; // 0 = drawn by chance
};
// The plant kinds as the source game draws them: footprint, height, the most a roof takes, how many an area holds.
constexpr PlantDef PLANT[PLANT_KINDS] = { { 2.6f, 2.6f, 2.5f, 1, 0 }, { 3.2f, 2.4f, 2.2f, 1, 0 }, { 1.9f, 1.9f, 1.5f, 2, 0 }, { 0.9f, 0.9f, 0.45f, 1, 0 }, { 3.4f, 1.8f, 1.6f, 2, 0 }, { 0.9f, 0.8f, 0.85f, 6, 30 }, { 4.6f, 3.4f, 0.4f, 3, 0 }, { 0.6f, 0.6f, 3.5f, 3, 0 }, { 0.9f, 0.7f, 1.2f, 2, 180 }, { 1.2f, 1.2f, 0.25f, 6, 0 }, { 0.7f, 0.7f, 0.9f, 5, 70 }, { 0.9f, 0.9f, 4.5f, 1, 0 } };
// Chance by use (residential, commercial, industrial) for the kinds that use one.
constexpr float USE_CHANCE[PLANT_KINDS][3] = { {}, { 0.7f, 0.5f, 0.2f }, { 0.75f, 0.55f, 0.35f }, {}, { 0.1f, 0.8f, 0.5f }, { 0.5f, 0.8f, 0.35f }, { 0.3f, 0.2f, 0.45f }, { 0.45f, 0.25f, 0.1f }, { 0.6f, 0.3f, 0.05f }, { 0.15f, 0.25f, 0.7f }, { 0.1f, 0.6f, 0.75f }, { 0, 0, 0.3f } };
constexpr int TRIES = 24, RING = 2;
constexpr uint32_t GOLDEN = 2654435761u, VOL_K = 40503u, VOL_OFF = 7, MAGIC = 0x5f3759df, AGE_MUL = 2246822519u, AGE_MOD = 1000;
constexpr int CYL_SIDES = 12, CYL_SIDES_COARSE = 6;

struct Packer {
	int nx = 0, ny = 0;
	std::vector<uint8_t> used;
	bool free(int x0, int y0, int w, int h) const {
		if (x0 < 0 || y0 < 0 || x0 + w > nx || y0 + h > ny) return false;
		for (int y = y0; y < y0 + h; y++)
			for (int x = x0; x < x0 + w; x++)
				if (used[size_t(y * nx + x)]) return false;
		return true;
	}
	void mark(int x0, int y0, int w, int h) {
		for (int y = std::max(0, y0 - RING); y < std::min(ny, y0 + h + RING); y++)
			for (int x = std::max(0, x0 - RING); x < std::min(nx, x0 + w + RING); x++) used[size_t(y * nx + x)] = 1;
	}
};

struct PlantEmit {
	Shell &shell, &parts;
	const Placed &pl;
	Basis f; // roof frame: o at the volume corner on the roof
	Lin c(const char *n) const { return paint_lin(pl.data->color(n)); }
	void piece(PlantKind k, float x, float y, float w, float d, float h, int variant);
	const RoofPlantLook &K = pl.look->shapes.roof_plant; // last: it reads pl
};

void PlantEmit::piece(PlantKind k, float x, float y, float w, float d, float h, int variant) {
	Lin metal = c("plant_metal"), dark = c("plant_dark"), top = c("machine_top"), trim = paint_lin(pl.b->paints[size_t(pl.b->trim)]);
	float cx = x + w / 2, cy = y + d / 2, r = std::min(w, d) / 2;
	switch (k) {
		case LIFT_ROOM: case TANK_BOX:
			shell.box(f, x, x + w, y, y + d, 0, h, trim, FIN_CONCRETE, SKIP_E2_NEG | SKIP_E2_POS);
			shell.rect(f, x, x + w, y, y + d, h, top, FIN_CONCRETE);
			if (k == LIFT_ROOM) shell.box(f, cx - K.door_w / 2, cx + K.door_w / 2, y - K.door_t, y, 0, K.door_h, c("plant_door"), FIN_METAL, SKIP_E1_POS);
			else { shell.box(f, x + K.lid_t, x + K.lid_t + K.lid, y + K.lid_t, y + K.lid_t + K.lid, h, h + K.lid_t, top, FIN_CONCRETE); parts.box(f, x + w, x + w + K.ladder, cy - K.ladder, cy, 0, h + K.ladder_up, dark, FIN_METAL); }
			break;
		case TANK: {
			Lin tank = lin_of(pl.data->tanks[size_t(variant) % pl.data->tanks.size()]);
			shell.box(f, x, x + w, y, y + d, 0, K.plinth, top, FIN_CONCRETE, SKIP_E2_NEG);
			cylinder(shell, f, cx, cy, r * K.tank_r, K.plinth, h - K.plinth, tank, tank, FIN_METAL);
			cylinder(shell, f, cx, cy, r * K.rim_r, h - K.plinth, h - K.rim_drop, tank, tank * K.top_shade, FIN_METAL);
			cylinder(shell, f, cx, cy, r * K.lid_r, h - K.rim_drop, h + K.lid_up, tank * K.lid_shade, tank * K.lid_shade, FIN_METAL);
			break;
		}
		case HATCH: shell.box(f, x, x + w, y, y + d, 0, h, top, FIN_CONCRETE, SKIP_E2_NEG); break;
		case AIR_HANDLER: {
			shell.box(f, x, x + w, y, y + d, K.casing_up, h, metal, FIN_METAL, SKIP_E2_NEG);
			bool long_x = w >= d;
			for (float t : K.fan_at)
				cylinder(shell, f, long_x ? x + w * t : cx, long_x ? cy : y + d * t, K.fan_r * std::min(w, d), h, h + K.fan_h, dark, dark, FIN_METAL);
			break;
		}
		case CONDENSER:
			shell.box(f, x, x + w, y, y + d, K.cond_up, h, metal, FIN_METAL, SKIP_E2_NEG);
			shell.rect(f, cx - K.fan_plate / 2, cx + K.fan_plate / 2, cy - K.fan_plate / 2, cy + K.fan_plate / 2, h + K.drain_t, dark, FIN_METAL);
			break;
		case SOLAR: {
			int rows = std::max(1, int(std::floor(d / K.panel_row)));
			float pd = std::min(K.panel_max, d / float(rows) - K.panel_gap);
			Lin glass = c("solar");
			for (int row = 0; row < rows; row++)
				for (float px = x; px + K.panel_w <= x + w + EPS; px += K.panel_step) {
					float py = y + float(row) * d / float(rows);
					Vector3 a = f.at(px, py, K.panel_z0), b = f.at(px + K.panel_w, py, K.panel_z0), cc = f.at(px + K.panel_w, py + pd, K.panel_z1 + K.panel_z0), dd = f.at(px, py + pd, K.panel_z1 + K.panel_z0);
					shell.quad(a, b, cc, dd, f.e2, glass, FIN_GLASS);
					parts.box(f, px + K.panel_w / 2 - K.pole, px + K.panel_w / 2 + K.pole, py + pd / 2 - K.pole, py + pd / 2 + K.pole, 0, K.panel_z0 + K.panel_z1 / 2, dark, FIN_METAL);
				}
			break;
		}
		case MAST:
			parts.box(f, cx - K.pole / 2, cx + K.pole / 2, cy - K.pole / 2, cy + K.pole / 2, 0, h, dark, FIN_METAL);
			parts.box(f, cx - K.bar / 2, cx + K.bar / 2, cy - K.pole / 2, cy + K.pole / 2, h - K.bar_drop - K.pole, h - K.bar_drop, dark, FIN_METAL);
			if (variant % 2) parts.box(f, cx - K.pole / 2, cx + K.pole / 2, cy - K.bar2 / 2, cy + K.bar2 / 2, h - K.bar2_drop - K.pole, h - K.bar2_drop, dark, FIN_METAL);
			break;
		case DISH:
			parts.box(f, cx - K.pole / 2, cx + K.pole / 2, cy - K.pole / 2, cy + K.pole / 2, 0, K.dish_post, dark, FIN_METAL);
			shell.box(f, x, x + w, cy - K.dish_plate_t / 2, cy + K.dish_plate_t / 2, K.dish_plate_z, h, variant % 2 ? dark : metal, FIN_METAL);
			break;
		case SKYLIGHT:
			shell.box(f, x, x + w, y, y + d, 0, h, top, FIN_CONCRETE, SKIP_E2_NEG | SKIP_E2_POS);
			shell.rect(f, x, x + w, y, y + d, h, c("skylight"), FIN_GLASS);
			break;
		case VENT:
			cylinder(shell, f, cx, cy, K.vent_r * 2 * r, 0, h - K.vent_cap, metal, metal, FIN_METAL);
			cylinder(shell, f, cx, cy, K.vent_cap_r * 2 * r, h - K.vent_cap, h, dark, dark, FIN_METAL);
			break;
		default:
			cylinder(shell, f, cx, cy, K.chim_r * 2 * r, 0, h - K.chim_top, c("chimney"), dark, FIN_BRICK);
			cylinder(shell, f, cx, cy, K.chim_r * 2 * r, h - K.chim_top, h, dark, dark, FIN_METAL);
			break;
	}
}

float chance(const RoofPlantLook &K, PlantKind k, int use, int storeys, float area, float age, bool core) {
	switch (k) {
		case LIFT_ROOM: return storeys >= K.lift_storeys && !core ? 1.f : 0.f;
		case TANK_BOX: return storeys >= K.tank_storeys ? USE_CHANCE[k][use] : 0.f;
		case TANK: return storeys < K.tank_storeys ? USE_CHANCE[k][use] : K.tall_tank[use];
		case HATCH: return K.hatch_p;
		case AIR_HANDLER: return area > K.big_roof ? USE_CHANCE[k][use] : 0.f;
		case CONDENSER: return USE_CHANCE[k][use] * (K.decay + K.age_k * (1 - age));
		case SOLAR: return USE_CHANCE[k][use] * (K.solar_age - age);
		case MAST: return storeys >= K.mast_storeys ? K.mast_tall : USE_CHANCE[k][use] * (K.mast_age + age);
		default: return USE_CHANCE[k][use];
	}
}
} // namespace

void cylinder(Shell &s, const Basis &b, float cx, float cy, float r, float z0, float z1, const Lin &side, const Lin &top, int8_t fin, bool capped) {
	int sides = s.coarse ? CYL_SIDES_COARSE : CYL_SIDES;
	for (int i = 0; i < sides; i++) {
		float a0 = TAU_F * float(i) / float(sides), a1 = TAU_F * float(i + 1) / float(sides);
		Vector3 p0 = b.at(cx + r * std::cos(a0), cy + r * std::sin(a0), z0), p1 = b.at(cx + r * std::cos(a1), cy + r * std::sin(a1), z0);
		Vector3 out = (b.e0 * std::cos((a0 + a1) / 2) + b.e1 * std::sin((a0 + a1) / 2));
		s.quad(p0, p1, p1 + b.e2 * (z1 - z0), p0 + b.e2 * (z1 - z0), out, side, fin);
		if (capped) s.tri(b.at(cx, cy, z1), p0 + b.e2 * (z1 - z0), p1 + b.e2 * (z1 - z0), b.e2, top, fin);
	}
}

void emit_roof_plant(Shell &shell, Shell &parts, const Placed &pl, const Volume &v, int vol, float roof_z) {
	const RoofPlantLook &K = pl.look->shapes.roof_plant;
	if (v.w < K.min_side || v.d < K.min_side) return;
	const Body &b = *pl.b;
	PlantEmit pe{ shell, parts, pl, Basis{ pl.lot.at(v.x, v.y, roof_z), pl.lot.e0, pl.lot.e1, pl.lot.e2 } };
	Lin dark = pe.c("plant_dark");
	parts.box(pe.f, K.drain_at - K.drain / 2, K.drain_at + K.drain / 2, K.drain_at - K.drain / 2, K.drain_at + K.drain / 2, 0, K.drain_t, dark, FIN_METAL);
	parts.box(pe.f, v.w - K.drain_at - K.drain / 2, v.w - K.drain_at + K.drain / 2, v.d - K.drain_at - K.drain / 2, v.d - K.drain_at + K.drain / 2, 0, K.drain_t, dark, FIN_METAL);
	Packer pk;
	pk.nx = int((v.w - 2 * K.margin) / K.cell);
	pk.ny = int((v.d - 2 * K.margin) / K.cell);
	if (pk.nx <= 0 || pk.ny <= 0) return;
	pk.used.assign(size_t(pk.nx * pk.ny), 0);
	for (const Core &c : b.cores) // the machine room square
		pk.mark(int((c.x - v.x - K.margin) / K.cell), int((c.y - v.y - K.margin) / K.cell), int(MACHINE_ROOM_M / K.cell), int(MACHINE_ROOM_M / K.cell));
	Mulberry32 r((pl.id * GOLDEN) ^ (uint32_t(vol + VOL_OFF) * VOL_K) ^ MAGIC);
	int use = b.use == USE_RESIDENTIAL ? 0 : b.use == USE_COMMERCIAL ? 1 : 2;
	float age = float((pl.id * AGE_MUL) % AGE_MOD) / float(AGE_MOD), area = v.w * v.d;
	for (int k = 0; k < PLANT_KINDS; k++) {
		const PlantDef &d = PLANT[k];
		float per_area = k == AIR_HANDLER ? pl.look->air_handler_per_area : d.per_area;
		float p = std::min(1.f, chance(K, PlantKind(k), use, v.top(), area, age, !b.cores.empty()));
		int count = per_area > 0 ? std::min(K.per_area_cap * d.max, int(std::round(area / per_area * p + K.round_jitter * float(r.next())))) : 0;
		for (int n = 0; per_area > 0 ? n < count : (n < d.max && r.next() < p); n++) {
			if (per_area <= 0) p *= K.decay;
			int variant = int(K.variants * r.next());
			bool small = pe.shell.coarse && d.h < K.coarse_min_h;
			bool turn = k != SOLAR && r.next() < K.turn;
			float w = (turn ? d.d : d.w) * (k == SOLAR ? K.solar_base + K.solar_var * float(variant) : 1.f), dd = turn ? d.w : d.d;
			int cw = int(std::ceil(w / K.cell)), ch = int(std::ceil(dd / K.cell));
			bool placed = false;
			for (int t = 0; t < TRIES && !placed; t++) {
				int cx = int(r.next() * pk.nx), cy = int(r.next() * pk.ny);
				if (!pk.free(cx, cy, cw, ch)) continue;
				pk.mark(cx, cy, cw, ch);
				if (!small) pe.piece(PlantKind(k), K.margin + float(cx) * K.cell, K.margin + float(cy) * K.cell, w, dd, d.h, variant);
				placed = true;
			}
			if (!placed) break;
		}
	}
}

} // namespace city::b40
