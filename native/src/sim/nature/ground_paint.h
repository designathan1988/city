// What the player painted over the land, in tiles of the height grid made only where touched: one surface over the
// grass (sand, soil, snow, rock...) and how much of it, one biome and how much, the density of each cover standing on
// the ground (forest, scrub, flowers, rocks) and the gullies cut or wiped. The render draws the surface through
// Terrain3D's control map (render/terrain_bridge) and the plants read the covers (render/trees, render/cover).
#pragma once

#include "core/grid.h"
#include "core/region_versions.h"
#include "sim/catalog/nature_types.h"

#include <array>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

namespace city {

struct PaintCell {
	uint8_t surface = 0; // 0 none, else a ground kind's table entry + 1
	uint8_t amount = 0; // 0..255 over the grass
	uint8_t biome = 0; // 0 none, else the biome's index + 1
	uint8_t biome_amount = 0;
	std::array<uint8_t, COVER_KINDS> cover{};
	uint8_t gully = 0, gully_wipe = 0;
	bool operator==(const PaintCell &o) const {
		return surface == o.surface && amount == o.amount && biome == o.biome && biome_amount == o.biome_amount
				&& cover == o.cover && gully == o.gully && gully_wipe == o.gully_wipe;
	}
};

constexpr int PAINT_TILE = 64; // samples per tile side
constexpr int PAINT_TILE_CELLS = PAINT_TILE * PAINT_TILE;
using PaintTile = std::array<PaintCell, PAINT_TILE_CELLS>;

struct PaintDab {
	float x = 0, z = 0, radius = 0;
	float strength = 0; // 0..1 at the centre
	float hardness = 0; // 0 the smooth dome .. 1 a hard rim
	int kind = 0; // index in PaintDef::kinds; GULLY_CUT / GULLY_WIPE for the gully brush
};
constexpr int GULLY_CUT = -1, GULLY_WIPE = -2;

class GroundPaint {
public:
	void init(int samples_per_side, float spacing_m, const PaintDef *def);
	void clear() { tiles_.clear(); tile_cover_.clear(); covered_ = 0; regions_.reset(); }
	int tiles_across() const { return across_; }
	uint64_t biome_stamp(float x0, float z0, float x1, float z1) const { return regions_.stamp(x0, z0, x1, z1); }
	// The cell of a sample, or an empty one where nothing is painted.
	const PaintCell &cell(int ix, int iz) const;
	// A cover's density 0..1 at a point (nearest sample).
	float cover_at(CoverKind kind, float x, float z) const;
	bool any_cover() const { return covered_ != 0; }
	// The tiles a dab reaches, in `out` (before it is laid, for undo).
	void tiles_under(float x, float z, float radius, std::vector<int> &out) const;
	// Lays the dab; the sample rectangle it changed goes in `rect` (x0, z0, x1, z1); false when nothing changed.
	bool dab(const PaintDab &d, int rect[4]);
	const PaintTile *tile(int key) const;
	void put_tile(int key, const PaintTile *from); // nullptr: the tile goes
	const std::unordered_map<int, std::unique_ptr<PaintTile>> &all() const { return tiles_; }
	// The sample rectangle of tile `key` (undo, load).
	void tile_rect(int key, int rect[4]) const;

private:
	RegionVersions regions_;
	void changed(int x0, int z0, int x1, int z1);
	PaintCell &touch(int ix, int iz);
	void paint_cell(PaintCell &c, const PaintDab &d, float w) const;
	int n_ = 0, across_ = 0;
	float spacing_ = 1;
	const PaintDef *def_ = nullptr;
	std::unordered_map<int, std::unique_ptr<PaintTile>> tiles_;
	std::unordered_map<int, size_t> tile_cover_;
	size_t covered_ = 0; // cells with an actual standing-cover channel, independent of surface/gully tile allocation
};

} // namespace city
