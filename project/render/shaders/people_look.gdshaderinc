// The cast (native/src/render/agents/people_renderer.cpp): the cooked bodies skinned with the clip palette (3 RGBA texels per bone and
// frame = rows of the 3x4 bind -> pose matrix, as 16-bit fixed point in RGBA8 texel pairs), coloured by their atlas. The four-bone blend is
// people_skin.gdshaderinc, the pose people_pose.gdshaderinc. UV2 = (region: 0 cloth, 1 skin, 2 hair, 3 eye; roughness). The light is a person's own
// (personMaterial.ts): skin wrapped past the terminator with the red of what scatters under it, hair lit along its strands (Kajiya-Kay, Marschner's two
// shifted lobes), and neither mirroring much of the sky.
render_mode cull_disabled; // the hair's cards and the clothes' open edges are seen from both sides
#include "res://render/shaders/contracts/people.gdshaderinc"

#include "res://render/shaders/people_skin.gdshaderinc"
#include "res://render/shaders/people_pose.gdshaderinc"
#include "res://render/shaders/people_face.gdshaderinc"
#include "res://render/shaders/fresnel.gdshaderinc"
#include "res://render/shaders/hash.gdshaderinc"

uniform sampler2DArray atlas : source_color, filter_linear_mipmap_anisotropic, repeat_disable;
uniform float alpha_test; // the texels of the hair's cards that are drawn


const float REFLECTS = 0.5; // what a dielectric mirrors (Godot's SPECULAR for an F0 of 4 %)
const float DULL_REFLECTS = 0.125; // skin and hair: a quarter of that
const float STRAND_ROOM = 1e-12; // a strand's or a normal's squared length under this is none
const float LEAST_GGX_ALPHA = 0.002, LEAST_NV = 1e-3, LEAST_VIS = 1e-5; // guards: the lobe's width, the view's cosine, the visibility's sum
const vec2 SPARKLE_CELLS = vec2(640.0, 48.0); // a strand's sparkle: the hair's cells across and down its atlas region, each its own by the sin hash
// A person's light (materials.json people.params), by eye as the 4.0's personMaterial.ts; each vector's parts are said here.
uniform vec3 inside_tone; // skin seen from inside (the cheeks through an open mouth) is the body's dark
uniform float skin_specular; // what skin keeps of the sheen
uniform float skin_wrap; // how far past the terminator the light wraps
uniform vec3 skin_scatter; // the red that scatters under the skin, in a band of the wrapped light
uniform vec2 skin_scatter_band; // the band rises to the first and falls by the second
uniform float hair_shade; // the side away from the light still glows through the strands
uniform vec4 hair_lobes; // the two lobes' cuticle tilts, their spread by the sparkle, the first's grey sheen
uniform vec4 hair_shine; // the first lobe's exponent; the second's share of the hair's colour, its sparkle threshold, its exponent

#ifdef BODY
uniform sampler2D bodies : filter_nearest, repeat_disable; // a row a body, three texels a bone: bind -> world (body_renderer.cpp)
varying flat float v_charred;
uniform vec3 charred_tone; // a burnt body's
#endif
varying flat float v_layer;
varying flat float v_region;
varying float v_rough;
varying vec3 v_strand;
varying float v_sparkle;

bool in_region(float region) {
	return abs(v_region - region) < 0.5;
}

// One lobe of the strands' highlight: `t` along the hair, shifted by the cuticle's tilt.
float strand_lobe(vec3 t, vec3 h, float exponent) {
	float th = dot(t, h);
	return smoothstep(-1.0, 0.0, th) * pow(sqrt(max(0.0, 1.0 - th * th)), exponent);
}

// GGX with Smith's correlated visibility, as the engine's own for what has no light of its own.
float ggx_specular(vec3 n, vec3 v, vec3 l, vec3 h, float rough) {
	float a = max(rough * rough, LEAST_GGX_ALPHA), a2 = a * a;
	float nh = clamp(dot(n, h), 0.0, 1.0), nl = clamp(dot(n, l), 0.0, 1.0), nv = clamp(dot(n, v), LEAST_NV, 1.0);
	float d = nh * nh * (a2 - 1.0) + 1.0;
	float ggx = a2 / (PI * d * d);
	float vis = 0.5 / (nl * sqrt(nv * nv * (1.0 - a2) + a2) + nv * sqrt(nl * nl * (1.0 - a2) + a2) + LEAST_VIS);
	float fresnel = schlick(clamp(dot(l, h), 0.0, 1.0));
	return ggx * vis * fresnel * nl;
}

