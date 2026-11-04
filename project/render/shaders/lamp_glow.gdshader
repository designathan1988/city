// The light of a street lamp at night (Roadcraft scenery.ts pool and beam): a pool on the ground and a cone down from the
// lens, added to what is behind. One mesh for both (render/props/prop_kit.h light()): UV2.y 1 the pool, 2 the beam;
// INSTANCE_CUSTOM.a the parts an instance shows. Nothing is drawn by day.
shader_type spatial;
render_mode blend_add, unshaded, cull_disabled, depth_draw_never, shadows_disabled;

global uniform float night;
uniform vec3 glow_color : source_color;
uniform float pool_gain = 0.32;
uniform float beam_gain = 0.08;

varying float v_part;
varying float v_fade;

#include "res://render/shaders/color_space.gdshaderinc"

void vertex() {
	v_part = UV2.y;
	v_fade = v_part < 1.5 ? 1.0 - UV.x : UV.y;
	int shown = int(INSTANCE_CUSTOM.a + 0.5);
	if ((shown & int(v_part + 0.5)) == 0) {
		VERTEX = vec3(0.0);
	}
}

void fragment() {
	float gain = v_part < 1.5 ? pool_gain : beam_gain;
	ALBEDO = linear_out(source_linear(glow_color) * v_fade * v_fade * gain * night);
	ALPHA = 1.0;
}
