import type { World } from '../world';

export function overrideTotal(world: World): void {
  world.total = 5;
}
