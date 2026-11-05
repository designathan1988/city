// The code of src/lab/catalog/city.ts that is the feature's (the rest of the file is shared).

// src/lab/catalog/city.ts:35-52 slice of src/lab/catalog/city.ts#GameProbe (shared): the feature's statements; "…" marks code left out
export interface GameProbe {
  // … 3 lines
  scene(): {
    readonly scene: Object3D;
    readonly stats: { readonly drawCalls: number; readonly drawnTriangles: number };
    strikeBuilding(b: Building, x: number, y: number, z: number, strength: number): boolean;
    explode(x: number, y: number, z: number, radius: number, hit: unknown): void;
    /** Everybody drawn last frame and the body the casting gave them (`renderer.ts` census). */
    census(): readonly { readonly seed: number; readonly model: string; readonly company: string }[];
    /** The camera the frame is drawn from. */
    viewCamera(): Camera;
  };
// … 3 lines
}
