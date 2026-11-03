// Map layers of the Layers popover (Roadcraft hud.ts:508-522, specs ui.md §7.2): zone colours on the zoned road
// sides and the congestion map (each carriageway coloured by its vehicles' mean speed against the limit). Each is one
// mesh rebuilt from CityState when the roads change (zones) or every refresh_s (congestion). Only reads the state.
#pragma once

#include "render/render_context.h"
#include "render/rs_util.h"
#include "sim/state/city_state.h"

#include <godot_cpp/variant/packed_color_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>

namespace city {

enum class Overlay : int { ZONES = 0, CONGESTION = 1, COUNT = 2 };

class OverlayRenderer {
public:
	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void reset();
	void set(Overlay o, bool on);
	bool is_on(Overlay o) const { return on_[int(o)]; }
	void roads_changed() { zones_dirty_ = true; }
	void update(const CityState &s, const PoseFrame &f, double dt);

private:
	struct Ribbon {
		godot::PackedVector3Array v;
		godot::PackedColorArray c;
	};
	void strip(Ribbon &out, const CityState &s, int seg, float u0, float u1, const godot::Color &col, bool on_road) const;
	void build_zones(const CityState &s);
	void build_congestion(const CityState &s, const PoseFrame &f);
	void upload(MeshDraw &d, Ribbon &r);

	godot::RID scenario_, material_;
	MeshDraw draws_[int(Overlay::COUNT)];
	bool on_[int(Overlay::COUNT)] = {};
	bool zones_dirty_ = true;
	double since_ = 0;
	float zone_depth_ = 0, lift_ = 0, sample_ = 1, refresh_ = 1, free_share_ = 1, jam_share_ = 0;
	godot::Color jam_colors_[3];
};

} // namespace city
