// The TypeScript and JavaScript facts, by the language's own compiler and type checker. Reads a job file, writes one JSON file.
'use strict';
const fs = require('fs');
const path = require('path');

const job = JSON.parse(fs.readFileSync(process.argv[2], 'utf8'));
const ts = require(job.typescript);
const root = job.root.replace(/\\/g, '/');
const rel = (f) => f.replace(/\\/g, '/').slice(root.length + 1);
const scope = new Set(job.files);

let options = { allowJs: true, checkJs: false, noEmit: true, skipLibCheck: true, target: ts.ScriptTarget.ESNext, module: ts.ModuleKind.ESNext, moduleResolution: ts.ModuleResolutionKind.Bundler, jsx: ts.JsxEmit.Preserve, allowImportingTsExtensions: true };
if (job.tsconfig) {
  const cfg = ts.parseJsonConfigFileContent(ts.readConfigFile(job.tsconfig, ts.sys.readFile).config, ts.sys, path.dirname(job.tsconfig));
  options = { ...cfg.options, allowJs: true, noEmit: true, skipLibCheck: true };
}
const program = ts.createProgram(job.files.map((f) => `${root}/${f}`), options);
const checker = program.getTypeChecker();
const out = { units: [], edges: [], refs: [], defs: [], sites: [], costs: [] };
const unitOfNode = new Map();
const used = new Set();

