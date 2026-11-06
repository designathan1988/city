// What a level of the building stream holds: a surface a job made, and the triangles and bytes each kind of surface keeps.
#pragma once

#include "render/buildings/shell.h"
#include "render/rs_util.h"

#include <cstdint>
#include <limits>

namespace city {

// What the meshes hold, surface by surface (BuildingRenderer::building_cost).
enum HeldKind { HELD_NEAR_WALLS, HELD_NEAR_PANES, HELD_NEAR_GUARDS, HELD_NEAR_CARDS, HELD_NEAR_PIECES, HELD_MID_WALLS, HELD_MID_PANES, HELD_MID_CARDS, HELD_KINDS };

struct HeldCost {
	int64_t triangles = 0, bytes = 0;
	void add(const b40::Shell &shell, bool with_uv2) {
		triangles += shell.triangles();
		bytes += shell.bytes(with_uv2);
	}
};

// A level surface the job made (bake: the arrays converted on the job's thread, rs_util.h OwnedMesh); no mesh when the shell held no triangle.
struct LevelSurface {
	OwnedMesh mesh;
	godot::AABB box;
	int glass_at = -1, sheer_at = -1, card_at = -1, lawn_at = -1, wash_at = -1; // the mesh surfaces of the panes, the glass guards, the railings, the lawns and the lanterns' light, -1: none
};

// A superchunk's far mesh, and when it is made again.
struct FarDraw {
	MeshDraw far;
	HeldCost held;
	bool dirty = false;
	bool built = false; // the far mesh of the current content is committed
	bool has_content = false; // and holds buildings
	int mask = 0; // its chunks drawn in detail (bit = chunk in the superchunk): hidden in the far mesh
	int mask_sent = -1; // the chunk_mask last given to the far instance (-1: none yet)
	int chunks = 0; // its streamed chunks: the superchunk lives while one is
	// A built far mesh is rebuilt on its own clock: not before `due` (a delay after the first change it waits for) nor within an interval of the
	// previous request (`asked`).
	double due = 0, asked = -std::numeric_limits<double>::infinity();
};

} // namespace city
