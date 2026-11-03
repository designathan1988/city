// Vehicles (native/src/render/agents). UV2.x tags each part (AgentTag in agent_meshes.h): 0 fixed colour, 1 paint,
// 2 headlamp, 3 tail lamp (brake), 4 glass (opaque: buses and trucks; a car's is agent_glass.gdshader), 5 skin, 6 clothes,
// 7 hair of the occupants. UV2.y = 1 marks wheel vertices: they turn about X round their hub (UV = hub y, z); UV2.y =
// 2 + k the occupant of seat k, there or not by the vehicle's seed (the instance colour's alpha, COLOR.a here) and
// seat_fill, dressed from the people's palettes by it. INSTANCE_CUSTOM = (paint r, g, b, wheel roll angle + 8 when
// braking); the Compatibility renderer stores instance custom data as half floats.
shader_type spatial;

global uniform float night;
uniform vec3 head_glow : source_color;
uniform vec3 tail_glow : source_color;
uniform float head_power;
uniform float tail_power;
uniform float brake_power;

const float BRAKE_FLAG = 8.0;
const float SEAT_LIMB = 2.0;
// palette sizes: agent_renderer.cpp CLOTHES_MAX, HAIR_MAX, SKIN_MAX
uniform vec4 clothes[16] : source_color;
uniform vec4 hair[8] : source_color;
uniform vec4 skin[8] : source_color;
uniform int clothes_n = 1;
uniform int hair_n = 1;
uniform int skin_n = 1;
uniform vec4 seat_fill = vec4(1.0);

varying flat float v_tag;
varying flat vec4 v_custom;
varying flat float v_seed;
varying flat float v_seat;

// A number in [0, 1) from the vehicle's seed, a seat and a purpose.
float pick(float seed, float seat, float what) {
	return fract(sin(dot(vec3(seed * 251.0, seat, what), vec3(12.9898, 78.233, 37.719))) * 43758.5453);
}

void vertex() {
	v_tag = UV2.x;
	v_custom = INSTANCE_CUSTOM;
	v_seed = COLOR.a;
	v_seat = UV2.y - SEAT_LIMB;
	if (UV2.y > SEAT_LIMB - 0.5) {
		int k = int(v_seat + 0.5);
		if (pick(v_seed, v_seat, 0.0) >= seat_fill[k]) VERTEX = vec3(0.0); // an empty seat: its occupant folds away
	} else if (UV2.y > 0.5) {
		float a = INSTANCE_CUSTOM.a - (INSTANCE_CUSTOM.a >= BRAKE_FLAG ? BRAKE_FLAG : 0.0);
		vec2 d = VERTEX.yz - UV;
		vec2 nd = NORMAL.yz;
		float c = cos(a), s = sin(a);
		VERTEX.yz = UV + vec2(d.x * c - d.y * s, d.x * s + d.y * c);
		NORMAL.yz = vec2(nd.x * c - nd.y * s, nd.x * s + nd.y * c);
	}
}

void fragment() {
	int tag = int(v_tag + 0.5);
	vec3 c = COLOR.rgb;
	float rough = 0.62;
	float metal = 0.0;
	vec3 emit = vec3(0.0);
	if (tag == 1) {
		c = v_custom.rgb;
		rough = 0.28;
		metal = 0.3;
	} else if (tag == 2) {
		emit = head_glow * night * head_power;
		rough = 0.2;
	} else if (tag == 3) {
		emit = tail_glow * max(night * tail_power, (v_custom.a >= BRAKE_FLAG ? 1.0 : 0.0) * brake_power);
		rough = 0.3;
	} else if (tag == 4) {
		rough = 0.05;
		metal = 0.5;
	} else if (tag == 5) {
		c = skin[int(pick(v_seed, v_seat, 1.0) * float(skin_n))].rgb;
		rough = 0.7;
	} else if (tag == 6) {
		c = clothes[int(pick(v_seed, v_seat, 2.0) * float(clothes_n))].rgb;
		rough = 0.9;
	} else if (tag == 7) {
		c = hair[int(pick(v_seed, v_seat, 3.0) * float(hair_n))].rgb;
		rough = 0.8;
	}
	ALBEDO = c;
	ROUGHNESS = rough;
	METALLIC = metal;
	EMISSION = emit;
}
