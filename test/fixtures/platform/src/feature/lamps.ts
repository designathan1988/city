import { LOD, Object3D, onFrame, type Scene } from 'fakegl';
import { later, later2 } from '../shared/queue';
import { measure } from '../shared/shapes';

/** The package's renderer calls `update` of every LOD it draws: an override of a member the package declares. */
class LampRoot extends LOD {
  override update(camera: unknown): void { void camera; }
  /** Not a member of the bases: the program calls it. */
  count(): number { return 1; }
  /** `this` seen as a type of the program: its members are the package's. */
  depth(): number { return (this as unknown as RootInternals)._depth; }
}
interface RootInternals { _depth: number }
/** Satisfies the shared `Shape` by its members: its getter and method run when shared code reads and calls them. */
class Disc { r = 1; get area(): number { return this.r * this.r; } grow(): void { this.r++; } }
/** Only forwards what it is given to shared code. */
function relay(f: () => number): void { later(f); }
/** Through a class of the program: still the package's `update`; and the package builds it (`clone`). */
class StreetLamps extends LampRoot {
  constructor(readonly n = 1) { super(); }
  override update(camera: unknown): void { super.update(camera); }
}

/** Members of the package's object it does not declare, reached through a type of the program. */
interface LodInternals { _levels: number; _pick(camera: unknown): number }

function noteScreen(): void {}

export function plant(scene: Scene): void {
  const root = new StreetLamps();
  const bulb = new Object3D();
  bulb.onBeforeRender = noteScreen;          // stored in a member of the package
  onFrame((t) => { void t; });               // given to a call of the package
  later(() => root.count());                 // given to shared code of the program, which calls it later
  later2({ run: () => root.count() });       // the same, as a member of an object literal
  const sizes = [1, 2].map((x) => x * 2);    // the standard library's iteration: run in place, not a hook
  const copies = Array.from(sizes, (x) => x); // the same
  void copies;
  const inner = root as unknown as LodInternals;
  void inner._pick(null);
  root.add(bulb);
  scene.add(root);
  void root.count();
  void root.depth();
  relay(() => root.count());
  void measure(new Disc());
}

