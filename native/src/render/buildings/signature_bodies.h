// The buildings with a design of their own (b40/signature_kinds.h): the shape and pictures cooked from what the 4.0's lot lab draws, one mesh a kind, loaded the
// first time one stands in the city, an instance each. They come and go with their chunk; the lot round them is the facade kit's as any other.
#pragma once

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/shader.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/rid.hpp>
#include <godot_cpp/variant/transform3d.hpp>

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace city {

struct SignaturePlace {
	int kind = 0; // the body's number
	godot::Transform3D at;
};

class SignatureBodies {
public:
	void init(const godot::RID &scenario, uint32_t layer);
	void reset();
	void set_chunk(int64_t key, const std::vector<SignaturePlace> &places);
	void drop(int64_t key);
	void visible(int64_t key, bool on);
	int drawn() const; // instances standing (tests)
	godot::Ref<godot::ArrayMesh> mesh(int kind); // nullptr when its file is missing or bad

private:
	struct Look {
		godot::Ref<godot::ShaderMaterial> made;
		size_t at = 0; // its record in the table
	};
	bool read_table();
	godot::Ref<godot::ShaderMaterial> look(uint32_t index);

	godot::RID scenario_;
	uint32_t layer_ = 0;
	godot::PackedByteArray table_; // materials.sig, unpacked
	std::vector<Look> looks_;
	godot::Ref<godot::Shader> solid_, blended_;
	std::vector<godot::Ref<godot::ArrayMesh>> meshes_;
	std::vector<bool> mesh_attempted_;
	bool table_attempted_ = false, table_ready_ = false; // asset epoch; reset(city) retains both positive and negative cache
	std::unordered_map<int64_t, std::vector<godot::RID>> chunks_;
};

} // namespace city
