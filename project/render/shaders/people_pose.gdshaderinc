// Where a cooked person's bone stands this frame, for whatever is drawn on that person: the body (people.gdshader) and what the hand holds
// (people_held.gdshader). INSTANCE_CUSTOM = (body, what they do standing, a seed, walk phase 0..1); the instance colour's alpha = the share of the walk
// in the pose, the rest standing: a walker slows into it and out of it (Pedestrian::rest); from CLIP_CODE_BASE on, the clip of that number alone:
// somebody in a vehicle. What they do standing is a clip's number and, as its fraction over ACT_SPAN, how far into it they are (Pedestrian::act_share):
// the rest of the standing is the idle. Below zero the clip is played from its start, at the phase in the fourth channel, and not by the clock:
// somebody crouching, who stands still until it is over, so no walk reads that channel.
uniform sampler2D palette : filter_nearest, repeat_disable;
uniform int block_rows; // a variation's rows of the palette; its walk first
uniform int idle_clip; // the clip of somebody standing with nothing to do
uniform float idle_clock; // the simulation's seconds, for whoever stands
uniform int clip_row[24]; // people_renderer.cpp CLIP_TABLE: each clip's first row in a variation's block
uniform int clip_frames[24];
uniform float clip_seconds[48]; // a man's and a woman's, by clip
uniform int walk_frames[32]; // by variation (people_renderer.cpp BODY_TABLE)
uniform int body_female[32];

const int TEXELS = 3;
const float BYTE = 255.0;
const float BODY_SPAN = 3.0; // cook_people.py PALETTE_SPAN
const float IDLE_SPREAD = 0.61803; // each person in another place of the clip (the golden ratio spreads whole numbers evenly)
const float CLIP_CODE_BASE = 2.0; // people_renderer.cpp: an alpha from here on is a clip, held at the phase in INSTANCE_CUSTOM.a
const float ACT_SPAN = 0.96; // people_renderer.cpp: the whole of an act, short of the next clip's number

// One palette row value: 16-bit fixed point over [-BODY_SPAN, BODY_SPAN], high bytes in texel 2x, low bytes in 2x + 1.
vec4 palette_row(int texel, int row) {
	vec4 hi = texelFetch(palette, ivec2(texel * 2, row), 0);
	vec4 lo = texelFetch(palette, ivec2(texel * 2 + 1, row), 0);
	return (hi * 256.0 + lo) * BYTE / 65535.0 * 2.0 * BODY_SPAN - BODY_SPAN;
}

BonePose walk_pose(int bone, int row) {
	return BonePose(palette_row(bone * TEXELS, row), palette_row(bone * TEXELS + 1, row), palette_row(bone * TEXELS + 2, row));
}

// Adds the pose of a clip (its first row, frames and phase 0..1) at `share`: the linear blend of the four bones.
void add_clip(int first, int frames, float phase, float share, ivec4 bones, vec4 weights, vec3 vertex, vec3 normal, inout vec3 pos, inout vec3 nor) {
	if (share == 0.0) return;
	float f = fract(phase) * float(frames);
	int f0 = int(floor(f));
	int row0 = first + f0;
	int row1 = first + (f0 + 1 < frames ? f0 + 1 : 0);
	float t = fract(f);
	for (int i = 0; i < 4; i++) {
		if (weights[i] == 0.0) continue;
		skin_add(pos, nor, share * weights[i], walk_pose(bones[i], row0), walk_pose(bones[i], row1), t, vertex, normal);
	}
}

// How far into what they do somebody standing is.
float act_share(vec4 custom) {
	return min(fract(abs(custom.g)) / ACT_SPAN, 1.0);
}

// A clip played by the clock, each person at their own place of it.
void add_standing(int clip, int block, int female, float seed, float share, ivec4 bones, vec4 weights, vec3 vertex, vec3 normal, inout vec3 pos, inout vec3 nor) {
	float phase = idle_clock / clip_seconds[clip * 2 + female] + seed * IDLE_SPREAD;
	add_clip(block + clip_row[clip], clip_frames[clip], phase, share, bones, weights, vertex, normal, pos, nor);
}

void person_pose(vec4 custom, float code, ivec4 bones, vec4 weights, vec3 vertex, vec3 normal, out vec3 pos, out vec3 nor) {
	float seed = floor(abs(custom.b) + 0.5); // below zero: frightened, people_face.gdshaderinc
	int variation = int(custom.r + 0.5);
	int block = variation * block_rows;
	pos = vec3(0.0);
	nor = vec3(0.0);
	if (code >= CLIP_CODE_BASE - 0.5) {
		int clip = int(code - CLIP_CODE_BASE + 0.5);
		add_clip(block + clip_row[clip], clip_frames[clip], custom.a, 1.0, bones, weights, vertex, normal, pos, nor);
		return;
	}
	if (code > 0.0) {
		add_clip(block, walk_frames[variation], custom.a, code, bones, weights, vertex, normal, pos, nor);
	}
	if (code < 1.0) {
		int female = body_female[variation];
		float doing = act_share(custom);
		if (doing > 0.0) {
			int clip = int(abs(custom.g));
			if (custom.g < 0.0) {
				add_clip(block + clip_row[clip], clip_frames[clip], custom.a, (1.0 - code) * doing, bones, weights, vertex, normal, pos, nor);
			} else {
				add_standing(clip, block, female, seed, (1.0 - code) * doing, bones, weights, vertex, normal, pos, nor);
			}
		}
		if (doing < 1.0) {
			add_standing(idle_clip, block, female, seed, (1.0 - code) * (1.0 - doing), bones, weights, vertex, normal, pos, nor);
		}
	}
}
