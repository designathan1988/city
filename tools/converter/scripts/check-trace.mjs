// A trace (`converter feature`'s boundary.json, over the whole program's graph) judged by what the program did
// (`scripts/oracle.mjs`): every function V8 ran is placed in the graph's innermost unit through the dev server's
// source maps, and
//  A. coverage: every unit of the feature's files that ran is in the trace;
//  B. stacks: everything the feature called (sampled stacks are real call paths) is in what the trace reaches from
//     the feature, directly or through three.js calling back; every way the program entered the feature (directly,
//     or three.js calling a callback of the feature) is known to the trace;
//  C. three.js: every three.js function the feature called directly is in its external APIs;
//  D. code made from strings and shaders: where they came from.
// `node scripts/check-trace.mjs <graph.json> <boundary.json> <oracle dir> <node_modules dir> [--json out]`
import fs from 'node:fs';
import path from 'node:path';
import { createRequire } from 'node:module';

const [graphFile, boundaryFile, oracle, nodeModules] = process.argv.slice(2);
const { TraceMap, originalPositionFor } = createRequire(path.join(nodeModules, '@jridgewell', 'trace-mapping', 'package.json'))('@jridgewell/trace-mapping');
const ir = JSON.parse(fs.readFileSync(graphFile, 'utf8'));
const b = JSON.parse(fs.readFileSync(boundaryFile, 'utf8'));
const coverage = JSON.parse(fs.readFileSync(path.join(oracle, 'coverage.json'), 'utf8'));
const profile = JSON.parse(fs.readFileSync(path.join(oracle, 'profile.json'), 'utf8'));
const sources = JSON.parse(fs.readFileSync(path.join(oracle, 'sources.json'), 'utf8'));
const strings = JSON.parse(fs.readFileSync(path.join(oracle, 'strings.json'), 'utf8'));
const shaders = JSON.parse(fs.readFileSync(path.join(oracle, 'shaders.json'), 'utf8'));

// ---- the graph: units by file, sorted for the innermost lookup; edges out
const units = new Map(ir.units.map((u) => [u.id, u]));
const byFile = new Map();
for (const u of ir.units) (byFile.get(u.file) ?? byFile.set(u.file, []).get(u.file)).push(u);
const before = (a, l, c) => a.line < l || (a.line === l && a.column <= c);
const innermost = (file, line, column) => {
  let best = null;
  for (const u of byFile.get(file) ?? []) {
    if (u.kind === 'interface' || u.kind === 'type') continue;
    if (before(u.start, line, column) && before({ line, column }, u.end.line, u.end.column)
      && (!best || before(best.start, u.start.line, u.start.column))) best = u;
  }
  return best;
};
const out = new Map();
for (const e of ir.edges) if (e.kind !== 'typeRef') (out.get(e.from) ?? out.set(e.from, []).get(e.from)).push(e);
const into = new Map();
for (const e of ir.edges) (into.get(e.to) ?? into.set(e.to, []).get(e.to)).push(e);

// ---- the trace: own, sliced, and what it reaches (own and slices, forward over the graph, to a fixpoint)
const own = new Set(b.own);
const sliced = new Set(b.sliced);
// What the feature can reach: forward from its own units. A slice of shared code is not a start: most of the
// declaration it is cut from is not the feature's (a slice of `createSceneRenderer` reaches nearly the whole program).
const reach = new Set(own);
const work = [...reach];
// a unit's nested units run as part of it
const children = new Map();
for (const u of ir.units) if (u.parent) (children.get(u.parent) ?? children.set(u.parent, []).get(u.parent)).push(u.id);
while (work.length) {
  const id = work.pop();
  for (const n of [...(out.get(id) ?? []).map((e) => e.to), ...(children.get(id) ?? [])]) if (units.has(n) && !reach.has(n)) { reach.add(n); work.push(n); }
}
// The stricter reach for stacks: only edges that run code (calls, constructions, implementations, callbacks stored,
// indirect calls, module loads, inheritance, and a reference to something callable); a read of shared state does not
// make its whole owner part of the feature.
const RUNS = new Set(['call', 'construct', 'implementation', 'callbackAssign', 'indirectCall', 'moduleLoad', 'inherit']);
const CALLABLE = new Set(['function', 'method', 'constructor', 'class', 'getter', 'setter']);
const runs = new Set(own);
const work2 = [...runs];
while (work2.length) {
  const id = work2.pop();
  const next = [...(out.get(id) ?? []).filter((e) => RUNS.has(e.kind) || (e.kind === 'reference' && CALLABLE.has(units.get(e.to)?.kind))).map((e) => e.to), ...(children.get(id) ?? [])];
  for (const n of next) if (units.has(n) && !runs.has(n)) { runs.add(n); work2.push(n); }
}
const featureFiles = new Set([...b.entries, ...[...own].map((id) => units.get(id).file)]);
const ownOrInside = (id) => { for (let u = units.get(id); u; u = u.parent ? units.get(u.parent) : null) if (own.has(u.id)) return true; return false; };

