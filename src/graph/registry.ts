// The set of units reached so far, the work queue, the typed edges and the external uses, with stable readable ids
// (`relative/file.ts#Class.member`, plus `@line` when two declarations share a name).
import type * as TS from 'typescript';
import { normalizePath, relativePath } from '../common/paths.ts';
import type { EdgeKind } from './ir.ts';
import { headOf, kindOf, nameOf, unitKey, type UnitRef } from './units.ts';
import type { TypeScriptApi } from './typescript.ts';

export interface Unit {
  readonly key: string;
  readonly id: string;
  readonly ref: UnitRef;
  readonly file: string;
  readonly name: string;
  readonly kind: ReturnType<typeof kindOf>;
  readonly parentKey: string | null;
}

export class Registry {
  readonly units = new Map<string, Unit>();
  readonly queue: string[] = [];
  /** from → to → kind → count */
  readonly edges = new Map<string, Map<string, Map<EdgeKind, number>>>();
  /** unit key → external key → kind → count */
  readonly external = new Map<string, Map<string, Map<EdgeKind, number>>>();
  readonly nondeterminism = new Map<string, Set<string>>();
  readonly #ids = new Set<string>();
  readonly #ts: TypeScriptApi;
  readonly #root: string;

  constructor(ts: TypeScriptApi, root: string) {
    this.#ts = ts;
    this.#root = root;
  }

  /** Registers the unit (and its class head) if new; records the edge from `from` when given. Returns its key. */
  add(ref: UnitRef, from: string | null, kind: EdgeKind | null): string {
    const key = unitKey(ref);
    if (from !== null && kind !== null && from !== key) this.edge(from, key, kind);
    if (this.units.has(key)) return key;
    const head = headOf(this.#ts, ref);
    const parentKey = head ? unitKey(head) : null;
    const file = normalizePath(ref.node.getSourceFile().fileName);
    const name = nameOf(this.#ts, ref);
    this.units.set(key, { key, id: this.#uniqueId(ref, file, name), ref, file, name, kind: kindOf(this.#ts, ref), parentKey });
    this.queue.push(key);
    if (head) this.add(head, null, null);
    return key;
  }

  edge(from: string, to: string, kind: EdgeKind, times = 1): void {
    bump(nested(this.edges, from, to), kind, times);
  }

  externalUse(from: string, externalKey: string, kind: EdgeKind): void {
    bump(nested(this.external, from, externalKey), kind, 1);
  }

  markNondeterministic(from: string, what: string): void {
    const set = this.nondeterminism.get(from) ?? new Set<string>();
    set.add(what);
    this.nondeterminism.set(from, set);
  }

  #uniqueId(ref: UnitRef, file: string, name: string): string {
    const base = `${relativePath(this.#root, file)}#${name}`;
    if (!this.#ids.has(base)) {
      this.#ids.add(base);
      return base;
    }
    const sf: TS.SourceFile = ref.node.getSourceFile();
    const withLine = `${base}@${sf.getLineAndCharacterOfPosition(ref.node.getStart(sf)).line + 1}`;
    let id = withLine;
    for (let n = 2; this.#ids.has(id); n++) id = `${withLine}~${n}`;
    this.#ids.add(id);
    return id;
  }
}

function nested(map: Map<string, Map<string, Map<EdgeKind, number>>>, a: string, b: string): Map<EdgeKind, number> {
  let inner = map.get(a);
  if (!inner) map.set(a, (inner = new Map()));
  let kinds = inner.get(b);
  if (!kinds) inner.set(b, (kinds = new Map()));
  return kinds;
}

function bump(kinds: Map<EdgeKind, number>, kind: EdgeKind, times: number): void {
  kinds.set(kind, (kinds.get(kind) ?? 0) + times);
}
