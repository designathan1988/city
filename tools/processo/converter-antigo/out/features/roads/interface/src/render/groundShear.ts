// What the feature uses of src/render/groundShear.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/groundShear.ts:4-7 interface Gradient
/** FEET ON A SLOPE, WITHOUT IK. */
export interface Gradient {
  readonly gx: number;
  readonly gy: number;
}

// src/render/groundShear.ts:13-21 function groundGradient
/** The ground's gradient at a point, from two forward differences off the height already known there. */
export function groundGradient(heightAt: (x: number, y: number) => number, x: number, y: number, h: number,
  reach = 0.75): Gradient;
