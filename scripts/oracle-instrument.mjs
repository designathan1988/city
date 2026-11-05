// The definitive oracle's instrumentation: a Vite plugin for a COPY of the 4.0 (never the source), which rewrites every
// function of the game (src/**) and of three.js so that each entry and exit is recorded on a shadow stack, in the page
// and in every worker. What it records is the complete dynamic call graph of the run (not a sample): for every edge
// caller -> callee that happened, the set of features that had a frame on the stack when it happened.
//
// - Every function with a body: `{ const __f = __O.e(id, mask); try { ...body } finally { __O.x(id, mask) } }`.
// - An expression-bodied arrow becomes a block returning the expression.
// - Async functions and generators leave the stack at each `await`/`yield` and come back after it, so a frame never
//   outlives its suspension: `await X` -> `__O.w(await __O.y(X, id, mask), id, mask)`.
// - A derived constructor starts its frame after its top-level `super(...)` (fields are set there).
// - A module's top level is a frame of its own (`module:<file>`), so code run on load has a caller.
// - What cannot be framed exactly (a `for await` loop, a `super` call not at the top) is only recorded (`r`): it is
//   listed in the table as `framed: false`, and edges below it are attributed to its caller.
// The table of ids (file, line, column, the graph's unit, the feature mask) is served at /__oracle/ids.
import fs from 'node:fs';
import path from 'node:path';
import { createRequire } from 'node:module';

const RUNTIME = `const __O=globalThis.__oracle||(globalThis.__oracle=(()=>{const st=[0];const cnt=new Int32Array(31);let mask=0;const edges=new Map();
const realm=(typeof location!=='undefined'?String(location.pathname):'?')+'#'+Math.random().toString(36).slice(2,8);
let pending=[],queued=false;const sync=typeof window==='undefined';
const flush=()=>{queued=false;if(pending.length===0)return;const body=JSON.stringify({realm,edges:pending});pending=[];
try{if(sync){const x=new XMLHttpRequest();x.open('POST','/__oracle/push',false);x.send(body);}else fetch('/__oracle/push',{method:'POST',body,keepalive:body.length<60000}).catch(()=>{});}catch(_){}};
const rec=(c)=>{const k=st[st.length-1]*262144+c;const v=edges.get(k);const n=v===undefined?mask:(v|mask);if(n!==v){edges.set(k,n);pending.push(k,n);if(!queued){queued=true;queueMicrotask(flush);}}};
const push=(c,m)=>{st.push(c);if(m)for(let b=0;b<31;b++)if(m&(1<<b)){if(cnt[b]++===0)mask|=(1<<b);}};
const pop=(c,m)=>{if(st[st.length-1]!==c)return;st.pop();if(m)for(let b=0;b<31;b++)if(m&(1<<b)){if(--cnt[b]===0)mask&=~(1<<b);}};
return{edges,st,e(c,m){rec(c);push(c,m);},x(c,m){pop(c,m);},r(c){rec(c);},y(v,c,m){pop(c,m);return v;},w(v,c,m){push(c,m);return v;},v(c,m,f){push(c,m);try{return f();}finally{pop(c,m);}}};})());\n`;

