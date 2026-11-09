// State rules: module-level mutable state, state written from many files, interface code writing game state, and
// simulation code that depends on the wall clock or unseeded randomness.
import type { UnitRecord } from '../graph/ir.ts';
import type { AuditContext, Finding, RuleOutcome } from './context.ts';

function writerFiles(ctx: AuditContext, target: string): string[] {
  const files = new Set<string>();
  for (const edge of ctx.incoming.get(target) ?? []) {
    if (edge.kind === 'write') files.add(ctx.byId.get(edge.from)!.file);
  }
  return [...files].sort();
}

export function moduleState(ctx: AuditContext): RuleOutcome {
  return ctx.ir.units
    .filter((u) => u.facts.mutableModuleState)
    .map((u): Finding => {
      const files = writerFiles(ctx, u.id);
      return { rule: 'module-state', severity: files.length > 1 ? 'high' : 'medium', unit: u.id, file: u.file, line: u.start.line,
        message: `${u.name}: mutable module variable written from ${files.length} file(s)${files.length ? ` (${files.slice(0, 4).join(', ')})` : ''}` };
    });
}

const folderOf = (file: string): string => file.slice(0, Math.max(0, file.lastIndexOf('/')));

/** The fields of each class, and the classes a field's declared type names (typeRef edges): unique_ptr<JobPool> → JobPool. */
class Members {
  readonly #ctx: AuditContext;
  readonly #fields = new Map<string, UnitRecord[]>();

  constructor(ctx: AuditContext) {
    this.#ctx = ctx;
    for (const u of ctx.ir.units) if (u.kind === 'field' && u.parent !== null) this.#fields.set(u.parent, [...(this.#fields.get(u.parent) ?? []), u]);
  }

  fieldsOf(cls: string): UnitRecord[] {
    return this.#fields.get(cls) ?? [];
  }

