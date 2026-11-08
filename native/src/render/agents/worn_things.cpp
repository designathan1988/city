// What people wear: the meshes, one a thing, each body's rest place and bone for each, the paints, and who wears what (worn_things.h).
#include "render/agents/worn_things.h"
#include "core/shader_contracts/people.h"

#include "core/color.h"
#include "core/random.h"
#include "render/agents/ragdoll.h"
#include "render/mesh_builder.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>

using namespace godot;

namespace city {

namespace {

struct NamedValue {
	const char *name;
	int value;
};
constexpr int BONE_HAND = -1, BONE_HEAD = -2; // the cooked file names these two bones itself; the others are a joint's
// The bones a thing can ride: the holding hand, the head, or the bone the ragdoll joint of that name sits on.
constexpr NamedValue BONES[] = { { "hand", BONE_HAND }, { "head", BONE_HEAD }, { "pelvis", ragdoll_joint::PEL }, { "chest", ragdoll_joint::CHE },
	{ "neck", ragdoll_joint::NEC } };
// The joints whose rest place a thing's parts are laid from.
constexpr NamedValue JOINTS[] = { { "pelvis", ragdoll_joint::PEL }, { "chest", ragdoll_joint::CHE }, { "neck", ragdoll_joint::NEC }, { "head", ragdoll_joint::HEA } };
constexpr const char *AGE_NAMES[PED_AGES] = { "adult", "child", "elder" }; // PedAge order
constexpr const char *PAINT = "paint"; // a part's colour that is the wearer's paint, picked from the thing's colours
constexpr int JOINT_XYZ = JOINT_FLOATS / JOINT_COUNT; // a joint's rest place in PersonVariation::joints

int bone_of(int choice, const PeopleAsset &asset) {
	if (choice == BONE_HAND) return asset.hand_bone;
	if (choice == BONE_HEAD) return asset.head_bone;
	return int(asset.joint_bone[size_t(choice)]);
}

// One part of a thing into its mesh, with its thing's index and whether it is painted in UV2.
void add_part(MeshBuilder &b, const DataValue &d, float thing) {
	const DataValue colour = d["colour"];
	const bool painted = colour.text() == PAINT;
	const Color col = painted ? Color(1, 1, 1) : rgba8_to_color(colour.colour()).srgb_to_linear(); // ALBEDO is linear
	const Vector2 t2(thing, painted ? 1.f : 0.f);
	const Vector3 at = d["at"].v3();
	const String shape = d["shape"].text();
	if (shape == "box") {
		const Vector3 half = d["size"].v3() * .5f;
		b.box(at - half, at + half, col, t2, true);
	} else if (shape == "cylinder") {
		b.cylinder(at, d["radius_m"].f(), d["height_m"].f(), d["sides"].whole(), col, true, t2);
	} else if (shape == "cone") {
		b.cone(at, d["radius_m"].f(), d["height_m"].f(), d["sides"].whole(), col, t2);
	} else {
		d["shape"].fail("box, cylinder or cone");
	}
}

} // namespace

bool WornThings::init(const RenderContext &ctx, const DataValue &worn, const PeopleAsset &asset, PackedStringArray &errors) {
	reset();
	material_ = ctx.material("people_worn", errors);
	if (!errors.is_empty()) return false;
	const DataValue kinds = worn["things"];
	const Array names = kinds.keys();
	if (names.size() > WORN_THINGS) {
		kinds.fail(String("at most ") + String::num_int64(WORN_THINGS) + String(" things, what the shader holds"));
		return false;
	}
	const float drawn_for = worn["body_height_m"].f();
	PackedInt32Array bones, colour_counts;
	PackedVector4Array places;
	PackedVector3Array paints;
	bones.resize(WORN_THINGS);
	bones.fill(0);
	colour_counts.resize(WORN_THINGS);
	colour_counts.fill(1);
	places.resize(int64_t(PEOPLE_BODIES) * WORN_THINGS);
	paints.resize(int64_t(WORN_THINGS) * WORN_COLOURS);
	things_ = std::vector<Thing>(size_t(names.size()));
	for (int k = 0; k < int(names.size()); k++) {
		Thing &t = things_[size_t(k)];
		const String name = names[k];
		t.name = name.utf8().get_data();
		const DataValue kind = kinds[t.name.c_str()];
		DataValue bone = kind["bone"], joint = kind["at"];
		const int choice = bone.reader.value_of(BONES, bone.utf8(), "bone");
		const int from = joint.reader.value_of(JOINTS, joint.utf8(), "joint");
		bones.set(k, bone_of(choice, asset));
		for (size_t v = 0; v < asset.variations.size(); v++) {
			const PersonVariation &pv = asset.variations[v];
			const float *rest = pv.joints + from * JOINT_XYZ;
			places.set(int64_t(v) * WORN_THINGS + k, Vector4(rest[0], rest[1], rest[2], pv.height / drawn_for));
		}
		const DataValue colours = kind["colours"];
		const int n = colours.size();
		if (n == 0 || n > WORN_COLOURS) {
			colours.fail(String("between 1 and ") + String::num_int64(WORN_COLOURS) + String(" colours, what the shader holds"));
			return false;
		}
		colour_counts.set(k, n);
		for (int i = 0; i < n; i++) {
			const Color c = rgba8_to_color(colours[i].colour()).srgb_to_linear();
			paints.set(int64_t(k) * WORN_COLOURS + i, Vector3(c.r, c.g, c.b));
		}
		const DataValue share = kind["share"];
		for (int a = 0; a < PED_AGES; a++) t.share[a] = share[AGE_NAMES[a]].f();
		t.rain_from = kind["rain_from"].f();
		MeshBuilder b;
		const DataValue parts = kind["parts"];
		for (int i = 0; i < parts.size(); i++) add_part(b, parts[i], float(k));
		if (!errors.is_empty()) return false;
		t.mesh = b.bake();
		t.keeper.draws(1);
	}
	RenderingServer *rs = RenderingServer::get_singleton();
	rs->material_set_param(material_, "worn_bone", bones);
	rs->material_set_param(material_, "worn_at", places);
	rs->material_set_param(material_, "worn_colours", paints);
	rs->material_set_param(material_, "worn_colour_count", colour_counts);
	rs->material_set_param(material_, "worn_roughness", worn["roughness"].f());
	return errors.is_empty();
}

void WornThings::reset() {
	for (Thing &t : things_) t.draw.free();
}

void WornThings::begin() {
	for (Thing &t : things_) {
		t.rows.reopen();
		t.keeper.begin();
	}
}

bool WornThings::wears(int k, uint32_t look, int age) const {
	const Thing &t = things_[size_t(k)];
	if (rain_ < t.rain_from) return false;
	return hash_float(hash_combine(look, uint32_t(k))) < t.share[age >= 0 && age < PED_AGES ? age : int(PedAge::ADULT)];
}

float *WornThings::row(uint64_t key, int k) {
	Thing &t = things_[size_t(k)];
	return t.keeper.place(key, 0, t.rows);
}

void WornThings::draw(const RID &scenario, const AABB &box, RenderLayer layer) {
	for (Thing &t : things_) {
		t.keeper.sweep([&](int) -> InstanceRows & { return t.rows; });
		t.draw.set(scenario, t.mesh.rid, material_, t.rows, box, true, layer);
	}
}

Dictionary WornThings::shown() const {
	Dictionary d;
	for (const Thing &t : things_) d[t.name.c_str()] = t.rows.count();
	return d;
}

} // namespace city
