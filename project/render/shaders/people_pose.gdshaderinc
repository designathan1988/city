// Where a cooked person's bone stands this frame, for whatever is drawn on that person: the body (people.gdshader), what the hand holds
// (people_held.gdshader) and the helmet (people_helmet.gdshader). INSTANCE_CUSTOM = (body, its row of the plays texture, a seed, what is
// held's share). The row holds the figure's plays (people_gait.cpp, the lab's citizenGait.ts gaitPlays): up to PEOPLE_PLAYS texels, each
// (clip, phase, weight, unused), the heaviest first and the unused ones of weight zero; the weights sum to one. A loop's phase is its
// fraction (its last row runs into its first); a one-shot's runs 0..1 over its rows, its last row its end. The bodies' palette blocks lie
// side by side, `palette_columns` across (people_asset.h). The instance data cannot carry the plays: the Compatibility renderer packs a
// MultiMesh's colour and custom data as half floats.
#include "res://render/shaders/bytes.gdshaderinc"
#include "res://render/shaders/contracts/people.gdshaderinc"
uniform sampler2D palette : filter_nearest, repeat_disable;
uniform sampler2D plays : filter_nearest, repeat_disable; // a figure's row: its plays (people_gait.cpp)
uniform int block_rows; // a variation's rows of the palette
uniform int palette_columns; // the bodies' blocks side by side in the palette
uniform int clip_row[PEOPLE_CLIPS]; // each clip's first row in a variation's block
uniform int clip_frames[PEOPLE_CLIPS];
uniform int clip_loop[PEOPLE_CLIPS]; // 1 a loop, 0 a one-shot

const float SIXTEEN_BIT_TOP = 65535.0; // a 16-bit value's top

// One palette value: 16-bit fixed point over [-PEOPLE_PALETTE_SPAN, PEOPLE_PALETTE_SPAN], high bytes in its first texel, low bytes in the next.
vec4 palette_row(int value, int row) {
	vec4 hi = texelFetch(palette, ivec2(value * PEOPLE_PALETTE_TEXELS, row), 0);
	vec4 lo = texelFetch(palette, ivec2(value * PEOPLE_PALETTE_TEXELS + 1, row), 0);
	return (hi * BYTE_STEPS + lo) * BYTE_TOP / SIXTEEN_BIT_TOP * 2.0 * PEOPLE_PALETTE_SPAN - PEOPLE_PALETTE_SPAN;
}

// A body's block: its first value across (its column) and its first row down.
ivec2 body_block(int variation) {
	return ivec2((variation % palette_columns) * PEOPLE_BONES * PEOPLE_BONE_TEXELS, (variation / palette_columns) * block_rows);
}

BonePose bone_pose(ivec2 block, int bone, int row) {
	int at = block.x + bone * PEOPLE_BONE_TEXELS;
	return BonePose(palette_row(at, row), palette_row(at + 1, row), palette_row(at + 2, row));
}

// Adds the pose of a clip at `phase` and `share`: the linear blend of the four bones. A bone of weight zero adds nothing, so its palette
// rows are not fetched (the weights are not sorted: what is held and the helmet pass (1, 0, 0, 0)).
void add_clip(ivec2 block, int clip, float phase, float share, ivec4 bones, vec4 weights, vec3 vertex, vec3 normal, inout vec3 pos, inout vec3 nor) {
	int frames = clip_frames[clip];
	bool loop = clip_loop[clip] != 0;
	float f = loop ? fract(phase) * float(frames) : clamp(phase, 0.0, 1.0) * float(frames - 1);
	int f0 = min(int(floor(f)), frames - 1);
	int first = block.y + clip_row[clip];
	int row1 = first + (f0 + 1 < frames ? f0 + 1 : (loop ? 0 : f0));
	float t = fract(f);
	for (int i = 0; i < 4; i++) {
		if (weights[i] == 0.0) {
			continue;
		}
		skin_add(pos, nor, share * weights[i], bone_pose(block, bones[i], first + f0), bone_pose(block, bones[i], row1), t, vertex, normal);
	}
}

// The play of `index` in the figure's row: (clip, phase, weight, unused).
vec4 figure_play(vec4 custom, int index) {
	return texelFetch(plays, ivec2(index, int(custom.g + 0.5)), 0);
}

// The clip that weighs most in the figure's pose (the face reads what it does from it).
int figure_clip(vec4 custom) {
	return int(figure_play(custom, 0).r + 0.5);
}

void person_pose(vec4 custom, ivec4 bones, vec4 weights, vec3 vertex, vec3 normal, out vec3 pos, out vec3 nor) {
	ivec2 block = body_block(int(custom.r + 0.5));
	pos = vec3(0.0);
	nor = vec3(0.0);
	for (int k = 0; k < PEOPLE_PLAYS; k++) {
		vec4 play = figure_play(custom, k);
		if (play.b <= 0.0) {
			break; // the heaviest first: the rest weigh nothing
		}
		add_clip(block, int(play.r + 0.5), play.g, play.b, bones, weights, vertex, normal, pos, nor);
	}
}
