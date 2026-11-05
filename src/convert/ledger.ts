// The conversion's ledger: every declaration of a feature, what it becomes in the target and why, from facts. A unit
// is translated by machine only when nothing wrong travels with it: the audit found no methodology in it that the
// translator cannot rewrite, and nothing it reaches is wrong either. Everything else is listed with the correct
// method and stays open until the game has it. Automatic translators do not reach hand-written quality on their own
// (arXiv 2602.00840), so the source's wrong forms are stopped here, before any code is written.
import { RULE_BY_ID } from '../audit/catalog.ts';
import type { AuditResult } from '../audit/run.ts';
import { classifyRoles, type Role, type UnitRole } from '../classify/roles.ts';
import type { EdgeRecord, GraphIR, UnitRecord } from '../graph/ir.ts';
import { ModulePlacement } from '../translate/modules.ts';
import type { Delivered } from './inventory.ts';

export type Disposition = 'exists' | 'translate' | 'method' | 'write';

/** Wrong forms the translator itself writes correctly (new Array(n) is a vector sized once): anything else keeps a unit from the machine. */
export const REWRITTEN: ReadonlySet<string> = new Set(['holey-array']);

export interface Method {
  readonly rule: string;
  readonly line: number;
  readonly message: string;
  readonly correct: string;
  readonly inTheGame: string;
}

export interface LedgerUnit {
  readonly id: string;
  readonly kind: string;
  readonly file: string;
  readonly line: number;
  readonly lines: number;
  readonly role: Role;
  readonly disposition: Disposition;
  /** Where it belongs in the target. */
  readonly owner: string;
  readonly why: string;
  readonly methods: readonly Method[];
}

export interface LedgerInput {
  readonly graph: GraphIR;
  readonly audit: Pick<AuditResult, 'findings'>;
  readonly own: readonly string[];
  /** Declarations of shared code the feature has a part of. */
  readonly sliced: readonly string[];
  readonly coverage: ReadonlyMap<string, Delivered>;
  /** Files and names the game owns already (the conversion library): reached, never translated. */
  readonly ownedFiles: readonly string[];
  readonly ownedNames: readonly string[];
}

const FOLLOWED: ReadonlySet<string> = new Set(['call', 'construct', 'read', 'reference']);
const TRANSLATABLE: ReadonlySet<string> = new Set(['function', 'variable', 'interface', 'type']);
const NO_OWNER = 'no owner yet: decide by what it does';
const RENDER_METHOD = 'three.js geometry goes through MeshBuilder, instances through MultiMesh grouped by area, materials through the game\'s shaders and tables, streaming through the render owners';
const UI_METHOD = 'interface code is GDScript in project/ui: reads CityStore.snapshot, sends CityStore.dispatch, texts by tr(), styles by theme_builder.gd';
const MIXED_METHOD = 'split by responsibility first: what changes state becomes a command in sim/commands, what draws goes to render, what shows goes to project/ui';

/** Where a unit of this role belongs. */
function ownerOf(unit: UnitRecord, role: Role, placement: ModulePlacement): string {
  const area = unit.file.split('/')[1] ?? '';
  if (role === 'ui') return area === 'editor' ? 'project/game/tools' : 'project/ui';
  if (role === 'mixed') return 'split by responsibility';
  if (role === 'render') return 'native/src/render';
  if (area === 'editor') return 'native/src/sim/commands';
  if (area === 'ui') return 'project/ui';
  const placed = placement.of(unit.file);
  return 'error' in placed ? NO_OWNER : placed.owner;
}

export class Ledger {
  readonly units: LedgerUnit[];
  readonly #byId: Map<string, UnitRecord>;
  readonly #children = new Map<string, string[]>();
  readonly #edges = new Map<string, EdgeRecord[]>();
  readonly #roles: Map<string, UnitRole>;
  readonly #findings = new Map<string, Method[]>();
  readonly #input: LedgerInput;

  constructor(input: LedgerInput) {
    this.#input = input;
    const { graph } = input;
    this.#byId = new Map(graph.units.map((u) => [u.id, u]));
    for (const e of graph.edges) {
      const list = this.#edges.get(e.from);
      if (list) list.push(e);
      else this.#edges.set(e.from, [e]);
    }
    for (const u of graph.units) {
      if (!u.parent) continue;
      const list = this.#children.get(u.parent);
      if (list) list.push(u.id);
      else this.#children.set(u.parent, [u.id]);
    }
    this.#roles = new Map(classifyRoles(graph.units, this.#byId, this.#edges).map((r) => [r.id, r]));
    for (const f of input.audit.findings) {
      // A pair of short identical runs is the target's clone gate to judge, on what is written.
      if (!f.unit || (f.rule === 'duplicated-code' && f.severity === 'low')) continue;
      const rule = RULE_BY_ID.get(f.rule);
      const method: Method = { rule: f.rule, line: f.line, message: f.message, correct: rule?.methodology ?? '', inTheGame: rule?.target ?? '' };
      const list = this.#findings.get(f.unit);
      if (list) list.push(method);
      else this.#findings.set(f.unit, [method]);
    }
    const placement = new ModulePlacement(graph.files.map((f) => f.path));
    const sliced = new Set(input.sliced);
    this.units = [...new Set([...input.own, ...input.sliced])].sort().map((id) => this.#entry(id, sliced.has(id), placement));
  }

  /** The unit and everything nested in it: a function's findings include its closures'. */
  #family(id: string): string[] {
    const out = [id];
    for (let i = 0; i < out.length; i++) out.push(...(this.#children.get(out[i]!) ?? []));
    return out;
  }

  #methods(id: string): Method[] {
    return this.#family(id).flatMap((u) => this.#findings.get(u) ?? []);
  }

  #owned(unit: UnitRecord): boolean {
    return this.#input.ownedFiles.includes(unit.file) || this.#input.ownedNames.includes(unit.name);
  }

