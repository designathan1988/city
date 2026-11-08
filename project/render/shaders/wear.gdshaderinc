// The wear of streets and footways (native/src/render/wear/wear_field.h): a window of bytes that wraps round the view, in
// tiles of a texture array, red the wheels and green the feet. Bilinear by hand, four texel reads: a layer's filter stops at
// its edge, and a seam every 1.28 km would show. Needs vnoise() of lattice_noise.gdshaderinc.
#include "res://render/shaders/contracts/streets.gdshaderinc"
uniform sampler2DArray wear_field : filter_nearest;
uniform vec2 wear_origin = vec2(0.0); // the window's low corner, cells
uniform float wear_cell_m; // set by wear_field.cpp
// What wear does, by eye as the 4.0's streets (data/render/materials.json road.params); each vector's parts are said here.
uniform vec2 rubber_look; // tyre rubber: how much it darkens, the wear at which it is whole
uniform vec4 pothole_look; // potholes: from this wear, whole at the next; their noise's edges
uniform vec3 pothole_dark; // the hole's colour
uniform vec2 pothole_more; // their noise a metre, how far they take the surface
uniform vec4 crack_look; // cracks: from this wear, whole at the next; their noise's edge, from and to
uniform vec2 crack_more; // their noise a metre, how much they darken
uniform vec4 stain_look; // feet's stains: whole at this wear; their noise's floor, depth and scale a metre
uniform vec3 stain_tint;
uniform bool wear_shown = true; // tests/perf/camera_pan: a frame without it
const int WEAR_RES = WEAR_LAYER_CELLS * WEAR_LAYERS_AXIS, WEAR_TILE = WEAR_LAYER_CELLS, WEAR_TILES = WEAR_LAYERS_AXIS; // the field's side, a layer's, layers a side

vec2 wear_texel(ivec2 c) {
	ivec2 w = c & ivec2(WEAR_RES - 1);
	ivec2 tile = w / WEAR_TILE;
	return texelFetch(wear_field, ivec3(w - tile * WEAR_TILE, tile.y * WEAR_TILES + tile.x), 0).rg;
}

// (wheels, feet) under a world point; none off the window.
vec2 wear_at(vec2 xz) {
	if (!wear_shown) return vec2(0.0);
	vec2 g = xz / wear_cell_m - 0.5, local = g - wear_origin;
	if (local.x < 0.0 || local.y < 0.0 || local.x >= float(WEAR_RES - 1) || local.y >= float(WEAR_RES - 1)) return vec2(0.0);
	ivec2 i = ivec2(floor(g));
	vec2 f = fract(g);
	return mix(mix(wear_texel(i), wear_texel(i + ivec2(1, 0)), f.x), mix(wear_texel(i + ivec2(0, 1)), wear_texel(i + ivec2(1, 1)), f.x), f.y);
}

// Tyre rubber darkens the tracks; hard-used, the surface breaks up. Noise only past each term's threshold.
vec3 wear_asphalt(vec3 albedo, float wear, vec2 xz) {
	albedo *= 1.0 - rubber_look.x * smoothstep(0.0, rubber_look.y, wear);
	if (wear > pothole_look.x) {
		float holes = smoothstep(pothole_look.x, pothole_look.y, wear) * smoothstep(pothole_look.z, pothole_look.w, vnoise(xz * pothole_more.x));
		albedo = mix(albedo, pothole_dark, holes * pothole_more.y);
	}
	if (wear > crack_look.x) {
		float cracks = smoothstep(crack_look.x, crack_look.y, wear) * smoothstep(crack_look.z, crack_look.w, 0.5 - abs(vnoise(xz * crack_more.x) - 0.5));
		albedo *= 1.0 - crack_more.y * cracks;
	}
	return albedo;
}

// Feet scuff and stain the paving where people walk most.
vec3 wear_paving(vec3 albedo, float wear, vec2 xz) {
	if (wear <= 0.0) return albedo;
	float stain = smoothstep(0.0, stain_look.x, wear) * (stain_look.y + stain_look.z * vnoise(xz * stain_look.w));
	return mix(albedo, albedo * stain_tint, stain);
}
