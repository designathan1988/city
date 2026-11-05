// The painted ground (see ground_paint.h). A dab takes a cell towards its layer by a smooth falloff from its centre to
// its rim, squeezed into the rim by the hardness; another surface over one laid before fades it out first.
#include "sim/nature/ground_paint.h"

#include "core/math.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
const PaintCell EMPTY_CELL{};
constexpr float BYTE_MAX = 255.f;
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
	tiles_.clear();
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
				if (c.biome_amount < SWITCH_BELOW) c.biome = uint8_t(k.slot + 1);
			} else {
				c.biome_amount = towards(c.biome_amount, BYTE_MAX, w);
			}
			break;
		case PaintClass::GROUND:
			if (c.surface != k.slot + 1) {
				c.amount = faded(c.amount, w);
				if (c.amount < SWITCH_BELOW) c.surface = uint8_t(k.slot + 1);
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
	bool changed = false;
	for (int iz = z0; iz <= z1; iz++)
		for (int ix = x0; ix <= x1; ix++) {
			float wx = (ix - n_ * 0.5f) * spacing_ - d.x, wz = (iz - n_ * 0.5f) * spacing_ - d.z;
			float r = std::sqrt(wx * wx + wz * wz) / d.radius;
			if (r >= 1.f) continue;
			float s = std::min(1.f, (1.f - r) / rim);
			float w = s * s * (3.f - 2.f * s) * d.strength;
			PaintCell &c = touch(ix, iz);
			paint_cell(c, d, w);
			changed = true;
		}
	rect[0] = x0; rect[1] = z0; rect[2] = x1; rect[3] = z1;
	return changed;
}

const PaintTile *GroundPaint::tile(int key) const {
	auto it = tiles_.find(key);
	return it == tiles_.end() ? nullptr : it->second.get();
}

void GroundPaint::put_tile(int key, const PaintTile *from) {
	if (!from) {
		tiles_.erase(key);
		return;
	}
	std::unique_ptr<PaintTile> &t = tiles_[key];
	if (!t) t = std::make_unique<PaintTile>();
	*t = *from;
}

void GroundPaint::tile_rect(int key, int rect[4]) const {
	rect[0] = (key % across_) * PAINT_TILE;
	rect[1] = (key / across_) * PAINT_TILE;
	rect[2] = std::min(n_ - 1, rect[0] + PAINT_TILE - 1);
	rect[3] = std::min(n_ - 1, rect[1] + PAINT_TILE - 1);
}

} // namespace city
