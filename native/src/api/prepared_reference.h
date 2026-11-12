// Opaque immutable indexed geometry root retained across asynchronous reference pictures and native preparation.
#pragma once
#include "sim/commands/survey_input.h"
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>

namespace city {
class PreparedReference : public godot::RefCounted {
	GDCLASS(PreparedReference, godot::RefCounted)
public:
	// Parts: points in Godot axes, already transformed once by the engine; index may be empty.
	static godot::Ref<PreparedReference> make(const godot::Array &parts);
	// Original API: expanded points already in survey plan axes. Keep its packed root without copying/expansion.
	static godot::Ref<PreparedReference> legacy(const godot::PackedVector3Array &triangles);
	const SurveyTriangles &triangles() const { return triangles_; }
protected:
	static void _bind_methods() {}
private:
	SurveyTriangles triangles_;
};
} // namespace city
