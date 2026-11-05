// What the feature uses of src/render/ragdoll.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/ragdoll.ts:14-33 interface RagdollCitizens
/** Ragdolls: people knocked over or killed by a blow, or tripping, their own bodies falling. */

export interface RagdollCitizens {
  capturedPose(id: number): { index: number; palette: Float32Array; transform: Matrix4 } | null;
  skeletonOf(index: number): { names: string[]; parents: number[]; inverses: Matrix4[]; local: Matrix4; bind: Matrix4 } | null;
  /** `charred`: burnt black (a bomb's direct hit). */
  drawPalette(index: number, palette: Float32Array, instance: Matrix4, charred?: boolean, alive?: boolean): void;
  clipPose(index: number, key: 'crouchUp' | 'idle' | 'getUpFront' | 'getUpBack' | 'crawl', phase: number): { palette: Float32Array; duration: number } | null;
  /** Bodies loaded now, for people with no pose of their own (indoors). */
  loadedIndices(): number[];
  /** The index to draw a piece torn off this person with (their twin, or the same body); null when none can be drawn. */
  twin?(index: number): number | null;
  /** A twin's index given back (its piece gone). */
  untwin?(index: number): void;
  /** Burns the person black (a bomb's direct hit). */
  char?(index: number): void;
  /** A bullet hole in this part of the person, bleeding into their clothes. */
  /** From world (fromX, fromY) when known: the shot's jolt through them too. */
  wound?(index: number, part: BodyPart, fromX?: number, fromY?: number): void;
  /** The person soaked in blood all over (shot to pieces). */
  drench?(index: number): void;
}
