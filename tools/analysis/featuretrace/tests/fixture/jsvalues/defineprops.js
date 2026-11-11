// `Object.defineProperty` and `Object.defineProperties` define an object's properties (log/trace-rules.md, J14; MDN):
// a data descriptor's `value` is what the property holds, an accessor's `get` what reading it gives.
class Spot {
  constructor() { this.v = 1; }
}

class Holder {
  constructor() {
    const spot = new Spot();
    Object.defineProperties(this, { spot: { configurable: true, enumerable: true, value: spot } });
    Object.defineProperty(this, 'id', { value: 7 });
    Object.defineProperty(this, 'twice', { get() { return new Spot(); } });
  }
}

export function heldSpot() { return new Holder().spot; }

export function heldId() { return new Holder().id; }

export function heldTwice() { return new Holder().twice; }
