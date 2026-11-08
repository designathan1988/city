// The core's enums for the scripts (CityCore::constants "enums"): {enum name: {enumerator name: value}}. The one place scripts,
// tests and captures read what a code means; none of them types an enum's number again.
#pragma once

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>

namespace city {

godot::Dictionary script_enums();

} // namespace city
