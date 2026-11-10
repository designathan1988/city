// Roof plant on flat roofs: the 12 kinds in catalogue
// order with their sizes, caps and chances by use, packed on a 0.5 m occupancy grid inside a 0.8 m margin, with a
// two-cell clear ring round every piece and the machine rooms. Stream = mulberry32(id*2654435761 ^ (vol+7)*40503 ^
// 0x5f3759df). Drains, masts and dish posts are fine parts.
#include "render/buildings/facade.h"

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
constexpr PlantDef PLANT[PLANT_KINDS] = { { 2.6f, 2.6f, 2.5f, 1, 0 }, { 3.2f, 2.4f, 2.2f, 1, 0 }, { 1.9f, 1.9f, 1.5f, 2, 0 }, { 0.9f, 0.9f, 0.45f, 1, 0 }, { 3.4f, 1.8f, 1.6f, 2, 0 }, { 0.9f, 0.8f, 0.85f, 6, 30 }, { 4.6f, 3.4f, 0.4f, 3, 0 }, { 0.6f, 0.6f, 3.5f, 3, 0 }, { 0.9f, 0.7f, 1.2f, 2, 180 }, { 1.2f, 1.2f, 0.25f, 6, 0 }, { 0.7f, 0.7f, 0.9f, 5, 70 }, { 0.9f, 0.9f, 4.5f, 1, 0 } };
// Chance by use (residential, commercial, industrial) for the kinds that use one.
constexpr float USE_CHANCE[PLANT_KINDS][3] = { {}, { 0.7f, 0.5f, 0.2f }, { 0.75f, 0.55f, 0.35f }, {}, { 0.1f, 0.8f, 0.5f }, { 0.5f, 0.8f, 0.35f }, { 0.3f, 0.2f, 0.45f }, { 0.45f, 0.25f, 0.1f }, { 0.6f, 0.3f, 0.05f }, { 0.15f, 0.25f, 0.7f }, { 0.1f, 0.6f, 0.75f }, { 0, 0, 0.3f } };
constexpr float FAN_AT[] = { 0.25f, 0.75f }, PANEL_GAP = 0.1f;
constexpr float TALL_TANK[3] = { 0.25f, 0.2f, 0.2f };
constexpr float COARSE_MIN_H = 1.2f, CELL = 0.5f, MARGIN = 0.8f, MIN_SIDE = 4.f, HATCH_P = 0.9f, BIG_ROOF = 150.f, AGE_K = 0.6f, MAST_TALL = 0.7f;
constexpr float SOLAR_AGE = 1.3f, MAST_AGE = 0.4f, DECAY = 0.6f, ROUND_JITTER = 0.8f, TURN = 0.5f, SOLAR_VAR = 0.25f, SOLAR_BASE = 0.7f;
constexpr int LIFT_STOREYS = 4, TANK_STOREYS = 6, MAST_STOREYS = 10, TRIES = 24, RING = 2, PER_AREA_CAP = 6, VARIANTS = 4;
constexpr uint32_t GOLDEN = 2654435761u, VOL_K = 40503u, VOL_OFF = 7, MAGIC = 0x5f3759df, AGE_MUL = 2246822519u, AGE_MOD = 1000;
constexpr float DRAIN = 0.36f, DRAIN_AT = 0.7f, DRAIN_T = 0.02f, PLINTH = 0.3f, TANK_R = 0.84f, RIM_R = 0.94f, RIM_DROP = 0.05f;
constexpr float LID_R = 0.4f, LID_UP = 0.05f, TOP_SHADE = 0.82f, LID_SHADE = 0.75f, CASING_UP = 0.2f, FAN_R = 0.32f, FAN_H = 0.12f;
constexpr float COND_UP = 0.15f, FAN_PLATE = 0.56f, PANEL_W = 1.05f, PANEL_STEP = 1.15f, PANEL_ROW = 1.7f, PANEL_MAX = 1.6f;
constexpr float PANEL_Z0 = 0.25f, PANEL_Z1 = 0.32f, POLE = 0.08f, BAR = 1.2f, BAR2 = 0.9f, BAR_DROP = 0.4f, BAR2_DROP = 0.9f;
constexpr float DISH_POST = 0.7f, DISH_PLATE_Z = 0.5f, DISH_PLATE_T = 0.1f, VENT_R = 0.4f, VENT_CAP_R = 0.5f, VENT_CAP = 0.15f;
constexpr float CHIM_R = 0.45f, CHIM_TOP = 0.3f, DOOR_W = 0.9f, DOOR_H = 2.05f, DOOR_T = 0.04f, LID = 0.8f, LID_T = 0.12f;
constexpr float LADDER = 0.1f, LADDER_UP = 0.9f;
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
};

