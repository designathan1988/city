// A material of a signature building (tools/assets/cook_signatures.py): a colour, a picture over it, how it reflects. Both signature shaders read it.
uniform vec4 albedo : source_color = vec4(1.0);
uniform sampler2D picture : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform bool pictured = false;
uniform float roughness = 0.8;
uniform float metallic = 0.0;
uniform vec3 emission : source_color = vec3(0.0);
uniform float alpha_cut = 0.0;
uniform bool two_sided = false;

vec4 look_colour(vec2 uv) {
	return pictured ? albedo * texture(picture, uv) : albedo;
}
