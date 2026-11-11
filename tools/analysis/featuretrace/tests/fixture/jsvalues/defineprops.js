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

// J14': a descriptor's members by their values: shorthand, a reference to a function, a descriptor no literal writes
// here, a key no literal names (put into the object, as `o[k] = v`).
function spotGetter() { return new Spot(); }

class Holder2 {
  constructor() {
    const value = new Spot();
    Object.defineProperty(this, 'short', { value });
    Object.defineProperty(this, 'byRef', { get: spotGetter });
    const desc = { value: new Spot() };
    Object.defineProperty(this, 'viaVar', desc);
  }
}

export function heldShort() { return new Holder2().short; }

export function heldByRef() { return new Holder2().byRef; }

export function heldViaVar() { return new Holder2().viaVar; }

export function heldComputed() {
  const box = {};
  const key = 'k';
  Object.defineProperty(box, key, { value: new Spot() });
  return box[key];
}

export function heldMap() {
  const box = {};
  const map = { a: { value: new Spot() } };
  Object.defineProperties(box, map);
  const k = 'a';
  return box[k];
}