void PlantEmit::piece(PlantKind k, float x, float y, float w, float d, float h, int variant) {
	Lin metal = c("plant_metal"), dark = c("plant_dark"), top = c("machine_top"), trim = paint_lin(pl.b->paints[size_t(pl.b->trim)]);
	float cx = x + w / 2, cy = y + d / 2, r = std::min(w, d) / 2;
	switch (k) {
		case LIFT_ROOM: case TANK_BOX:
			shell.box(f, x, x + w, y, y + d, 0, h, trim, FIN_CONCRETE, SKIP_E2_NEG | SKIP_E2_POS);
			shell.rect(f, x, x + w, y, y + d, h, top, FIN_CONCRETE);
			if (k == LIFT_ROOM) shell.box(f, cx - DOOR_W / 2, cx + DOOR_W / 2, y - DOOR_T, y, 0, DOOR_H, c("plant_door"), FIN_METAL, SKIP_E1_POS);
			else { shell.box(f, x + LID_T, x + LID_T + LID, y + LID_T, y + LID_T + LID, h, h + LID_T, top, FIN_CONCRETE); parts.box(f, x + w, x + w + LADDER, cy - LADDER, cy, 0, h + LADDER_UP, dark, FIN_METAL); }
			break;
		case TANK: {
			Lin tank = lin_of(pl.data->tanks[size_t(variant) % pl.data->tanks.size()]);
			shell.box(f, x, x + w, y, y + d, 0, PLINTH, top, FIN_CONCRETE, SKIP_E2_NEG);
			cylinder(shell, f, cx, cy, r * TANK_R, PLINTH, h - PLINTH, tank, tank, FIN_METAL);
			cylinder(shell, f, cx, cy, r * RIM_R, h - PLINTH, h - RIM_DROP, tank, tank * TOP_SHADE, FIN_METAL);
			cylinder(shell, f, cx, cy, r * LID_R, h - RIM_DROP, h + LID_UP, tank * LID_SHADE, tank * LID_SHADE, FIN_METAL);
			break;
		}
		case HATCH: shell.box(f, x, x + w, y, y + d, 0, h, top, FIN_CONCRETE, SKIP_E2_NEG); break;
		case AIR_HANDLER: {
			shell.box(f, x, x + w, y, y + d, CASING_UP, h, metal, FIN_METAL, SKIP_E2_NEG);
			bool long_x = w >= d;
			for (float t : FAN_AT)
				cylinder(shell, f, long_x ? x + w * t : cx, long_x ? cy : y + d * t, FAN_R * std::min(w, d), h, h + FAN_H, dark, dark, FIN_METAL);
			break;
		}
		case CONDENSER:
			shell.box(f, x, x + w, y, y + d, COND_UP, h, metal, FIN_METAL, SKIP_E2_NEG);
			shell.rect(f, cx - FAN_PLATE / 2, cx + FAN_PLATE / 2, cy - FAN_PLATE / 2, cy + FAN_PLATE / 2, h + DRAIN_T, dark, FIN_METAL);
			break;
		case SOLAR: {
			int rows = std::max(1, int(std::floor(d / PANEL_ROW)));
			float pd = std::min(PANEL_MAX, d / float(rows) - PANEL_GAP);
			Lin glass = c("solar");
			for (int row = 0; row < rows; row++)
				for (float px = x; px + PANEL_W <= x + w + EPS; px += PANEL_STEP) {
					float py = y + float(row) * d / float(rows);
					Vector3 a = f.at(px, py, PANEL_Z0), b = f.at(px + PANEL_W, py, PANEL_Z0), cc = f.at(px + PANEL_W, py + pd, PANEL_Z1 + PANEL_Z0), dd = f.at(px, py + pd, PANEL_Z1 + PANEL_Z0);
					shell.quad(a, b, cc, dd, f.e2, glass, FIN_GLASS);
					parts.box(f, px + PANEL_W / 2 - POLE, px + PANEL_W / 2 + POLE, py + pd / 2 - POLE, py + pd / 2 + POLE, 0, PANEL_Z0 + PANEL_Z1 / 2, dark, FIN_METAL);
				}
			break;
		}
		case MAST:
			parts.box(f, cx - POLE / 2, cx + POLE / 2, cy - POLE / 2, cy + POLE / 2, 0, h, dark, FIN_METAL);
			parts.box(f, cx - BAR / 2, cx + BAR / 2, cy - POLE / 2, cy + POLE / 2, h - BAR_DROP - POLE, h - BAR_DROP, dark, FIN_METAL);
			if (variant % 2) parts.box(f, cx - POLE / 2, cx + POLE / 2, cy - BAR2 / 2, cy + BAR2 / 2, h - BAR2_DROP - POLE, h - BAR2_DROP, dark, FIN_METAL);
			break;
		case DISH:
			parts.box(f, cx - POLE / 2, cx + POLE / 2, cy - POLE / 2, cy + POLE / 2, 0, DISH_POST, dark, FIN_METAL);
			shell.box(f, x, x + w, cy - DISH_PLATE_T / 2, cy + DISH_PLATE_T / 2, DISH_PLATE_Z, h, variant % 2 ? dark : metal, FIN_METAL);
			break;
		case SKYLIGHT:
			shell.box(f, x, x + w, y, y + d, 0, h, top, FIN_CONCRETE, SKIP_E2_NEG | SKIP_E2_POS);
			shell.rect(f, x, x + w, y, y + d, h, c("skylight"), FIN_GLASS);
			break;
		case VENT:
			cylinder(shell, f, cx, cy, VENT_R * 2 * r, 0, h - VENT_CAP, metal, metal, FIN_METAL);
			cylinder(shell, f, cx, cy, VENT_CAP_R * 2 * r, h - VENT_CAP, h, dark, dark, FIN_METAL);
			break;
		default:
			cylinder(shell, f, cx, cy, CHIM_R * 2 * r, 0, h - CHIM_TOP, c("chimney"), dark, FIN_BRICK);
			cylinder(shell, f, cx, cy, CHIM_R * 2 * r, h - CHIM_TOP, h, dark, dark, FIN_METAL);
			break;
	}
}

