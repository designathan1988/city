// The building body: the vocabulary codes (bays, roofs, dress, finishes), in the order the lists of
// data/render/buildings.json name them, and the
// plain structures the generator fills (lot plan, volumes with storeys and bay overrides, lot elements, cores).
// Lengths in metres in the lot frame: x along the street front from the lot's middle, y away from the street, z up.
#pragma once

#include "sim/buildings/b40/lot_edges.h"

#include <cstdint>
#include <unordered_map>
#include <map>
#include <utility>
#include <vector>

namespace city::b40 {

constexpr float EPS = 1e-3f, FAR_AWAY = 1e9f; // geometric tolerance; 'no bound yet'
constexpr int MAX_FACES = 24, KEY_STOREY_SHIFT = 16, KEY_SIDE_SHIFT = 8;
constexpr float RISER_M = 0.17f; // the rise of a step, drawn at entrances and flights (ABNT NBR 9050 6.8: 0.16 to 0.18 m)

// The components a bay of a facade can hold (names in buildings.json "openings" and the styles' rhythms).
enum Bay : uint8_t { BAY_WALL, BAY_WINDOW, BAY_SASH, BAY_FRENCH, BAY_BAY_WINDOW, BAY_RIBBON, BAY_WIDE, BAY_BALCONY, BAY_DOOR,
	BAY_DOUBLE_DOOR, BAY_GARAGE_DOOR, BAY_SHOPFRONT, BAY_LOADING_DOOR, BAY_PILLAR, BAY_LOGGIA, BAY_BRISE, BAY_SHUTTERED,
	BAY_GOURMET, BAY_COUNT, BAY_NONE = 0xff };
// A coherent facade, applied in one command (Build > Facade).
enum FacadeKind : uint8_t { PATTERN_RESIDENTIAL, PATTERN_STOREFRONT, PATTERN_OFFICE, PATTERN_INDUSTRIAL, PATTERN_ARCADE, PATTERN_GALLERY,
	PATTERN_ART_DECO, PATTERN_ART_DECO_CROWN, PATTERN_OBSERVATION, PATTERN_COUNT };
// Where a component goes when a bay is clicked.
enum FacadeScope : uint8_t { SCOPE_BAY, SCOPE_ROW, SCOPE_COLUMN, SCOPE_STOREY, SCOPE_SIDE, SCOPE_VOLUME };
// What a paint pick reaches (Build > Paint).
enum PaintScope : uint8_t { PAINT_BUILDING_WALL, PAINT_BUILDING_ROOF, PAINT_BUILDING_TRIM, PAINT_VOLUME_WALL, PAINT_VOLUME_ROOF, PAINT_SIDE, PAINT_FLOOR };
enum Roof : uint8_t { ROOF_FLAT, ROOF_TERRACE, ROOF_GABLE, ROOF_HIP, ROOF_SHED, ROOF_SAWTOOTH };
enum Lines : uint8_t { LINES_EVERY, LINES_BASE, LINES_SLAB, LINES_NONE };
enum Crown : uint8_t { CROWN_CORNICE, CROWN_DECO, CROWN_SLAB, CROWN_ATTIC, CROWN_NONE };
enum Ground : uint8_t { GROUND_SHOP, GROUND_LOBBY, GROUND_HOUSE, GROUND_WORKS };
enum Kind : uint8_t { KIND_HOUSE, KIND_FLATS, KIND_TOWER, KIND_SHOP, KIND_OFFICE, KIND_WORKS, KIND_COUNT };
enum Use : uint8_t { USE_RESIDENTIAL, USE_COMMERCIAL, USE_INDUSTRIAL };
// Architectural styles, in the order of buildings.json "styles" (their indices there).
enum Style : uint8_t { STYLE_COLONIAL, STYLE_ART_DECO, STYLE_MODERNIST, STYLE_CONTEMPORARY, STYLE_BRICK, STYLE_TROPICAL,
	STYLE_GLASS, STYLE_INDUSTRIAL, STYLE_COLONIAL_HOUSE, STYLE_BUNGALOW, STYLE_MODERN_HOUSE, STYLE_TOWNHOUSE, STYLE_COUNT };
// Finishes in the COOKED texture-array layer order (project/assets/surfaces/building-finishes.*.png).
enum Finish : int8_t { FIN_NONE = -1, FIN_PLASTER, FIN_STUCCO, FIN_CERAMIC, FIN_BRICK, FIN_STONE, FIN_CONCRETE, FIN_WOOD,
	FIN_METAL, FIN_GLASS, FIN_TILE, FIN_SLATE, FIN_PANEL, FIN_ROOFING, FIN_COUNT };

// A material: finish layer + sRGB colour 0xRRGGBB.
struct Paint {
	int8_t finish = FIN_PLASTER;
	uint32_t rgb = 0;
};

// The plan of a lot: what its front, each side and its back are used for, and the building's envelope.
enum FrontUse : uint8_t { FRONT_STREET, FRONT_GARDEN, FRONT_CARPAD, FRONT_FORECOURT, FRONT_PLAZA, FRONT_APRON };
enum SideUse : uint8_t { SIDE_ATTACHED, SIDE_PATH, SIDE_DRIVE, SIDE_DRIVE_PATH, SIDE_GARDEN };
enum BackUse : uint8_t { BACK_NONE, BACK_YARD, BACK_PARKING, BACK_SERVICE, BACK_COURTYARD, BACK_LOADING };
struct SidePlan {
	SideUse use = SIDE_ATTACHED;
	float width = 0;
	float walk = 0; // SIDE_DRIVE_PATH: the footpath along the drive, on the building's side
};
struct LotPlan {
	Kind kind = KIND_HOUSE;
	float W = 0, D = 0;
	FrontUse front = FRONT_STREET;
	float front_depth = 0;
	SidePlan left, right;
	BackUse back = BACK_NONE;
	float back_depth = 0;
	int rows = 0;
	float x0 = 0, y0 = 0, x1 = 0, y1 = 0; // building envelope, x from the lot's left edge
	LotEdges edges; // what the lot's edges meet: the gates keep off its shut stretches, its street sides are closed as fronts
};

// The composed geometry of one face: bay count, opening shares, sill, piers.
struct FaceGeometry {
	int bays = -1;
	float window_w = 0, window_h = 0, sill = 0;
	float pier_w = 0, pier_d = 0;
	int pier_every = -1;
};

struct Relief {
	int side = 0, bay0 = 0, bay1 = 0, storey0 = 0, storey1 = 0;
	float depth = 0; // > 0 projects out of the wall, < 0 recesses
};

struct Point2 {
	float x = 0, y = 0;
};

// What the player sets on a roof, at a point of the plan; `turn` in radians, `h` what stands up from the roof.
enum RoofGear : uint8_t { GEAR_SOLAR, GEAR_SKYLIGHT, GEAR_VENT, GEAR_CHIMNEY, GEAR_TANK, GEAR_SPIRE, GEAR_LANTERN, GEAR_COUNT };
// A flag flown from a mast (a spire, a lantern): one of the Roadcraft's designs (flags.ts FLAG_PATTERNS, drawn by
// building_parts.gdshader) in three colours, 0xRRGGBB: the field, the second and the third.
enum FlagPattern : uint8_t {
	FLAG_SOLID, FLAG_BICOLOUR_H, FLAG_BICOLOUR_V, FLAG_TRICOLOUR_H, FLAG_TRICOLOUR_V, FLAG_CROSS, FLAG_NORDIC,
	FLAG_DIAGONAL, FLAG_SALTIRE, FLAG_CANTON, FLAG_STRIPES, FLAG_BORDER, FLAG_DISC, FLAG_CHEVRON, FLAG_COUNT
};
constexpr int FLAG_COLOURS = 3, FLAG_NONE = -1;
constexpr uint32_t FLAG_FIELD_BLUE = 0x0b5ea8, FLAG_WHITE = 0xffffff, FLAG_RED = 0xc8102e; // the default design's three colours
struct FlagDesign {
	int8_t pattern = FLAG_NONE; // a FlagPattern; FLAG_NONE: no flag flown
	uint32_t rgb[FLAG_COLOURS] = { FLAG_FIELD_BLUE, FLAG_WHITE, FLAG_RED }; // the Roadcraft's default design (flags.ts DEFAULT_FLAG)
};
constexpr FlagDesign DEFAULT_FLAG{ FLAG_TRICOLOUR_V }; // what a new lantern flies (buildingRoofs.ts); a spire flies none until set
struct RoofPart {
	uint8_t kind = GEAR_VENT;
	float x = 0, y = 0, w = 1, d = 1, h = 1, turn = 0;
	FlagDesign flag; // on a spire or a lantern
};

// How a block counts in the body (blocks.h): a solid one adds its space, a void takes it from the others, an intersect keeps of the others what lies in it, an exclusive
// one does the first where it stands alone and the second where it meets another.
enum BlockMode : uint8_t { BLOCK_SOLID, BLOCK_VOID, BLOCK_INTERSECT, BLOCK_XOR };

struct Volume {
	float x = 0, y = 0, w = 0, d = 0;
	int base = 0, storeys = 1;
	Roof roof = ROOF_FLAT;
	float pitch = -1; // degrees; < 0 = not set yet
	int ridge = -1, fall = -1;
	std::vector<Point2> outline; // normalised to the rectangle, CCW; empty = rectangle
	int wall = -1, roof_paint = -1; // paint indices into Body::paints (-1 = the building's)
	std::vector<uint8_t> fill; // per storey
	std::unordered_map<uint32_t, uint8_t> bays; // (storey, side, bay) -> Bay override
	std::unordered_map<uint32_t, uint8_t> side_fill; // (storey, side) -> Bay: a whole side of a storey, over `fill`
	std::unordered_map<uint32_t, int> storey_paint; // (storey, side) -> paint index (ArtDeco stone base)
	FaceGeometry faces[MAX_FACES];
	std::vector<Relief> reliefs;
	std::vector<RoofPart> gear;
	Lines lines = LINES_NONE;
	Crown crown = CROWN_NONE;
	bool keep = false; // not composed (shop back store, works hall)
	BlockMode mode = BLOCK_SOLID;
	float lift = 0; // a split level: how far the block's floors stand from the building's, metres
	int top() const { return base + storeys; }
	int sides() const { return outline.empty() ? 4 : int(outline.size()); }
};

// How far a bay of a storey stands from its wall.
inline float relief_depth(const Volume &v, int side, int bay, int k) {
	for (const Relief &r : v.reliefs)
		if (r.side == side && bay >= r.bay0 && bay <= r.bay1 && k >= r.storey0 && k <= r.storey1) return r.depth;
	return 0;
}

inline uint32_t bay_key(int storey, int side, int bay) { return (uint32_t(storey) << KEY_STOREY_SHIFT) | (uint32_t(side) << KEY_SIDE_SHIFT) | uint32_t(bay); }
inline uint32_t side_key(int storey, int side) { return (uint32_t(storey) << KEY_SIDE_SHIFT) | uint32_t(side); }

// A map keyed by storey in another numbering, dropping what falls outside [0, count).
template <typename M>
void move_storeys(M &m, int shift_bits, int shift, int count) {
	M next;
	for (const auto &e : m) {
		int k = int(e.first >> shift_bits) + shift;
		if (k < 0 || k >= count) continue;
		uint32_t rest = e.first & ((1u << shift_bits) - 1);
		next[(uint32_t(k) << shift_bits) | rest] = e.second;
	}
	m = std::move(next);
}

// What a side of a storey is filled with, and what one bay of it holds: its own override, else the side's, else the storey's.
inline uint8_t fill_at(const Volume &v, int storey, int side) {
	auto it = v.side_fill.find(side_key(storey, side));
	return it != v.side_fill.end() ? it->second : v.fill[size_t(storey)];
}
inline uint8_t bay_at(const Volume &v, int storey, int side, int bay) {
	auto it = v.bays.find(bay_key(storey, side, bay));
	return it != v.bays.end() ? it->second : fill_at(v, storey, side);
}

// Lot elements laid as boxes: kind, centre, size (w along the lot x, d along the lot y), elevation and facing. The
// facing of a car bay, a lorry dock, a bench or a lounger is the way its open end looks: 0 the street (-y), 1 +x,
// 2 away from the street (+y), 3 -x. A car gate is wide enough for a car; a person's gate is not.
enum Element : uint8_t { EL_PAVEMENT, EL_SLAB, EL_PILLAR, EL_CANOPY, EL_AWNING, EL_WALL, EL_FENCE, EL_RAILING, EL_HEDGE,
	EL_GATE, EL_CAR_GATE, EL_PLANTER, EL_BENCH, EL_BOLLARD, EL_LAMP, EL_BIN, EL_AC, EL_SURFACE, EL_STALL, EL_DOCK, EL_DRAIN,
	EL_TREE, EL_SHRUB, EL_FLOWERS, EL_POOL, EL_LOUNGER, EL_TABLE, EL_SWING, EL_SHED, EL_LINE, EL_PALLET, EL_CAGE, EL_STAIR, EL_RAMP,
	EL_ROCKS, EL_PARKING, EL_CLOCK };
enum Facing : uint8_t { FACE_STREET, FACE_RIGHT, FACE_BACK, FACE_LEFT };
// A planter's soil lies this far under its rim (drawn by render/buildings/lot_emit.cpp). What grows in a planter stands on that soil:
// its element's z is the planter's height less this (lot_space.cpp LotSpace::planter; render/trees/garden_place.cpp stands it there).
constexpr float PLANTER_SOIL_DROP_M = 0.06f;
// The elements that are plants of the gardens: a tree, a shrub, a run of hedge, a flower bed (drawn by render/trees/garden_place.h).
inline bool is_plant(Element kind) { return kind == EL_TREE || kind == EL_SHRUB || kind == EL_HEDGE || kind == EL_FLOWERS; }
struct LotElement {
	Element kind = EL_PAVEMENT;
	float x = 0, y = 0, w = 1, d = 1, z = 0, h = 1;
	int facing = 0;
	int paint = -1;
	float base = 0; // walls/fences: the base wall under a fence; a flight of a traced stair: how far up it starts
	float angle = 0; // radians about its middle: a piece of a run traced across the lot
	bool round = false; // a cover whose outline is the ellipse its rectangle holds (cover_shape.h): a plaza, a basin, a court's circle
};

// A core (stair and lift shaft) is a square of this side in the lot frame, from its (x, y) corner: the generator places it, the cut view draws it.
constexpr float CORE_SIDE_M = 3.f;

enum CoreKind : uint8_t { CORE_LIFT, CORE_STAIR, CORE_STAIR_LIFT };
struct Core {
	float x = 0, y = 0;
	int to = 0;
	CoreKind kind = CORE_LIFT;
};
inline float core_width(const Core &c, float side = CORE_SIDE_M) { return c.kind == CORE_STAIR ? 2 * side : side; } // a flight alone runs two modules

// A piece of furniture or a light as the player arranged it, in the lot frame; `kind` a FurnitureKind, `angle` the way it faces.
struct PlacedPiece {
	uint8_t kind = 0;
	float x = 0, y = 0, angle = 0;
};

// One lot's building: plan, heights, paints, volumes, elements (all in the lot frame).
struct Body {
	LotPlan plan;
	Kind kind = KIND_HOUSE;
	Use use = USE_RESIDENTIAL;
	Ground ground = GROUND_HOUSE;
	uint8_t style = 0;
	int palette = 0;
	float ground_h = 3, storey_h = 3;
	std::vector<Paint> paints;
	int wall = -1, roof = -1, trim = -1; // building defaults (paint indices)
	std::vector<Volume> volumes;
	std::vector<LotElement> elements;
	std::vector<Core> cores;
	std::map<int, std::vector<PlacedPiece>> furnishing; // by level: the floors the player arranged
	float entrance_x = 0;
	int entrance_volume = 0;
	// A design of its own (signature_kinds.h): its number, and where the lot's front lay in the design's frame. 0 is the facade kit. An edit gives it back to the kit.
	uint8_t signature = 0;
	float signature_x = 0, signature_y = 0;
	// Drawing only, never stored (interior/cut.h): this body cut open at this floor, for a camera looking along cut_view (lot frame).
	int cut_level = -1;
	Point2 cut_view;
	int add_paint(const Paint &p) {
		paints.push_back(p);
		return int(paints.size()) - 1;
	}
	float level_h(int level) const { return level == 0 ? ground_h : storey_h; }
	float level_z(int level) const { return level <= 0 ? 0.f : ground_h + storey_h * float(level - 1); }
	float block_z(const Volume &v, int level) const { return v.lift + level_z(level); } // a level of a block, over the building's floor
	const Paint &wall_of(const Volume &v) const { return paints[v.wall >= 0 ? v.wall : wall]; }
	const Paint &roof_of(const Volume &v) const { return paints[v.roof_paint >= 0 ? v.roof_paint : roof]; }
};

} // namespace city::b40
