import { Vec, noise } from 'fakelib';
import { bump, clamp, SPEED_LIMIT } from '@lib/math';
import type { Shape } from './shape';

export class World {
  shapes: Shape[] = [];
  position = new Vec();
  total = 0;

  tick(dt: number): number {
    let sum = 0;
    for (const shape of this.shapes) {
      const sample = { area: shape.area() };
      sum += clamp(sample.area * dt, 0, SPEED_LIMIT);
    }
    this.position = this.position.add(new Vec());
    bump();
    return sum + noise(Math.random()) + performance.now() * 0;
  }

  listen(shape: Shape): void {
    shape.onChange = (area) => {
      this.total = area;
    };
    this.shapes.push(shape);
  }
}
