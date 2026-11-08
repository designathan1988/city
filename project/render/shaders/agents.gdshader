// Vehicles (native/src/render/agents). UV2.x tags each part (AgentTag in agent_meshes.h): 0 fixed colour, 1 paint,
// 2 headlamp, 3 tail lamp (brake), 4 glass (opaque: the far level; the near one's is agent_glass.gdshader),
// 5 a lit sign (the bus's destination, day and night), 6 a roof (the paint, or black on a share of the cars), 7 the gloss black round
// the glass. The lenses of a vehicle standing with nobody aboard are off, their vertex colours; on anything else they burn, day and
// night. UV2.y = 1 marks wheel vertices: they turn about X round their hub (UV = hub y, z). INSTANCE_CUSTOM = (paint index, wheel roll
// angle + 8 when braking, passengers, 0); COLOR.a is the vehicle's seed. The Compatibility renderer stores instance custom data as half
// floats: whole numbers below 2048 exactly. Who rides is drawn by PeopleRenderer.
shader_type spatial;
#include "res://render/shaders/contracts/agents.gdshaderinc"

#include "res://render/shaders/color_space.gdshaderinc"
#include "res://render/shaders/emission_light.gdshaderinc"

global uniform float night;
uniform vec3 head_glow : source_color;
uniform vec3 tail_glow : source_color;
uniform float head_power;
uniform float tail_power;
uniform float brake_ratio; // a stop lamp's light against its rear position lamp's
uniform vec3 head_lit; // a lens burning, by day: the radiance the tone mapping shows as its colour (render/tone_map.gd)
uniform vec3 tail_lit;
uniform vec3 brake_lit;
uniform vec3 black_roof : source_color;
uniform float black_roof_share;
uniform vec3 sign_glow : source_color;
uniform float sign_power;
// roughness by surface (materials.json agent: sources there); every surface is a dielectric
uniform float paint_roughness;
uniform vec3 sheen_sky : source_color; // what a clear coat mirrors above the horizon and below it
uniform vec3 sheen_ground : source_color;
uniform float sheen_power;
uniform float lens_roughness;
uniform float trim_roughness;

#include "res://render/shaders/agent_door.gdshaderinc"
// the paints as numbers (vec3, not colours): used as they are, like the parts' own vertex colours; the riders' helmets too
uniform vec3 paints[AGENT_PAINTS + AGENT_LINE_PAINTS]; // the paints, then the colours of the transport lines
varying flat float v_tag;
varying flat vec4 v_custom;
varying flat float v_seed;
// A number in [0, 1) from the vehicle's seed, a seat and a purpose.
float pick(float seed, float seat, float what) {
	return fract(sin(dot(vec3(seed * 251.0, seat, what), vec3(12.9898, 78.233, 37.719))) * 43758.5453);
}

void vertex() {
	v_tag = UV2.x;
	v_custom = INSTANCE_CUSTOM;
	v_seed = COLOR.a;
	if (UV2.y > DOOR_MARK) {
		door_swing(VERTEX, NORMAL, UV, INSTANCE_CUSTOM.g, UV2.y);
	} else if (UV2.y > LIMB_WHEEL - 0.5) {
		float a = mod(INSTANCE_CUSTOM.g, AGENT_BRAKE_FLAG);
		vec2 d = VERTEX.yz - UV;
		vec2 nd = NORMAL.yz;
		float c = cos(a), s = sin(a);
		VERTEX.yz = UV + vec2(d.x * c - d.y * s, d.x * s + d.y * c);
		NORMAL.yz = vec2(nd.x * c - nd.y * s, nd.x * s + nd.y * c);
	}
}

// The clear coat of a painted panel: the world mirrored by Schlick's Fresnel, the sky over a soft horizon and the street under it. That line
// running along the doors and the sky growing on the roof at a glancing view is what reads as car paint.
vec3 sheen(vec3 n, vec3 v) {
	vec3 r = reflect(-v, n);
	float f = 0.04 + 0.96 * pow(1.0 - clamp(dot(n, v), 0.0, 1.0), 5.0);
	vec3 world = mix(source_linear(sheen_ground), source_linear(sheen_sky) * (0.7 + 0.5 * clamp(r.y, 0.0, 1.0)), smoothstep(-0.04, 0.06, r.y));
	return world * f * sheen_power * (1.0 - night);
}

void fragment() {
	int tag = int(v_tag + 0.5);
	vec3 c = srgb_to_linear(COLOR.rgb); // COLOR is sRGB on both renderers; everything below in linear
	float rough = trim_roughness;
	vec3 emit = vec3(0.0);
	vec3 coat = sheen(mat3(INV_VIEW_MATRIX) * NORMAL, mat3(INV_VIEW_MATRIX) * VIEW);
	if (tag == TAG_PAINT) {
		c = srgb_to_linear(paints[int(v_custom.r + 0.5)]);
		rough = paint_roughness;
		emit = coat;
	} else if (tag == TAG_ROOF) {
		c = pick(v_seed, 0.0, 9.0) < black_roof_share ? source_linear(black_roof) : srgb_to_linear(paints[int(v_custom.r + 0.5)]);
		rough = paint_roughness;
		emit = coat;
	} else if (tag == TAG_GLOSS) {
		rough = paint_roughness;
		emit = coat;
	} else if (tag == TAG_HEAD) {
		bool on = v_custom.b > -0.5;
		emit = on ? head_lit + source_linear(head_glow) * night * head_power : vec3(0.0);
		c = on ? vec3(0.0) : c;
		rough = lens_roughness;
	} else if (tag == TAG_TAIL) {
		bool on = v_custom.b > -0.5;
		bool braking = mod(v_custom.g, AGENT_DOOR_STEP) >= AGENT_BRAKE_FLAG; // by day each shows as its colour: more red than that, and the tone mapping turns it cream
		emit = on ? (braking ? brake_lit : tail_lit) + source_linear(tail_glow) * tail_power * night * (braking ? brake_ratio : 1.0) : vec3(0.0);
		c = on ? vec3(0.0) : c;
		rough = lens_roughness;
	} else if (tag == TAG_GLASS) {
		rough = lens_roughness;
	} else if (tag == TAG_SIGN) {
		emit = source_linear(sign_glow) * sign_power;
		rough = lens_roughness;
	}
	ALBEDO = linear_out(c);
	ROUGHNESS = rough;
	METALLIC = 0.0;
	EMISSION = linear_out(emission_shown(emit)); // the lamps authored as the screen shows them (render/lamp_radiance.gd, agents.json trim)
}
