// A declared type bounds its place (log/trace-rules.md, S2): what flows in passes only if the type admits it.
// `via` passes any value on, as code with no declared type does.
export class BoundBox {
  open(): number { return 1; }
}

export class BoundBig extends BoundBox {
  big = true;
}

export class BoundTile {
  lay(): number { return 2; }
}

interface BoundShape {
  readonly w: number;
  readonly h?: number;
}

function via(v) { return v; }

export function bNum(x: number) { return x; }
bNum(1);
bNum(via("s"));
bNum(via(new BoundBox()));

export function bBox(b: BoundBox) { return b; }
bBox(new BoundBox());
bBox(new BoundBig());
bBox(via(new BoundTile()));
bBox(via({ open() { return 3; } }));

export function bShape(s: BoundShape) { return s; }
bShape({ w: 1 });
bShape(via({ h: 2 }));
bShape(via(4));
bShape(via("text"));

export function bLength(s: { length: number }) { return s; }
bLength(via("text"));
bLength(via(5));

export function bOpt(x?: number) { return x; }
bOpt();
bOpt(2);
bOpt(via("s"));

export class BoundHolder {
  n: number = 0;
  put(v) { this.n = v; }
  get() { return this.n; }
}
new BoundHolder().put("s");
new BoundHolder().put(3);

export function bRet(v): number { return v; }
bRet("s");
bRet(5);

export function bRec(r: Record<string, BoundBox>) { return r; }
bRec({ a: new BoundBox() });
bRec(via("s"));

export function bUnion(x: BoundBox | null) { return x; }
bUnion(null);
bUnion(via(new BoundTile()));
bUnion(new BoundBox());

export async function bAsync(v): Promise<number> { return v; }
bAsync("s");
bAsync(1);

export function bAny(x: any, y: unknown) { return [x, y]; }
bAny(via("s"), via(new BoundTile()));
