// The language's own predicates narrow (log/trace-rules.md, G1''; ECMAScript ArrayBuffer.isView, Number.isInteger,
// Number.isSafeInteger, Number.isFinite, Number.isNaN): a view is a typed array or a DataView, the host's; the
// Number tests are true of numbers alone.
class Gadget {
  n: number;
  constructor(n: number) { this.n = n; }
}

export function viewOnly(v) { if (ArrayBuffer.isView(v)) return v; return null; }
viewOnly(new Gadget(1));
viewOnly(new Float32Array(2));
viewOnly([1]);

export function viewMakes(v) {
  if (ArrayBuffer.isView(v)) { const Kind = v.constructor; return new Kind(4); }
  return null;
}
viewMakes(new Gadget(2));

export function intOnly(n) { if (Number.isInteger(n)) return n; return null; }
intOnly("s");
intOnly(3);
intOnly(new Gadget(3));

export function anyObjectArray(o: object) { if (Array.isArray(o)) return o; return null; }
