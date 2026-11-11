// Values a declaration admits (log/trace-rules.md, "What narrowing showed on the lab app": T1', T1'', G8'), and an
// argument a call leaves out (MDN Functions: undefined).
export interface Runner { done: boolean; mode: 'walk' | 'run'; act: { kind: string } | null; note?: string; }
export class Pack { open(): void {} }
const runners: Map<number, Runner> = new Map();

// T1': a container's element and an interface's object hold what their declared properties admit.
export function declLive(id: number) { const p = runners.get(id); if (!p || p.done) return null; return p; }
export function declWalking(id: number) { const p = runners.get(id); if (p && p.mode === 'walk') return p; return null; }
export function declNote(r: Runner) { return r.note; }
export function declKinds(x: number | string | boolean | null | undefined | symbol | bigint | object) { return x; }
export function declNever(x: never) { return x; }
export function declOptional(x?: Pack) { return x; }
export type ActOf = Runner['act'];
export function declLookup(a: ActOf) { return a; }
export function declConditional<T>(x: T extends string ? Pack : null) { return x; }
export function declTypeof(x: number | Pack) { if (typeof x === "number") return x; return null; }
export function declFinish(p: Runner) { p.done = true; p.mode = 'run'; }
export interface Bag { [key: string]: Pack; }
const bags: Map<number, Bag> = new Map();
export function declIndexed(id: number) { const b = bags.get(id); if (b && b.anything) return b; return null; }
export function declIndexValue(b: Bag) { return b.thing; }

// T1'': a literal of the program is the value of its kind.
export function declNumber() { return 1; }
export function declTemplate(n: number) { return `a${n}`; }
export function declNot(x: Pack) { return !x; }
export function declTypeofText(x: Pack) { return typeof x; }
export function declMinus(a: number, b: number) { return a - b; }

// G8': a read through a getter is its result; a property with a setter alone reads undefined.
export class Scoped {
  scope: Set<number> | null = null;
  get scoped(): boolean { return this.scope !== null; }
  set level(v: number) { this.scope = null; }
}
export function declGetter(s: Scoped) { if (s.scoped) return s; return null; }
export function declSetterOnly(s: Scoped) { return s.level; }

// `a && b` is a only when a is falsy; `a || b` a only when truthy; `a ?? b` a only when neither null nor undefined.
export function declAndValue(c: Pack | null) { return c && 1; }
export function declOrValue(c: Pack | null) { return c || 'none'; }
export function declNullishValue(c: Pack | null) { return c ?? 'none'; }

// An object literal typed as an interface is one of its objects: what is written through either is in both (S1).
export interface Car { shadow: Pack | null; }
export function makeCar(): Car { return { shadow: null }; }
const cars: Map<number, Car> = new Map();
export function declShadowWrite(id: number) { const c = cars.get(id); if (c) c.shadow = new Pack(); }
export function declShadowRead() { const c = makeCar(); if (c.shadow) return c; return null; }

// `this` is the object the method is called on (J2'): an inherited chainable method returns its receiver.
export class Chain { setTag(t: string) { return this; } }
export class ChainLeaf extends Chain { leafOnly(): void {} }
export class ChainOther extends Chain { otherOnly(): void {} }
export function declFluent() { return new ChainLeaf().setTag('a'); }
export function declFluentOther() { new ChainOther().setTag('b').otherOnly(); }
export class Lone { me() { return this; } }

// An intersection with a primitive is that primitive: a branded id is a number, no object.
declare const LaneBrand: unique symbol;
export type LaneId = number & { readonly [LaneBrand]: true };
export function declBranded(id: LaneId) { return id; }

// Two objects typed as one interface stay two objects: one's own values are not the other's (S1 is per object).
export interface Spot { mark: object; }
const spotA: Spot = { mark: new Pack() };
const rawB = { mark: { tag: 1 } };
export const spotB: Spot = rawB;
export function declSpotB() { return rawB.mark; }
export function declSpotTyped() { return spotB; }
export function declSpotA() { return spotA.mark; }

// JavaScript has no overloads: a method of one receiver is not chosen away by another's arity (C4 is C++'s).
export interface Emitter { emitted(kind: string, text: string, data?: number): void; }
const emitter: Emitter = { emitted(kind: string, text: string, data: number) {} };
export function declArity() { emitter.emitted('a', 'b'); }

// `satisfies T` keeps the value as it is (TypeScript 4.9); a constant spread is the object it holds.
const SHAPE_BASE = { carried: undefined } as const satisfies { [K in keyof Runner]?: undefined };
export function declSpreadBase() { const r = { ...SHAPE_BASE, own: 1 }; r.carried = new Pack(); return r.carried; }
export function declSatisfies() { return { held: new Pack() } satisfies { held: Pack | null }; }

// A constructor parameter written with `private`, `public`, `protected` or `readonly` is a property of the class.
export class Keeper { constructor(private readonly pack: Pack) {} keeperPack() { return this.pack; } }
export function declParamProperty() { return new Keeper(new Pack()).keeperPack(); }

// A call of a member reaches each receiver's method and what each receiver's field holds, together.
export function makePanel(): { toggle(): void } { const flip = (): void => {}; return { toggle: flip }; }
export function declFieldCall() { makePanel().toggle(); }

// An optional method may be absent (T1').
export interface Reads { at(): Pack; atInto?(): Pack; }
export function declOptionalMethod(f: Reads) { return f.atInto ? f.atInto() : f.at(); }

// The count of an array the program builds is a number (MDN Array: length).
const shelf = [new Pack()];
export function declCount() { const all = shelf.filter((p) => !!p); if (!all.length) return null; return all[0]; }

// A call statement that is no assertion function proves nothing of its argument, a string included (G1).
export function declPlainCall(k: string) { const seen = new Set<string>(); seen.add(k); return k; }
declPlainCall('a');

// An argument a call leaves out is undefined.
export function declLeft(a: Pack, b) { return b; }
export function declLeftCall() { return declLeft(new Pack()); }
