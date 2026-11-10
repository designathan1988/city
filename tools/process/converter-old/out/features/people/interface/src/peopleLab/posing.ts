// What the feature uses of src/peopleLab/posing.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/peopleLab/posing.ts:30-30 type Pose
/** A pose: each turned bone's local rotation (x, y, z, w), by name. */
export type Pose = Record<string, [number, number, number, number]>;

// src/peopleLab/posing.ts:32-32 type Face
/** A face: weights of the 52 ARKit shapes (`faceRig.ts`), or of the game's channels in older saves. */
export type Face = Record<string, number>;

// src/peopleLab/posing.ts:35-35 interface Key
/** A keyframe: a pose and a face at a time, seconds. */
export interface Key { readonly t: number; readonly pose: Pose; readonly face?: Face }

// src/peopleLab/posing.ts:38-43 interface Motion
/** What a person keeps of the posing (`LabPerson.motion`): the joints moved (the rig), the pose held, the face set, the animation's keys. */
export interface Motion {
  readonly rig?: Readonly<Record<string, [number, number, number]>>;
  readonly pose?: Pose;
  readonly face?: Face;
  readonly keys?: readonly Key[];
}
