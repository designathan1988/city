// `converter convert`: a feature carried as far as facts allow. Its ledger decides each unit, the machine translates
// what is clean, the quarantine judges every output, and the report lists what stays open with its correct method.
// Nothing is written to the target: accepted files go to --out/files. Exit 0 only when nothing is open.
import { createHash } from 'node:crypto';
import fs from 'node:fs';
import path from 'node:path';
import type * as TS from 'typescript';
import { runAudit } from '../audit/run.ts';
import { parseFlags, type FlagSpec } from '../common/args.ts';
import { removeTree } from '../common/files.ts';
import type { GraphIR } from '../graph/ir.ts';
import { loadTypeScript, openProgram } from '../graph/typescript.ts';
import { prepareTranslation, translate, type PreparedProgram } from '../translate/run.ts';
import type { TranslationSpec } from '../translate/spec.ts';
import { moduleSpecs, ownedNames, readLibrary } from './autospec.ts';
import { readCoverage } from './inventory.ts';
import { Ledger, counts, type LedgerUnit } from './ledger.ts';
import { compileFiles, scanFiles, scanUnits, targetLimits, type QuarantineFinding } from './quarantine.ts';

export const CONVERT_FLAGS: readonly FlagSpec[] = [
  { name: 'feature', required: true, help: 'the feature\'s name' },
  { name: 'source-root', required: true, help: 'root of the TypeScript source project' },
  { name: 'typescript', help: 'TypeScript package directory (default: the source project\'s own)' },
  { name: 'graph', required: true, help: 'the whole program\'s graph (converter graph)' },
  { name: 'boundary', required: true, help: 'the feature\'s boundary.json (converter feature)' },
  { name: 'target-root', required: true, help: 'root of the target game (read only)' },
  { name: 'target-graph', required: true, help: 'the target\'s C++ graph (converter cppgraph)' },
  { name: 'library', required: true, help: 'what the game already owns (configs/conversion-library.json)' },
  { name: 'ports', required: true, help: 'the port folders: each coverage.md (or the older cobertura.md) says what the game has' },
  { name: 'cxx', required: true, help: 'the C++ compiler that checks each output' },
  { name: 'constants', help: 'JSON: constant name -> its measurement or primary source' },
  { name: 'out', required: true, help: 'output folder: files/, ledger.json, registration.json, conversao.md' },
];

const AUDIT_EXTENSION = '.cpp';
const DEFAULT_MAX_SCATTERING = 0.5;
const DEFAULT_MIN_CONSTANT_FILES = 3;

interface ModuleOutcome {
  readonly module: string;
  readonly accepted: boolean;
  readonly files: readonly string[];
  readonly emitted: readonly string[];
  /** The source declarations its files hold (`module#name`), for a later module that shares one of them. */
  readonly declared?: readonly string[];
  readonly refusals: readonly string[];
}

interface Registration {
  readonly subsystem: string;
  readonly includes: readonly string[];
  readonly mapLine: string;
  readonly state: { readonly header: string; readonly anchor: string; readonly member: string; readonly root: string } | null;
}

function auditOf(graph: GraphIR, rulesFile: string): ReturnType<typeof runAudit> {
  const rules = JSON.parse(fs.readFileSync(rulesFile, 'utf8')) as {
    max_lines?: Record<string, number>; max_function_lines?: Record<string, number>; magic_numbers?: { allowed_values?: number[]; exempt?: string[] };
  };
  return runAudit(graph, {
    hot: [], uiDirs: ['src/ui'], stateDirs: ['src/world', 'src/sim'], simDirs: ['src/world', 'src/sim'],
    maxFileLines: rules.max_lines?.[AUDIT_EXTENSION], maxFunctionLines: rules.max_function_lines?.[AUDIT_EXTENSION], features: [],
    maxScattering: DEFAULT_MAX_SCATTERING, writerDirs: [], channels: [], stateRoot: null, minConstantFiles: DEFAULT_MIN_CONSTANT_FILES,
    allowedNumbers: rules.magic_numbers?.allowed_values ?? null, numberExempt: rules.magic_numbers?.exempt ?? [],
  });
}

function writeTree(root: string, files: ReadonlyMap<string, string>): void {
  for (const [file, text] of files) {
    const target = path.join(root, file);
    fs.mkdirSync(path.dirname(target), { recursive: true });
    fs.writeFileSync(target, text, 'utf8');
  }
}

