// What the feature uses of src/render/people/skinAppearance.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/people/skinAppearance.ts:12-12 interface SkinAppearance
export interface SkinAppearance { texture: Texture; eyeTexture: Texture; tint: Color; hair: Color; hairTexture?: Texture; browTexture?: Texture; lashTexture?: Texture; beardTexture?: Texture; garments: (Texture | null)[]; outfitTint: Color | null; beard: number; makeup: number }

// src/render/people/skinAppearance.ts:22-30 interface CardAtlas
/** One array of same-sized layers and who holds which layer, owned by whoever makes the materials */
interface CardAtlas {
  /** Width and height of every layer, pixels. */
  readonly size: number;
  /** The layer a texture is copied into (when first asked for); -1 without a picture to copy. */
  layerOf(texture: Texture | undefined): number;
  /** The array, ONE uniform for every material: when the array grows, every program reads the new one */
  readonly uniform: { value: DataArrayTexture | null };
  dispose(): void;
}

// src/render/people/skinAppearance.ts:33-39 interface PersonLayers
/** A person's two arrays (`SMALL_LAYER`, `LARGE_LAYER`): a texture goes in the one of its own size */
export interface PersonLayers {
  readonly small: CardAtlas;
  readonly large: CardAtlas;
  /** Where a texture is drawn from: [0, 0] without one, else [1 small or 2 large, its layer]. */
  place(texture: Texture | undefined | null): Place;
  dispose(): void;
}

// src/render/people/skinAppearance.ts:41-58 function createPersonLayers
export function createPersonLayers(): PersonLayers;

// src/render/people/skinAppearance.ts:292-364 function loadSkinAppearance
/** Existing CC0 skin and eye packs, selected by the authored body and look. */
export async function loadSkinAppearance(person: PersonSpec): Promise<SkinAppearance>;

// src/render/people/skinAppearance.ts:369-374 function releaseSkinAppearance
/** Releases one user's textures without invalidating another person's shared garment. */
export function releaseSkinAppearance(appearance: SkinAppearance): void;

// src/render/people/skinAppearance.ts:377-531 function applySkinAppearance
/** Extends the crowd shader after its bone-palette hook, preserving one draw batch. */
export function applySkinAppearance(material: MeshStandardMaterial, geometry: BufferGeometry, skin: SkinAppearance, layers: PersonLayers): void;

// src/render/people/skinAppearance.ts:534-534 type Place
/** Where a picture is drawn from: [0 none, 1 the small array or 2 the large one, its layer]. */
type Place = readonly [number, number];
