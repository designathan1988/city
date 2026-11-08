// The mesh every bay with an opening of the mid level shares (bay_kit.h). Its positions are not metres: x and y name the lines of
// the bay and z the plane, and building_bay.gdshaderinc puts them where the instance says, so one mesh stands for every opening
// of the city. Laid out in that index space, which keeps the order of the real lines, so the faces wind and point as they will.
#include "render/buildings/bay_kit.h"

#include "core/shader_contracts/buildings.h"
#include "render/mesh_builder.h"

using namespace godot;

namespace city::b40 {

namespace {
// The lines of a bay (BayLines) and its pieces (BayPiece) are core/shader_contracts/buildings.h's: building_bay.gdshaderinc reads them.
const Vector3 ALONG(1, 0, 0), UP(0, 1, 0), OUT(0, 0, 1);

void piece(MeshBuilder &mb, const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector3 &d, const Vector3 &out, BayPiece kind) {
	const Vector2 t(float(kind), 0);
	const Vector2 uv[4] = { t, t, t, t };
	mb.quad(a, b, c, d, Color(1, 1, 1, 1), out, uv, Vector2(0, BAY_MESH_MARK));
}

void wall(MeshBuilder &mb, float x0, float x1, float y0, float y1) {
	piece(mb, Vector3(x0, y0, BAY_Z_WALL), Vector3(x1, y0, BAY_Z_WALL), Vector3(x1, y1, BAY_Z_WALL), Vector3(x0, y1, BAY_Z_WALL), OUT, BAY_PIECE_WALL);
}
} // namespace

Ref<ArrayMesh> bay_mesh() {
	MeshBuilder mb;
	wall(mb, BAY_X_LEFT, BAY_X_JAMB_LEFT, BAY_Y_FOOT, BAY_Y_TOP);
	wall(mb, BAY_X_JAMB_RIGHT, BAY_X_RIGHT, BAY_Y_FOOT, BAY_Y_TOP);
	wall(mb, BAY_X_JAMB_LEFT, BAY_X_JAMB_RIGHT, BAY_Y_FOOT, BAY_Y_SILL);
	wall(mb, BAY_X_JAMB_LEFT, BAY_X_JAMB_RIGHT, BAY_Y_HEAD, BAY_Y_TOP);
	const Vector3 bl(BAY_X_JAMB_LEFT, BAY_Y_SILL, BAY_Z_WALL), br(BAY_X_JAMB_RIGHT, BAY_Y_SILL, BAY_Z_WALL), tl(BAY_X_JAMB_LEFT, BAY_Y_HEAD, BAY_Z_WALL), tr(BAY_X_JAMB_RIGHT, BAY_Y_HEAD, BAY_Z_WALL);
	const Vector3 deep(0, 0, BAY_Z_PANE);
	piece(mb, bl + deep, bl, tl, tl + deep, ALONG, BAY_PIECE_REVEAL);
	piece(mb, br, br + deep, tr + deep, tr, -ALONG, BAY_PIECE_REVEAL);
	piece(mb, bl + deep, br + deep, br, bl, UP, BAY_PIECE_REVEAL);
	piece(mb, tl, tr, tr + deep, tl + deep, -UP, BAY_PIECE_REVEAL);
	piece(mb, bl + deep, br + deep, tr + deep, tl + deep, OUT, BAY_PIECE_PANE);
	const Vector3 sl(BAY_X_SILL_LEFT, BAY_Y_SILL, BAY_Z_WALL), sr(BAY_X_SILL_RIGHT, BAY_Y_SILL, BAY_Z_WALL), lip(0, 0, BAY_Z_SILL), drop(0, BAY_Y_UNDER_SILL - BAY_Y_SILL, 0);
	piece(mb, sl, sr, sr + lip, sl + lip, UP, BAY_PIECE_SILL);
	piece(mb, sl + lip + drop, sr + lip + drop, sr + lip, sl + lip, OUT, BAY_PIECE_SILL);
	return mb.commit();
}

} // namespace city::b40
