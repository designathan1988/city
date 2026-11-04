// The Roadcraft "4.0" building body: the vocabulary codes (bays, roofs, dress, finishes) in the 4.0 order and the
// plain structures the generator fills (lot plan, volumes with storeys and bay overrides, lot elements, cores).
// Lengths in metres in the lot frame: x along the street front from the lot's middle, y away from the street, z up.
#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace city::b40 {

constexpr float EPS = 1e-3f, FAR_AWAY = 1e9f; // geometric tolerance; 'no bound yet'
constexpr int MAX_FACES = 16, KEY_STOREY_SHIFT = 16, KEY_SIDE_SHIFT = 8;

// BAY_COMPONENTS, 4.0 order.
enum Bay : uint8_t { BAY_WALL, BAY_WINDOW, BAY_SASH, BAY_FRENCH, BAY_BAY_WINDOW, BAY_RIBBON, BAY_WIDE, BAY_BALCONY, BAY_DOOR,
	BAY_DOUBLE_DOOR, BAY_GARAGE_DOOR, BAY_SHOPFRONT, BAY_LOADING_DOOR, BAY_PILLAR, BAY_LOGGIA, BAY_BRISE, BAY_SHUTTERED,
	BAY_GOURMET, BAY_COUNT, BAY_NONE = 0xff };
enum Roof : uint8_t { ROOF_FLAT, ROOF_TERRACE, ROOF_GABLE, ROOF_HIP, ROOF_SHED, ROOF_SAWTOOTH };
enum Lines : uint8_t { LINES_EVERY, LINES_BASE, LINES_SLAB, LINES_NONE };
enum Crown : uint8_t { CROWN_CORNICE, CROWN_DECO, CROWN_SLAB, CROWN_ATTIC, CROWN_NONE };
enum Ground : uint8_t { GROUND_SHOP, GROUND_LOBBY, GROUND_HOUSE, GROUND_WORKS };
enum Kind : uint8_t { KIND_HOUSE, KIND_FLATS, KIND_TOWER, KIND_SHOP, KIND_OFFICE, KIND_WORKS, KIND_COUNT };
enum Use : uint8_t { USE_RESIDENTIAL, USE_COMMERCIAL, USE_INDUSTRIAL };
// Styles, 4.0 architecture.ts order (indices into buildings.json "styles").
enum Style : uint8_t { STYLE_COLONIAL, STYLE_ART_DECO, STYLE_MODERNIST, STYLE_CONTEMPORARY, STYLE_BRICK, STYLE_TROPICAL,
	STYLE_GLASS, STYLE_INDUSTRIAL, STYLE_COLONIAL_HOUSE, STYLE_BUNGALOW, STYLE_MODERN_HOUSE, STYLE_SOBRADO, STYLE_COUNT };
// Finishes in the COOKED texture-array layer order (project/assets/surfaces/building-finishes.*.png).
enum Finish : int8_t { FIN_NONE = -1, FIN_PLASTER, FIN_STUCCO, FIN_CERAMIC, FIN_BRICK, FIN_STONE, FIN_CONCRETE, FIN_WOOD,
	FIN_METAL, FIN_GLASS, FIN_TILE, FIN_SLATE, FIN_PANEL, FIN_ROOFING, FIN_COUNT };

// A material: finish layer + sRGB colour 0xRRGGBB.
struct Paint {
	int8_t finish = FIN_PLASTER;
	uint32_t rgb = 0;
};

// Lot plan (4.0 world/growth/lotPlan.ts).
enum FrontUse : uint8_t { FRONT_STREET, FRONT_GARDEN, FRONT_CARPAD, FRONT_FORECOURT, FRONT_PLAZA, FRONT_APRON };
enum SideUse : uint8_t { SIDE_ATTACHED, SIDE_PATH, SIDE_DRIVE, SIDE_DRIVE_PATH, SIDE_GARDEN };
enum BackUse : uint8_t { BACK_NONE, BACK_YARD, BACK_PARKING, BACK_SERVICE, BACK_COURTYARD, BACK_LOADING };
struct SidePlan {
	SideUse use = SIDE_ATTACHED;
	float width = 0;
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
};

// The composed geometry of one face (4.0 facadeGeometry): bay count, opening shares, sill, piers.
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
	std::unordered_map<uint32_t, int> storey_paint; // (storey, side) -> paint index (ArtDeco stone base)
	FaceGeometry faces[MAX_FACES];
	std::vector<Relief> reliefs;
	Lines lines = LINES_NONE;
	Crown crown = CROWN_NONE;
	bool keep = false; // not composed (shop back store, works hall)
	int top() const { return base + storeys; }
	int sides() const { return outline.empty() ? 4 : int(outline.size()); }
};

inline uint32_t bay_key(int storey, int side, int bay) { return (uint32_t(storey) << KEY_STOREY_SHIFT) | (uint32_t(side) << KEY_SIDE_SHIFT) | uint32_t(bay); }
inline uint32_t side_key(int storey, int side) { return (uint32_t(storey) << KEY_SIDE_SHIFT) | uint32_t(side); }

// Lot elements laid as boxes (4.0 element table): kind, centre, facing (0..3, 1/3 swap w and d), size, elevation.
enum Element : uint8_t { EL_PAVEMENT, EL_SLAB, EL_PILLAR, EL_CANOPY, EL_AWNING, EL_WALL, EL_FENCE, EL_RAILING, EL_HEDGE,
	EL_GATE, EL_PLANTER, EL_BENCH, EL_BOLLARD, EL_LAMP, EL_BIN, EL_PARKING, EL_AC, EL_TREE_PIT, EL_SURFACE };
struct LotElement {
	Element kind = EL_PAVEMENT;
	float x = 0, y = 0, w = 1, d = 1, z = 0, h = 1;
	int facing = 0;
	int paint = -1;
	float base = 0; // walls/fences: the base wall under a fence
};

struct Core {
	float x = 0, y = 0;
	int to = 0;
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
	float entrance_x = 0;
	int entrance_volume = 0;
	int add_paint(const Paint &p) {
		paints.push_back(p);
		return int(paints.size()) - 1;
	}
	float level_h(int level) const { return level == 0 ? ground_h : storey_h; }
	float level_z(int level) const { return level <= 0 ? 0.f : ground_h + storey_h * float(level - 1); }
	const Paint &wall_of(const Volume &v) const { return paints[v.wall >= 0 ? v.wall : wall]; }
	const Paint &roof_of(const Volume &v) const { return paints[v.roof_paint >= 0 ? v.roof_paint : roof]; }
};

} // namespace city::b40
