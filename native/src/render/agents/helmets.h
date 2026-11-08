// A motorcyclist's helmet (riderPoses.ts helmetShape): one small mesh, one MultiMesh, an instance with the rider's own numbers, the bone that poses it
// the head's (project/render/shaders/people_helmet.gdshader). Each body's fit comes from people.bin. Data: agents.json "held" "helmet".
#pragma once

#include "render/data_reader.h"
#include "render/render_context.h"
#include "render/row_keeper.h"
#include "render/rs_util.h"

namespace city {

class Helmets {
public:
	bool init(const RenderContext &ctx, const DataValue &helmet, godot::PackedStringArray &errors);
	void reset();
	godot::RID material() const { return material_; }
	void begin();
	float *row(uint64_t key) { return keeper_.place(key, 0, rows_); } // the row of the rider `key`
	void draw(const godot::RID &scenario, const godot::AABB &box, RenderLayer layer);

private:
	OwnedMesh mesh_;
	MultiMeshDraw draw_;
	InstanceRows rows_;
	godot::RID material_;
	RowKeeper keeper_;
};

} // namespace city
