// Fixture: a caller outside the feature that reaches it only through a class: a method's result, a getter, and a
// parameter's default value are values that flow like any other.
import type { Vec2 } from '../core/vec2';
import { postById, type Post, type YardData } from '../world/yard';

export class Picker {
  constructor(private readonly yard: () => YardData) {}

  private get data(): YardData { return this.yard(); }

  /** The id of the post nearest a point within reach. */
  pick(p: Vec2, reach = 4): number | null {
    let best: Post | null = null;
    for (const q of this.data.posts) if (Math.hypot(q.x - p.x, q.y - p.y) < reach) best = q;
    return best?.id ?? null;
  }

  picked(p: Vec2): Post | null {
    const id = this.pick(p);
    return id === null ? null : postById(this.data, id);
  }
}
