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
