// The building mesh writer: one vertex stream for every face of a chunk level, with the
// vertex code the shader reads (COLOR.a = surface kind; UV2 = finish layer + 1 or pane size; UV = pane/window cell;
// CUSTOM0..3, bytes, on the near and mid shells: the room behind a pane, a far wall's window row, the frame),
// the tone (macro noise x foot grime) baked into the sRGB vertex colour, and box/rectangle helpers in any
// orthonormal basis (a facade's along/up/out, the lot's x/y/up).
#pragma once

#include "sim/buildings/b40/paint.h"
#include "sim/buildings/b40/types.h"

#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/rid.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <vector>

namespace city::b40 {

// Surface kinds (COLOR.a byte), read by project/render/shaders/building.gdshader.
enum SurfaceKind : uint8_t { SK_SHELL, SK_PANE, SK_PANE_DARK, SK_FAR_WALL, SK_LAMP, SK_LAWN, SK_PANE_CLEAR, SK_GROUND, SK_COUNT }; // SK_GROUND: a shell face lying on the lot, which stays on it when a level steps back

// A byte's 1.0 in CUSTOM0..3 (building_pane.gdshaderinc ROOM_SPAN_M, FRAME_SPAN_M): 5 cm and 1 mm steps, more than a
// pane shows.
constexpr float ROOM_SPAN_M = 12.75f, FRAME_SPAN_M = 0.255f;

// The room behind a pane, seen from the pane's middle: how far its walls stand along the facade, its depth, the floor
// and ceiling from the pane's middle, the finish of its floor, and the largest piece of furniture seen through it
// (-1 none): its height, its centre along the facade, half its width, its near and far side from the glass.
struct PaneRoom {
	float back = 0, ahead = 0, depth = 0, sill = 0, head = 0;
	uint8_t floor = 0;
	int piece = -1;
	float piece_h = 0, piece_at = 0, piece_half = 0, piece_near = 0, piece_far = 0;
};

// What a pane draws: its room's seed (one room, one lamp), a byte of its own for what hangs in it, the darker glass,
// the room, and the frame in front of the glass (the trim's sRGB colour; thickness 0: none).
struct PaneLook {
	uint16_t slot = 0;
	uint8_t bay = 0;
	bool dark = false;
	bool plain = false; // clear glass, no room behind it (a building cut open: its inside is real)
	PaneRoom room;
	Lin trim;
	float frame = 0;
	float recess = 0; // metres of opening the pane draws behind the wall's face, the glass at its back (0: none)
};

// What a card draws (building_card.gdshader cardOn).
enum CardKind : uint8_t { CARD_ROOF_RAILING = 2, CARD_RAILING_FRONT, CARD_RAILING_SIDE, CARD_BRISE };

// A far wall's window row: shares of the bay and the storey, flags (building_pane.gdshaderinc far_window).
constexpr int WINDOW_FLAG_BLANK = 4, WINDOW_FLAG_OFFICE = 8, WINDOW_DARK_SHIFT = 4; // the shift: PaneDark
struct FarWindows {
	float width = 0, sill = 0, head = 0;
	int flags = 0;
	Lin trim;
};

// An orthonormal frame: p = o + e0*a + e1*b + e2*c.
struct Basis {
	godot::Vector3 o, e0, e1, e2;
	godot::Vector3 at(float a, float b, float c) const { return o + e0 * a + e1 * b + e2 * c; }
};

// Box faces to leave out (bit per face).
constexpr uint8_t SKIP_NONE = 0, SKIP_E2_NEG = 1, SKIP_E2_POS = 2, SKIP_E0_NEG = 4, SKIP_E0_POS = 8, SKIP_E1_NEG = 16, SKIP_E1_POS = 32;

// One box of an instanced shell drawn as a unit-cube MultiMesh instance: the three basis columns already scaled by the
// box sides (axis x size), the centre, the shaded sRGB colour, its finish and the bar width (second largest side).
constexpr int BOX_BASIS_FLOATS = 9; // three basis columns of three floats

struct BoxInstance {
	float basis[BOX_BASIS_FLOATS] = {};
	float origin[3] = {};
	uint32_t color = 0xffffffff;
	int8_t fin = -1;
	float thick = 0;
};

// One bay of the mid level, an instance of the shared mesh (bay_kit.h): its axes along the bay, up it and out of the wall, the first
// two as long as the bay and the third a metre, then four exact integers as floats (building_bay.gdshaderinc): the opening's shares,
// the wall's and the trim's sRGB, and the seed, finish, window flags and depth.
struct BayInstance {
	float basis[BOX_BASIS_FLOATS] = {};
	float origin[3] = {};
	float shares = 0, wall = 0, trim = 0, code = 0;
};

// A piece of bay_props.h: which model, its axes with their scale and where it stands.
struct PropInstance {
	float basis[BOX_BASIS_FLOATS] = {};
	float origin[3] = {};
	uint32_t color = 0xffffffff;
	uint8_t kind = 0;
};

constexpr int CUSTOM_ARRAYS = 4;

// A flag flown from a mast, collected by a shell like its boxes: the top of its hoist in the world, its size and its design,
// drawn as one cloth instance of the chunk (flag_cloth.h, building_parts.gdshader).
struct FlagInstance {
	godot::Vector3 top;
	float width = 0, height = 0;
	FlagDesign design;
};

struct Shell {
	std::vector<godot::Vector3> pos, nrm;
	std::vector<godot::Color> col;
	std::vector<godot::Vector2> uv, uv2;
	std::vector<int32_t> idx;
	std::vector<BoxInstance> boxes; // box() collects instances instead of triangles when `instanced`
	std::vector<BayInstance> bays; // the mid level's openings (bay())
	std::vector<PropInstance> props; // what hangs on the bays and stands on their balconies
	std::vector<FlagInstance> flags; // the flags of the masts it drew (roof_gear.cpp)
	std::vector<uint32_t> custom[CUSTOM_ARRAYS]; // RGBA8 per vertex, in the near and mid shells (`rooms`)
	float grime_floor = -FAR_AWAY; // world height of the current block's floor (walls darken over its first 1.2 m)
	float shade = 1.f; // extra multiplier (rain streak tops)
	bool tone = true;
	bool coarse = false; // mid/far levels: fewer sides on round pieces
	bool instanced = false; // boxes become unit-cube instances; quads/tris still become geometry
	Shell *box_sink = nullptr; // where this shell's boxes go, as instances: a row of a band in place of a dozen triangles in the mesh
	bool rooms = false; // carries CUSTOM0..3 (the material reads them on panes and far walls alone)
	int custom_used = CUSTOM_ARRAYS; // how many of them go to the GPU: the far level takes its windows' code alone
	const std::vector<Lin> *far_tones = nullptr; // the far level: each finish's colour from afar over the tint, by layer
	Shell *sheer = nullptr; // where this shell's glass guards go: a surface drawn half seen through
	Shell *washes = nullptr; // where this shell's lantern light goes: a surface added to what is behind it
	Shell *lawns = nullptr; // where this shell's lawns go: a surface in the terrain's own grass
	Shell *cards = nullptr; // where this shell's railings go: a surface whose bars the fragment draws
	bool veiled = false; // the mid level's parts: what repeats under a pixel goes to a card
	bool slim = false; // boxes seen edge on, deep but thin: they leave by their thinnest side
	Shell *flat = nullptr; // the mid level: where its bays also go as merged rectangles, for where they are too small for their geometry
	Shell *glass = nullptr; // where this shell's panes and far walls go: a second surface, so only they pay for CUSTOM0..3