/** Every named constant the accepted files hold, for the rule that each needs a source. */
function constantsIn(files: ReadonlyMap<string, string>): string[] {
  const out = new Set<string>();
  for (const [file, text] of files) {
    if (!file.endsWith('.h')) continue;
    for (const m of text.matchAll(/constexpr\s+[\w:]+\s+([A-Z][A-Z0-9_]*)\s*=/g)) out.add(m[1]!);
  }
  return [...out].sort();
}

function render(feature: string, units: readonly LedgerUnit[], outcomes: readonly ModuleOutcome[], open: readonly LedgerUnit[], unsourced: readonly string[], done: ReadonlySet<string>): string {
  const c = counts(units);
  const lines = [`# Conversion: ${feature}`, '',
    `Units: ${units.length}. In the game: ${c.exists}. Translated and accepted: ${done.size}. Open: ${open.length}. Constants without a source: ${unsourced.length}.`, '',
    open.length === 0 && unsourced.length === 0 ? 'Complete.' : '**Not complete.** Each open unit below still has to be written in its owner, by the method given.', '',
    '## Modules translated', '', ...(outcomes.length ? outcomes.map((o) => `- \`${o.module}\`: ${o.accepted ? `accepted, ${o.files.length} files${o.refusals.length ? `; roots left open: ${o.refusals.slice(0, 6).join('; ')}` : ''}` : `refused: ${o.refusals.slice(0, 6).join('; ')}`}`) : ['None.']), '',
    '## Open units', '', '| Unit | Role | Owner | Why | Method |', '|---|---|---|---|---|'];
  for (const u of open) {
    const methods = [...new Set(u.methods.map((m) => `${m.rule}: ${m.correct}`))].join('<br>');
    lines.push(`| \`${u.id}\` | ${u.role} | \`${u.owner}\` | ${u.why.replace(/\|/g, '/')} | ${methods.replace(/\|/g, '/')} |`);
  }
  lines.push('', '## Constants that need a measurement or a primary source', '', ...(unsourced.length ? unsourced.map((k) => `- \`${k}\``) : ['None.']), '');
  return lines.join('\n');
}

interface JudgeInput {
  readonly sourceRoot: string;
  readonly typescript: string | undefined;
  readonly targetRoot: string;
  readonly targetGraph: string;
  readonly cxx: string;
  readonly rules: string;
  readonly candidate: string;
  readonly stopOf: (id: string) => string | null;
  readonly prepared: PreparedProgram;
}

interface Judged {
  readonly outcome: ModuleOutcome;
  readonly files: Map<string, string>;
  readonly registration: Registration | null;
  /** What a refusal is made of, for attributing it to roots: the translator's errors, the quarantine's findings,
   *  the files written and the units emitted (their C++ names). */
  readonly attempt?: { readonly errors: readonly string[]; readonly findings: readonly QuarantineFinding[]; readonly files: ReadonlyMap<string, string>; readonly units: readonly { readonly source: string; readonly cpp: string }[] };
}

/**
 * Each root of a module and the module's declarations it reaches (itself included): a function, a constant or an
 * index of the same file it names, at any depth. A refusal of a unit is a refusal of every root that reaches it.
 */
function reachOf(spec: TranslationSpec, input: JudgeInput): Map<string, Set<string>> | null {
  const { ts, sp, sources, relative } = input.prepared;
  const file = sources.find((f) => relative(f) === spec.source);
  if (!file) return null;
  const top = new Map<string, TS.Node>();
  for (const st of file.statements) {
    if (ts.isFunctionDeclaration(st) && st.name) top.set(st.name.text, st);
    if (ts.isVariableStatement(st)) for (const d of st.declarationList.declarations) if (ts.isIdentifier(d.name)) top.set(d.name.text, d);
    if ((ts.isClassDeclaration(st) || ts.isInterfaceDeclaration(st) || ts.isTypeAliasDeclaration(st) || ts.isEnumDeclaration(st)) && st.name) top.set(st.name.text, st);
  }
  const uses = new Map<string, Set<string>>();
  for (const [name, node] of top) {
    const set = new Set<string>();
    const visit = (n: TS.Node): void => {
      if (ts.isIdentifier(n) && top.has(n.text) && n.text !== name) {
        const decl = sp.checker.getSymbolAtLocation(n)?.declarations?.[0];
        if (decl && decl.getSourceFile() === file) set.add(n.text);
      }
      ts.forEachChild(n, visit);
    };
    visit(node);
    uses.set(name, set);
  }
  const reach = new Map<string, Set<string>>();
  for (const root of spec.units) {
    const seen = new Set<string>([root]);
    const stack = [root];
    while (stack.length > 0) for (const next of uses.get(stack.pop()!) ?? []) if (!seen.has(next)) { seen.add(next); stack.push(next); }
    reach.set(root, seen);
  }
  return reach;
}

