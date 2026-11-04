// Fixture: a helper shared by features (core): translated once, into the core owner.
import type { Vec2 } from './vec2';

/** The distance between two points. */
export function dist(a: Vec2, b: Vec2): number {
  return Math.hypot(a.x - b.x, a.y - b.y);
}
