// A street lamp's pool of light on whatever lies under it (render/props/lamp_pools.h): one instance of a unit box a lit lamp, its basis
// the pool's whole axes (across the street, up through the slab it lights, along the street at its grade; square to each other), its
// origin the pool's middle on the street's top. Each pixel's surface is read back from the depth buffer and lit by the pool's fall in
// the lamp's frame (lamp_pool.gdshaderinc, the Roadcraft 4.0's stops), added as the reference lays its pools: the radiance the
// Environment's ACES shows as the street light's colour times its opacity (tone_inverse.gdshaderinc), so overlapping pools add up. Only
// at night and only past the lamps with a light of their own (`lamp_lit`, render/props/lamp_lights.h): a pool wholly nearer than where
// they start giving way, and every pool by day, collapses to a point and costs no pixel.
shader_type spatial;
render_mode blend_add, unshaded, cull_front, depth_test_disabled, depth_draw_never, shadows_disabled, fog_disabled;

global uniform float night;
global uniform vec2 lamp_lit; // where the lamps with a light of their own start giving way to the pools, and where they end

uniform sampler2D depth_texture : hint_depth_texture, filter_nearest;
uniform vec3 display_colour = vec3(1.0); // as the screen shows it: read through source_display (color_space.gdshaderinc)
uniform float opacity = 0.0;

const float SLAB_LIT = 0.25; // of the slab's half-height from the street's top, lit whole; nothing at its edge

varying flat vec3 v_centre;
varying flat vec3 v_across;
varying flat vec3 v_up;
varying flat vec3 v_along;

#include "res://render/shaders/color_space.gdshaderinc"
#include "res://render/shaders/tone_inverse.gdshaderinc"
#include "res://render/shaders/lamp_pool.gdshaderinc"

void vertex() {
	v_centre = MODEL_MATRIX[3].xyz;
	v_across = MODEL_MATRIX[0].xyz;
	v_up = MODEL_MATRIX[1].xyz;
	v_along = MODEL_MATRIX[2].xyz;
	float reach = 0.5 * max(length(v_across), length(v_along)); // the farthest of the pool from its middle on the ground
	if (night <= 0.0 || length(v_centre.xz - CAMERA_POSITION_WORLD.xz) + reach < lamp_lit.x) {
		VERTEX = vec3(0.0);
	}
}

// Where `p` (from the pool's middle) lies along `axis`, in shares of its half: the box spans half the axis either way.
float share_of(vec3 p, vec3 axis) {
	return 2.0 * dot(p, axis) / dot(axis, axis);
}

void fragment() {
	float depth = texture(depth_texture, SCREEN_UV).x;
#if CURRENT_RENDERER == RENDERER_COMPATIBILITY
	vec3 ndc = vec3(SCREEN_UV, depth) * 2.0 - 1.0;
#else
	vec3 ndc = vec3(SCREEN_UV * 2.0 - 1.0, depth);
#endif
	vec4 seen = INV_VIEW_MATRIX * INV_PROJECTION_MATRIX * vec4(ndc, 1.0);
	vec3 world = seen.xyz / seen.w;
	vec3 p = world - v_centre;
	float r = length(vec2(share_of(p, v_across), share_of(p, v_along)));
	float h = abs(share_of(p, v_up));
	if (r >= 1.0 || h >= 1.0) {
		discard;
	}
	float fall = pool_fall(r) * (1.0 - smoothstep(SLAB_LIT, 1.0, h)) * pool_handed_over(length(world.xz - CAMERA_POSITION_WORLD.xz), lamp_lit);
	ALBEDO = linear_out(display_to_radiance(source_display(display_colour) * (fall * opacity * night)));
}
