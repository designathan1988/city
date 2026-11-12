// How big a species' model draws: the points of its meshes as the kit bakes them (render/trees/tree_kit.h), measured once when the
// species is built. Whoever stands a plant in a room (render/trees/garden_place.h) and the probes that hold the plants against their
// rooms (render/trees/garden_census.h) read it: nothing else says how wide, deep or tall a model is.
// The contract with render/shaders/vegetation.gdshaderinc (tree_vertex): an instance whose basis is sx long across the model's x, sy
// up and sz across its z draws a point at (x * sx, y * sy, z * sz), and a leaf card sx times its model size, turned to the eye,
// whatever the instance's other scales.
#pragma once

#include "render/mesh_builder.h"

#include <algorithm>
#include <vector>

namespace city {

struct DrawnPoint {
	float x = 0, y = 0, z = 0;
	float reach = 0; // how far what is drawn at the point stands from it, model units: a card's leaves; 0 for a face's corner
};

class DrawnBody {
public:
	// `body`: the faces. `cards`: the leaf cards (a card's four corners at its centre, UV2.y its half side) and the frond strips as
	// they lie. `tuft`: how far a card's leaves stand from its centre, of its side (LeafAtlas::tuft_reach).
	void measure(const MeshBuilder &body, const MeshBuilder &cards, float tuft);
	bool empty() const { return points_.empty(); }
	const std::vector<DrawnPoint> &points() const { return points_; }
	float faces_x() const { return faces_x_; } // the faces' half length along the model's x, for a scale of one: no card in it
	float back_x(float sx) const; // how far the model draws toward -x from its axis
	float fore_x(float sx) const; // and toward +x
	float half_x(float sx) const { return std::max(back_x(sx), fore_x(sx)); } // the drawn half extent along the model's x
	float half_z(float sx, float sz) const; // along its z
	float around(float sx, float sz) const; // the farthest drawn point from its axis in plan: its half extent whatever its turn
	float top(float sx, float sy) const; // the highest drawn point
	// The largest z scale at which the model drawn sx along its x stands within `half` of its axis across; 0 when its cards alone pass it.
	float deep_within(float sx, float half) const;
	// The largest y scale at which the model drawn sx along its x stands no higher than `top`; 0 when its cards alone pass it.
	float tall_within(float sx, float top) const;

private:
	std::vector<DrawnPoint> points_;
	float faces_x_ = 0;
};

} // namespace city
