// Vehicles (native/src/render/agents). UV2.x tags each part (AgentTag in agent_meshes.h): 0 fixed colour, 1 paint,
// 2 headlamp, 3 tail lamp (brake), 4 glass. UV2.y = 1 marks wheel vertices: they turn about X round their hub (UV =
// hub y, z). INSTANCE_CUSTOM = (paint r, g, b, wheel roll angle + 8 when braking); the Compatibility renderer
// stores instance custom data as half floats.
shader_type spatial;

global uniform float night;
uniform vec3 head_glow : source_color;
uniform vec3 tail_glow : source_color;
uniform float head_power;
uniform float tail_power;
uniform float brake_power;

const float BRAKE_FLAG = 8.0;

varying flat float v_tag;
varying flat vec4 v_custom;

void vertex() {
	v_tag = UV2.x;
	v_custom = INSTANCE_CUSTOM;
	if (UV2.y > 0.5) {
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
	}
	ALBEDO = c;
	ROUGHNESS = rough;
	METALLIC = metal;
	EMISSION = emit;
}
