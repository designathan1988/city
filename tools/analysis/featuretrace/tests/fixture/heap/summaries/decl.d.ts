// Declarations written for the generic summaries test (log/trace-core-plan.md, Task 5): positions whose types say
// nothing of where a value goes (any, unknown, object), a precise generic function, an intersection, in the test's
// own names.
type PropertyKey = string | number | symbol;
interface Descriptor { value?: any; get?(): any; }
declare function define(target: object, key: PropertyKey, descriptor: Descriptor): void;
declare function merge(target: object, source: object): void;
declare function pass(x: any): any;
declare function keep<T>(x: T, other: unknown): T;
declare function both<T extends {}, U>(target: T, source: U): T & U;
declare function invoke(target: any, key: PropertyKey, ...args: any[]): any;
declare function note(message: string, ...data: any[]): void;