/** The C++ function a line of a generated file is in (its definition's name), or null (a header line, a type). */
function functionAt(text: string, line: number): string | null {
  const lines = text.split('\n');
  for (let i = Math.min(line, lines.length) - 1; i >= 0; i--) {
    const l = lines[i]!;
    if (/^}/.test(l) && i < line - 1) return null; // a function closed before the line: the line is outside one
    const m = /^(?!\s|\/\/|#|namespace\b|struct\b|enum\b|using\b|template\b)[^;]*?\b([A-Za-z_]\w*)\s*\([^;]*$/.exec(l);
    if (m) return m[1]!;
  }
  return null;
}

/** The units a refused judgment's findings belong to (unit → the first reason), or null when one cannot be placed. */
function failingUnits(judged: Judged): Map<string, string> | null {
  const a = judged.attempt;
  if (!a) return null;
  const out = new Map<string, string>();
  const bySource = new Map(a.units.map((u) => [u.cpp, u.source]));
  for (const e of a.errors) {
    const unit = /\[unit ([^\]]+)\]$/.exec(e)?.[1];
    if (!unit) return null;
    if (!out.has(unit)) out.set(unit, e.replace(/ \[unit [^\]]+\]$/, ''));
  }
  for (const f of a.findings) {
    if (f.rule === 'stopped-unit') { const u = f.file.slice(f.file.indexOf('#') + 1); if (!out.has(u)) out.set(u, `${f.rule}: ${f.detail}`); continue; }
    if (f.rule !== 'does-not-compile') return null; // a file-wide finding (a wrong form, a long file): no one unit
    const text = a.files.get(f.file);
    if (!text) return null;
    const lines = [...f.detail.matchAll(/:(\d+):\d+: error:/g)].map((m) => Number(m[1]));
    if (lines.length === 0) return null;
    for (const line of lines) {
      const cpp = functionAt(text, line);
      const unit = cpp ? bySource.get(cpp) : undefined;
      if (!unit) return null;
      if (!out.has(unit)) out.set(unit, `${f.rule} ${f.file}: ${f.detail}`);
    }
  }
  return out;
}

/**
 * The roots of a refused module by attribution: each finding belongs to a unit, a root that reaches a failing unit
 * is refused with its reason, the others are judged together again (a judgment that may show the next finding), to
 * the fixed point. One translation and one compile per round instead of one per root; null when a finding cannot be
 * placed (then the module is judged root by root).
 */
async function attributeByRoot(spec: TranslationSpec, input: JudgeInput, first: Judged): Promise<Judged | null> {
  const reach = reachOf(spec, input);
  if (!reach) return null;
  const blocked = new Map<string, string>();
  let roots = [...spec.units];
  let judged = first;
  for (;;) {
    const bad = failingUnits(judged);
    if (!bad || bad.size === 0) return null;
    for (const root of roots) for (const [unit, why] of bad) if (reach.get(root)?.has(unit)) { blocked.set(root, unit === root ? why : `reaches ${unit}: ${why}`); break; }
    const next = roots.filter((r) => !blocked.has(r));
    if (next.length === roots.length) return null; // the findings reach no root: not placed
    if (next.length === 0) return { ...first, outcome: { ...first.outcome, refusals: [...blocked].map(([u, w]) => `${u}: ${w}`) } };
    roots = next;
    judged = await judge({ ...spec, units: roots }, input);
    if (judged.outcome.accepted) return { ...judged, outcome: { ...judged.outcome, refusals: [...blocked].map(([u, w]) => `${u}: ${w}`) } };
  }
}

