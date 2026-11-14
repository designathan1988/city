// The far traffic's cars: the streets outside the window of detailed traffic drawn from the flows' numbers (sim/state/flow_mark.h),
// nobody simulated one by one. A street's cars are its count: its queue standing packed at its end, the rest spaced over the free
// stretch before it and moving round that stretch at the street's speed, each car placed by the shader from the clock
// (far_cars.gdshader): nothing is written a frame. The window's own streets are written too, from the vehicles they hold: a
// detailed vehicle is drawn as itself only near the point looked at and near the eye (detailed_within), and the shader shows
// the stand-in wherever it is not, so no street in view is ever empty of the cars it carries. They are drawn on a layer of
// their own (LAYER_FAR_TRAFFIC): the camera stops seeing the vehicles' layer past the detail cut-off. The streets are filed by the
// sector of the map their middle is in; a sector is one MultiMesh with a box of its own, written again only when a street
// of it changed. Sizes and bounds: agents.json far_traffic, far_shadow.sector_m.
#pragma once

#include "render/mesh_builder.h"
#include "render/rs_util.h"
#include "sim/state/change_log.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace city {

class FarTraffic {
public:
	// `hull`: a car's plain body, its front along +Z; `paints`: the cars' colours; `slot_m`: the room a standing car takes.
	void init(const godot::RID &scenario, const godot::RID &material, const MeshBuilder &hull, float sector_m, float slot_m, int car_limit, float reach_m,
			const std::vector<godot::Color> &paints);
	// Where a vehicle is drawn as itself: within `published_m` of the point looked at and `drawn_m` of the eye (metres, flat).
	void detailed_within(float published_m, float drawn_m);
	void look_at(const godot::Vector3 &ground) { look_ = ground; } // the ground point the view looks at, each frame
	void collect(ChangeLog &log); // the streets whose drawn numbers changed, taken out of the log under its mutex
	void draw(double clock_s); // the sectors a mark touched written again and sent; the shader's clock and the point looked at
	void clear(); // another city: nothing drawn until its streets are told (the hull stays)
	void free(); // the hull too: before init, and at the end
	int cars() const { return cars_; } // drawn over the whole map
	int sectors() const { return int(sectors_.size()); }

private:
	struct Street { // by ds: what it was last told as, and the sector it is filed in
		FlowMark told;
		bool filed = false;
		int64_t sector = 0;
	};
	struct Sector {
		std::vector<int> streets;
		InstanceRows rows;
		MultiMeshDraw draw;
		float reach = 0; // how far past its square a street of it may run
		int cars = 0;
		bool dirty = false;
	};
	void take(const FlowMark &m);
	void file(int ds, Street &st);
	void unfile(int ds, Street &st);
	void write(int64_t key, Sector &sector);
	godot::RID scenario_, material_;
	godot::Vector3 look_;
	OwnedMesh hull_;
	float sector_m_ = 0, slot_m_ = 0, reach_m_ = 0;
	int car_limit_ = 0, cars_ = 0;
	std::vector<FlowMark> taken_; // the log's marks of this frame
	std::vector<godot::Color> paints_;
	std::vector<Street> streets_;
	std::unordered_map<int64_t, Sector> sectors_;
};

} // namespace city
