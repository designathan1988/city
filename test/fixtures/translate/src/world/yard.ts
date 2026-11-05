// Fixture: a small feature with every case the flow of values must decide (each one a bug a translator makes when
// it decides types in several places).
import type { Vec2 } from '../core/vec2';
import { identityMemo } from '../core/memo';
import { dist } from '../core/geo';

export interface Post {
  readonly id: number;
  readonly x: number;
  readonly y: number;
  readonly tag?: string;
}

export interface YardData {
  readonly posts: readonly Post[];
  readonly nextId: number;
}

export const emptyYard = (): YardData => ({ posts: [], nextId: 1 });

const CELL = 16;

/** Edits return a new yard: the target changes it in place. */
export function addPost(t: YardData, x: number, y: number): { data: YardData; id: number } {
  const id = t.nextId;
  return { data: { ...t, posts: [...t.posts, { id, x, y }], nextId: id + 1 }, id };
}

export function removePost(t: YardData, id: number): YardData {
  return { ...t, posts: t.posts.filter((p) => p.id !== id) };
}

/** Posts by cell: a 64-bit key from the target's owner of cell keys, lists filled through an alias. */
const postCells = identityMemo((t: YardData) => {
  const cells = new Map<number, Post[]>();
  for (const p of t.posts) {
    const key = (Math.floor(p.x / CELL) + 32768) * 65536 + (Math.floor(p.y / CELL) + 32768);
    const list = cells.get(key);
    if (list) list.push(p); else cells.set(key, [p]);
  }
  return cells;
});

/** A counter whose only values are integers, through a cycle (i = i + 1). */
export function countNear(t: YardData, p: Vec2, reach: number): number {
  let n = 0;
  const cells = postCells(t);
  for (let x = Math.floor((p.x - reach) / CELL); x <= Math.floor((p.x + reach) / CELL); x++) {
    for (const q of cells.get((x + 32768) * 65536 + (Math.floor(p.y / CELL) + 32768)) ?? []) {
      if (dist(q, p) < reach) n = n + 1;
    }
  }
  return n;
}

/** The nearest post: set inside a callback (narrowing does not see it), an annotation narrower than the value. */
export function nearest(t: YardData, p: Vec2): { i: number; d: number } | null {
  let best: { i: number; d: number; at: Vec2 } | null = null;
  t.posts.forEach((q, i) => {
    const at = { x: q.x, y: q.y };
    const d = Math.hypot(at.x - p.x, at.y - p.y);
    if (!best || d < best.d) best = { i, d, at };
  });
  return best;
}

/** A reference to an element, never pointed elsewhere; a value rounded the JavaScript way. */
export function firstX(t: YardData): number {
  const first = t.posts[0]!;
  return Math.round(first.x);
}

/** Point indices kept in a map through a helper whose parameters come from integer call sites. */
export function chain(points: readonly Vec2[]): number {
  const at = new Map<number, number[]>();
  const link = (a: number, b: number): void => {
    for (const end of [a, b]) {
      const list = at.get(end);
      if (list) list.push(a + b); else at.set(end, [a + b]);
    }
  };
  for (let i = 1; i < points.length; i++) link(i - 1, i);
  return at.size;
}

/** A literal the target does not allow bare: its constant is named from what receives it. */
export function limited(n: number): number {
  const speed = Math.min(30, n);
  return speed;
}

/** A post by its id: the id's callers in the program are methods of a class (an id is an integer through them). */
export function postById(t: YardData, id: number): Post | null {
  return t.posts.find((p) => p.id === id) ?? null;
}

/** The median of values left as they are: a sorted copy (the spread copies; sorting it leaves the caller's). */
export function median(values: readonly number[]): number | null {
  if (!values.length) return null;
  const sorted = [...values].sort((a, b) => a - b);
  return sorted[Math.floor(sorted.length / 2)]!;
}

/** The slope of y over t: pairs read by position. */
export function slope(points: readonly (readonly [t: number, y: number])[]): number {
  let st = 0;
  let sy = 0;
  let den = 0;
  for (const [t, y] of points) { st += t; sy += y; den += (t - 1) ** 2; }
  return points.length > 0 ? sy / Math.max(st, 1) + den : 0;
}

/** Posts some test says are kept: a membership the caller gives (a set, or a rule in place). */
export function keptCount(t: YardData, kept: { has(id: number): boolean }): number {
  let n = 0;
  for (const p of t.posts) if (kept.has(p.id)) n++;
  return n;
}

