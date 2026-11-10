// The buildings with a design of their own (b40/signature_kinds.h): the shape and pictures cooked from what the 4.0's lot lab draws, one mesh a kind,
// an instance each. They come and go with their chunk; the lot round them is the facade kit's as any other. The first time a kind stands
// in the city a job reads and unpacks its file and makes its arrays, levels and pictures; the main thread only uploads them (sync, within
// the frame's ledger) and places the instances that waited. A file that cannot be read is said once (push_error) and never read again.
#pragma once

#include "core/job_pool.h"
#include "render/chunk_jobs.h"
#include "render/frame_admission.h"
#include "render/rs_util.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/shader.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/rid.hpp>
#include <godot_cpp/variant/transform3d.hpp>

#include <cstdint>
#include <memory>
#include <mutex>
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
	void set_pool(JobPool *pool) { pool_ = pool; }
	void reset();
	void set_chunk(int64_t key, const std::vector<SignaturePlace> &places);
	void drop(int64_t key);
	void visible(int64_t key, bool on);
	int drawn() const; // instances standing (tests)
	// The kinds a job made ready, uploaded as the frame's ledger admits; the instances that waited for them placed.
	void sync(FrameAdmission &f);
	// The kind's mesh now (a body drawn apart: body_stage.h), made on the calling thread if no job made it yet; none when its
	// file is missing or bad.
	godot::RID mesh(int kind);

private:
	static constexpr int RECORD_FLOATS = 10; // a look's floats in the file (signature_bodies.cpp names them)
	struct Look {
		godot::Ref<godot::ShaderMaterial> made;
		float v[RECORD_FLOATS] = {};
		uint32_t flags = 0, width = 0, height = 0; // its picture's size, 0 when it has none
		size_t at = 0; // its picture's stream in looks_file_
		uint32_t packed = 0; // that stream's bytes
	};
	// A kind made ready off the main thread: each surface's arrays, levels and look, and the pictures of the looks it uses.
	struct Surface {
		godot::Array arrays;
		godot::Dictionary lods;
		uint32_t look = 0;
		int64_t bytes = 0;
	};
	struct Prepared {
		int kind = 0;
		bool ok = false;
		std::string file;
		std::vector<Surface> surfaces;
		std::unordered_map<uint32_t, godot::Ref<godot::Image>> pictures;
		int64_t bytes() const;
	};
	enum class State : uint8_t { NONE, ASKED, READY, FAILED };
	bool read_table(); // any thread, once
	std::shared_ptr<Prepared> prepare(int kind); // any thread
	bool finish(Prepared &p); // main thread: the uploads
	godot::Ref<godot::Image> picture(uint32_t index) const; // a look's picture with its mipmaps, any thread (after read_table)
	godot::Ref<godot::ShaderMaterial> look(uint32_t index, const godot::Ref<godot::Image> &picture);
	void release(int kind); // the places that waited for the kind
	void place(int64_t key, const SignaturePlace &p, bool on);

	godot::RID scenario_;
	uint32_t layer_ = 0;
	JobPool *pool_ = nullptr;
	std::mutex table_mutex_; // the table is read once, by whichever thread first needs it
	bool table_read_ = false, table_ok_ = false;
	godot::PackedByteArray looks_file_; // materials.sig as it is on disk: the pictures packed, each in its own stream
	std::vector<Look> looks_;
	godot::Ref<godot::Shader> solid_, blended_;
	std::vector<OwnedMesh> meshes_; // by kind, each made whole with its looks (surface_data.h)
	std::vector<State> state_; // by kind
	std::vector<std::shared_ptr<Prepared>> ready_; // made, waiting for the ledger
	Inbox<std::shared_ptr<Prepared>> inbox_;
	std::unordered_map<int64_t, std::vector<godot::RID>> chunks_;
	std::unordered_map<int64_t, std::vector<SignaturePlace>> waiting_; // places whose kind is still being made
	std::unordered_map<int64_t, bool> shown_; // each chunk's visibility, for the instances placed later
};

} // namespace city