const nameOf = (n) => {
  if (ts.isConstructorDeclaration(n)) return 'constructor';
  if (n.name && (ts.isIdentifier(n.name) || ts.isStringLiteral(n.name) || ts.isPrivateIdentifier(n.name) || ts.isNumericLiteral(n.name))) return n.name.text;
  return null;
};
const kindOf = (n) => {
  if (ts.isClassLike(n)) return 'class';
  if (ts.isInterfaceDeclaration(n) || ts.isTypeAliasDeclaration(n)) return 'alias';
  if (ts.isEnumDeclaration(n)) return 'enum';
  if (ts.isEnumMember(n)) return 'enumerator';
  if (ts.isPropertyDeclaration(n) || ts.isPropertySignature(n)) return 'field';
  if (ts.isVariableDeclaration(n)) return n.initializer && (ts.isArrowFunction(n.initializer) || ts.isFunctionExpression(n.initializer)) ? 'function' : 'global';
  if (ts.isMethodDeclaration(n) || ts.isConstructorDeclaration(n) || ts.isGetAccessorDeclaration(n) || ts.isSetAccessorDeclaration(n)) return 'method';
  if (ts.isFunctionDeclaration(n)) return 'function';
  return null;
};
const shapeOf = (n, sf) => n.getText(sf).replace(/\/\/[^\n]*|\/\*[\s\S]*?\*\//g, '').replace(/"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'|`(?:\\.|[^`\\])*`/g, 's').replace(/\b\d[\w.]*\b/g, '0').replace(/[A-Za-z_$][\w$]*/g, 'a').replace(/\s+/g, '');

function declare(sf, file) {
  const stack = [`ts:${file}`];
  const visit = (n) => {
    const kind = kindOf(n);
    const name = kind ? nameOf(n) : null;
    let pushed = false;
    if (kind && name !== null && !(ts.isVariableDeclaration(n) && kind === 'global' && stack.length > 1 && !ts.isSourceFile(n.parent.parent.parent))) {
      let id = `${stack[stack.length - 1]}#${name}`;
      while (used.has(id)) id += "'";
      used.add(id);
      const a = sf.getLineAndCharacterOfPosition(n.getStart(sf)).line + 1;
      const b = sf.getLineAndCharacterOfPosition(n.end).line + 1;
      out.units.push([id, kind, name, file, a, b, stack[stack.length - 1], shapeOf(n, sf), { statements: n.getText(sf).split(';').length }]);
      out.defs.push(['method', name, id]);
      unitOfNode.set(n, id);
      stack.push(id);
      pushed = true;
    }
    ts.forEachChild(n, visit);
    if (pushed) stack.pop();
  };
  ts.forEachChild(sf, visit);
}

function ownerOf(n, file) {
  for (let p = n; p; p = p.parent) { const k = unitOfNode.get(p); if (k) return k; }
  return `ts:${file}`;
}
function target(symbol) {
  if (!symbol) return null;
  const s = symbol.flags & ts.SymbolFlags.Alias ? checker.getAliasedSymbol(symbol) : symbol;
  for (const d of s.declarations || []) {
    const k = unitOfNode.get(d);
    if (k) return k;
    const sf = d.getSourceFile();
    if (!scope.has(rel(sf.fileName))) return `ext:${(d.parent && d.parent.name && d.parent.name.getText ? d.parent.name.getText() : '')}.${s.name}`;
  }
  return null;
}
const written = (n) => {
  const p = n.parent;
  return ts.isBinaryExpression(p) && p.left === n && p.operatorToken.kind >= ts.SyntaxKind.FirstAssignment && p.operatorToken.kind <= ts.SyntaxKind.LastAssignment;
};

function walk(sf, file) {
  const at = (n) => sf.getLineAndCharacterOfPosition(n.getStart(sf));
  let depth = 0;
  const visit = (n) => {
    const line = at(n).line + 1;
    const here = ownerOf(n, file);
    if (ts.isCallExpression(n) || ts.isNewExpression(n)) {
      const callee = n.expression;
      const id = ts.isPropertyAccessExpression(callee) ? callee.name : callee;
      out.sites.push([file, line, at(id).character + 1, 'call']);
      const sym = checker.getSymbolAtLocation(id);
      const to = target(sym);
      const kind = ts.isNewExpression(n) ? 'construct' : 'call';
      if (to) out.edges.push([here, kind, to, file, line, 'types', { depth, locks: [] }]);
      else out.edges.push([here, kind, `name:method:${id.getText(sf)}`, file, line, 'name', { depth, locks: [] }]);
      if (to && to.startsWith('ext:')) out.costs.push([here, 'boundary', file, line, depth, [], { api: to.slice(4) }]);
      for (const a of n.arguments || []) if (ts.isArrowFunction(a) || ts.isFunctionExpression(a)) out.edges.push([here, 'callable', `name:method:${id.getText(sf)}`, file, line, 'types', { via: id.getText(sf) }]);
    } else if (ts.isPropertyAccessExpression(n) && !(ts.isCallExpression(n.parent) && n.parent.expression === n)) {
      const to = target(checker.getSymbolAtLocation(n.name));
      if (to) out.edges.push([here, written(n) ? 'write' : 'read', to, file, line, 'types', { depth, locks: [] }]);
      out.sites.push([file, line, at(n.name).character + 1, 'access']);
    } else if (ts.isStringLiteralLike(n) && !ts.isImportDeclaration(n.parent) && !ts.isExportDeclaration(n.parent)) {
      out.refs.push([null, n.text, here, file, line, 'text']);
      out.sites.push([file, line, at(n).character + 1, 'string']);
    } else if (ts.isImportDeclaration(n) || ts.isExportDeclaration(n)) {
      if (n.moduleSpecifier) {
        const s = checker.getSymbolAtLocation(n.moduleSpecifier);
        const d = s && s.valueDeclaration;
        const f = d && ts.isSourceFile(d) ? rel(d.fileName) : null;
        out.edges.push([`ts:${file}`, 'import', f && scope.has(f) ? `ts:${f}` : `ext:${n.moduleSpecifier.text}`, file, line, 'types', {}]);
        out.sites.push([file, line, 1, 'import']);
      }
    } else if (ts.isHeritageClause(n)) {
      for (const t of n.types) { const to = target(checker.getSymbolAtLocation(t.expression)); if (to) out.edges.push([here, 'inherit', to, file, line, 'types', {}]); }
    } else if (ts.isAwaitExpression(n)) {
      out.costs.push([here, 'await', file, line, depth, [], {}]);
    }
    const loop = ts.isForStatement(n) || ts.isForOfStatement(n) || ts.isForInStatement(n) || ts.isWhileStatement(n) || ts.isDoStatement(n);
    if (loop) { depth++; out.costs.push([here, 'loop', file, line, depth, [], { over: '', early: false, forever: false }]); }
    ts.forEachChild(n, visit);
    if (loop) depth--;
  };
  ts.forEachChild(sf, visit);
}

for (const sf of program.getSourceFiles()) { const f = rel(sf.fileName); if (scope.has(f)) { out.units.push([`ts:${f}`, 'file', path.basename(f), f, 1, sf.getLineAndCharacterOfPosition(sf.end).line + 1, null, '', {}]); out.defs.push(['file', f, `ts:${f}`]); declare(sf, f); } }
for (const sf of program.getSourceFiles()) { const f = rel(sf.fileName); if (scope.has(f)) walk(sf, f); }
fs.writeFileSync(process.argv[3], JSON.stringify(out));
