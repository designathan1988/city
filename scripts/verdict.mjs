// THE VERDICT: a feature's trace (`converter feature`'s boundary.json) against the complete dynamic call graph of a
// run (scripts/oracle-calls.mjs over a copy instrumented by scripts/oracle-instrument.mjs). The feature is defined
// by its seed files only (the inventory's `sementes`), never by what the trace decided, so the ground truth is
// independent of the tool under test. A trace PASSES when every rule holds; one counterexample FAILS it.
//
// The trace's claim T = own ∪ sliced ∪ what own reaches over the graph's edges that run code (a call, a construction,
// an implementation, a callback stored or called, a load, inheritance, and a read of or reference to a function: a
// function value read is one that may be called).
//  R1 coverage  every unit of a seed file that ran is in T.
//  R2 extent    every unit of the program that ran while a frame of a seed file was on the stack (what the feature
//               ran, at any depth, through any file, through three.js calling back) is in T.
//  R3 entries   every way the run entered the feature from outside is known to the trace: the caller has an edge
//               to it in the graph, or is among the hooks; when the caller is three.js or the browser (a task, an
//               event, a frame callback), the trace names it as an entry from the platform (or as a hook).
//  R4 external  every three.js function the feature's code called directly is among the trace's external APIs.
// `node scripts/verdict.mjs <graph.json> <oracle dir> <features.json> <features out dir> [--strip-platform]
//   [--drop-own <fraction> --seed <n>] [--json <file>]`
//  --strip-platform and --drop-own are CONTROLS: they break the traces on purpose; the verdict must then FAIL.
import fs from 'node:fs';
import path from 'node:path';

const args = process.argv.slice(2);
const [graphFile, oracleDir, featuresFile, featuresOut] = args;
const flag = (n) => { const i = args.indexOf(n); return i >= 0 ? args[i + 1] : null; };
const stripPlatform = args.includes('--strip-platform');
const dropOwn = Number(flag('--drop-own') ?? 0);
let seed = Number(flag('--seed') ?? 1);
const rand = () => { seed = (seed * 1103515245 + 12345) & 0x7fffffff; return seed / 0x7fffffff; };

const ir = JSON.parse(fs.readFileSync(graphFile, 'utf8'));
const { features: bitNames, table } = JSON.parse(fs.readFileSync(path.join(oracleDir, 'ids.json'), 'utf8'));
const calls = JSON.parse(fs.readFileSync(path.join(oracleDir, 'calls.json'), 'utf8'));
// Each run function placed in the graph under test by its position (the table's unit is the graph of the run's time:
// a fix that moves a unit, as a class inside a function now belonging to it, must be judged on the graph it made).
{
  const byFileU = new Map();
  for (const u of ir.units) if (u.kind !== 'interface' && u.kind !== 'type') (byFileU.get(u.file) ?? byFileU.set(u.file, []).get(u.file)).push(u);
  const le = (a, l, c) => a.line < l || (a.line === l && a.column <= c);
  for (const t of table) {
    if (!t?.file || t.external || !t.unit || t.unit.startsWith('module:')) continue;
    let best = null;
    for (const u of byFileU.get(t.file) ?? []) if (le(u.start, t.line, t.column) && le({ line: t.line, column: t.column }, u.end.line, u.end.column) && (!best || le(best.start, u.start.line, u.start.column))) best = u;
    if (best) t.unit = best.id;
  }
}
const defs = JSON.parse(fs.readFileSync(featuresFile, 'utf8'));
const units = new Map(ir.units.map((u) => [u.id, u]));
const out = new Map();
for (const e of ir.edges) (out.get(e.from) ?? out.set(e.from, []).get(e.from)).push(e);
const children = new Map();
for (const u of ir.units) if (u.parent) (children.get(u.parent) ?? children.set(u.parent, []).get(u.parent)).push(u.id);
const topOf = new Map(); // file -> its top-level units (what runs on load)
for (const u of ir.units) if (!u.parent) (topOf.get(u.file) ?? topOf.set(u.file, []).get(u.file)).push(u.id);
const RUNS = new Set(['call', 'construct', 'implementation', 'callbackAssign', 'indirectCall', 'moduleLoad', 'inherit']);
const CALLABLE = new Set(['function', 'method', 'constructor', 'class', 'getter', 'setter']);

