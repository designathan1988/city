// CityLab: the F7 laboratory's rules as script API, an area of the core (api/city_core.h) with no state of its own: the frames
// game/lab/doctor_run.gd measured go in, the verdict, the findings and the A/B come out (api/lab/diagnosis.h).
#pragma once

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/variant/dictionary.hpp>

namespace city {

class CityLab : public godot::Object {
	GDCLASS(CityLab, godot::Object)

public:
	// `frames` as FrameMeter.frames() gives it, `rules` the `doctor` table of data/game/render_switches.json.
	godot::Dictionary diagnose(const godot::Dictionary &frames, const godot::Dictionary &rules) const;
	// Three windows of frames: before the change, with it, and after undoing it.
	godot::Dictionary compare(const godot::Dictionary &before, const godot::Dictionary &after, const godot::Dictionary &again) const;

protected:
	static void _bind_methods();
};

} // namespace city
