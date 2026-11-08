// The layout of the cooked signature designs (tools/assets/cook_signatures.py), read by the bodies drawn (signature_bodies.cpp) and by the
// bodies cut for a fall (signature_ruin.cpp): the files' folder and marks, a look's record in the table, a body's vertex.
#pragma once

#include "core/bytes.h"
#include "sim/buildings/b40/signature_kinds.h"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/string.hpp>

#include <cstdint>
#include <string>

namespace city::signature_file {

constexpr const char *FOLDER = "res://assets/lots/";
constexpr uint32_t MAGIC_LOOKS = 0x4D534352u, MAGIC_BODY = 0x42534352u; // "RCSM", "RCSB"
constexpr uint32_t FLAG_BLENDED = 1, FLAG_TWO_SIDED = 2, FLAG_WINDOWS = 4, FLAG_PAINTED_WINDOWS = 8, MOST_SURFACES = 64, MOST_LOOKS = 1024;
constexpr int NAME_BYTES = 24, RECORD_FLOATS = 10, RGBA = 4, NORMAL_BYTES = 4;
constexpr size_t VERTEX_BYTES = 5 * sizeof(float) + NORMAL_BYTES; // position, normal, uv: what each vertex takes in the file
constexpr int RED = 0, ROUGHNESS = 3, METALLIC = 4, GLOW = 5, OPACITY = 8, ALPHA_CUT = 9; // a record's floats
constexpr float NORMAL_SCALE = 127.f;

// A file: u32 magic, u32 size unpacked, then the packed bytes.
inline godot::PackedByteArray unpacked(const godot::String &path, uint32_t magic) {
	const godot::PackedByteArray raw = godot::FileAccess::get_file_as_bytes(path);
	ByteReader r{ raw.ptr(), size_t(raw.size()) };
	const uint32_t found = r.get<uint32_t>(), size = r.get<uint32_t>();
	if (!r.ok || found != magic) return godot::PackedByteArray();
	return raw.slice(int64_t(r.o)).decompress(size, godot::FileAccess::COMPRESSION_DEFLATE);
}

// The file of a body's number (b40/signature_kinds.h: 1 and up).
inline std::string file_of(int kind) {
	return std::string(FOLDER) + b40::SIGNATURE_KINDS[kind - 1] + ".sig";
}

// A look's record: its floats (colour, how it reflects, its glow, opacity, the picture's cut) and its flags.
struct LookRecord {
	float v[RECORD_FLOATS] = {};
	uint32_t flags = 0;
};

// The record of the look whose entry starts at `at` in the unpacked table.
inline LookRecord record_at(const godot::PackedByteArray &table, size_t at) {
	ByteReader r{ table.ptr(), size_t(table.size()) };
	r.o = at + NAME_BYTES;
	LookRecord out;
	for (float &f : out.v) f = r.get<float>();
	out.flags = r.get<uint32_t>();
	return out;
}

} // namespace city::signature_file
