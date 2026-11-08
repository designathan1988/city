// A person's face this instant (faceExpression.ts faceAt), worked out on the GPU from what the instance already says: its seed, the clock, what it is
// doing and whether it is frightened. Only the near mesh's face vertices carry a row of face_table (cooked with the people, read by people_asset.cpp): where each moves, by
// channel; no other vertex pays for any of it.
#include "res://render/shaders/contracts/people.gdshaderinc"
#include "res://render/shaders/bytes.gdshaderinc"
uniform sampler2D face_table : filter_nearest, repeat_disable;
uniform int act_face[PEOPLE_CLIPS]; // by clip: what the face does at it (FACE_ACT_*)
uniform float body_mood[PEOPLE_BODIES]; // by variation: -1 low .. 1 bright (cast.json)

// The face table's channels, in the order of its cooked columns (people_asset.cpp checks PEOPLE_FACE_CHANNELS of them).
const int BLINK = 0, JOY = 1, SADNESS = 2, ANGER = 3, SURPRISE = 4, BROW_RAISE = 5, OPEN = 6, ROUND = 7, PUCKERED = 8, SPREAD = 9, SHUT = 10, LOOK_LEFT = 11, LOOK_RIGHT = 12;
const int VISEME_COUNT = 9; // the mouth shapes a syllable is drawn from, a vowel's more often (faceExpression.ts VISEMES)
const int VISEMES[VISEME_COUNT] = { OPEN, SPREAD, SPREAD, ROUND, PUCKERED, SHUT, SHUT, OPEN, SPREAD };
const uint FACE_SALT = 0x4c9e3721u, STRESS_SALT = 7u; // a person's face its own stream of the hash; a syllable's stress another
// The person's hash: Wellons's lowbias32 ("Hash function prospector", 2018), the seed offset by the golden ratio's 2^32 part.
const uint GOLDEN_WORD = 0x9e3779b9u, LOWBIAS_A = 0x21f0aaadu, LOWBIAS_B = 0x735a2d97u, LOWBIAS_SHIFT_A = 16u, LOWBIAS_SHIFT_B = 15u;
// The hash's fields: a nibble for the blink's rate, ten bits for the gaze's phase, six for the sob's, a byte for the speech's.
const uint BLINK_SHIFT = 8u, NIBBLE = 15u, GAZE_SHIFT = 5u, TEN_BITS = 1023u, SIX_BITS = 63u, TALK_SHIFT = 16u;
const float BEAT_SPAN = 64.0; // the speech's phase: a byte over this, up to four syllables
// The model's rates and weights, by eye as the 4.0's faces (materials.json people.params); each vector's parts are said here.
uniform vec3 face_blink; // a blink from this share of its cycle; its cycles a second, more by this for each step of a nibble
uniform vec2 face_gaze; // the gaze's sway: its rate, its reach
uniform vec2 face_talk; // speech's syllables a second; a stressed one's brow
uniform int face_stress_every; // one syllable in this many is stressed
uniform vec4 face_sob; // sobbing: its rate; the brow raised; the jaw, from and over the sob squared
uniform vec2 face_sob_blink; // the eyes shut, from and over the sob
uniform vec4 face_scream; // terror: the scream's cycles a second, the share of each spent screaming, the jaw's floor, its shake (rad/s)
uniform vec2 face_terror; // its sadness and its lips stretched back
uniform vec4 face_mood; // joy by a bright mood, by talking, by listening; sadness by a low mood

uint person_hash(uint id) {
	uint h = id + GOLDEN_WORD;
	h = (h ^ (h >> LOWBIAS_SHIFT_A)) * LOWBIAS_A;
	h = (h ^ (h >> LOWBIAS_SHIFT_B)) * LOWBIAS_B;
	return h ^ (h >> LOWBIAS_SHIFT_B);
}

int face_row(float uv2_x) {
	return int(uv2_x / PEOPLE_FACE_ROW_STEP);
}

