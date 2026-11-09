// Fixture: a helper only the feature uses; dead code elsewhere also names it, and must not make it shared.
import { roundTo } from '../shared/math';

export function dockNear(x: number): number {
  return roundTo(x, 5);
}
