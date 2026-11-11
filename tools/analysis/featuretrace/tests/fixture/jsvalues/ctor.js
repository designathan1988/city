// An object's `constructor` property is its class (log/trace-rules.md, J4'; MDN Object.prototype.constructor):
// `new this.constructor(...)` makes an object of the receiver's own class.
class Cloner {
  constructor(v) { this.v = v; }
  clone() { return new this.constructor(this.v); }
}

class SubCloner extends Cloner {}

export function cloned() { return new SubCloner(1).clone(); }

export function ctorOf() { return new Cloner(1).constructor; }