// What the vertex of `row` moves by, at rest. `code`: the instance colour's alpha (people_pose.gdshaderinc).
vec3 face_move(int row, vec4 custom, float code) {
	float w[PEOPLE_FACE_CHANNELS];
	for (int c = 0; c < PEOPLE_FACE_CHANNELS; c++) {
		w[c] = 0.0;
	}
	float time = idle_clock;
	uint hash = person_hash(uint(floor(abs(custom.b) + 0.5)) ^ FACE_SALT);
	float low = float(hash & BYTE_MASK_U);
	int doing = code >= PEOPLE_CLIP_CODE_BASE - 0.5 ? 0 : act_face[int(abs(custom.g))];
	bool frightened = custom.b < 0.0;

	float blink = fract(time * (face_blink.y + float((hash >> BLINK_SHIFT) & NIBBLE) * face_blink.z) + low / BYTE_TOP);
	w[BLINK] = blink > face_blink.x ? sin((blink - face_blink.x) / (1.0 - face_blink.x) * PI) : 0.0;
	float gaze = sin(time * face_gaze.x + float((hash >> GAZE_SHIFT) & TEN_BITS)) * face_gaze.y;
	w[LOOK_LEFT] = max(gaze, 0.0);
	w[LOOK_RIGHT] = max(-gaze, 0.0);

	if (frightened && doing == FACE_ACT_CROUCH) {
		// Sobbing (FACS sadness AU1+4+15 with the jaw catching)
		float sob = max(0.0, sin(time * face_sob.x + float(hash & SIX_BITS)));
		w[SADNESS] = 1.0;
		w[BROW_RAISE] = face_sob.y;
		w[OPEN] = face_sob.z + face_sob.w * sob * sob;
		w[BLINK] = face_sob_blink.x + face_sob_blink.y * sob;
	} else if (frightened) {
		// Terror (FACS fear, AU1+2+4+5+20+26): brows up and drawn together, eyes wide, lips stretched back, the jaw dropped
		float scream = fract(time * face_scream.x + low / BYTE_TOP);
		w[BLINK] = 0.0;
		w[SURPRISE] = 1.0;
		w[SADNESS] = face_terror.x;
		w[OPEN] = scream < face_scream.y ? face_scream.z + 0.25 * sin(time * face_scream.w + low) : 0.25;
		w[SPREAD] = face_terror.y;
	} else {
		float mood = body_mood[int(custom.r + 0.5)];
		w[JOY] = max(mood, 0.0) * face_mood.x + (doing == FACE_ACT_TALK ? face_mood.y : doing == FACE_ACT_LISTEN ? face_mood.z : 0.0);
		w[SADNESS] = max(-mood, 0.0) * face_mood.w;
		if (doing == FACE_ACT_TALK || doing == FACE_ACT_PHONE) {
			float beat = time * face_talk.x + float((hash >> TALK_SHIFT) & BYTE_MASK_U) / BEAT_SPAN;
			float syllable = floor(beat);
			float open = sin((beat - syllable) * PI);
			uint said = uint(syllable);
			w[VISEMES[int(person_hash(hash ^ said) % uint(VISEME_COUNT))]] += open;
			if (person_hash(hash ^ (said * STRESS_SALT)) % uint(face_stress_every) == 0u) {
				w[BROW_RAISE] += face_talk.y * open;
			}
		}
	}

	ivec2 at = ivec2((row % PEOPLE_FACE_SPAN) * PEOPLE_FACE_CHANNELS, row / PEOPLE_FACE_SPAN);
	vec3 move = vec3(0.0);
	for (int c = 0; c < PEOPLE_FACE_CHANNELS; c++) {
		if (w[c] > 0.0) {
			move += min(w[c], 1.0) * texelFetch(face_table, at + ivec2(c, 0), 0).rgb;
		}
	}
	return move;
}
