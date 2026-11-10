// What a level of the building stream holds: a surface a job made, and the triangles and bytes each kind of surface keeps.
#pragma once

#include "render/buildings/far_casts.h"
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

// A level's mesh a job made, whole with its materials (bake: the arrays converted on the job's thread, mesh_draw.h OwnedMesh); no mesh when
// its shells held no triangle.
struct LevelSurface {
	OwnedMesh mesh;
	godot::AABB box;
};

// The materials a level's mesh is made with, a surface each: its walls, its panes or far walls, its glass guards, its railings, its lawns
// and the lanterns' light.
struct LevelMaterials {
	godot::RID walls, glass, sheer, card, lawn, wash;
	static LevelMaterials of_walls(const godot::RID &material) { // a mesh of one surface
		LevelMaterials m;
		m.walls = material;
		return m;
	}
};

// What a chunk's meshes are made with, the renderer's one set (made at init, copied into each job's result): the near and the mid level's
// surfaces (the mid level's panes are far walls in the walls' own material, and it has no guards nor lantern light), the near level's
// fine pieces (the walls' material) and the mid level's hull (the shadow caster's).
struct ChunkMaterials {
	LevelMaterials near, mid;
	godot::RID parts, cast;
};

// A superchunk's far mesh with what it throws in the sun (far_casts.h), and when it is made again.
struct FarDraw {
	MeshDraw far;
	FarCasts casts;
	void free() {
		far.free();
		casts.free();
	}
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

// What the inside of a building costs cut open (BuildingRenderer::interior_cost).
struct BuildingInteriorCost {
	int shell_triangles = 0, shell_boxes = 0; // the cut building without its inside
	int inside_triangles = 0, inside_boxes = 0; // the floor, cores, walls and furniture
	int64_t inside_bytes = 0;
};

// What the view asked of the last frame and what the frame's budget left unasked.
struct BuildingViewNeed {
	int want = 0, unmet = 0, requested = 0, done = 0;
};

} // namespace city
