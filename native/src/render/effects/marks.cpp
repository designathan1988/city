// The ground marks (marks.h).
#include "render/effects/marks.h"

#include <godot_cpp/classes/box_mesh.hpp>

#include <algorithm>

using namespace godot;

namespace city {

void Marks::init(const RID &scenario, const RID &material, int room, int soot_kept, int blood_kept) {
	material_ = material;
	soot_kept_ = size_t(soot_kept);
	blood_kept_ = size_t(blood_kept);
	Ref<BoxMesh> box; // the decal's volume (marks.gdshader), drawn from inside
	box.instantiate();
	box->set_size(Vector3(1, 1, 1));
	layer_.init(scenario, box, material, room, false, world_box()); // see-through: the map's box keeps its place in the sort (mm_layer.h)
	layer_.count(0);
}

void Marks::reset() {
	craters_.clear();
	stale_.clear();
	soot_.clear();
	blood_.clear();
	rings_.clear();
	dirty_ = true;
}

void Marks::release() { layer_.release(); }

void Marks::add_soot(const Mark &m) {
	soot_.push_back(m);
	if (soot_.size() > soot_kept_) soot_.pop_front();
	dirty_ = true;
}

void Marks::add_blood(const Mark &m) {
	blood_.push_back(m);
	if (blood_.size() > blood_kept_) blood_.pop_front();
	dirty_ = true;
}

void Marks::ring(const Mark &m) {
	rings_.push_back(m);
	dirty_ = true;
}

void Marks::write(const Mark &m, int slot) {
	// as tall as it is wide, its middle on the mark's plane: the ground it lies on, as far under the plane as the mark is wide
	layer_.put(slot, Vector3(m.size, 0, 0), Vector3(0, m.size, 0), Vector3(0, 0, m.size), Vector3(m.x, m.y, m.z), float(m.kind), m.seed, float(m.born), float(m.life));
}

void Marks::update(double now) {
	RenderingServer *rs = RenderingServer::get_singleton();
	// the shader ages the marks by `now`: told while a mark stands (with none, nothing is drawn that reads it)
	if (!craters_.empty() || !soot_.empty() || !blood_.empty() || !rings_.empty()) rs->material_set_param(material_, "now", float(now));
	const size_t before = rings_.size();
	rings_.erase(std::remove_if(rings_.begin(), rings_.end(), [&](const Mark &r) { return now > r.born + r.life; }), rings_.end());
	if (!dirty_ && rings_.size() == before) return;
	layer_.reserve(int(craters_.size() + soot_.size() + blood_.size() + rings_.size()));
	int n = 0;
	for (const Mark &m : craters_) write(m, n++);
	for (const Mark &m : soot_) write(m, n++);
	for (const Mark &m : blood_) write(m, n++);
	for (const Mark &m : rings_) write(m, n++);
	shown_ = n;
	layer_.count(n);
	layer_.shown(n > 0);
	layer_.upload();
	dirty_ = false;
}

} // namespace city
