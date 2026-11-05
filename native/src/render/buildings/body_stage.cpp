// The body on its own (body_stage.h): one mesh, facade parts in it as plain boxes, the panes as a second surface.
#include "render/buildings/body_stage.h"

#include "render/buildings/pane_rooms.h"
#include "sim/buildings/interior/interior.h"
#include "sim/persistence/body_codec.h"

#include <godot_cpp/classes/rendering_server.hpp>

using namespace godot;

namespace city {

int BodyStage::show(const b40::Data &data, const b40::BuildingLook &look, const b40::Body &body, const RID &scenario, bool ghost) {
	b40::PaneRooms rooms(data, body, b40::purpose_of(body, 0, 0));
	b40::Placed pl;
	pl.data = &data;
	pl.look = &look;
	pl.b = &body;
	pl.rooms = &rooms;
	pl.lot = b40::Basis{ Vector3(), Vector3(1, 0, 0), Vector3(0, 0, -1), Vector3(0, 1, 0) };
	pl.id = body_digest(body);
	pl.ground = [](const Vector3 &) { return 0.f; };
	b40::Shell shell, glass, sheer, cards;
	glass.rooms = true;
	shell.glass = &glass;
	shell.sheer = &sheer;
	shell.cards = &cards;
	b40::emit_facades(shell, shell, pl);
	b40::emit_roofs(shell, shell, pl);
	b40::emit_lot(shell, shell, pl);
	shell.glass = nullptr;
	shell.sheer = nullptr;
	shell.cards = nullptr;
	const int handle = next_++;
	RenderingServer *rs = RenderingServer::get_singleton();
	const RID mesh = rs->mesh_create();
	shell.commit_to(mesh, true);
	glass.commit_to(mesh, true);
	sheer.commit_to(mesh, true);
	cards.commit_to(mesh, true);
	Vector3 lo, hi, glass_lo, glass_hi;
	shell.bounds(lo, hi);
	for (const b40::Shell *more : { &glass, &sheer, &cards }) {
		if (more->triangles() == 0) continue;
		more->bounds(glass_lo, glass_hi);
		lo = lo.min(glass_lo);
		hi = hi.max(glass_hi);
	}
	MeshDraw &draw = shown_[handle];
	if (!ghost) { // as the city's chunks are drawn: the walls and the panes each with their material
		int surface = 0;
		if (shell.triangles() > 0) rs->mesh_surface_set_material(mesh, surface++, solid_);
		if (glass.triangles() > 0) rs->mesh_surface_set_material(mesh, surface++, glass_);
		if (sheer.triangles() > 0) rs->mesh_surface_set_material(mesh, surface++, sheer_);
		if (cards.triangles() > 0) rs->mesh_surface_set_material(mesh, surface, card_);
	}
	draw.adopt(scenario, ghost ? ghost_ : RID(), !ghost, mesh);
	rs->instance_set_custom_aabb(draw.inst, AABB(lo, hi - lo));
	draw.visible(true);
	drawn_[handle] = int(shell.triangles() + glass.triangles() + sheer.triangles() + cards.triangles());
	return handle;
}

void BodyStage::move(int handle, const Transform3D &at) {
	auto it = shown_.find(handle);
	if (it != shown_.end()) RenderingServer::get_singleton()->instance_set_transform(it->second.inst, at);
}

int BodyStage::triangles(int handle) const {
	auto it = drawn_.find(handle);
	return it == drawn_.end() ? 0 : it->second;
}

void BodyStage::remove(int handle) {
	drawn_.erase(handle);
	auto it = shown_.find(handle);
	if (it == shown_.end()) return;
	it->second.free();
	shown_.erase(it);
}

void BodyStage::clear() {
	for (auto &e : shown_) e.second.free();
	shown_.clear();
	drawn_.clear();
}

} // namespace city
