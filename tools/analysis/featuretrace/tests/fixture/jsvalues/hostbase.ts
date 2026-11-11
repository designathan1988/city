// A member a class of the program does not declare, on a class whose base the tracer does not read (the host's Map),
// can be that base's, whatever code writes under that name elsewhere (log/trace-rules.md, V2').
class Keyed<K, V> extends Map<K, V> {
  version = 0;
}

export function writesGet(o: { get?: unknown }) { o.get = () => 1; }

export function hostGet() { const m = new Keyed<number, string>(); return m.get(1); }
