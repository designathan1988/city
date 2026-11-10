// What the feature uses of src/render/playEffects.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/playEffects.ts:58-92 interface PlayEffects
export interface PlayEffects {
  /** The effects' own seconds for a frame of `wallDt` (the weapons lab slows, stops and steps them). */
  clock(wallDt: number): number;
  setEffectsSpeed(speed: number): void;
  stepEffects(seconds: number): void;
  /** The bodies posed and drawn with the crowd's meshes (`AgentRenderOptions.ragdolls`). */
  frame(citizens: RagdollCitizens, sim: SimClient, wallDt: number, fxDt: number): void;
  /** Somebody whose own body lies on the ground: not drawn standing as well. */
  hides(id: number): boolean;
  /** Debris, ruins, blood stains, tracers: after the agents. */
  update(wallDt: number, fxDt: number): void;
  /** How far an explosion shakes the camera now. */
  shake(): number;
  busy(): boolean;
  /** Whether anything that casts a shadow moves: debris and wrecks not at rest, bodies falling (`busy` asks frames; this, the shadow cascades again, AGT-01). */
  castersMoving(): boolean;
  shootBody(a: readonly [number, number, number], b: readonly [number, number, number]): { alive: number; part: BodyPart } | 'hit' | null;
  ragdollProbe(): RagdollProbe[];
  clearCasualties(): void;
  vehicleHit(x: number, y: number, z: number, dirX: number, dirY: number, glass: boolean, blood: boolean): void;
  dropVehicle(v: { id: number; archetype: Archetype; x: number; y: number; angle: number; color: number; dirX: number; dirY: number }): void;
  wound(x: number, y: number, z: number, dirX: number, dirY: number, severed: Severable | null): void;
  shot(from: readonly [number, number, number], to: readonly [number, number, number]): void;
  strikeBuilding(b: Building, x: number, y: number, z: number, strength: number): boolean;
  explode(x: number, y: number, z: number, radius: number, hit: BlastHit): void;
  burn(x: number, y: number, z: number, size: number, seconds: number): void;
  soot(x: number, y: number, z: number, radius: number): void;
  geyser(x: number, y: number, z: number, seconds?: number): void;
  sparkAt(x: number, y: number, z: number, seconds: number): void;
  leak(x: number, y: number, z: number, seconds: number): void;
  onBuildingDown(listener: (id: number) => void): void;
  flingOccupants(list: readonly Occupant[]): void;
  forgetRuin(id: number): void;
  dispose(): void;
}
