// What the feature uses of src/render/wear.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/wear.ts:28-41 interface WearField
export interface WearField {
  readonly texture: Texture;
  /** The window's low corner (world x, y) and side, for the shader (`applyWear`). */
  readonly window: { readonly value: Vector3 };
  /** A vehicle at (x, y) heading `angle`, `width` across its wheels, for `dt` seconds. */
  wheels(x: number, y: number, angle: number, width: number, dt: number): void;
  /** A walker at (x, y), for `dt` seconds. */
  feet(x: number, y: number, dt: number): void;
  /** The window kept round where the view looks: wear is only kept near it (the traffic it shows is the micro window's). */
  follow(x: number, y: number): void;
  /** Uploads the field when it is due. */
  tick(dt: number): void;
  dispose(): void;
}

// src/render/wear.ts:139-187 function applyWear
/** Makes a road surface material show the wear field: `channel` 0 reads the wheels */
export function applyWear(material: Material, field: WearField, channel: 0 | 1, key: string): void;
