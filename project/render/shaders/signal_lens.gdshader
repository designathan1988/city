// The signal lenses (native/src/render/props/signal_lamps.h): one mesh holds every lens of a post, drawn once per post.
// COLOR.rgb is the lens unlit (sRGB), UV2.x which lens it is (prop_kit.h Lens); INSTANCE_CUSTOM.r and .g are the car lens
// and the walkers' lens the post shows lit (-1: none). An unlit lens is a dark coloured filter lit by the scene (glossy,
// as the Roadcraft 4.0 lens). A lit lens is a light: no albedo nor reflection, only lit_radiance, the scene radiance the
// Environment's tone mapping shows as the lens colour (render/tone_map.gd, set by city_world.gd).
shader_type spatial;

uniform vec3 lit_radiance[5]; // per Lens (prop_kit.h Lens::COUNT)
uniform float roughness = 0.35;

varying flat float v_lit;
varying flat vec3 v_radiance;

vec3 to_linear(vec3 c) {
	return mix(c / 12.92, pow((c + 0.055) / 1.055, vec3(2.4)), step(0.04045, c));
}

void vertex() {
	float lens = UV2.x;
	v_lit = abs(lens - INSTANCE_CUSTOM.r) < 0.5 || abs(lens - INSTANCE_CUSTOM.g) < 0.5 ? 1.0 : 0.0;
	v_radiance = lit_radiance[clamp(int(lens + 0.5), 0, 4)];
}

void fragment() {
	if (v_lit > 0.5) {
		ALBEDO = vec3(0.0);
		SPECULAR = 0.0;
		EMISSION = v_radiance;
	} else {
		// COLOR as linear albedo: Compatibility (OUTPUT_IS_SRGB) hands it over already decoded (see road.gdshader).
		ALBEDO = OUTPUT_IS_SRGB ? COLOR.rgb : to_linear(COLOR.rgb);
		ROUGHNESS = roughness;
	}
}
