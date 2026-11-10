// The congestion map: each carriageway coloured by its segment's level, read from a texture of levels (a texel a segment id,
// native/src/render/overlays/overlay_renderer.h), so a refresh of the traffic's speeds sends that texture and makes no mesh.
// Flat, unshaded, see-through, as the zone colours are (overlay.gdshader).
shader_type spatial;
render_mode unshaded, blend_mix, cull_disabled, depth_draw_never, shadows_disabled;

#include "res://render/shaders/color_space.gdshaderinc"

const int STOPS = 3; // the ramp's colours: jammed, middling, free

uniform sampler2D levels : filter_nearest, repeat_disable; // R8: 0 jammed .. 1 free
uniform int levels_wide = 1; // texels a row: segment `id` is at (id % wide, id / wide)
uniform vec3 ramp[STOPS]; // screen colours (the tokens), jammed first
uniform float opacity = 1.0;

varying flat int segment; // the vertex's segment id, carried in UV.x

void vertex() {
	segment = int(round(UV.x));
}

void fragment() {
	float at = texelFetch(levels, ivec2(segment % levels_wide, segment / levels_wide), 0).r * float(STOPS - 1);
	int stop = min(int(at), STOPS - 2);
	ALBEDO = linear_out(srgb_to_linear(mix(ramp[stop], ramp[stop + 1], at - float(stop))));
	ALPHA = opacity;
}
