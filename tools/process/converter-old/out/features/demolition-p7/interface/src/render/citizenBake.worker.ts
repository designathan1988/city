// What the feature uses of src/render/citizenBake.worker.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/citizenBake.worker.ts:54-54 type BakedClip
export type BakedClip = Omit<ClipFrames, 'head'> & { head?: number[] };

// src/render/citizenBake.worker.ts:56-62 interface BakeReply
/** What a job answers: its clips, the person read, or why it failed. */
export interface BakeReply {
  readonly id: number; readonly clips?: (BakedClip | null)[]; readonly person?: DecodedPerson; readonly pixels?: Float32Array;
  /** A person's height and the same body at their class's height (`Morpher.height`), and their face's rows (none: no sliders). */
  readonly heights?: readonly number[]; readonly face?: Float32Array | null;
  readonly klass?: ClassRigMade; readonly worn?: WornPiece;
  readonly error?: string;
}

// src/render/citizenBake.worker.ts:89-91 function serve
/** The bakes' service of the job system (`job.worker.ts`). */
export function serve(scope: JobScope): void;
