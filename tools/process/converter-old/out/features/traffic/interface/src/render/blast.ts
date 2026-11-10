// What the feature uses of src/render/blast.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/blast.ts:35-35 type DebrisKind
/** Explosions, and what they throw */

type DebrisKind = 'asphalt' | 'concrete' | 'earth' | 'brick' | 'wood' | 'metal' | 'glass' | 'char' | 'leaf' | 'flesh';

// src/render/blast.ts:42-62 interface DebrisSpec
interface DebrisSpec {
  /** `car`: a car's shell (bonnet, windscreen, roof, boot), length x height x width. */
  /** `slab`: a broken plate of the ground, jagged at its edges (unit size, thin along y). */
  /** `mesh`: a geometry of its own (`geometry`, world units about its centre): a vehicle's own burnt-out body. */
  readonly shape: 'box' | 'cylinder' | 'car' | 'slab' | 'mesh';
  readonly kind: DebrisKind;
  readonly geometry?: BufferGeometry;
  /** World position of the centre (three's frame: y up). */
  readonly at: Vector3;
  /** Box: width, height, depth. Cylinder: radius, length, radius. */
  readonly size: Vector3;
  readonly turn?: Quaternion;
  readonly velocity: Vector3;
  readonly spin?: Vector3;
  /** Seconds of fire on it (a burning wreck), 0 for none. */
  readonly burn?: number;
  /** Laid where it is and left there (a heaved slab of the ground), never moved. */
  readonly still?: boolean;
  /** Colour over the kind's. */
  readonly color?: number;
}

// src/render/blast.ts:64-66 interface BlastWorld
interface BlastWorld {
  groundAt(x: number, y: number): number;
}

// src/render/blast.ts:68-97 interface Blast
export interface Blast {
  readonly group: Group;
  /** The explosion itself at world (x, y), height z: `radius` its reach; `ground` what it tears up. */
  explode(x: number, y: number, z: number, radius: number, ground: 'road' | 'earth' | 'building'): void;
  /** A crater (a hole and its broken rim) and the scorch round it. */
  crater(x: number, y: number, z: number, radius: number, ground: 'road' | 'earth'): void;
  debris(spec: DebrisSpec): void;
  /** A wire torn loose: pinned at `from` (world, three's frame), its loose end at `to`, thrown by `kick`. */
  wire(from: Vector3, to: Vector3, kick: Vector3): void;
  /** An electric discharge at a point, repeated for a while (a broken line on the ground). */
  arc(at: Vector3, seconds: number): void;
  /** How hard the camera shakes now (world units). */
  shake(): number;
  /** A fire that keeps burning at world (x, y), height z, `size` across, for `seconds`: flames and a black column. */
  burn(x: number, y: number, z: number, size: number, seconds: number): void;
  /** Soot and ash laid on the ground at world (x, y): the street left dirty by the blast, for good. */
  soot(x: number, y: number, z: number, radius: number): void;
  /** A broken hydrant at world (x, y): a jet of water for `seconds` (`GEYSER_SECONDS` if not given), dying down at its end. */
  geyser(x: number, y: number, z: number, seconds?: number): void;
  /** A burst pipe: a thin spurt of water for a moment, not a hydrant's jet. */
  leak(x: number, y: number, z: number, seconds: number): void;
  /** Whether anything is still moving, burning or flashing. */
  active(): boolean;
  /** Whether anything that casts a shadow moves (debris and wrecks not yet at rest): fire, smoke, water, sparks and wires cast none (AGT-01). */
  casting(): boolean;
  /** Everything gone: debris, wrecks, craters, soot, fires, wires, jets (the weapons lab's clean slate). */
  clear(): void;
  update(dt: number, world: BlastWorld): void;
  dispose(): void;
}

// src/render/blast.ts:129-129 interface Rope
interface Rope { p: Vector3[]; o: Vector3[]; pin: Vector3; seg: number; sparks: number; age: number }

// src/render/blast.ts:208-750 function createBlast
/** `light`: the flash, made by the renderer and in the scene since it opened (`PlayEffectsContext.lights`). */
export function createBlast(exhaust: Exhaust, light: PointLight): Blast;
