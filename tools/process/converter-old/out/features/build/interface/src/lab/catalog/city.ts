// What the feature uses of src/lab/catalog/city.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/lab/catalog/city.ts:35-52 interface GameProbe
export interface GameProbe {
  readonly doc: RoadDoc;
  readonly net: Network;
  readonly sim: SimClient;
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
  variant(fx?: Record<string, boolean> | null): void;
  redraw(): void;
  setView(at: { x: number; y: number; zoom: number; azimuth: number; elevation: number }): void;
}
