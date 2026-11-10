// What the feature uses of src/render/buildings/landmarks.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/landmarks.ts:8-10 interface LandmarkSink
/** The five buildings of the reference sheet the player gave on 2026-10-09 */

interface LandmarkSink extends Sink {
  lamp(x: number, y: number, z?: number, h?: number): void;
}

// src/render/buildings/landmarks.ts:11-11 type Design
type Design = (p: LandmarkSink, W: number, D: number, g: number, s: number, upper: number) => void;

// src/render/buildings/landmarks.ts:27-128 function hotelBeauxArts
const hotelBeauxArts: Design = (p, W, D, g, s, upper) =>;

// src/render/buildings/landmarks.ts:361-415 function glassTower
const glassTower: Design = (p, W, D, g, s, upper) =>;

// src/render/buildings/landmarks.ts:418-420 variable LANDMARKS
/** The landmark designs, by tower kind. */
export const LANDMARKS: Readonly<Record<string, Design>> = {
  beauxArts: hotelBeauxArts, decoTower, modernOffice, residentialTerrace, glassSpire: glassTower,
};
