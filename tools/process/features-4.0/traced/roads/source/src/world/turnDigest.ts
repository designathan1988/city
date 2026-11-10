// The code of src/world/turnDigest.ts that is the feature's (the rest of the file is shared).

// src/world/turnDigest.ts:4-27 class TurnDigest: the feature's own
export class TurnDigest {
  a = 0x811c9dc5;
  b = 0x01000193;
  /** Folds in another digest's two lanes. */
  mix(a: number, b: number): void {
    this.a = Math.imul(this.a ^ a, 0x01000193) >>> 0;
    this.b = Math.imul(this.b ^ b, 0x5bd1e995) >>> 0;
    this.b = (this.b ^ (this.b >>> 13)) >>> 0;
  }
  add(n: number): void {
    const q = Math.round(n * 1e5);
    this.a = Math.imul(this.a ^ (q & 0xffff), 0x01000193) >>> 0;
    this.a = Math.imul(this.a ^ ((q >>> 16) & 0xffff), 0x01000193) >>> 0;
    this.b = Math.imul(this.b ^ q, 0x5bd1e995) >>> 0;
    this.b = (this.b ^ (this.b >>> 13)) >>> 0;
  }
  point(p: Vec2): void {
    this.add(p.x);
    this.add(p.y);
  }
  value(): string {
    return `${this.a.toString(36)}${this.b.toString(36)}`;
  }
}
