// An operator before a call written with type arguments applies to the whole call and what follows it (ECMAScript
// UnaryExpression, AwaitExpression; log/trace-rules.md, J13): `void f<T>(x).m()` is `void (f<T>(x).m())`.
class Ware {
  open(): number { return 1; }
}

function wareOf<T>(): Ware { return new Ware(); }

async function laterWare<T>(): Promise<Ware> { return new Ware(); }

export function regroupVoid() { return void wareOf<number>().open(); }

export function regroupNot() { return !wareOf<number>(); }

export async function regroupAwait() { const w = await laterWare<number>(); return w.open(); }

export function regroupMinus() { return 2 + -wareOf<number>().open(); }

export function regroupTypeof() { return typeof wareOf<number>(); }

export function regroupTwice() { return !wareOf<number>()!.open(); }