export function oracleInstrument({ root, graph, boundaries }) {
  const req = createRequire(path.join(path.resolve(root), 'package.json'));
  const ts = req('typescript');
  const MS = req('magic-string');
  const MagicString = MS.default ?? MS;
  const norm = (p) => p.replaceAll('\\', '/');
  const rootNorm = norm(path.resolve(root));
  // the graph's units, by file, for the innermost unit of a position
  const ir = JSON.parse(fs.readFileSync(graph, 'utf8'));
    const byFile = new Map();
  for (const u of ir.units) if (u.kind !== 'interface' && u.kind !== 'type') (byFile.get(u.file) ?? byFile.set(u.file, []).get(u.file)).push(u);
  const before = (a, l, c) => a.line < l || (a.line === l && a.column <= c);
  const innermost = (file, line, column) => {
    let best = null;
    for (const u of byFile.get(file) ?? []) if (before(u.start, line, column) && before({ line, column }, u.end.line, u.end.column) && (!best || before(best.start, u.start.line, u.start.column))) best = u;
    return best;
  };
  // features: bit i is boundaries[i], by its seed files
  const features = boundaries.map((b) => ({ name: b.name, entries: new Set(b.seeds) }));
  if (features.length > 31) throw new Error("at most 31 features");
  const fileMask = (rel) => { let m = 0; features.forEach((f, i) => { if (f.entries.has(rel)) m |= 1 << i; }); return m; };
  const table = [{ file: null, name: '(root: a task the browser started)' }];

  return {
    name: 'oracle-instrument',
    enforce: 'pre',
    configureServer(server) {
      server.middlewares.use('/__oracle/ids', (_req, res) => { res.setHeader('content-type', 'application/json'); res.end(JSON.stringify({ features: features.map((f) => f.name), table })); });
      // every realm's new edges as they happen (a worker the game ends takes nothing with it)
      const realms = new Map();
      server.middlewares.use('/__oracle/push', (req, res) => {
        const chunks = [];
        req.on('data', (c) => chunks.push(c));
        req.on('end', () => {
          try {
            const { realm, edges } = JSON.parse(Buffer.concat(chunks).toString('utf8'));
            const m = realms.get(realm) ?? realms.set(realm, new Map()).get(realm);
            for (let i = 0; i < edges.length; i += 2) m.set(edges[i], (m.get(edges[i]) ?? 0) | edges[i + 1]);
          } catch { /* a cut request */ }
          res.statusCode = 204; res.end();
        });
      });
      server.middlewares.use('/__oracle/edges', (_req, res) => { res.setHeader('content-type', 'application/json'); res.end(JSON.stringify([...realms].map(([realm, m]) => ({ realm, edges: [...m] })))); });
    },
    transform(code, id) {
      const clean = norm(id.split('?')[0]);
      let rel;
      let external = false;
      const at3 = clean.indexOf('/node_modules/three/');
      if (clean.startsWith(rootNorm + '/src/') && /\.(ts|tsx|js|mjs)$/.test(clean) && !clean.endsWith('.d.ts')) rel = clean.slice(rootNorm.length + 1);
      else if (at3 >= 0 && /\.(js|mjs)$/.test(clean)) { rel = 'three/' + clean.slice(at3 + '/node_modules/three/'.length); external = true; }
      else return null;
      const kind = clean.endsWith('.tsx') ? ts.ScriptKind.TSX : clean.endsWith('.ts') ? ts.ScriptKind.TS : ts.ScriptKind.JS;
      const sf = ts.createSourceFile(clean, code, ts.ScriptTarget.Latest, true, kind);
      const s = new MagicString(code);
      const lc = (pos) => { const p = sf.getLineAndCharacterOfPosition(pos); return { line: p.line + 1, column: p.character + 1 }; };
      const nameOf = (n) => {
        const own = n.name && (ts.isIdentifier(n.name) || ts.isStringLiteral(n.name) || ts.isPrivateIdentifier(n.name)) ? n.name.text : ts.isConstructorDeclaration(n) ? 'constructor' : null;
        const cls = ts.isClassLike(n.parent) && n.parent.name ? n.parent.name.text : null;
        const v = !own && n.parent && ts.isVariableDeclaration(n.parent) && ts.isIdentifier(n.parent.name) ? n.parent.name.text : null;
        const p = !own && n.parent && ts.isPropertyAssignment(n.parent) && ts.isIdentifier(n.parent.name) ? n.parent.name.text : null;
        const name = own ?? v ?? p ?? '(anonymous)';
        return cls ? `${cls}.${name}` : name;
      };
      const register = (n, bodyPos, framed) => {
        const at = lc(bodyPos);
        const unit = external ? null : innermost(rel, at.line, at.column);
        // The feature is what its seed files are (the inventory's definition, not the trace's): a frame of a seed file
        // raises the feature's bit, so the run's ground truth never depends on what the trace under test decided.
        const mask = external ? 0 : fileMask(rel);
        const fid = table.length;
        const start = lc(n.getStart(sf));
        table.push({ file: rel, line: start.line, column: start.column, unit: unit?.id ?? null, mask, name: nameOf(n), external, framed });
        return { fid, mask };
      };
      // the awaits and yields of a function's own body (not of the functions inside it)
      const suspensions = (fn) => {
        const found = [];
        let forAwait = false;
        const visit = (n) => {
          if (n !== fn && ts.isFunctionLike(n)) return;
          if (ts.isAwaitExpression(n) || ts.isYieldExpression(n)) found.push(n);
          if (ts.isForOfStatement(n) && n.awaitModifier) forAwait = true;
          ts.forEachChild(n, visit);
        };
        ts.forEachChild(fn, visit);
        return { found, forAwait };
      };
      // Post-order: a function inside is rewritten first, so where both insert at one place (an arrow's body that is
      // another arrow, a body that ends where an inner one ends) the inner's text comes first and closes first.
      const visit = (n) => {
        ts.forEachChild(n, visit);
        // a field's initializer and a static block run outside any function body: frames of their own (the field's unit)
        if (ts.isPropertyDeclaration(n) && n.initializer && ts.isClassLike(n.parent) && !ts.isArrowFunction(n.initializer) && !ts.isFunctionExpression(n.initializer)) {
          const { fid, mask } = register(n, n.initializer.getStart(sf), true);
          s.appendLeft(n.initializer.getStart(sf), `__O.v(${fid},${mask},()=>(`);
          s.appendRight(n.initializer.end, '))');
        }
        if (ts.isClassStaticBlockDeclaration(n)) {
          const { fid, mask } = register(n, n.body.getStart(sf) + 1, true);
          s.appendLeft(n.body.getStart(sf) + 1, `__O.e(${fid},${mask});try{`);
          s.appendRight(n.body.end - 1, `}finally{__O.x(${fid},${mask})}`);
        }
        if (ts.isFunctionLike(n) && n.body && !(n.flags & ts.NodeFlags.Ambient)) {
          const body = n.body;
          const isAsync = (ts.getCombinedModifierFlags(n) & ts.ModifierFlags.Async) !== 0;
          const isGen = !!n.asteriskToken;
          const { found, forAwait } = isAsync || isGen ? suspensions(n) : { found: [], forAwait: false };
          let framed = !forAwait;
          let openAt = null; // where `try{` goes (after `{`, or after a derived constructor's super call)
          let derivedOpen = null; // a derived constructor: its frame opens at `{`, before super (its arguments and the base's constructor are its own)
          if (ts.isBlock(body)) {
            openAt = body.getStart(sf) + 1;
            if (ts.isConstructorDeclaration(n) && ts.isClassLike(n.parent) && (n.parent.heritageClauses ?? []).some((h) => h.token === ts.SyntaxKind.ExtendsKeyword)) {
              const sup = body.statements.find((st) => ts.isExpressionStatement(st) && ts.isCallExpression(st.expression) && st.expression.expression.kind === ts.SyntaxKind.SuperKeyword);
              if (sup) { openAt = sup.end; derivedOpen = body.getStart(sf) + 1; } else framed = false;
            }
          }
          const { fid, mask } = register(n, ts.isBlock(body) ? body.getStart(sf) + 1 : body.getStart(sf), framed);
          // openings, then the suspensions, then the closing: at one place, what opens first closes last
          if (!framed) {
            if (ts.isBlock(body)) s.appendLeft(body.getStart(sf) + 1, `__O.r(${fid});`);
            else s.appendLeft(body.getStart(sf), `(__O.r(${fid}),(`);
          } else if (ts.isBlock(body) && derivedOpen !== null) { s.appendLeft(derivedOpen, `__O.e(${fid},${mask});`); s.appendLeft(openAt, 'try{'); }
          else if (ts.isBlock(body)) s.appendLeft(openAt, `__O.e(${fid},${mask});try{`);
          else s.appendLeft(body.getStart(sf), `{__O.e(${fid},${mask});try{return(`);
          // parameter initializers run before the body: in the function's frame
          if (framed) {
            const inits = [];
            const pattern = (b) => { if (ts.isObjectBindingPattern(b) || ts.isArrayBindingPattern(b)) { for (const e of b.elements) { if (ts.isOmittedExpression(e)) continue; if (e.initializer) inits.push(e.initializer); if (!ts.isIdentifier(e.name)) pattern(e.name); } } };
            for (const p of n.parameters) { if (p.initializer) inits.push(p.initializer); if (!ts.isIdentifier(p.name)) pattern(p.name); }
            for (const init of inits) { s.appendLeft(init.getStart(sf), `__O.v(${fid},${mask},()=>(`); s.appendRight(init.end, '))'); }
          }
          if (framed) for (const a of found) {
            if (ts.isYieldExpression(a) && !a.expression) { s.appendLeft(a.getStart(sf), `__O.w((__O.y(0,${fid},${mask}),`); s.appendRight(a.end, `),${fid},${mask})`); continue; }
            s.appendLeft(a.getStart(sf), '__O.w(');
            s.appendLeft(a.expression.getStart(sf), '__O.y(');
            s.appendRight(a.expression.end, `,${fid},${mask})`);
            s.appendRight(a.end, `,${fid},${mask})`);
          }
          if (!framed) { if (!ts.isBlock(body)) s.appendRight(body.end, '))'); }
          else if (ts.isBlock(body)) s.appendRight(body.end - 1, `}finally{__O.x(${fid},${mask})}`);
          else s.appendRight(body.end, `)}finally{__O.x(${fid},${mask})}}`);
        }
      };
      ts.forEachChild(sf, visit);
      // the module's top level: a frame (unless it awaits at the top)
      // a module awaiting at its top is framed like an async function (it leaves the stack at each await)
      const topAwaits = [];
      let topForAwait = false;
      { const v = (n) => { if (ts.isFunctionLike(n) || ts.isClassLike(n)) return; if (ts.isAwaitExpression(n)) topAwaits.push(n); if (ts.isForOfStatement(n) && n.awaitModifier) topForAwait = true; ts.forEachChild(n, v); }; ts.forEachChild(sf, v); }
      const topAwait = topForAwait;
      const mid = table.length;
      const mmask = external ? 0 : fileMask(rel);
      table.push({ file: rel, line: 1, column: 1, unit: `module:${rel}`, mask: mmask, name: `module:${rel}`, external, framed: !topAwait });
      if (!topAwait) for (const a of topAwaits) { s.appendLeft(a.getStart(sf), '__O.w('); s.appendLeft(a.expression.getStart(sf), '__O.y('); s.appendRight(a.expression.end, `,${mid},${mmask})`); s.appendRight(a.end, `,${mid},${mmask})`); }
      s.prepend(RUNTIME + (topAwait ? `__O.r(${mid});\n` : `__O.e(${mid},${mmask});\n`));
      if (!topAwait) s.append(`\n;__O.x(${mid},${mmask});\n`);
      return { code: s.toString(), map: s.generateMap({ hires: 'boundary', source: clean, includeContent: true }) };
    },
  };
}