// ---- runtime positions -> graph units, through each served module's inline source map
const maps = new Map();
const srcRel = (url) => { const m = /^https?:\/\/[^/]+\/(src\/[^?#]+)/.exec(url); return m ? decodeURIComponent(m[1]) : null; };
const textByUrl = new Map(Object.values(sources).map((s) => [s.url, s.text]));
const mapOf = (url) => {
  if (maps.has(url)) return maps.get(url);
  const text = textByUrl.get(url);
  let tm = null;
  const m = text && /\/\/# sourceMappingURL=data:application\/json;(?:charset=utf-8;)?base64,([A-Za-z0-9+/=]+)\s*$/.exec(text);
  if (m) tm = new TraceMap(JSON.parse(Buffer.from(m[1], 'base64').toString('utf8')));
  const starts = [0];
  if (text) for (let i = 0; i < text.length; i++) if (text.charCodeAt(i) === 10) starts.push(i + 1);
  const v = { tm, starts };
  maps.set(url, v);
  return v;
};
const unitAt = (url, line0, col0) => { // generated 0-based line/column
  const rel = srcRel(url);
  if (!rel) return null;
  const { tm } = mapOf(url);
  let line = line0 + 1, column = col0 + 1;
  if (tm) {
    const o = originalPositionFor(tm, { line: line0 + 1, column: col0 });
    if (o.line == null) return { unmapped: `${rel}@${line0 + 1}:${col0 + 1}` };
    line = o.line; column = o.column + 1;
  }
  const u = innermost(rel, line, column);
  return u ? { unit: u.id, file: rel, line } : { unmapped: `${rel}:${line}:${column} (no unit)` };
};
const unitAtOffset = (url, offset) => {
  const { starts } = mapOf(url);
  let lo = 0, hi = starts.length - 1;
  while (lo < hi) { const m = (lo + hi + 1) >> 1; if (starts[m] <= offset) lo = m; else hi = m - 1; }
  return unitAt(url, lo, offset - starts[lo]);
};

// ---- A. coverage
const ran = new Map(); // unit -> count
const unmapped = [];
for (const s of coverage) {
  if (!srcRel(s.url) || !textByUrl.has(s.url)) continue;
  for (const f of s.functions) {
    const r = f.ranges[0];
    if (r.count === 0 || r.startOffset === 0) continue;
    const at = unitAtOffset(s.url, r.startOffset);
    if (!at) continue;
    if (at.unmapped) { unmapped.push(at.unmapped); continue; }
    ran.set(at.unit, (ran.get(at.unit) ?? 0) + r.count);
  }
}
const entryFiles = new Set(b.entries);
const ranInFeatureFiles = [...ran.keys()].filter((id) => entryFiles.has(units.get(id).file));
const coverageMissing = ranInFeatureFiles.filter((id) => !reach.has(id) && !sliced.has(id));

// ---- B. stacks
const nodes = new Map(profile.nodes.map((n) => [n.id, n]));
const parent = new Map();
for (const n of profile.nodes) for (const c of n.children ?? []) parent.set(c, n.id);
const frameOf = new Map();
const classify = (n) => {
  if (frameOf.has(n.id)) return frameOf.get(n.id);
  const cf = n.callFrame;
  let f;
  if (/\/node_modules\/(\.vite[^/]*\/deps\/)?three/.test(cf.url) || /\/deps\/three/.test(cf.url)) f = { three: cf.functionName || '(anonymous)' };
  else if (srcRel(cf.url)) { const at = unitAt(cf.url, cf.lineNumber, cf.columnNumber); f = at?.unit ? { unit: at.unit } : { other: at?.unmapped ?? cf.url }; }
  else f = { other: cf.url || cf.functionName };
  frameOf.set(n.id, f);
  return f;
};
const calledMissing = new Map(); // unit -> example path
const calledShared = new Set(); // shared units seen running under a feature frame
const calledViaThree = new Map();
const entries = new Map(); // feature unit -> { direct: Set(callers), viaThree: Set(three fns) }
const threeCalled = new Map(); // three function -> count, called directly by a feature frame
for (const n of profile.nodes) {
  const f = classify(n);
  if (!f.unit) continue;
  // the path from the root
  const chain = [];
  for (let id = n.id; id !== undefined; id = parent.get(id)) chain.unshift(classify(nodes.get(id)));
  const i = chain.findIndex((x) => x.unit && ownOrInside(x.unit));
  if (i < 0) continue;
  // entering the feature: the first feature frame of the chain, and what is right before it
  const first = chain[i].unit;
  let k = i - 1;
  const throughThree = [];
  while (k >= 0 && !chain[k].unit) { if (chain[k].three) throughThree.unshift(chain[k].three); k--; }
  const e = entries.get(first) ?? entries.set(first, { direct: new Set(), viaThree: new Set() }).get(first);
  if (throughThree.length > 0) e.viaThree.add(`${chain[k]?.unit ?? '(root)'} -> three:${throughThree.join('>')}`);
  else if (k >= 0) e.direct.add(chain[k].unit);
  // what the feature called: game frames below a feature frame
  if (chain.length - 1 > i && f.unit !== first && !ownOrInside(f.unit)) calledShared.add(f.unit);
  if (chain.length - 1 > i && f.unit !== first && !ownOrInside(f.unit) && !runs.has(f.unit)) {
    const prev = chain.at(-2);
    (prev.three ? calledViaThree : calledMissing).set(f.unit, chain.slice(i).map((x) => x.unit ?? `three:${x.three ?? x.other}`).join(' > '));
  }
  // three.js called by the feature
}
for (const n of profile.nodes) {
  const f = classify(n);
  const p = parent.get(n.id);
  if (!f.three || p === undefined) continue;
  const pf = classify(nodes.get(p));
  if (pf.unit && ownOrInside(pf.unit)) threeCalled.set(f.three, (threeCalled.get(f.three) ?? 0) + 1);
}
const hookBy = new Set(b.hooks.flatMap((h) => h.by));
const platformUnits = new Set((b.platform ?? []).map((p) => p.unit));
const entryReport = [...entries].map(([unit, e]) => ({
  unit,
  direct: [...e.direct].map((c) => ({ caller: c, known: hookBy.has(c) || reach.has(c) || (out.get(c) ?? []).some((x) => x.to === unit) })),
  viaThree: [...e.viaThree],
  installedBy: (into.get(unit) ?? []).filter((x) => x.kind !== 'typeRef').map((x) => `${x.kind}:${x.from}`),
}));
// C. three.js names in the external APIs: `three:Class.member` or `three:Class`
const externalNames = new Set(b.external.filter((x) => x.key.startsWith('three:')).flatMap((x) => { const k = x.key.slice(6); return [k, k.split('.').at(-1)]; }));
const threeMissing = [...threeCalled].filter(([fn]) => fn !== '(anonymous)' && !externalNames.has(fn));

// D. strings and shaders
const fromFeature = (stack) => [...featureFiles].some((f) => stack.includes(`/${f}`));
const report = {
  trace: { own: own.size, sliced: sliced.size, reach: reach.size, runs: runs.size, featureFiles: [...featureFiles] },
  runtime: { unitsRan: ran.size, unmappedFunctions: unmapped.length, featureUnitsRan: ranInFeatureFiles.length },
  A_coverage_missing: coverageMissing,
  B_called_shared_seen: calledShared.size,
  B_called_missing: Object.fromEntries(calledMissing),
  unmapped,
  B_called_via_three_missing: Object.fromEntries(calledViaThree),
  B_entries: entryReport,
  // entered through three.js: the trace must say the platform calls it (boundary.platform, src/feature/platform.ts)
  B_entries_unknown: entryReport.filter((e) => e.direct.some((d) => !d.known) || (e.viaThree.length > 0 && !platformUnits.has(e.unit))).map((e) => e.unit),
  C_three_called: threeCalled.size,
  C_three_missing: threeMissing,
  D_code_from_strings: strings.length, D_code_from_strings_by_feature: strings.filter((s) => fromFeature(s.stack)).length,
  D_shaders: shaders.length,
};
const flag = process.argv.indexOf('--json');
if (flag > 0) fs.writeFileSync(process.argv[flag + 1], JSON.stringify(report, null, 1));
console.log(JSON.stringify({ ...report, B_entries: `${entryReport.length} feature units entered`, unmappedSample: unmapped.slice(0, 5) }, null, 1));
