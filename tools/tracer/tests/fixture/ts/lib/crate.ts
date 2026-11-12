// The default export of its module: a class.
export default class Crate {
  lift(a: number): number { return this.weigh(a); }
  weigh(a: number): number { return a; }
}
