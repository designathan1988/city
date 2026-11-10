// The mesh every bay with an opening of the mid level shares (bay_kit.h). Its positions are not metres: x and y name the lines of
// the bay and z the plane, and building_bay.gdshaderinc puts them where the instance says, so one mesh stands for every opening
// of the city. Laid out in that index space, which keeps the order of the real lines, so the faces wind and point as they will.
#include "render/buildings/bay_kit.h"

#include "render/mesh_builder.h"

using namespace godot;

namespace city::b40 {

namespace {
// The lines of a bay (building_bay.gdshaderinc).
constexpr float LEFT = 0, SILL_LEFT = 0.5f, JAMB_LEFT = 1, JAMB_RIGHT = 2, SILL_RIGHT = 2.5f, RIGHT = 3;
constexpr float WALL_FOOT = 0, UNDER_SILL = 0.5f, SILL = 1, HEAD = 2, TOP = 3;
constexpr float WALL_PLANE = 0, PANE_PLANE = -1, SILL_PLANE = 1;
enum PieceKind { PIECE_WALL, PIECE_REVEAL, PIECE_PANE, PIECE_SILL };
const Vector3 ALONG(1, 0, 0), UP(0, 1, 0), OUT(0, 0, 1);

void piece(MeshBuilder &mb, const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector3 &d, const Vector3 &out, PieceKind kind) {
	const Vector2 t(float(kind), 0);
	const Vector2 uv[4] = { t, t, t, t };
	mb.quad(a, b, c, d, Color(1, 1, 1, 1), out, uv, Vector2(0, BAY_MESH_MARK));
}

void wall(MeshBuilder &mb, float x0, float x1, float y0, float y1) {
	piece(mb, Vector3(x0, y0, WALL_PLANE), Vector3(x1, y0, WALL_PLANE), Vector3(x1, y1, WALL_PLANE), Vector3(x0, y1, WALL_PLANE), OUT, PIECE_WALL);
}
} // namespace

OwnedMesh bay_mesh(const RID &material) {
	MeshBuilder mb;
	wall(mb, LEFT, JAMB_LEFT, WALL_FOOT, TOP);
	wall(mb, JAMB_RIGHT, RIGHT, WALL_FOOT, TOP);
	wall(mb, JAMB_LEFT, JAMB_RIGHT, WALL_FOOT, SILL);
	wall(mb, JAMB_LEFT, JAMB_RIGHT, HEAD, TOP);
	const Vector3 bl(JAMB_LEFT, SILL, WALL_PLANE), br(JAMB_RIGHT, SILL, WALL_PLANE), tl(JAMB_LEFT, HEAD, WALL_PLANE), tr(JAMB_RIGHT, HEAD, WALL_PLANE);
	const Vector3 deep(0, 0, PANE_PLANE);
	piece(mb, bl + deep, bl, tl, tl + deep, ALONG, PIECE_REVEAL);
	piece(mb, br, br + deep, tr + deep, tr, -ALONG, PIECE_REVEAL);
	piece(mb, bl + deep, br + deep, br, bl, UP, PIECE_REVEAL);
	piece(mb, tl, tr, tr + deep, tl + deep, -UP, PIECE_REVEAL);
	piece(mb, bl + deep, br + deep, tr + deep, tl + deep, OUT, PIECE_PANE);
	const Vector3 sl(SILL_LEFT, SILL, WALL_PLANE), sr(SILL_RIGHT, SILL, WALL_PLANE), lip(0, 0, SILL_PLANE), drop(0, UNDER_SILL - SILL, 0);
	piece(mb, sl, sr, sr + lip, sl + lip, UP, PIECE_SILL);
	piece(mb, sl + lip + drop, sr + lip + drop, sr + lip, sl + lip, OUT, PIECE_SILL);
	return mb.bake(material, false).shared(); // its positions are indices, not metres: kept as they are; shared by every chunk's draw
}

} // namespace city::b40
