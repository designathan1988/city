// What the feature uses of src/peopleLab/pose.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/peopleLab/pose.ts:6-6 type PoseName
/** THE LAB'S POSES: a relaxed standing pose, breathing, and a few gestures, aimed in world space so no bone's own axes have to be known */

export type PoseName = 'idle' | 'wave' | 'cheer' | 'hips';

// src/peopleLab/pose.ts:101-101 field ClipPlayer.transfer
private transfer: NeutralWalk | null = null;

// src/peopleLab/pose.ts:102-102 field ClipPlayer.clip
private clip: LibraryClip | null = null;

// src/peopleLab/pose.ts:108-116 method ClipPlayer.play
/** Plays `name` from now; false when the library has no such clip. */
  play(name: LibraryClipName | null, now: number): boolean;

// src/peopleLab/pose.ts:118-118 getter ClipPlayer.playing
get playing(): boolean;

// src/peopleLab/pose.ts:124-128 method ClipPlayer.apply
/** The pose at `now` (seconds); a clip that does not loop holds its last frame. */
  apply(now: number): void;