/** Kept by a set and by a rule written in place. */
export function keptBoth(t: YardData): number {
  const chosen = new Set<number>();
  for (const p of t.posts) if (p.x > 0) chosen.add(p.id);
  const even = { has: (id: number): boolean => id % 2 === 0 || chosen.has(id) };
  return keptCount(t, chosen) + keptCount(t, even);
}

/** Totals by reduce, a numeric separator, a prefix test and NaN for nothing. */
export function summed(t: YardData, label: string): number {
  if (label.startsWith('(') || label.endsWith(')')) return NaN;
  const total = t.posts.map((p) => p.x).reduce((a, b) => a + b, 0);
  return total / 1_000.5;
}

const BRACKETED = /^\(.*\)$/;

/** A label's short form: the scheme cut, a prefix replaced once and a part everywhere; whether it is bracketed. */
export function shortLabel(label: string): number {
  const cut = label.replace(/^[a-z]+:\/\//i, '').replace('a', 'A').replaceAll('-', '');
  return cut.length + (BRACKETED.test(label) ? 1000 : 0);
}

/** A hash of heights read through ArrayLike, mixed by Math.imul with a hex constant and a key past int32. */
export function heightHash(t: YardData, k: number): number {
  const h: ArrayLike<number> = t.posts.map((p) => p.x);
  let s = 0;
  for (let i = 0; i < h.length; i++) s += h[i]!;
  return (Math.imul(k | 0, 0x27d4eb2d) ^ Math.imul(s | 0, 0x9e3779b1)) + (k > 0 ? 0x1_0000_0000 : 0);
}

/** How a post of a kind looks: the target owns it (spec.functions lookOf, spec.types PostLook). */
export interface PostLook {
  readonly height: number;
  readonly lit: boolean;
}

export function lookOf(kind: number): PostLook {
  return { height: kind, lit: kind > 1 };
}

/** The height of the lit posts of a kind: a value the target owns, its fields by the target's names. */
export function litHeight(t: YardData, kind: number): number {
  const look = lookOf(kind | 0);
  return look.lit ? look.height * t.posts.length : 0;
}

/** What only reads a place: a post and a look at one are both a { x, y }, and stay two types. */
function spot(p: { readonly x: number; readonly y: number }): number {
  return p.x + p.y;
}

/** The spots of many: a list of posts read as a list of places. */
function spots(list: readonly { readonly x: number }[]): number {
  let n = 0;
  for (const p of list) n += p.x;
  return n;
}

/** A template: each call its own T, with a callback and a container. */
function keptOf<T>(list: readonly T[], keep: (item: T) => boolean): T[] {
  const out: T[] = [];
  for (const item of list) if (keep(item)) out.push(item);
  return out;
}

/** A template over what has an id. */
function highestId<T extends { readonly id: number }>(list: readonly T[]): number {
  let best = -1;
  for (const item of list) if (item.id > best) best = item.id;
  return best;
}

/** Posts and plain numbers through the same templates, and two different types through the same place. */
export function placesAndIds(t: YardData, p: Vec2): number {
  const near = keptOf(t.posts, (q) => q.x > 0);
  const big = keptOf([1, 2, 3], (n) => n > 1);
  return spot(p) + spot(t.posts[0]!) + spots(t.posts) + near.length + big.length + highestId(t.posts);
}

/** A constant of its own, a root even if nothing here reads it. */
export const TURN = Math.PI * 2;

/** A kind by name: a switch with clauses that share a body, a default in the middle of nothing. */
export function kindCost(label: string, n: number): number {
  switch (label) {
    case 'Alpha':
    case 'Beta':
      return n + 1;
    case 'Gamma': {
      const twice = n * 2;
      return twice;
    }
    case '':
      n = n - 1;
      break;
    default:
      n = n + TURN;
  }
  return n;
}

/** A promise of the program: a negative count never comes. */
export function checkedCount(t: YardData): number {
  if (t.posts.length < 0) throw new Error(`no count ${t.nextId}`);
  return t.posts.length;
}

/** Ids walked as a Set and a Map keep JavaScript's order: insertion, the first place of a member. */
export function idOrder(t: YardData): number[] {
  const seen = new Set<number>();
  const sizes = new Map<number, number>();
  for (const p of t.posts) {
    seen.add(p.id % 3);
    sizes.set(p.id % 5, p.id);
  }
  seen.delete(1);
  const out = [...seen];
  for (const [k, v] of sizes) out.push(k + v);
  sizes.forEach((v, k) => { out.push(v - k); });
  for (const v of sizes.values()) out.push(v);
  return out;
}
