// A mound of rubble (native/src/render/effects/mounds.cpp): concrete dust over lumps, the wall's own colour in the broken pieces, dark where they
// leave gaps. It rises as the building falls into it. INSTANCE_CUSTOM: born (effects clock), seed, the wall's colour packed.
shader_type spatial;
render_mode cull_back;

uniform float now = 0.0;
uniform float fall_s = 2.2; // how long the building takes to come down
uniform vec3 dust : source_color = vec3(0.56, 0.54, 0.5);
uniform vec3 concrete : source_color = vec3(0.47, 0.455, 0.43);
uniform float grain_gap = 0.72; // how dark the gaps between the pieces are

varying vec3 v_world;
varying vec3 v_wall;
varying flat float v_seed;

#include "res://render/shaders/lattice_noise.gdshaderinc"
#include "res://render/shaders/color_space.gdshaderinc"

void vertex() {
	float age = now - INSTANCE_CUSTOM.x;
	vec3 world = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
	// No two alike, whatever the mesh: its height goes by the place, in slumps as wide as a room and chunks a stride wide.
	vec2 at = world.xz + INSTANCE_CUSTOM.y * 91.0;
	float slump = 0.35 + 1.1 * vnoise(at * 0.09), chunk = 0.72 + 0.56 * vnoise(at * 0.62);
	VERTEX.y *= smoothstep(0.0, fall_s, age) * slump * chunk;
	v_world = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
	uint packed = uint(INSTANCE_CUSTOM.z);
	v_wall = vec3(float(packed & 255u), float((packed >> 8u) & 255u), float((packed >> 16u) & 255u)) / 255.0;
	v_seed = INSTANCE_CUSTOM.y;
}

void fragment() {
	vec2 p = v_world.xz + v_seed * 53.0;
	float big = vnoise(p * 0.21), mid = vnoise(p * 0.9), fine = vnoise(p * 3.7);
	vec3 c = mix(source_linear(concrete), source_linear(dust), big * 0.6 + mid * 0.4);
	c = mix(c, source_linear(v_wall), 0.32 * mid * fine);
	c *= mix(grain_gap, 1.0, fine);
	// Broken faces, one a triangle: the surface as it was laid, its slope read from the pixels.
	vec3 facet = normalize(cross(dFdx(v_world), dFdy(v_world)));
	facet = facet.y < 0.0 ? -facet : facet;
	NORMAL = normalize((VIEW_MATRIX * vec4(facet, 0.0)).xyz);
	ALBEDO = linear_out(c);
	ROUGHNESS = 0.96;
}