/** A refusal belongs to the root that caused it: a refused module is judged root by root and written again with those that pass. */
async function judgeByRoot(spec: TranslationSpec, input: JudgeInput): Promise<Judged> {
  const whole = await judge(spec, input);
  if (whole.outcome.accepted || spec.units.length < 2) return whole;
  const attributed = await attributeByRoot(spec, input, whole);
  if (attributed) return attributed;
  // the roots judged at the same time (each its own overlay; the compiles run together)
  const alone = await Promise.all(spec.units.map(async (unit) => ({ unit, judged: await judge({ ...spec, units: [unit] }, input) })));
  const passed = alone.filter((a) => a.judged.outcome.accepted).map((a) => a.unit);
  if (passed.length === 0) return whole;
  const kept = await judge({ ...spec, units: passed }, input);
  if (!kept.outcome.accepted) return whole;
  const left = alone.filter((a) => !a.judged.outcome.accepted).map((a) => `${a.unit}: ${a.judged.outcome.refusals[0] ?? 'refused'}`);
  return { ...kept, outcome: { ...kept.outcome, refusals: left } };
}

async function judge(spec: TranslationSpec, input: JudgeInput): Promise<Judged> {
  const module = spec.source;
  const refused = (refusals: readonly string[]): Judged => ({ outcome: { module, accepted: false, files: [], emitted: [], refusals }, files: new Map(), registration: null });
  let result: ReturnType<typeof translate>;
  try {
    result = translate({ sourceRoot: input.sourceRoot, tsconfig: path.join(input.sourceRoot, 'tsconfig.json'), typescriptDir: input.typescript, spec, targetRoot: input.targetRoot,
      targetGraph: input.targetGraph, prepared: input.prepared });
  } catch (error) {
    return refused([`the translator stopped: ${(error as Error).message.split('\n')[0]}`]);
  }
  if (result.errors.length > 0) return { ...refused(result.errors), attempt: { errors: result.errors, findings: [], files: result.files, units: result.units } };
  // one overlay per judgment (its units in the name): judgments of one module may run together
  const overlay = path.join(input.candidate, module.replace(/[^\w.-]+/g, '_'), createHash('sha256').update(spec.units.join('\0')).digest('hex').slice(0, 12));
  removeTree(overlay);
  writeTree(overlay, result.files);
  const findings: QuarantineFinding[] = [...scanUnits(input.stopOf, result.units), ...scanFiles(result.files, targetLimits(input.rules)),
    ...(await compileFiles(input.cxx, overlay, input.targetRoot, result.files))];
  if (findings.length > 0) return { ...refused(findings.map((f) => `${f.rule} ${f.file}: ${f.detail}`)), attempt: { errors: [], findings, files: result.files, units: result.units } };
  const registration: Registration = { subsystem: spec.subsystem, includes: spec.layer.includes, mapLine: spec.registry?.mapLine ?? '',
    state: spec.state.source ? { header: result.stateHeader, anchor: spec.state.anchor ?? '', member: spec.state.memberLine ?? '', root: spec.state.root } : null };
  return { outcome: { module, accepted: true, files: [...result.files.keys()], emitted: result.units.map((u) => u.source), declared: result.declared, refusals: [] }, files: result.files, registration };
}

