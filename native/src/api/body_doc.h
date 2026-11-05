// A building as a document: a project of lots/*.json, or a building of a Roadcraft city, read into a body. An unknown name is an error
// the caller sees, never a silent default.
#pragma once

#include "sim/buildings/b40/data.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

namespace city {

// `front`: where the body's lot front lay in the document's frame, once the body is fitted to its own plan (model_ops.h fit_plan).
bool body_from_doc(const godot::Dictionary &doc, const b40::Data &data, b40::Body &out, float &front_x, float &front_y, godot::PackedStringArray &errors);

} // namespace city
