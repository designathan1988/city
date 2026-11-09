// The painted ground (see ground_paint.h). A dab takes a cell towards its layer by a smooth falloff from its centre to
// its rim, squeezed into the rim by the hardness; another surface over one laid before fades it out first, and once it is gone
// the same dab lays its own (Roadcraft render/terrain.ts rasterPaint, rasterGeology: every layer moves towards the dab's at once).
#include "sim/nature/ground_paint.h"

#include "core/math.h"

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstring>

namespace city {

namespace {
const PaintCell EMPTY_CELL{};
constexpr size_t PAINT_CELL_BYTES = 6; // a cell's own bytes before its covers' (PaintCell)
static_assert(sizeof(PaintCell) == PAINT_CELL_BYTES + COVER_KINDS, "a cell is its bytes: compared whole");
bool same(const PaintCell &a, const PaintCell &b) { return std::memcmp(&a, &b, sizeof(PaintCell)) == 0; }
bool blank(const PaintTile &t) {
	return std::all_of(t.begin(), t.end(), [](const PaintCell &c) { return same(c, EMPTY_CELL); });
}
bool has_biome(const PaintCell &c) { return c.biome != 0 && c.biome_amount != 0; }
bool has_cover(const PaintCell &c) { return std::any_of(c.cover.begin(), c.cover.end(), [](uint8_t v) { return v != 0; }); }
constexpr float BYTE_MAX = 255.f; // a byte's top
constexpr float SWITCH_BELOW = 24.f; // a surface this thin gives way to the one being laid (a tenth of full)

uint8_t towards(uint8_t v, float target, float w) {
	float n = v + (target - v) * w;
	return uint8_t(clampf(std::ceil(n), 0.f, BYTE_MAX));
}
uint8_t faded(uint8_t v, float w) { return uint8_t(clampf(std::floor(v * (1.f - w)), 0.f, BYTE_MAX)); }
} // namespace

void GroundPaint::init(int samples_per_side, float spacing_m, const PaintDef *def) {
	n_ = samples_per_side;
	spacing_ = spacing_m;
	def_ = def;
	across_ = (n_ + PAINT_TILE - 1) / PAINT_TILE;
	clear();
}

void GroundPaint::clear() {
	tiles_.clear();
	biome_cells_ = cover_cells_ = 0;
}

void GroundPaint::count(const PaintCell &c, int sign) {
	biome_cells_ += has_biome(c) ? sign : 0;
	cover_cells_ += has_cover(c) ? sign : 0;
}

void GroundPaint::count(const PaintTile &t, int sign) {
	for (const PaintCell &c : t) count(c, sign);
}

const PaintCell &GroundPaint::cell(int ix, int iz) const {
	if (ix < 0 || iz < 0 || ix >= n_ || iz >= n_) return EMPTY_CELL;
	auto it = tiles_.find((iz / PAINT_TILE) * across_ + ix / PAINT_TILE);
	if (it == tiles_.end()) return EMPTY_CELL;
	return (*it->second)[size_t((iz % PAINT_TILE) * PAINT_TILE + ix % PAINT_TILE)];
}

float GroundPaint::cover_at(CoverKind kind, float x, float z) const {
	if (tiles_.empty()) return 0;
	int ix = int(std::floor(x / spacing_ + n_ * 0.5f + 0.5f)), iz = int(std::floor(z / spacing_ + n_ * 0.5f + 0.5f));
	return cell(ix, iz).cover[size_t(kind)] / BYTE_MAX;
}

void GroundPaint::tiles_under(float x, float z, float radius, std::vector<int> &out) const {
	int x0 = clampi(int(std::floor((x - radius) / spacing_ + n_ * 0.5f)) / PAINT_TILE, 0, across_ - 1);
	int x1 = clampi(int(std::ceil((x + radius) / spacing_ + n_ * 0.5f)) / PAINT_TILE, 0, across_ - 1);
	int z0 = clampi(int(std::floor((z - radius) / spacing_ + n_ * 0.5f)) / PAINT_TILE, 0, across_ - 1);
	int z1 = clampi(int(std::ceil((z + radius) / spacing_ + n_ * 0.5f)) / PAINT_TILE, 0, across_ - 1);
	for (int j = z0; j <= z1; j++)
		for (int i = x0; i <= x1; i++) out.push_back(j * across_ + i);
}

PaintCell &GroundPaint::touch(int ix, int iz) {
	std::unique_ptr<PaintTile> &t = tiles_[(iz / PAINT_TILE) * across_ + ix / PAINT_TILE];
	if (!t) t = std::make_unique<PaintTile>();
	return (*t)[size_t((iz % PAINT_TILE) * PAINT_TILE + ix % PAINT_TILE)];
}

void GroundPaint::paint_cell(PaintCell &c, const PaintDab &d, float w) const {
	if (d.kind == GULLY_CUT) {
		c.gully = towards(c.gully, BYTE_MAX, w);
		c.gully_wipe = faded(c.gully_wipe, w);
		return;
	}
	if (d.kind == GULLY_WIPE) {
		c.gully_wipe = towards(c.gully_wipe, BYTE_MAX, w);
		c.gully = faded(c.gully, w);
		return;
	}
	const PaintKindDef &k = def_->kinds[size_t(d.kind)];
	switch (k.cls) {
		case PaintClass::ERASER:
			c.amount = faded(c.amount, w);
			c.biome_amount = faded(c.biome_amount, w);
			for (uint8_t &v : c.cover) v = faded(v, w);
			break;
		case PaintClass::COVER: c.cover[size_t(k.slot)] = towards(c.cover[size_t(k.slot)], BYTE_MAX, w); break;
		case PaintClass::BIOME:
			if (c.biome != k.slot + 1) {
				c.biome_amount = faded(c.biome_amount, w);
				if (c.biome_amount < SWITCH_BELOW) { // the old one gone, the dab lays its own (as the lab's weights move both at once)
					c.biome = uint8_t(k.slot + 1);
					c.biome_amount = towards(c.biome_amount, BYTE_MAX, w);
				}
			} else {
				c.biome_amount = towards(c.biome_amount, BYTE_MAX, w);
			}
			break;
		case PaintClass::GROUND:
			if (c.surface != k.slot + 1) {
				c.amount = faded(c.amount, w);
				if (c.amount < SWITCH_BELOW) { // the old one gone, the dab lays its own: one dab of a landform's rock lays it
					c.surface = uint8_t(k.slot + 1);
					c.amount = towards(c.amount, BYTE_MAX, w);
				}
			} else {
				c.amount = towards(c.amount, BYTE_MAX, w);
			}
			break;
	}
}

bool GroundPaint::dab(const PaintDab &d, int rect[4]) {
	if (d.radius <= 0 || !def_) return false;
	int x0 = clampi(int(std::floor((d.x - d.radius) / spacing_ + n_ * 0.5f)), 0, n_ - 1), x1 = clampi(int(std::ceil((d.x + d.radius) / spacing_ + n_ * 0.5f)), 0, n_ - 1);
	int z0 = clampi(int(std::floor((d.z - d.radius) / spacing_ + n_ * 0.5f)), 0, n_ - 1), z1 = clampi(int(std::ceil((d.z + d.radius) / spacing_ + n_ * 0.5f)), 0, n_ - 1);
	float rim = std::max(def_->land_rim, 1.f - d.hardness);
	int cx0 = INT_MAX, cz0 = INT_MAX, cx1 = INT_MIN, cz1 = INT_MIN;
	std::vector<int> wiped; // tiles a cell of which went back to no paint: they go when no paint is left in them
	for (int iz = z0; iz <= z1; iz++)
		for (int ix = x0; ix <= x1; ix++) {
			float wx = (ix - n_ * 0.5f) * spacing_ - d.x, wz = (iz - n_ * 0.5f) * spacing_ - d.z;
			float r = std::sqrt(wx * wx + wz * wz) / d.radius;
			if (r >= 1.f) continue;
			float s = std::min(1.f, (1.f - r) / rim);
			float w = s * s * (3.f - 2.f * s) * d.strength;
			const PaintCell &was = cell(ix, iz);
			PaintCell now = was;
			paint_cell(now, d, w);
			if (same(now, was)) continue;
			PaintCell &c = touch(ix, iz);
			count(c, -1);
			c = now;
			count(c, 1);
			if (same(c, EMPTY_CELL)) wiped.push_back((iz / PAINT_TILE) * across_ + ix / PAINT_TILE);
			cx0 = std::min(cx0, ix); cx1 = std::max(cx1, ix);
			cz0 = std::min(cz0, iz); cz1 = std::max(cz1, iz);
		}
	if (cx0 > cx1) return false;
	std::sort(wiped.begin(), wiped.end());
	wiped.erase(std::unique(wiped.begin(), wiped.end()), wiped.end());
	for (int key : wiped) {
		auto it = tiles_.find(key);
		if (it != tiles_.end() && blank(*it->second)) tiles_.erase(it);
	}
	rect[0] = cx0; rect[1] = cz0; rect[2] = cx1; rect[3] = cz1;
	return true;
}

const PaintTile *GroundPaint::tile(int key) const {
	auto it = tiles_.find(key);
	return it == tiles_.end() ? nullptr : it->second.get();
}

void GroundPaint::put_tile(int key, const PaintTile *from) {
	auto it = tiles_.find(key);
	if (it != tiles_.end()) {
		count(*it->second, -1);
		if (!from || blank(*from)) {
			tiles_.erase(it);
			return;
		}
		*it->second = *from;
		count(*from, 1);
		return;
	}
	if (!from || blank(*from)) return;
	tiles_.emplace(key, std::make_unique<PaintTile>(*from));
	count(*from, 1);
}

void GroundPaint::tile_rect(int key, int rect[4]) const {
	rect[0] = (key % across_) * PAINT_TILE;
	rect[1] = (key / across_) * PAINT_TILE;
	rect[2] = std::min(n_ - 1, rect[0] + PAINT_TILE - 1);
	rect[3] = std::min(n_ - 1, rect[1] + PAINT_TILE - 1);
}

} // namespace city
