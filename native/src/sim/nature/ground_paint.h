// What the player painted over the land, in tiles of the height grid that exist only while a cell of theirs holds paint:
// one surface over the grass (sand, soil, snow, rock...) and how much of it, one biome and how much, the density of each
// cover standing on the ground (forest, scrub, flowers, rocks) and the gullies cut or wiped. The render draws the surface
// through Terrain3D's control map (render/terrain_bridge) and the plants read the biomes and covers (render/trees,
// render/cover). A map's paint has one form whatever made it (a dab, an undo, a load): its tiles holding paint.
#pragma once

#include "core/grid.h"
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
	void bind(const PaintDef *def) { def_ = def; } // a loaded city taking the live one's place points at the live catalog
	void clear();
	int tiles_across() const { return across_; }
	// The cell of a sample, or an empty one where nothing is painted.
	const PaintCell &cell(int ix, int iz) const;
	// A cover's density 0..1 at a point (nearest sample).
	float cover_at(CoverKind kind, float x, float z) const;
	// Whether any cell holds what the plants read: a biome with an amount (the ecology) or a cover's density.
	bool any_cover() const { return biome_cells_ + cover_cells_ > 0; }
	// The tiles a dab reaches, in `out` (before it is laid, for undo).
	void tiles_under(float x, float z, float radius, std::vector<int> &out) const;
	// Lays the dab; the sample rectangle of the cells whose bytes it changed goes in `rect` (x0, z0, x1, z1); false when
	// none changed. A tile is made only for a cell that takes paint, and goes when its last painted cell is wiped.
	bool dab(const PaintDab &d, int rect[4]);
	const PaintTile *tile(int key) const;
	void put_tile(int key, const PaintTile *from); // nullptr, or a tile holding no paint: the tile goes
	const std::unordered_map<int, std::unique_ptr<PaintTile>> &all() const { return tiles_; }
	// The sample rectangle of tile `key` (undo, load).
	void tile_rect(int key, int rect[4]) const;

private:
	PaintCell &touch(int ix, int iz);
	void paint_cell(PaintCell &c, const PaintDab &d, float w) const;
	void count(const PaintCell &c, int sign); // the cells holding a biome or a cover
	void count(const PaintTile &t, int sign);
	int n_ = 0, across_ = 0;
	float spacing_ = 1;
	const PaintDef *def_ = nullptr;
	std::unordered_map<int, std::unique_ptr<PaintTile>> tiles_;
	int biome_cells_ = 0, cover_cells_ = 0;
};

} // namespace city