float chance(PlantKind k, int use, int storeys, float area, float age, bool core) {
	switch (k) {
		case LIFT_ROOM: return storeys >= LIFT_STOREYS && !core ? 1.f : 0.f;
		case TANK_BOX: return storeys >= TANK_STOREYS ? USE_CHANCE[k][use] : 0.f;
		case TANK: return storeys < TANK_STOREYS ? USE_CHANCE[k][use] : TALL_TANK[use];
		case HATCH: return HATCH_P;
		case AIR_HANDLER: return area > BIG_ROOF ? USE_CHANCE[k][use] : 0.f;
		case CONDENSER: return USE_CHANCE[k][use] * (DECAY + AGE_K * (1 - age));
		case SOLAR: return USE_CHANCE[k][use] * (SOLAR_AGE - age);
		case MAST: return storeys >= MAST_STOREYS ? MAST_TALL : USE_CHANCE[k][use] * (MAST_AGE + age);
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
	if (v.w < MIN_SIDE || v.d < MIN_SIDE) return;
	const Body &b = *pl.b;
	PlantEmit pe{ shell, parts, pl, Basis{ pl.lot.at(v.x, v.y, roof_z), pl.lot.e0, pl.lot.e1, pl.lot.e2 } };
	Lin dark = pe.c("plant_dark");
	parts.box(pe.f, DRAIN_AT - DRAIN / 2, DRAIN_AT + DRAIN / 2, DRAIN_AT - DRAIN / 2, DRAIN_AT + DRAIN / 2, 0, DRAIN_T, dark, FIN_METAL);
	parts.box(pe.f, v.w - DRAIN_AT - DRAIN / 2, v.w - DRAIN_AT + DRAIN / 2, v.d - DRAIN_AT - DRAIN / 2, v.d - DRAIN_AT + DRAIN / 2, 0, DRAIN_T, dark, FIN_METAL);
	Packer pk;
	pk.nx = int((v.w - 2 * MARGIN) / CELL);
	pk.ny = int((v.d - 2 * MARGIN) / CELL);
	if (pk.nx <= 0 || pk.ny <= 0) return;
	pk.used.assign(size_t(pk.nx * pk.ny), 0);
	for (const Core &c : b.cores) // the machine room square
		pk.mark(int((c.x - v.x - MARGIN) / CELL), int((c.y - v.y - MARGIN) / CELL), int(3 / CELL), int(3 / CELL));
	Mulberry32 r((pl.id * GOLDEN) ^ (uint32_t(vol + VOL_OFF) * VOL_K) ^ MAGIC);
	int use = b.use == USE_RESIDENTIAL ? 0 : b.use == USE_COMMERCIAL ? 1 : 2;
	float age = float((pl.id * AGE_MUL) % AGE_MOD) / float(AGE_MOD), area = v.w * v.d;
	for (int k = 0; k < PLANT_KINDS; k++) {
		const PlantDef &d = PLANT[k];
		float per_area = k == AIR_HANDLER ? pl.look->air_handler_per_area : d.per_area;
		float p = std::min(1.f, chance(PlantKind(k), use, v.top(), area, age, !b.cores.empty()));
		int count = per_area > 0 ? std::min(PER_AREA_CAP * d.max, int(std::round(area / per_area * p + ROUND_JITTER * float(r.next())))) : 0;
		for (int n = 0; per_area > 0 ? n < count : (n < d.max && r.next() < p); n++) {
			if (per_area <= 0) p *= DECAY;
			int variant = int(VARIANTS * r.next());
			bool small = pe.shell.coarse && d.h < COARSE_MIN_H;
			bool turn = k != SOLAR && r.next() < TURN;
			float w = (turn ? d.d : d.w) * (k == SOLAR ? SOLAR_BASE + SOLAR_VAR * float(variant) : 1.f), dd = turn ? d.w : d.d;
			int cw = int(std::ceil(w / CELL)), ch = int(std::ceil(dd / CELL));
			bool placed = false;
			for (int t = 0; t < TRIES && !placed; t++) {
				int cx = int(r.next() * pk.nx), cy = int(r.next() * pk.ny);
				if (!pk.free(cx, cy, cw, ch)) continue;
				pk.mark(cx, cy, cw, ch);
				if (!small) pe.piece(PlantKind(k), MARGIN + float(cx) * CELL, MARGIN + float(cy) * CELL, w, dd, d.h, variant);
				placed = true;
			}
			if (!placed) break;
		}
	}
}

} // namespace city::b40