  /** Why the machine may not write this unit, looking at the unit alone; null when it may. */
  #stops(unit: UnitRecord): string | null {
    const role = this.#roles.get(unit.id)?.role;
    if (role === 'render' || role === 'ui' || role === 'mixed') return `${role} code`;
    const parent = unit.parent === null ? undefined : this.#byId.get(unit.parent);
    if (parent?.kind === 'class') return 'a member of a class';
    // A field of a plain type travels with the type.
    if (unit.kind === 'field') return null;
    if (!TRANSLATABLE.has(unit.kind)) return `a ${unit.kind}`;
    if (unit.facts.mutableModuleState) return 'mutable module state';
    const wrong = this.#methods(unit.id).filter((m) => !REWRITTEN.has(m.rule));
    return wrong.length > 0 ? `wrong methodology: ${[...new Set(wrong.map((m) => m.rule))].join(', ')}` : null;
  }

  /** The first thing the unit reaches that the machine may not write, with what stopped it; null when it reaches none. */
  #reachesStop(start: UnitRecord): string | null {
    const queue = this.#family(start.id);
    const seen = new Set(queue);
    for (let i = 0; i < queue.length; i++) {
      for (const edge of this.#edges.get(queue[i]!) ?? []) {
        if (!FOLLOWED.has(edge.kind) || seen.has(edge.to)) continue;
        seen.add(edge.to);
        const to = this.#byId.get(edge.to);
        // What is not a unit of the program (a library, the platform) is judged by the translator, construct by construct.
        if (!to || this.#owned(to)) continue;
        const delivered = this.delivered(to);
        if (delivered) return `${to.id}, which the game already has in ${delivered.target || delivered.port}: name it in the conversion library`;
        const stop = this.#stops(to);
        if (stop) return `${to.id} (${stop})`;
        for (const id of this.#family(to.id)) {
          seen.add(id);
          queue.push(id);
        }
      }
    }
    return null;
  }

  delivered(unit: UnitRecord): Delivered | undefined {
    return this.#input.coverage.get(unit.id) ?? this.#input.coverage.get(unit.file);
  }

  #entry(id: string, sliced: boolean, placement: ModulePlacement): LedgerUnit {
    const unit = this.#byId.get(id);
    if (!unit) return { id, kind: 'statement', file: id.split('#')[0]!, line: 0, lines: 0, role: 'compute', disposition: 'write', owner: NO_OWNER, why: 'not a declaration of the program graph', methods: [] };
    const role = this.#roles.get(id)?.role ?? 'compute';
    const owner = ownerOf(unit, role, placement);
    const methods = this.#methods(id);
    const base = { id, kind: unit.kind, file: unit.file, line: unit.start.line, lines: unit.lines, role, owner, methods };
    const delivered = this.delivered(unit);
    if (delivered) return { ...base, disposition: 'exists', owner: delivered.target || owner, why: `port ${delivered.port}: ${delivered.state}` };
    if (sliced) return { ...base, disposition: 'write', why: 'a part of shared code: it joins the code of the owner that holds the rest' };
    if (role === 'render') return { ...base, disposition: 'write', why: RENDER_METHOD };
    if (role === 'ui') return { ...base, disposition: 'write', why: UI_METHOD };
    if (role === 'mixed') return { ...base, disposition: 'write', why: MIXED_METHOD };
    const stop = this.#stops(unit);
    if (stop) return { ...base, disposition: methods.some((m) => !REWRITTEN.has(m.rule)) ? 'method' : 'write', why: stop };
    const reached = this.#reachesStop(unit);
    if (reached) return { ...base, disposition: 'method', why: `reaches ${reached}` };
    return { ...base, disposition: 'translate', why: methods.length > 0 ? `the translator rewrites ${methods.map((m) => m.rule).join(', ')}` : 'computes or edits; nothing wrong in it or in what it reaches' };
  }

  /** Why the translator may not have emitted this source unit (a function it traced); null when it may. */
  stopOf(id: string): string | null {
    const unit = this.#byId.get(id);
    if (!unit) return `${id} is not a declaration of the program graph`;
    if (this.#owned(unit)) return `${id} belongs to the game (conversion library)`;
    return this.#stops(unit);
  }
}

export function counts(units: readonly LedgerUnit[]): Record<Disposition, number> {
  const out: Record<Disposition, number> = { exists: 0, translate: 0, method: 0, write: 0 };
  for (const u of units) out[u.disposition]++;
  return out;
}
