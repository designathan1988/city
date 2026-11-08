// A person's face this instant (faceExpression.ts faceAt), worked out on the GPU from what the instance already says: its seed, the clock, what it is
// doing and whether it is frightened. Only the near mesh's face vertices carry a row of face_table (tools/assets/cook_people.py): where each moves, by
// channel; no other vertex pays for any of it.
#include "res://render/shaders/shader_contracts.gdshaderinc"
uniform sampler2D face_table : filter_nearest, repeat_disable;
uniform int act_face[PEOPLE_CLIPS]; // by clip: what the face does at it (FACE_ACT_*)
uniform float body_mood[PEOPLE_BODIES]; // by variation: -1 low .. 1 bright (cast.json)

const float FACE_ROW_STEP = 4.0; // cook_people.py: UV2.x is the region plus this many a row
const int FACE_CHANNELS = 13;
const int FACE_SPAN = 8; // rows side by side in the table
const int BLINK = 0, JOY = 1, SADNESS = 2, ANGER = 3, SURPRISE = 4, BROW_RAISE = 5, OPEN = 6, ROUND = 7, PUCKERED = 8, SPREAD = 9, SHUT = 10, LOOK_LEFT = 11, LOOK_RIGHT = 12;
const int VISEME_COUNT = 9;
const int VISEMES[VISEME_COUNT] = { OPEN, SPREAD, SPREAD, ROUND, PUCKERED, SHUT, SHUT, OPEN, SPREAD };
const uint FACE_SALT = 0x4c9e3721u;
const uint BYTE_MASK = 255u;
const float BYTE_MAX = 255.0;
const float SYLLABLES = 4.2; // a second, ordinary speech
const float BLINK_FROM = 0.965, BLINK_RATE = 0.22, BLINK_RATE_STEP = 0.008; // one every 3-5 s, 150 ms closing and opening
const float GAZE_RATE = 0.55, GAZE_REACH = 0.32;
const float STRESSED_BROW = 0.4;
const int STRESS_EVERY = 5;
const uint STRESS_SALT = 7u;
const float FACE_PI = 3.14159265;

uint person_hash(uint id) {
	uint h = id + 0x9e3779b9u;
	h = (h ^ (h >> 16u)) * 0x21f0aaadu;
	h = (h ^ (h >> 15u)) * 0x735a2d97u;
	return h ^ (h >> 15u);
}

int face_row(float uv2_x) {
	return int(uv2_x / FACE_ROW_STEP);
}

// What the vertex of `row` moves by, at rest. `code`: the instance colour's alpha (people_pose.gdshaderinc).
vec3 face_move(int row, vec4 custom, float code) {
	float w[FACE_CHANNELS];
	for (int c = 0; c < FACE_CHANNELS; c++) {
		w[c] = 0.0;
	}
	float time = idle_clock;
	uint hash = person_hash(uint(floor(abs(custom.b) + 0.5)) ^ FACE_SALT);
	float low = float(hash & BYTE_MASK);
	int doing = code >= PEOPLE_CLIP_CODE_BASE - 0.5 ? 0 : act_face[int(abs(custom.g))];
	bool frightened = custom.b < 0.0;

	float blink = fract(time * (BLINK_RATE + float((hash >> 8u) & 15u) * BLINK_RATE_STEP) + low / BYTE_MAX);
	w[BLINK] = blink > BLINK_FROM ? sin((blink - BLINK_FROM) / (1.0 - BLINK_FROM) * FACE_PI) : 0.0;
	float gaze = sin(time * GAZE_RATE + float((hash >> 5u) & 1023u)) * GAZE_REACH;
	w[LOOK_LEFT] = max(gaze, 0.0);
	w[LOOK_RIGHT] = max(-gaze, 0.0);

	if (frightened && doing == FACE_ACT_CROUCH) {
		// Sobbing (FACS sadness AU1+4+15 with the jaw catching)
		float sob = max(0.0, sin(time * 5.5 + float(hash & 63u)));
		w[SADNESS] = 1.0;
		w[BROW_RAISE] = 0.35;
		w[OPEN] = 0.15 + 0.45 * sob * sob;
		w[BLINK] = 0.55 + 0.3 * sob;
	} else if (frightened) {
		// Terror (FACS fear, AU1+2+4+5+20+26): brows up and drawn together, eyes wide, lips stretched back, the jaw dropped
		float scream = fract(time * 0.7 + low / BYTE_MAX);
		w[BLINK] = 0.0;
		w[SURPRISE] = 1.0;
		w[SADNESS] = 0.45;
		w[OPEN] = scream < 0.75 ? 0.75 + 0.25 * sin(time * 23.0 + low) : 0.25;
		w[SPREAD] = 0.7;
	} else {
		float mood = body_mood[int(custom.r + 0.5)];
		w[JOY] = max(mood, 0.0) * 0.3 + (doing == FACE_ACT_TALK ? 0.25 : doing == FACE_ACT_LISTEN ? 0.15 : 0.0);
		w[SADNESS] = max(-mood, 0.0) * 0.35;
		if (doing == FACE_ACT_TALK || doing == FACE_ACT_PHONE) {
			float beat = time * SYLLABLES + float((hash >> 16u) & BYTE_MASK) / 64.0;
			float syllable = floor(beat);
			float open = sin((beat - syllable) * FACE_PI);
			uint said = uint(syllable);
			w[VISEMES[int(person_hash(hash ^ said) % uint(VISEME_COUNT))]] += open;
			if (person_hash(hash ^ (said * STRESS_SALT)) % uint(STRESS_EVERY) == 0u) {
				w[BROW_RAISE] += STRESSED_BROW * open;
			}
		}
	}

	ivec2 at = ivec2((row % FACE_SPAN) * FACE_CHANNELS, row / FACE_SPAN);
	vec3 move = vec3(0.0);
	for (int c = 0; c < FACE_CHANNELS; c++) {
		if (w[c] > 0.0) {
			move += min(w[c], 1.0) * texelFetch(face_table, at + ivec2(c, 0), 0).rgb;
		}
	}
	return move;
}
