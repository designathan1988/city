// The code of src/sim/meso/slots.ts that is the feature's (the rest of the file is shared).

// src/sim/meso/slots.ts:119-226 class Threads: the feature's own
export class Threads {
  /** Each key's group, in the order the groups were made (as a Map's). */
  private readonly groups = new Map<number, number>();
  private gKey = new Float64Array(16);
  private gHead = new Int32Array(16);
  private gTail = new Int32Array(16);
  private gCount = new Int32Array(16);
  private readonly gFree: number[] = [];
  private gTop = 0;
  private next = new Int32Array(0);
  private prev = new Int32Array(0);
  /** Each slot's group, -1 for none. */
  private groupOf = new Int32Array(0);

  /** Room for slots below `n`. */
  reserve(n: number): void {
    if (n <= this.groupOf.length) return;
    const cap = Math.max(n, this.groupOf.length * 2, 64);
    this.next = grown(this.next, cap, -1);
    this.prev = grown(this.prev, cap, -1);
    this.groupOf = grown(this.groupOf, cap, -1);
  }

  /** `slot` put last in `key`'s group (out of the one it was in). */
  add(slot: number, key: number): void {
    this.reserve(slot + 1);
    if (this.groupOf[slot]! >= 0) this.remove(slot);
    let g = this.groups.get(key);
    if (g === undefined) {
      g = this.gFree.pop() ?? this.gTop++;
      if (g >= this.gKey.length) {
        const n = this.gKey.length * 2;
        this.gKey = grown(this.gKey, n); this.gHead = grown(this.gHead, n); this.gTail = grown(this.gTail, n); this.gCount = grown(this.gCount, n);
      }
      this.groups.set(key, g);
      this.gKey[g] = key; this.gHead[g] = -1; this.gTail[g] = -1; this.gCount[g] = 0;
    }
    const tail = this.gTail[g]!;
    this.prev[slot] = tail;
    this.next[slot] = -1;
    if (tail >= 0) this.next[tail] = slot; else this.gHead[g] = slot;
    this.gTail[g] = slot;
    this.gCount[g] = this.gCount[g]! + 1;
    this.groupOf[slot] = g;
  }

  /** `slot` out of its group; a group left empty goes. */
  remove(slot: number): void {
    const g = slot < this.groupOf.length ? this.groupOf[slot]! : -1;
    if (g < 0) return;
    const p = this.prev[slot]!, n = this.next[slot]!;
    if (p >= 0) this.next[p] = n; else this.gHead[g] = n;
    if (n >= 0) this.prev[n] = p; else this.gTail[g] = p;
    this.groupOf[slot] = -1;
    this.gCount[g] = this.gCount[g]! - 1;
    if (this.gCount[g] === 0) { this.groups.delete(this.gKey[g]!); this.gFree.push(g); }
  }

  /** The key of `slot`'s group (NaN: none). */
  keyOf(slot: number): number {
    const g = slot < this.groupOf.length ? this.groupOf[slot]! : -1;
    return g >= 0 ? this.gKey[g]! : Number.NaN;
  }

  /** The group of `key` (-1: none). */
  group(key: number): number {
    return this.groups.get(key) ?? -1;
  }

  /** Every group, in the order they were made. */
  allGroups(): IterableIterator<number> {
    return this.groups.values();
  }

  /** `visit` called with every group, in the order they were made (`Map.prototype.forEach`): DO NOT walk them with an iterator a step, a result object a group (the meso's step, sim20a). */
  eachGroup(visit: (group: number) => void): void {
    this.groups.forEach(visit);
  }

  /** One past the highest group index ever made: a resumable walk over the groups goes from 0 to it, skipping the empty (`count` 0). */
  get groupLimit(): number {
    return this.gTop;
  }

  /** A group's first slot (-1: none), the slot after `slot` in its group (-1: the last), how many it holds. */
  head(g: number): number {
    return g >= 0 ? this.gHead[g]! : -1;
  }

  after(slot: number): number {
    return this.next[slot]!;
  }

  count(g: number): number {
    return g >= 0 ? this.gCount[g]! : 0;
  }

  /** The slots of `key`'s group, as a new list. */
  list(key: number): number[] {
    const out: number[] = [];
    for (let s = this.head(this.group(key)); s >= 0; s = this.next[s]!) out.push(s);
    return out;
  }

  get bytes(): number {
    return this.next.byteLength * 3 + this.gKey.byteLength + this.gHead.byteLength * 3;
  }
}
