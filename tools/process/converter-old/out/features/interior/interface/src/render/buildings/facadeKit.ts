// What the feature uses of src/render/buildings/facadeKit.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/facadeKit.ts:6-17 interface Sink
/** The facade kit: architecture built as geometry */

/** What the kit draws into (`signature.ts` `Parts`). */
export interface Sink {
  box(mat: string, x0: number, x1: number, y0: number, y1: number, z0: number, z1: number): void;
  pushRaw(mat: string, g: BufferGeometry): void;
  rail(x0: number, x1: number, y0: number, y1: number, z: number, h?: number): void;
  plant(x: number, y: number, z: number, size?: number): void;
  bush(x: number, y: number, z: number, r: number, dark?: boolean): void;
  tree(x: number, y: number, r?: number, h?: number): void;
  hip(mat: string, x0: number, x1: number, y0: number, y1: number, z: number, h: number, top?: number): void;
  gable(mat: string, x0: number, x1: number, y0: number, y1: number, z: number, h: number, along?: 'x' | 'y'): void;
  cylinder(mat: string, x: number, y: number, z0: number, z1: number, r: number): void;
  Y(y: number): number;
}