// the run: every edge of every realm, merged (ids are global to the server)
const edges = new Map();
for (const r of calls.realms) for (const [k, m] of r.edges) edges.set(k, (edges.get(k) ?? 0) | m);
const P = (k) => Math.floor(k / 262144);
const C = (k) => k % 262144;
const ran = new Set();
for (const k of edges.keys()) ran.add(C(k));

const results = [];
for (const def of defs) {
  const bit = 1 << bitNames.indexOf(def.name);
  const b = JSON.parse(fs.readFileSync(path.join(featuresOut, def.name, 'boundary.json'), 'utf8'));
  const seeds = new Set(def.seeds);
  let own = new Set(b.own);
  let platform = new Set((b.platform ?? []).map((p) => p.unit));
  if (stripPlatform) platform = new Set();
  const dropped = new Set();
  if (dropOwn > 0) for (const id of [...own].sort()) if (rand() < dropOwn) { own.delete(id); dropped.add(id); }
  const sliced = new Set(b.sliced);
  // T: own, sliced, and what own reaches over edges that run code (a dropped unit is out of T entirely)
  const T = new Set(own);
  const work = [...own];
  while (work.length) {
    const id = work.pop();
    const next = [...(out.get(id) ?? []).filter((e) => RUNS.has(e.kind) || ((e.kind === 'reference' || e.kind === 'read') && CALLABLE.has(units.get(e.to)?.kind))).map((e) => e.to), ...(children.get(id) ?? [])];
    for (const n of next) if (units.has(n) && !T.has(n) && !dropped.has(n)) { T.add(n); work.push(n); }
  }
  for (const id of sliced) if (!dropped.has(id)) T.add(id);
  // Exact: the unit itself is in T (the closure already holds every unit nested in what it reaches). Only a slice of
  // shared code covers what is nested in it: its nested units are not listed, the slice is cut from the whole.
  const inT = (unit) => { if (T.has(unit)) return true; for (let u = units.get(unit)?.parent ? units.get(units.get(unit).parent) : null; u; u = u.parent ? units.get(u.parent) : null) if (sliced.has(u.id) && !dropped.has(u.id)) return true; return false; };
  const hookUnits = new Set(b.hooks.map((h) => h.unit));
  const hookPairs = new Set(b.hooks.flatMap((h) => h.by.map((by) => `${by}>${h.unit}`)));
  const ancestors = (unit) => { const a = []; for (let u = units.get(unit); u; u = u.parent ? units.get(u.parent) : null) a.push(u.id); return a; };
  const graphEdge = (from, to) => {
    const fromAll = ancestors(from); // the caller's unit or a unit around it (an arrow inside a method)
    const toAll = new Set(ancestors(to));
    return fromAll.some((f) => (out.get(f) ?? []).some((e) => toAll.has(e.to)) || toAll.has(f) || [...toAll].some((t) => hookPairs.has(`${f}>${t}`)));
  };
  const extMembers = new Set([...b.external, ...(b.castExternal ?? [])].filter((x) => x.key.startsWith('three:')).flatMap((x) => { const k = x.key.slice(6); return [k, k.split('.').at(-1)]; }));

  const R1 = [], R2 = [], R3 = [], R4 = [];
  const seen = { seedUnitsRan: new Set(), extent: new Set(), entries: 0 };
  // R1: what ran in the seed files
  for (const id of ran) {
    const t = table[id];
    if (!t?.file || t.external || !seeds.has(t.file) || !t.unit || t.unit.startsWith('module:')) continue;
    seen.seedUnitsRan.add(t.unit);
    if (!inT(t.unit)) R1.push(`${t.unit} (${t.file}:${t.line})`);
  }
  for (const [k, m] of edges) {
    const p = table[P(k)], c = table[C(k)];
    if (!c) continue;
    const inFeature = (m & bit) !== 0;
    // R2: ran while the feature was on the stack
    if (inFeature && !c.external && c.unit && !c.unit.startsWith('module:')) {
      seen.extent.add(c.unit);
      if (!inT(c.unit)) R2.push(`${c.unit} (${c.file}:${c.line}) <- ${p?.unit ?? p?.name}`);
    }
    // R3: entered from outside
    const cSeed = !c.external && c.file && seeds.has(c.file) && c.unit && !c.unit.startsWith('module:');
    if (cSeed && !inFeature) {
      seen.entries++;
      let known;
      let how;
      if (P(k) === 0) { how = 'the browser (a task, an event, a frame)'; known = platform.has(c.unit) || hookUnits.has(c.unit) || ancestors(c.unit).some((a) => platform.has(a)); }
      else if (p.external) { how = `three.js ${p.name}`; known = platform.has(c.unit) || hookUnits.has(c.unit) || ancestors(c.unit).some((a) => platform.has(a)); }
      else if (p.unit?.startsWith('module:')) { const f = p.unit.slice(7); how = `loading ${f}`; known = (topOf.get(f) ?? []).some((top) => graphEdge(top, c.unit)); }
      // shared code calling back a function the feature gave it: known when the trace names that entry (`platform`, program:)
      else { how = p.unit; known = graphEdge(p.unit, c.unit) || hookUnits.has(c.unit) || ancestors(c.unit).some((x) => platform.has(x)); }
      if (!known) R3.push(`${c.unit} (${c.file}:${c.line}) entered by ${how}`);
    }
    // R4: three.js called by the feature's code
    if (c.external && p && !p.external && p.unit && own.has(p.unit)) {
      const member = c.name.split('.').at(-1);
      const cls = c.name.split('.')[0];
      if (c.name !== '(anonymous)' && !extMembers.has(member) && !(member === 'constructor' && extMembers.has(cls)) && !extMembers.has(c.name)) R4.push(`three:${c.name} <- ${p.unit}`);
    }
  }
  const uniq = (a) => [...new Set(a)].sort();
  const r = { feature: def.name, pass: R1.length + R2.length + R3.length + R4.length === 0,
    exercised: { seedUnitsRan: seen.seedUnitsRan.size, extentUnits: seen.extent.size, entries: seen.entries, ownDropped: dropped.size, droppedThatRan: [...dropped].filter((d) => seen.seedUnitsRan.has(d) || seen.extent.has(d)).length,
      droppedDetected: [...dropped].filter((d) => (seen.seedUnitsRan.has(d) || seen.extent.has(d)) && [...R1, ...R2].some((x) => x.startsWith(`${d} `))).length },
    R1_coverage: uniq(R1), R2_extent: uniq(R2), R3_entries: uniq(R3), R4_external: uniq(R4) };
  results.push(r);
}
const integrity = { pageErrors: calls.errors.length, stacksBackToRoot: calls.live.every((l) => !l.stack || (l.stack.length === 1 && l.stack[0] === 0)),
  unframed: table.filter((t) => t && t.framed === false).map((t) => t.name), edges: edges.size };
