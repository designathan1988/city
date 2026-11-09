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
  return spot(p) + spot(t.posts[0]!) + (t.posts[9]?.x ?? 0) * 0 + ([1, 2][5] ?? 4) + spots(t.posts) + near.length + big.length + highestId(t.posts);
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

const HALF_SPAN = spanOf(40);

/** A constant that calls a function declared after it, and one that reads it. */
function spanOf(n: number): number {
  return n / 2;
}

const QUARTER_SPAN = HALF_SPAN / 2;

/** Prices by name: a dictionary, read, written, walked. */
const PRICES: Record<string, number> = { Alpha: 3, Beta: 5 };

export function priceOf(label: string): number {
  const table: Record<string, number> = {};
  table['Delta'] = 7;
  let sum = QUARTER_SPAN;
  for (const [k, v] of Object.entries(PRICES)) sum += v + k.length;
  return sum + (PRICES[label] ?? 1) + (table[label] ?? 2);
}

/** A running count: a class, its instance shared by whoever holds it. */
export class Tally {
  total = 0;
  #steps: number[] = [];
  readonly label: string;

  constructor(label: string) {
    this.label = label;
  }

  add(n: number): Tally {
    this.total += n;
    this.#steps.push(n);
    return this;
  }

  get count(): number {
    return this.#steps.length;
  }

  mean(): number {
    return this.count > 0 ? this.total / this.count : 0;
  }

  static of(n: number): Tally {
    return new Tally('one').add(n);
  }
}

/** A link: a class that holds itself, absent at the end, with no constructor of its own for its fields. */
class Link {
  value = 1;
  next: Link | null = null;
}

/** Two names of one instance see the same changes. */
export function tallied(values: number[]): number {
  const t = new Tally('sum');
  const same = t;
  for (const v of values) same.add(v);
  t.add(2).add(3);
  return t.total + same.count + t.mean() + Tally.of(4).total + t.label.length;
}

export function chainSum(n: number): number {
  const head = new Link();
  let tail = head;
  for (let i = 0; i < n % 5; i++) {
    const made = new Link();
    made.value = i + 2;
    tail.next = made;
    tail = made;
  }
  let sum = 0;
  for (let at: Link | null = head; at !== null; at = at.next) sum += at.value;
  return sum;
}

/** A kept list of anything: a generic class, a template in C++. */
export class Shelf<T> {
  #items: T[] = [];

  put(item: T): void {
    this.#items.push(item);
  }

  at(i: number): T {
    return this.#items[i]!;
  }

  get size(): number {
    return this.#items.length;
  }
}

enum Grade {
  Low = 2,
  High = 7,
}

/** A value of an inferred type: the class is named by nobody here but its factory. */
function shelfOf(n: number): Shelf<number> {
  const made = new Shelf<number>();
  for (let i = 0; i < n % 4; i++) made.put(i * Grade.High);
  return made;
}

export function shelved(n: number): number {
  const kept = shelfOf(n);
  const names = new Shelf<string>();
  names.put('ab');
  return kept.size + (kept.size > 0 ? kept.at(kept.size - 1) : Grade.Low) + names.at(0).length;
}

/** A class with one member C++ has no form for, and one that calls it: both leave, the class stays. */
class Mixed {
  n = 3;

  doubled(): number {
    return this.n * 2;
  }

  kind(v: number): string {
    return new Intl.Collator().resolvedOptions().locale + v;
  }

  named(): string {
    return this.kind(this.n);
  }
}

export function mixed(k: number): number {
  return new Mixed().doubled() + k;
}

type TagId = number & { readonly tagBrand: true };

/** Per tag and per leg: entries made on first use, and what is read out of a map changed where it lies. */
class Legs {
  counts = new Map<TagId, Map<TagId, number>>();
  rates = new Map<TagId, Map<TagId, { r: number }>>();

  record(a: TagId, b: TagId): void {
    let legs = this.counts.get(a);
    if (!legs) this.counts.set(a, legs = new Map());
    legs.set(b, (legs.get(b) ?? 0) + 1);
  }

  bump(a: TagId, b: TagId): void {
    let legs = this.rates.get(a);
    if (!legs) this.rates.set(a, legs = new Map());
    const known = legs.get(b);
    if (!known) { legs.set(b, { r: 1 }); return; }
    known.r += 2;
  }

  raise(a: TagId): void {
    for (const known of this.rates.get(a)?.values() ?? []) known.r += 10;
  }