	bool empty() const { return idx.empty() && boxes.empty(); }
	size_t triangles() const { return idx.size() / 3; }
	// Quad a-b-c-d facing `out` in the paint (linear colour) with its finish.
	void quad(const godot::Vector3 &a, const godot::Vector3 &b, const godot::Vector3 &c, const godot::Vector3 &d, const godot::Vector3 &out,
			const Lin &lin, int8_t fin, SurfaceKind kind = SK_SHELL);
	void tri(const godot::Vector3 &a, const godot::Vector3 &b, const godot::Vector3 &c, const godot::Vector3 &out, const Lin &lin, int8_t fin);
	// Rectangle in the e0/e1 plane of `f` at e2 = c, facing +e2 (or -e2 when `back`).
	void rect(const Basis &f, float a0, float a1, float b0, float b1, float c, const Lin &lin, int8_t fin, bool back = false);
	void box(const Basis &f, float a0, float a1, float b0, float b1, float c0, float c1, const Lin &lin, int8_t fin, uint8_t skip = SKIP_NONE);
	// A railing's face at e2 = c: one quad, its bars drawn by building_card.gdshader, so they hold at any distance.
	void card(const Basis &f, float a0, float a1, float b0, float b1, float c, const Lin &lin, CardKind kind, float share = 0);
	// A bay with an opening, `f` at its left end on the storey floor: an instance, no triangles (bay_kit.h).
	void bay(const Basis &f, float W, float H, const Lin &wall, int8_t fin, uint32_t seed, const FarWindows &w, float depth);
	// A glass pane of w x h metres at e2 = c.
	void pane(const Basis &f, float a0, float a1, float b0, float b1, float c, const PaneLook &look);
	// The light of a lantern on its wall: a quad over a..b, at c off it. UV over the quad, UV2 the lantern's place (x, z).
	void wash(const Basis &f, float a0, float a1, float b0, float b1, float c);
	// A far wall rectangle carrying painted windows: uv = (bays, storeys) at its corners.
	void far_wall(const godot::Vector3 p[4], const godot::Vector3 &out, const godot::Vector2 cell[4], const Lin &lin, int8_t fin, uint32_t seed, const FarWindows &w);
	void commit_to(const godot::RID &mesh, bool with_uv2 = true, bool blends = false) const; // a surface that blends keeps its order
	int64_t bytes(bool with_uv2) const; // of the arrays commit_to hands over (chunk_jobs.h UploadBudget)
	void bounds(godot::Vector3 &mn, godot::Vector3 &mx) const;

private:
	int vert(const godot::Vector3 &p, const godot::Vector3 &n, const godot::Color &c, const godot::Vector2 &t, const godot::Vector2 &t2);
	Lin far_tint(const Lin &lin, int8_t fin) const;
	int sheet(const Basis &f, float a0, float a1, float b0, float b1, float c, const godot::Color &code, const godot::Vector2 &t2);
	godot::Color shaded(const godot::Vector3 &p, const Lin &lin, bool wall, SurfaceKind kind) const;
	void face(const godot::Vector3 *p, int count, const godot::Vector3 &out, const Lin &lin, int8_t fin, SurfaceKind kind);
};

float macro_noise(const godot::Vector3 &p);

} // namespace city::b40
