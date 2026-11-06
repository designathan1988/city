// The body on its own (body_stage.h): one mesh, facade parts in it as plain boxes, the panes as a second surface.
#include "render/buildings/body_stage.h"

#include "render/buildings/pane_rooms.h"
#include "sim/buildings/interior/interior.h"
#include "sim/persistence/body_codec.h"

#include <godot_cpp/classes/rendering_server.hpp>

using namespace godot;

namespace city {

namespace {
constexpr int AXIS_FLOATS = 3, ALPHA_BITS = 8;

// A piece of the bays in the body's own mesh: its tones over its colour, no finish.
void append_prop(b40::Shell &s, const MeshBuilder &model, const b40::PropInstance &p) {
	auto axis = [&p](int i) { const float *a = p.basis + i * AXIS_FLOATS; return Vector3(a[0], a[1], a[2]); };
	const Vector3 x = axis(0), y = axis(1), z = axis(2);
	const Vector3 o(p.origin[0], p.origin[1], p.origin[2]);
	const b40::Lin colour = b40::lin_of(p.color >> ALPHA_BITS);
	auto at = [&](int i) { const Vector3 &v = model.v[i]; return o + x * v.x + y * v.y + z * v.z; };
	for (int64_t i = 0; i + 2 < model.idx.size(); i += 3) {
		const int a = model.idx[i], b = model.idx[i + 1], c = model.idx[i + 2];
		const Vector3 &n = model.n[a];
		const Color t = model.c[a];
		s.tri(at(a), at(b), at(c), x * n.x + y * n.y + z * n.z, b40::Lin{ colour.r * t.r, colour.g * t.g, colour.b * t.b }, b40::FIN_NONE);
	}
}
} // namespace

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
	const Ref<ArrayMesh> design = designs_ ? designs_->mesh(body.signature) : Ref<ArrayMesh>();
	if (design.is_null()) {
		b40::emit_facades(shell, shell, pl);
		b40::emit_roofs(shell, shell, pl);
	}
	b40::emit_lot(shell, shell, pl);
	for (const b40::PropInstance &p : shell.props) append_prop(shell, look.bay.models[p.kind], p);
	shell.glass = nullptr;
	shell.sheer = nullptr;
	shell.cards = nullptr;
	const int handle = next_++;
	RenderingServer *rs = RenderingServer::get_singleton();
	const RID mesh = rs->mesh_create();
	for (b40::Shell *part : { &shell, &glass, &sheer, &cards }) // a design of its own leaves the shell with no more than its lot, or nothing
		if (part->triangles() > 0) part->commit_to(mesh, true);
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
	draw.adopt(scenario, ghost ? ghost_ : RID(), !ghost, mesh, LAYER_BASE);
	rs->instance_set_custom_aabb(draw.inst, AABB(lo, hi - lo));
	draw.visible(true);
	drawn_[handle] = int(shell.triangles() + glass.triangles() + sheer.triangles() + cards.triangles());
	if (design.is_valid()) { // the lot's frame here is the world's: its y runs along -z, as the design's own
		const Design own{ rs->instance_create2(design->get_rid(), scenario), Transform3D(Basis(), pl.lot.at(-body.signature_x, -body.signature_y, 0)) };
		rs->instance_set_layer_mask(own.inst, LAYER_BASE);
		rs->instance_set_transform(own.inst, own.local);
		if (ghost) rs->instance_geometry_set_material_override(own.inst, ghost_);
		own_[handle] = own;
	}
	return handle;
}

void BodyStage::move(int handle, const Transform3D &at) {
	auto it = shown_.find(handle);
	if (it != shown_.end()) RenderingServer::get_singleton()->instance_set_transform(it->second.inst, at);
	auto own = own_.find(handle);
	if (own != own_.end()) RenderingServer::get_singleton()->instance_set_transform(own->second.inst, at * own->second.local);
}

int BodyStage::triangles(int handle) const {
	auto it = drawn_.find(handle);
	return it == drawn_.end() ? 0 : it->second;
}

void BodyStage::remove(int handle) {
	drawn_.erase(handle);
	auto own = own_.find(handle);
	if (own != own_.end()) {
		RenderingServer::get_singleton()->free_rid(own->second.inst);
		own_.erase(own);
	}
	auto it = shown_.find(handle);
	if (it == shown_.end()) return;
	it->second.free();
	shown_.erase(it);
}

void BodyStage::clear() {
	for (auto &e : own_) RenderingServer::get_singleton()->free_rid(e.second.inst);
	own_.clear();
	for (auto &e : shown_) e.second.free();
	shown_.clear();
	drawn_.clear();
}

} // namespace city
