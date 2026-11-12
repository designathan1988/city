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
bool covered(const PaintCell &c) { return std::any_of(c.cover.begin(), c.cover.end(), [](uint8_t v) { return v != 0; }); }
} // namespace

void GroundPaint::init(int samples_per_side, float spacing_m, const PaintDef *def) {
	n_ = samples_per_side;
	spacing_ = spacing_m;
	def_ = def;
	across_ = (n_ + PAINT_TILE - 1) / PAINT_TILE;
	clear();
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
	if (d.radius <= 0 || d.strength <= 0 || !def_) return false;
	int x0 = clampi(int(std::floor((d.x - d.radius) / spacing_ + n_ * 0.5f)), 0, n_ - 1), x1 = clampi(int(std::ceil((d.x + d.radius) / spacing_ + n_ * 0.5f)), 0, n_ - 1);
	int z0 = clampi(int(std::floor((d.z - d.radius) / spacing_ + n_ * 0.5f)), 0, n_ - 1), z1 = clampi(int(std::ceil((d.z + d.radius) / spacing_ + n_ * 0.5f)), 0, n_ - 1);
	float rim = std::max(def_->land_rim, 1.f - d.hardness);
	bool changed = false;
	int minx = n_, minz = n_, maxx = -1, maxz = -1;
	int bx0 = n_, bz0 = n_, bx1 = -1, bz1 = -1;
	for (int iz = z0; iz <= z1; iz++)
		for (int ix = x0; ix <= x1; ix++) {
			float wx = (ix - n_ * 0.5f) * spacing_ - d.x, wz = (iz - n_ * 0.5f) * spacing_ - d.z;
			float r = std::sqrt(wx * wx + wz * wz) / d.radius;
			if (r >= 1.f) continue;
			float s = std::min(1.f, (1.f - r) / rim);
			float w = s * s * (3.f - 2.f * s) * d.strength;
			const PaintCell before = cell(ix, iz);
			PaintCell after = before;
			paint_cell(after, d, w);
			if (after == before) continue;
			if (after.biome != before.biome || after.biome_amount != before.biome_amount) {
				bx0 = std::min(bx0, ix); bz0 = std::min(bz0, iz);
				bx1 = std::max(bx1, ix); bz1 = std::max(bz1, iz);
			}
			const bool was_cover = covered(before), now_cover = covered(after);
			if (was_cover != now_cover) {
				const int key = (iz / PAINT_TILE) * across_ + ix / PAINT_TILE;
				if (now_cover) { tile_cover_[key]++; covered_++; }
				else {
					auto found = tile_cover_.find(key);
					if (--found->second == 0) tile_cover_.erase(found);
					covered_--;
				}
			}
			touch(ix, iz) = after;
			changed = true;
			minx = std::min(minx, ix); minz = std::min(minz, iz);
			maxx = std::max(maxx, ix); maxz = std::max(maxz, iz);
		}
	if (bx1 >= bx0) this->changed(bx0, bz0, bx1, bz1);
	rect[0] = minx; rect[1] = minz; rect[2] = maxx; rect[3] = maxz;
	return changed;
}

const PaintTile *GroundPaint::tile(int key) const {
	auto it = tiles_.find(key);
	return it == tiles_.end() ? nullptr : it->second.get();
}

void GroundPaint::changed(int x0, int z0, int x1, int z1) {
	const auto world = [&](int sample) { return (float(sample) - float(n_) * .5f) * spacing_; };
	regions_.touch(world(x0) - spacing_, world(z0) - spacing_, world(x1) + spacing_, world(z1) + spacing_);
}

void GroundPaint::put_tile(int key, const PaintTile *from) {
	const PaintTile *before = tile(key);
	if ((!before && !from) || (before && from && *before == *from)) return;
	bool biome_changed = false;
	for (size_t i = 0; i < PAINT_TILE_CELLS && !biome_changed; i++) {
		const PaintCell &a = before ? (*before)[i] : EMPTY_CELL, &b = from ? (*from)[i] : EMPTY_CELL;
		biome_changed = a.biome != b.biome || a.biome_amount != b.biome_amount;
	}
	if (biome_changed) { int rect[4]; tile_rect(key, rect); changed(rect[0], rect[1], rect[2], rect[3]); }
	const auto old = tile_cover_.find(key);
	if (old != tile_cover_.end()) { covered_ -= old->second; tile_cover_.erase(old); }
	if (!from) {
		tiles_.erase(key);
		return;
	}
	std::unique_ptr<PaintTile> &t = tiles_[key];
	if (!t) t = std::make_unique<PaintTile>();
	*t = *from;
	const size_t count = size_t(std::count_if(t->begin(), t->end(), covered));
	if (count != 0) { tile_cover_[key] = count; covered_ += count; }
}

void GroundPaint::tile_rect(int key, int rect[4]) const {
	rect[0] = (key % across_) * PAINT_TILE;
	rect[1] = (key / across_) * PAINT_TILE;
	rect[2] = std::min(n_ - 1, rect[0] + PAINT_TILE - 1);
	rect[3] = std::min(n_ - 1, rect[1] + PAINT_TILE - 1);
}

} // namespace city
