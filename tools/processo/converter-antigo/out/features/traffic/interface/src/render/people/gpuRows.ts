// What the feature uses of src/render/people/gpuRows.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/people/gpuRows.ts:13-31 interface GpuRows
export interface GpuRows {
  /** The texture the shaders read (another one after `grow`). */
  readonly texture: Texture;
  readonly rows: number;
  /** Texels a row. */
  readonly width: number;
  readonly bytes: number;
  /** `data` (`width * 4` floats a row; a short last row is padded) up at `row`, at the next `flush`. */
  write(row: number, data: Float32Array): void;
  /** `count` rows from `from` copied to `to` on the GPU, at the next `flush`. */
  copy(from: number, to: number, count: number): void;
  /** Room for `rows` rows; the rows on the GPU are carried over there at the next `flush`. */
  grow(rows: number): void;
  /** Every waiting step done, in the order asked. */
  flush(renderer: WebGLRenderer): void;
  /** The GPU's copy is gone (a WebGL context restored): nothing is carried over, the owner writes every row again. */
  reset(): void;
  dispose(): void;
}

// src/render/people/gpuRows.ts:33-33 variable copier
let copier: { scene: Scene; camera: OrthographicCamera; material: ShaderMaterial } | null = null;

// src/render/people/gpuRows.ts:34-34 variable warmed
let warmed = false;
