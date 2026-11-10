// The wear of streets and footways (native/src/render/wear/wear_field.h): a window of bytes that wraps round the view, in
// tiles of a texture array, red the wheels and green the feet. Bilinear by hand, four texel reads: a layer's filter stops at
// its edge, and a seam every 1.28 km would show. Needs vnoise() of lattice_noise.gdshaderinc.
uniform sampler2DArray wear_field : filter_nearest;
uniform vec2 wear_origin = vec2(0.0); // the window's low corner, cells
uniform float wear_cell_m = 5.0;
uniform bool wear_shown = true; // tests/perf/camera_pan: a frame without it
const int WEAR_RES = 2048, WEAR_TILE = 256, WEAR_TILES = 8; // wear_field.h

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
	albedo *= 1.0 - 0.38 * smoothstep(0.0, 0.8, wear);
	if (wear > 0.55) {
		float holes = smoothstep(0.55, 1.0, wear) * smoothstep(0.632, 0.812, vnoise(xz * 0.45));
		albedo = mix(albedo, vec3(0.06, 0.06, 0.065), holes * 0.9);
	}
	if (wear > 0.3) {
		float cracks = smoothstep(0.3, 0.9, wear) * smoothstep(0.47, 0.5, 0.5 - abs(vnoise(xz * 1.7) - 0.5));
		albedo *= 1.0 - 0.3 * cracks;
	}
	return albedo;
}

// Feet scuff and stain the paving where people walk most.
vec3 wear_paving(vec3 albedo, float wear, vec2 xz) {
	if (wear <= 0.0) return albedo;
	float stain = smoothstep(0.0, 0.9, wear) * (0.6 + 0.4 * vnoise(xz * 0.8));
	return mix(albedo, albedo * vec3(0.62, 0.6, 0.55), stain);
}
