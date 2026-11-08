// Wear: streets and footways that age with use. A field of bytes round the view, red the wheels, green the feet, 5 m a
// texel over 10.24 km, a window that wraps round (a cell keeps its texel while the view moves, and a texel that comes back
// into the window for another place is cleared). Godot has no partial texture update (RenderingServer texture_2d_update sends
// the whole image), so the field is a texture array of tiles and only the tiles that changed are sent. Never saved: the
// window follows the view, the wear past it is let go. Read by road.gdshader (wear.gdshaderinc).
#pragma once

#include "render/render_context.h"
#include "core/shader_contracts/streets.h"
#include "sim/state/pose_buffer.h"

#include <godot_cpp/variant/packed_byte_array.hpp>

#include <array>
#include <cstdint>
#include <vector>

namespace city {

class WearField {
public:
	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void reset(); // a new city: all clean
	void free();
	// The agents of a fresh publication wear what they stand on, for `elapsed` simulated seconds (up to most_step_s).
	void wear(const PoseFrame &f, float elapsed);
	void follow(float x, float z); // keeps the window round where the view looks
	void paint(float x0, float z0, float x1, float z1, float value); // a test sets wheels and feet over a rectangle: the same use in both games
	int64_t bytes() const { return int64_t(RES) * RES * CHANNELS; } // of the texture, and as much again here
	int64_t upload(float dt); // sends the tiles that changed and are due; returns their bytes (the upload ledger's)
	int64_t worn_cells() const; // cells any agent has left wear on (render_cost probe)
	int64_t publications() const { return publications_; }

private:
	static constexpr int LAYER_CELLS = WEAR_LAYER_CELLS, LAYERS_AXIS = WEAR_LAYERS_AXIS, RES = LAYER_CELLS * LAYERS_AXIS, CHANNELS = 2;
	void add(int channel, float x, float z, float amount);
	void clear_cell_range(int gx0, int gx1, int gy0, int gy1);
	void touch(int tile);
	void send_origin() const;
	float random();
	std::array<godot::PackedByteArray, LAYERS_AXIS * LAYERS_AXIS> tiles_;
	std::array<float, LAYERS_AXIS * LAYERS_AXIS> since_{}; // seconds since the tile was last sent
	std::array<bool, LAYERS_AXIS * LAYERS_AXIS> dirty_{};
	std::vector<int> pending_;
	godot::RID texture_, material_;
	int ox_ = 0, oy_ = 0; // the window's low corner, cells
	float cell_ = 1, upload_every_ = 1, wheels_ = 0, feet_ = 0, track_ = 0, most_step_ = 0;
	int hold_ = 0, per_frame_ = 1;
	uint32_t state_ = 1;
	int64_t publications_ = 0;
};

} // namespace city