export async function runConvertCommand(argv: readonly string[]): Promise<void> {
  const flags = parseFlags(argv, CONVERT_FLAGS);
  const feature = flags.require('feature');
  const sourceRoot = path.resolve(flags.require('source-root'));
  const targetRoot = path.resolve(flags.require('target-root'));
  const out = path.resolve(flags.require('out'));
  const library = readLibrary(flags.require('library'));
  const rules = path.join(targetRoot, library.rules);
  const graph = JSON.parse(fs.readFileSync(flags.require('graph'), 'utf8')) as GraphIR;
  const boundary = JSON.parse(fs.readFileSync(flags.require('boundary'), 'utf8')) as { own: string[]; sliced: string[] };
  const ledger = new Ledger({ graph, audit: auditOf(graph, rules), own: boundary.own, sliced: boundary.sliced, coverage: readCoverage(flags.require('ports')),
    ownedFiles: library.ownedFiles, ownedNames: ownedNames(library) });
  const typescript = flags.one('typescript');
  const ts = loadTypeScript(sourceRoot, typescript);
  const sp = openProgram(ts, sourceRoot, path.join(sourceRoot, 'tsconfig.json'));
  const specs = moduleSpecs(ts, sp, sourceRoot, feature, ledger.units, library, graph.files.map((f) => f.path));
  removeTree(out);
  fs.mkdirSync(out, { recursive: true });
  const accepted = new Map<string, string>();
  const outcomes: ModuleOutcome[] = [];
  const registrations: Registration[] = [];
  const sources = flags.one('constants');
  const known = sources && fs.existsSync(sources) ? JSON.parse(fs.readFileSync(sources, 'utf8')) as Record<string, string> : {};
  // Written after every module: a run stopped halfway leaves everything it decided.
  const report = (): { open: number; unsourced: number; done: number } => {
    writeTree(path.join(out, 'files'), accepted);
    // Translated means written: a function is done only if an accepted output holds it; a type travels with its module.
    const emitted = new Set(outcomes.flatMap((o) => o.emitted));
    const acceptedModules = new Set(outcomes.filter((o) => o.accepted).map((o) => o.module));
    const isDone = (u: LedgerUnit): boolean => u.disposition === 'translate'
      && (u.kind === 'function' || u.kind === 'variable' ? emitted.has(u.id) || emitted.has(u.id.slice(u.id.indexOf('#') + 1)) : acceptedModules.has(u.file));
    const done = new Set(ledger.units.filter(isDone).map((u) => u.id));
    const open = ledger.units.filter((u) => u.disposition !== 'exists' && !done.has(u.id));
    const unsourced = constantsIn(accepted).filter((k) => !known[k]);
    fs.writeFileSync(path.join(out, 'ledger.json'), JSON.stringify({ feature, counts: counts(ledger.units), done: [...done], units: ledger.units, outcomes, unsourced }, null, 1), 'utf8');
    fs.writeFileSync(path.join(out, 'registration.json'), JSON.stringify(registrations, null, 1), 'utf8');
    fs.writeFileSync(path.join(out, 'conversao.md'), render(feature, ledger.units, outcomes, open, unsourced, done), 'utf8');
    return { open: open.length, unsourced: unsourced.length, done: done.size };
  };
  let totals = report();
  const first = specs[0];
  if (first) {
    const targetGraph = path.resolve(flags.require('target-graph'));
    const prepared = prepareTranslation({ sourceRoot, tsconfig: path.join(sourceRoot, 'tsconfig.json'), typescriptDir: typescript, spec: first, targetRoot, targetGraph });
    for (const spec of specs) {
      const judgeInput = { sourceRoot, typescript, targetRoot, targetGraph, cxx: flags.require('cxx'), rules, candidate: path.join(out, 'candidate'),
        stopOf: (id: string) => ledger.stopOf(id), prepared };
      let judged = await judgeByRoot(spec, judgeInput);
      const clashes = (j: Judged): string[] => [...j.files].filter(([file, text]) => accepted.has(file) && accepted.get(file) !== text).map(([file]) => file);
      let clash = clashes(judged);
      // a module another accepted one shares, written with a different part of it: translated again rooting what the
      // earlier ones wrote, so the shared files are their union (a superset each earlier module's files compile with)
      if (clash.length > 0) {
        const shared = [...new Set(outcomes.filter((o) => o.accepted && o.files.some((f) => clash.includes(f))).flatMap((o) => o.declared ?? []))].sort();
        const merged = await judgeByRoot({ ...spec, shared }, judgeInput);
        if (merged.outcome.accepted) {
          judged = merged;
          clash = [];
        }
      }
      if (clash.length > 0) outcomes.push({ ...judged.outcome, accepted: false, files: [], emitted: [], refusals: [`${clash[0]} differs from another module's output`] });
      else {
        outcomes.push(judged.outcome);
        for (const [file, text] of judged.files) accepted.set(file, text);
        if (judged.registration) registrations.push(judged.registration);
      }
      const last = outcomes[outcomes.length - 1]!;
      console.log(`  ${last.module}: ${last.accepted ? `accepted, ${last.refusals.length} root(s) left open` : `refused (${last.refusals.length})`}`);
      totals = report();
    }
  }
  console.log(`convert ${feature}: ${ledger.units.length} units; in the game ${counts(ledger.units).exists}; translated ${totals.done}; open ${totals.open}; ` +
    `constants without a source ${totals.unsourced}; ${accepted.size} files -> ${out}`);
  process.exitCode = totals.open === 0 && totals.unsourced === 0 ? 0 : 1;
}
