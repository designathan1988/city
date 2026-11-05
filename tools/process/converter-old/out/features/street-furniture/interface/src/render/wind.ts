// What the feature uses of src/render/wind.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/wind.ts:13-19 variable windUniforms
/** Wind: the one moving thing in a scene of static props. */

/** Shared by every material with wind; `renderer.ts` advances the time. */
export const windUniforms = {
  uWindTime: { value: 0 },
  /** Direction the wind blows TOWARDS, in three's XZ plane. */
  uWindDir: { value: new Vector2(0.82, -0.57).normalize() },
  /** How hard it blows, 1 the breeze the plants were tuned to (`setWindWeather`). */
  uWindStrength: { value: 1 },
};

// src/render/wind.ts:28-33 interface WindResponse
export interface WindResponse {
  /** Top displacement as a fraction of height, at a full gust. */
  readonly sway: number;
  /** Leaf or blade flutter, as a fraction of height. */
  readonly flutter: number;
}

// src/render/wind.ts:103-105 function markSway
/** A material the wind moves (its shader reads `windUniforms`): every shader that sways marks its material, lit and depth. */
export function markSway(material: Material): void;

// src/render/wind.ts:144-148 variable WIRE_WIND_UNIFORMS_GLSL
/** The uniforms a wire in the wind reads (`wireWindGlsl`), declared once in the vertex shader. */
export const WIRE_WIND_UNIFORMS_GLSL = `
        uniform float uWindTime;
        uniform vec2 uWindDir;
        uniform float uWindStrength;
        uniform float uWireAmp;`;
