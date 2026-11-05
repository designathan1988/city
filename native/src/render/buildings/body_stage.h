// A body drawn whole, away from the city's chunks: the ghost under the pointer and the Models shelf's pictures. Its mesh is made once,
// with the emitters the city draws with, on flat ground with the middle of the lot's front at the origin; the ghost then only moves.
#pragma once

#include "render/buildings/facade.h"
#include "render/rs_util.h"

#include <godot_cpp/variant/transform3d.hpp>

#include <unordered_map>

namespace city {

class BodyStage {
public:
	void init(const godot::RID &solid, const godot::RID &glass, const godot::RID &sheer, const godot::RID &card, const godot::RID &ghost) {
		card_ = card;
		solid_ = solid;
		glass_ = glass;
		sheer_ = sheer;
		ghost_ = ghost;
	}
	int show(const b40::Data &data, const b40::BuildingLook &look, const b40::Body &body, const godot::RID &scenario, bool ghost);
	void move(int handle, const godot::Transform3D &at);
	void remove(int handle);
	int triangles(int handle) const; // what a handle draws (the tests read it)
	void clear();

private:
	godot::RID solid_, glass_, sheer_, card_, ghost_;
	std::unordered_map<int, MeshDraw> shown_;
	std::unordered_map<int, int> drawn_;
	int next_ = 1;
};

} // namespace city