  dropAll(keep: TagId): void {
    for (const a of [...this.counts.keys()]) if (a !== keep) this.counts.delete(a);
  }
}

/** Ids past what a float holds: 2^25 and 2^25 + 1 are two keys. */
export function legsOf(n: number): number {
  const big = 33554432 as TagId;
  const next = 33554433 as TagId;
  const legs = new Legs();
  legs.record(big, next);
  legs.record(big, next);
  legs.record(next, big);
  legs.bump(big, next);
  legs.bump(big, next);
  legs.raise(big);
  legs.dropAll(big);
  return legs.counts.size * 100 + (legs.counts.get(big)?.get(next) ?? 0) * 10 + (legs.rates.get(big)?.get(next)?.r ?? 0) + (n > 0 ? 1 : 0);
}

/** What only the running program knows: read by what it turns out to be. */
function readRaw(raw: unknown): number {
  if (typeof raw !== 'object' || raw === null) return typeof raw === 'string' ? raw.length : -1;
  const o = raw as { x?: unknown; tag?: unknown };
  const x = typeof o.x === 'number' ? o.x : 0;
  const tag = typeof o.tag === 'string' ? o.tag.length : 0;
  return x + tag;
}

export function rawRead(n: number): number {
  const made: unknown = n > 0 ? { x: n, tag: 'ab' } : n < 0 ? 'text' : null;
  return readRaw(made);
}

class Base {
  v = 1;
  read(): number { return this.v; }
  twice(): number { return this.read() * 2; }
}

class Child extends Base {
  constructor() {
    super();
    this.v = 5;
  }
  override read(): number { return this.v + 1; }
}

/** The base's method calls what the subclass wrote. */
export function inherited(n: number): number {
  const b: Base = new Child();
  return b.twice() + n;
}

class Sized {
  #w = 0;
  get width(): number { return this.#w; }
  set width(n: number) { this.#w = n * 2; }
}

export function sized(n: number): number {
  const s = new Sized();
  s.width = n;
  return s.width;
}

interface Filled { a: number; b?: number; c: number }

/** Filled in the order written, what is optional only sometimes. */
export function filled(n: number): number {
  const base: Filled = { a: 1, c: 2 };
  const made: Filled = { ...base, c: n, ...(n > 0 ? { b: 7 } : {}) };
  return made.a + made.c + (made.b ?? 0);
}

/** What two classes share: a method and a value one holds in a field and the other works out. */
export interface Gauge {
  readonly level: number;
  read(scale: number): number;
}

class Tank implements Gauge {
  level = 3;
  read(scale: number): number { return this.level * scale; }
}

class Dial implements Gauge {
  #turns = 2;
  get level(): number { return this.#turns + 1.5; }
  read(scale: number): number { return this.level + scale; }
}

function gaugeSum(g: Gauge, scale: number): number {
  return g.level + g.read(scale);
}

/** One function reads both through what they share. */
export function gauged(scale: number): number {
  const all: Gauge[] = [new Tank(), new Dial()];
  let sum = 0;
  for (const g of all) sum += gaugeSum(g, scale);
  return sum;
}

/** The kinds of prop a post holds: a string-literal union, an enum in C++. */
export type PropKind = 'pot' | 'bush';

/** A pot's height, m: made by a call (a function called on first use), of the name a kind of prop also has. */
export const POT = spanOf(3);

/** Every kind in order: the member named as the constant above stays the member. */
const PROP_KINDS: readonly PropKind[] = ['pot', 'bush'];

const GROUND_ATTRIBUTE = 'ground';

/** A text made from another: TypeScript types it as the text itself, a value, never an enumeration. */
const PARS = `attribute float ${GROUND_ATTRIBUTE};`;

/** A prop that may say its kind: an optional field of an enumeration (the lab's vehicle archetypes, `style?: BodyStyle`). */
interface Prop {
  readonly kind?: PropKind;
}

/** One prop that says its kind with a literal (the lab writes `style: 'hatch'`), one that leaves it out. */
const PROPS: readonly Prop[] = [{ kind: 'bush' }, {}];

/** A kind's place among the kinds, with the pot's height, the made text's length and how many props say that kind. */
export function propPlace(kind: PropKind): number {
  return PROP_KINDS.indexOf(kind) + POT + PARS.length + PROPS.filter((p) => p.kind === kind).length;
}