void vertex() {
	vec3 pos = vec3(0.0), nor = vec3(0.0);
	int row = face_row(UV2.x);
#ifdef BODY
	ivec4 bones = skin_bones(BONE_INDICES);
	int slot = int(INSTANCE_CUSTOM.g + 0.5);
	for (int i = 0; i < 4; i++) {
		int at = bones[i] * 3;
		BonePose p = BonePose(texelFetch(bodies, ivec2(at, slot), 0), texelFetch(bodies, ivec2(at + 1, slot), 0), texelFetch(bodies, ivec2(at + 2, slot), 0));
		skin_add(pos, nor, BONE_WEIGHTS[i], p, p, 0.0, VERTEX, NORMAL);
	}
	v_charred = INSTANCE_CUSTOM.a;
#else
	vec3 rest = row > 0 ? VERTEX + face_move(row, INSTANCE_CUSTOM) : VERTEX;
	person_pose(INSTANCE_CUSTOM, skin_bones(BONE_INDICES), BONE_WEIGHTS, rest, NORMAL, pos, nor);
#endif
	VERTEX = pos;
	NORMAL = nor / max(length(nor), STRAND_ROOM); // a part gone closes on its joint: nothing left to face
	v_layer = INSTANCE_CUSTOM.r;
	v_region = UV2.x - float(row) * PEOPLE_FACE_ROW_STEP;
	v_rough = UV2.y;
}

void fragment() {
	vec4 texel = texture(atlas, vec3(UV, v_layer));
	vec3 c = texel.rgb * COLOR.rgb;
	if (!FRONT_FACING && v_region > 0.5 && v_region < 1.5) c *= inside_tone;
#ifdef BODY
	c = mix(c, charred_tone, v_charred);
#endif
	ALBEDO = c;
	ALPHA = texel.a;
	ALPHA_SCISSOR_THRESHOLD = alpha_test;
	ROUGHNESS = v_rough;
	SPECULAR = in_region(1.0) || in_region(2.0) ? DULL_REFLECTS : REFLECTS;
	vec3 along = dFdy(VERTEX) * dFdx(UV).x - dFdx(VERTEX) * dFdy(UV).x; // the strands run with the atlas's rows
	along -= NORMAL * dot(along, NORMAL);
	v_strand = dot(along, along) > STRAND_ROOM ? normalize(along) : vec3(0.0, 1.0, 0.0);
	v_sparkle = fract(sin(dot(floor(UV * SPARKLE_CELLS), SIN_HASH.xy)) * SIN_HASH_SCALE);
}

void light() {
	float nl = dot(NORMAL, LIGHT);
	float facing = clamp(nl, 0.0, 1.0);
	vec3 sun = LIGHT_COLOR * ATTENUATION;
	vec3 h = normalize(LIGHT + VIEW);
	vec3 lit = vec3(facing);
	vec3 shine = vec3(ggx_specular(NORMAL, VIEW, LIGHT, h, ROUGHNESS));
	if (in_region(1.0)) {
		float wrapped = clamp((nl + skin_wrap) / (1.0 + skin_wrap), 0.0, 1.0);
		lit = vec3(wrapped) + smoothstep(0.0, skin_scatter_band.x, wrapped) * smoothstep(skin_scatter_band.y, skin_scatter_band.x, wrapped) * skin_scatter;
		shine *= skin_specular;
	} else if (in_region(2.0)) {
		lit = vec3(mix(hair_shade, 1.0, facing));
		float shift = v_sparkle - 0.5;
		vec3 t1 = normalize(v_strand + (hair_lobes.x + hair_lobes.z * shift) * NORMAL), t2 = normalize(v_strand + (hair_lobes.y + hair_lobes.z * shift) * NORMAL);
		shine = ((vec3(hair_lobes.w) + ALBEDO * 0.25) * strand_lobe(t1, h, hair_shine.x) + ALBEDO * hair_shine.y * step(hair_shine.z, v_sparkle) * strand_lobe(t2, h, hair_shine.w)) * clamp(nl * 0.5 + 0.5, 0.0, 1.0);
	}
	DIFFUSE_LIGHT += lit * sun / PI;
	SPECULAR_LIGHT += shine * sun * SPECULAR_AMOUNT;
}
