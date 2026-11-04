// The outside walls of the floor a building is cut open at: the ones between the camera and the floor come down to a
// skirting (a top and an inner face close them), the others show a plaster face inside. Called for every bay of that floor
// by the facade emitter (facade_emit.cpp).
#include "render/buildings/interior_emit.h"

namespace city::b40 {

bool cut_bay(Shell &shell, const Placed &pl, const BayCtx &c, Point2 out) {
	const Body &b = *pl.b;
	const Paint &plaster = pl.look->interior.paint("inner_wall");
	const Lin inner = paint_lin(plaster);
	const bool facing_camera = out.x * b.cut_view.x + out.y * b.cut_view.y < 0;
	if (!facing_camera) {
		shell.rect(c.f, 0, c.W, 0, c.H, -CUT_WALL_THICK_M, inner, plaster.finish, true);
		return false;
	}
	const float low = std::min(CUT_WALL_LOW_M, c.H);
	shell.rect(c.f, 0, c.W, 0, low, 0, c.wall, c.wall_fin);
	shell.rect(c.f, 0, c.W, 0, low, -CUT_WALL_THICK_M, inner, plaster.finish, true);
	const Basis flat{ c.f.o, c.f.e0, c.f.e2, c.f.e1 }; // its b axis out of the wall, its c axis up
	shell.rect(flat, 0, c.W, -CUT_WALL_THICK_M, 0, low, inner, plaster.finish);
	return true;
}

} // namespace city::b40