const verdict = { mode: stripPlatform ? 'CONTROL strip-platform' : dropOwn ? `CONTROL drop-own ${dropOwn}` : 'REAL', integrity,
  pass: results.every((r) => r.pass) && integrity.pageErrors === 0 && integrity.stacksBackToRoot, results };
const j = flag('--json');
if (j) fs.writeFileSync(j, JSON.stringify(verdict, null, 1));
console.log(`${verdict.mode}: ${verdict.pass ? 'PASS' : 'FAIL'} — edges ${integrity.edges}, page errors ${integrity.pageErrors}, stacks back to root ${integrity.stacksBackToRoot}, unframed ${integrity.unframed.length}`);
for (const r of results) {
  const n = (a) => a.length;
  console.log(`  ${r.pass ? 'PASS' : 'FAIL'} ${r.feature.padEnd(17)} ran: ${String(r.exercised.seedUnitsRan).padStart(4)} seed units, extent ${String(r.exercised.extentUnits).padStart(4)}, entries ${String(r.exercised.entries).padStart(4)}` +
    `${dropOwn ? `, dropped ${r.exercised.ownDropped} (${r.exercised.droppedThatRan} ran, ${r.exercised.droppedDetected} caught)` : ''} | R1 ${n(r.R1_coverage)} R2 ${n(r.R2_extent)} R3 ${n(r.R3_entries)} R4 ${n(r.R4_external)}`);
  for (const [k, v] of [['R1', r.R1_coverage], ['R2', r.R2_extent], ['R3', r.R3_entries], ['R4', r.R4_external]]) for (const x of v.slice(0, 4)) console.log(`      ${k} ${x}`);
}
