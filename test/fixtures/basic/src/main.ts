import { createCounter } from './factory';
import { Square } from './shape';
import { World } from './world';

export function start(): World {
  const counter = createCounter(2);
  counter.add();
  const world = new World();
  world.listen(new Square());
  world.tick(1 / 60);
  new Worker(new URL('./sim.worker.ts', import.meta.url), { type: 'module' });
  void import('./lazy');
  return world;
}