  typesOf(field: UnitRecord): UnitRecord[] {
    return [...new Set((this.#ctx.outgoing.get(field.id) ?? []).filter((e) => e.kind === 'typeRef')
      .map((e) => this.#ctx.byId.get(e.to)!).filter((t) => t.kind === 'class'))];
  }

  /** The classes `start` holds, at any depth, itself included. */
  held(start: UnitRecord, seen = new Set<string>()): Set<string> {
    if (seen.has(start.id)) return seen;
    seen.add(start.id);
    for (const field of this.fieldsOf(start.id)) for (const type of this.typesOf(field)) this.held(type, seen);
    return seen;
  }
}

/**
 * The owner of every unit in the state tree under `root`. A member of the root belongs to the folder declaring its
 * type (CityState::traffic: TrafficState → sim/traffic), or to the root's folder when it is a plain value; that owner
 * goes down through the types each member holds. A type reached from two owners is a shared value type (a pose, a
 * spatial hash), owned by whoever holds each instance: it is left out of the tree.
 */
function stateOwners(members: Members, root: UnitRecord): Map<string, string> {
  const owners = new Map<string, Set<string>>();
  const claim = (id: string, owner: string): void => void owners.set(id, (owners.get(id) ?? new Set()).add(owner));
  for (const member of members.fieldsOf(root.id)) {
    const types = members.typesOf(member);
    claim(member.id, types.length > 0 ? folderOf(types[0]!.file) : folderOf(root.file));
    for (const type of types) for (const cls of members.held(type, new Set([root.id]))) if (cls !== root.id) claim(cls, folderOf(type.file));
  }
  return new Map([...owners].filter(([, set]) => set.size === 1).map(([id, set]) => [id, [...set][0]!]));
}

/**
 * State written from outside its owner. With a state root, only its tree is state and the owner is the subsystem
 * holding it (see stateOwners); without one, any field, owned by the folder declaring it. Not outside the owner: the
 * class's own methods (encapsulation: Rng::next writing Rng::s), the architecture's designated writers (`writerDirs`),
 * and the channels (`channels`) with everything they hold (a triple buffer's frames, filled by its producer).
 * The finding is each write from outside the owner, on the unit that writes (the methodology: one owner mutates, the
 * others send it a command); the state written has no behaviour, and what only reads it did nothing wrong.
 */
export function scatteredWrites(ctx: AuditContext): RuleOutcome {
  const findings: Finding[] = [];
  const members = new Members(ctx);
  const named = (id: string): boolean => ctx.options.channels.some((c) => id === c || id.startsWith(c + '::') || id.startsWith(c + '.'));
  const channelClasses = new Set(ctx.ir.units.filter((u) => u.kind === 'class' && named(u.id)).flatMap((u) => [...members.held(u)]));
  const isChannel = (id: string): boolean => named(id) || channelClasses.has(id);
  const rootId = ctx.options.stateRoot;
  const root = rootId === null ? null : ctx.byId.get(rootId);
  if (root === undefined) return { skipped: `--state-root ${rootId} is not a unit of the graph` };
  const tree = root === null ? null : stateOwners(members, root);
  for (const unit of ctx.ir.units) {
    if (isChannel(unit.id) || (unit.parent !== null && isChannel(unit.parent))) continue;
    let owner = folderOf(unit.file);
    if (tree !== null) {
      const held = tree.get(unit.id) ?? (unit.parent === null ? undefined : tree.get(unit.parent));
      // Outside the tree: a value or utility type, owned by whoever holds each instance.
      if (held === undefined) continue;
      owner = held;
    }
    const writers = (ctx.incoming.get(unit.id) ?? []).filter((e) => e.kind === 'write').map((e) => ctx.byId.get(e.from)!)
      .filter((w) => unit.parent === null || w.parent !== unit.parent);
    // The owner is the exact folder: a subfolder is another subsystem (src/ui is not src).
    const outside = writers.filter((w) => folderOf(w.file) !== owner && !ctx.inDirs(folderOf(w.file), ctx.options.writerDirs));
    const foreign = [...new Set(outside.map((w) => folderOf(w.file)))].sort();
    for (const writer of new Map(outside.map((w) => [w.id, w])).values()) {
      findings.push({ rule: 'scattered-writes', severity: foreign.length >= 2 ? 'high' : 'medium', unit: writer.id, file: writer.file, line: writer.start.line,
        message: `${writer.name} writes ${unit.name} (owned by ${owner}) from ${folderOf(writer.file)}; ${foreign.length} owner(s) outside write it: ${foreign.slice(0, 6).join(', ')}` });
    }
  }
  return findings;
}

export function uiWritesState(ctx: AuditContext): RuleOutcome {
  const { uiDirs, stateDirs } = ctx.options;
  if (uiDirs.length === 0 || stateDirs.length === 0) return { skipped: 'needs --ui-dir and --state-dir' };
  const findings: Finding[] = [];
  for (const edge of ctx.ir.edges) {
    if (edge.kind !== 'write') continue;
    const from = ctx.byId.get(edge.from)!;
    const to = ctx.byId.get(edge.to)!;
    if (!ctx.inDirs(from.file, uiDirs) || !ctx.inDirs(to.file, stateDirs)) continue;
    findings.push({ rule: 'ui-writes-state', severity: 'high', unit: from.id, file: from.file, line: from.start.line,
      message: `${from.name} writes ${to.name} (${to.file}:${to.start.line})` });
  }
  return findings;
}

export function nondeterminismInSimulation(ctx: AuditContext): RuleOutcome {
  if (ctx.options.simDirs.length === 0) return { skipped: 'needs --sim-dir' };
  return ctx.ir.units
    .filter((u) => ctx.inDirs(u.file, ctx.options.simDirs) && u.facts.nondeterminism.length > 0)
    .map((u): Finding => ({ rule: 'nondeterminism-in-simulation', severity: 'high', unit: u.id, file: u.file, line: u.start.line,
      message: `${u.name} uses ${u.facts.nondeterminism.join(', ')}` }));
}
