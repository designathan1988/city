// A mound of rubble (native/src/render/effects/mounds.cpp): concrete dust over lumps, the wall's own colour in the broken pieces, dark where they
// leave gaps. It rises as the building falls into it. INSTANCE_CUSTOM: born (effects clock), seed, the wall's colour packed.
shader_type spatial;
render_mode cull_back;

uniform float now = 0.0; // the effects clock, seconds (mounds.cpp)
// A mound's look: data/render/materials.json rubble.params and the blast_dust set, by eye as the 4.0's rubble; parts said here.
uniform float fall_s; // how long the building takes to come down
uniform vec3 dust : source_color;
uniform vec3 concrete : source_color;
uniform float grain_gap; // how dark the gaps between the pieces are
uniform vec4 mound_shape; // its height by slumps, from and over their noise; by chunks, from and over theirs
uniform vec2 mound_noise; // the slumps' and the chunks' noise a metre
uniform vec3 rubble_noise; // the big, mid and fine noise a metre
uniform vec2 rubble_mix; // dust over concrete: the big noise's share and the mid's
uniform float rubble_wall; // the wall's own colour in the broken pieces, over the mid and fine noise
uniform float roughness;
const float SEED_SPREAD = 91.0, SEED_SPREAD_FINE = 53.0; // a mound's seed moves its noises this far (any scale off the lattice)

varying vec3 v_world;
varying vec3 v_wall;
varying flat float v_seed;

#include "res://render/shaders/lattice_noise.gdshaderinc"
#include "res://render/shaders/color_space.gdshaderinc"
#include "res://render/shaders/bytes.gdshaderinc"

void vertex() {
	float age = now - INSTANCE_CUSTOM.x;
	vec3 world = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
	// No two alike, whatever the mesh: its height goes by the place, in slumps as wide as a room and chunks a stride wide.
	vec2 at = world.xz + INSTANCE_CUSTOM.y * SEED_SPREAD;
	float slump = mound_shape.x + mound_shape.y * vnoise(at * mound_noise.x), chunk = mound_shape.z + mound_shape.w * vnoise(at * mound_noise.y);
	VERTEX.y *= smoothstep(0.0, fall_s, age) * slump * chunk;
	v_world = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
	uint packed = uint(INSTANCE_CUSTOM.z);
	v_wall = vec3(float(packed & BYTE_MASK_U), float((packed >> BYTE_SHIFT) & BYTE_MASK_U), float((packed >> (2u * BYTE_SHIFT)) & BYTE_MASK_U)) / BYTE_TOP;
	v_seed = INSTANCE_CUSTOM.y;
}

void fragment() {
	vec2 p = v_world.xz + v_seed * SEED_SPREAD_FINE;
	float big = vnoise(p * rubble_noise.x), mid = vnoise(p * rubble_noise.y), fine = vnoise(p * rubble_noise.z);
	vec3 c = mix(source_linear(concrete), source_linear(dust), big * rubble_mix.x + mid * rubble_mix.y);
	c = mix(c, source_linear(v_wall), rubble_wall * mid * fine);
	c *= mix(grain_gap, 1.0, fine);
	// Broken faces, one a triangle: the surface as it was laid, its slope read from the pixels.
	vec3 facet = normalize(cross(dFdx(v_world), dFdy(v_world)));
	facet = facet.y < 0.0 ? -facet : facet;
	NORMAL = normalize((VIEW_MATRIX * vec4(facet, 0.0)).xyz);
	ALBEDO = linear_out(c);
	ROUGHNESS = roughness;
}
