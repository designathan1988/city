// Declarations written for the platform test (log/trace-core-plan.md, Task 4): the shapes a TypeScript `lib` has —
// generic interfaces merged across declarations, index signatures, callbacks, construct signatures, a type alias of a
// union, an interface extending a generic one, iterators keyed by the well-known symbol — in the test's own words.
interface StepYield<T> { done?: false; value: T; }
interface StepDone { done: true; value: undefined; }
type Step<T> = StepYield<T> | StepDone;
interface Iterator<T> { next(): Step<T>; }
interface IterableIterator<T> extends Iterator<T> { [Symbol.iterator](): IterableIterator<T>; }

interface Array<T> {
  length: number;
  [n: number]: T;
  push(...items: T[]): number;
  map<U>(callbackfn: (value: T, index: number, array: T[]) => U): U[];
}
interface Array<T> {
  [Symbol.iterator](): IterableIterator<T>;
}

interface Set<T> {
  add(value: T): this;
  [Symbol.iterator](): IterableIterator<T>;
}
interface SetConstructor { new <T>(values?: readonly T[] | null): Set<T>; }
declare var Set: SetConstructor;

interface Map<K, V> {
  get(key: K): V | undefined;
  set(key: K, value: V): this;
}
interface MapConstructor { new <K, V>(): Map<K, V>; }
declare var Map: MapConstructor;

interface Thenable<T> { then<R>(onfulfilled: (value: T) => R | Thenable<R>): Thenable<R>; }
interface Promise<T> { then<R>(onfulfilled: (value: T) => R | Thenable<R>): Promise<R>; }
interface PromiseConstructor {
  new <T>(executor: (resolve: (value: T) => void) => void): Promise<T>;
  resolve<T>(value: T): Promise<T>;
}
declare var Promise: PromiseConstructor;

interface Element { focus(): void; }
interface Listener { handle(event: Element): void; }
interface Page {
  find(selector: string): Element | null;
  listen(listener: Listener | ((event: Element) => void)): void;
}
declare var page: Page;

declare module "host" {
  global {
    interface Page { title(): Element; }
  }
}
