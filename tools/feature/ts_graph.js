// The TypeScript program's graph as its compiler resolves it (the TypeScript compiler's type checker, with the
// project's own tsconfig): every function, method and arrow; every call and `new` to the exact declaration the checker
// binds it to; every function handed to another as a value, the parameter it goes through, the functions that run
// what their parameter holds and those that pass it on; every field read and written; the conditions each sits
// under; the calls that leave the program (libraries, the platform); the loops. Same rows as cpp_graph.py.
//
//   node ts_graph.js PROJECT_DIR SOURCE_ROOT OUT.json
const path = require('path');
const fs = require('fs');
const [projectDir, sourceRoot, outFile] = process.argv.slice(2);
const ts = require(path.join(projectDir, 'node_modules', 'typescript'));

const norm = (p) => path.resolve(p).replace(/\\/g, '/').toLowerCase();
const root = norm(sourceRoot);
const t0 = Date.now();
const configPath = ts.findConfigFile(projectDir, ts.sys.fileExists, 'tsconfig.json');
const parsed = ts.getParsedCommandLineOfConfigFile(configPath, {}, { ...ts.sys, onUnRecoverableConfigFileDiagnostic: () => {} });
const program = ts.createProgram({ rootNames: parsed.fileNames, options: { ...parsed.options, noEmit: true } });
const checker = program.getTypeChecker();
const mine = (sf) => sf && !sf.isDeclarationFile && norm(sf.fileName).startsWith(root) && !/[\\/](tests?|__tests__)[\\/]|\.(test|spec)\.tsx?$/.test(sf.fileName);

const graph = { language: 'typescript', root, functions: {}, calls: [], hands: [], invokes: [], forwards: [], stores: [],
  field_calls: [], fields: [], loops: [], texts: [], numbers: [], consts: [], registers: [], errors: {} };

// The texts written in place inside an argument, a few levels down.
function stringsIn(e, depth) {
  if (!e || depth < 0) return [];
  if (ts.isStringLiteral(e) || ts.isNoSubstitutionTemplateLiteral(e)) return [e.text];
  const out = [];
  ts.forEachChild(e, (k) => { out.push(...stringsIn(k, depth - 1)); });
  return out;
}
// A named constant read: a `const` (or `readonly` field) of the program, and what it is written as.
function constOf(e) {
  if (!ts.isIdentifier(e) && !ts.isPropertyAccessExpression(e)) return null;
  let s = checker.getSymbolAtLocation(e);
  if (s && s.flags & ts.SymbolFlags.Alias) s = checker.getAliasedSymbol(s);
  const d = s && s.valueDeclaration;
  if (!d || !d.initializer || !mine(d.getSourceFile())) return null;
  const isConst = ts.isVariableDeclaration(d) && d.parent && (d.parent.flags & ts.NodeFlags.Const);
  const isReadonly = ts.isPropertyDeclaration(d) && d.modifiers && d.modifiers.some((m) => m.kind === ts.SyntaxKind.ReadonlyKeyword);
  if (!isConst && !isReadonly) return null;
  if (isFn(d.initializer)) return null;
  return { name: s.name, value: text(d.initializer).slice(0, 160) };
}

const isFn = (n) => ts.isFunctionDeclaration(n) || ts.isMethodDeclaration(n) || ts.isConstructorDeclaration(n) ||
  ts.isGetAccessorDeclaration(n) || ts.isSetAccessorDeclaration(n) || ts.isArrowFunction(n) || ts.isFunctionExpression(n);
const where = (n) => { const sf = n.getSourceFile(); const lc = sf.getLineAndCharacterOfPosition(n.getStart(sf)); return [norm(sf.fileName), lc.line + 1, lc.character + 1]; };
const idOf = (n) => { const [f, l, c] = where(n); return `${f}:${l}:${c}`; };
const text = (n) => n.getText(n.getSourceFile()).replace(/\s+/g, ' ').slice(0, 300);
const lineOf = (n) => where(n)[1];

function nameOf(n, owner) {
  const named = n.name && (ts.isIdentifier(n.name) || ts.isPrivateIdentifier(n.name)) ? n.name.text : null;
  const cls = n.parent && (ts.isClassDeclaration(n.parent) || ts.isClassExpression(n.parent)) && n.parent.name ? n.parent.name.text + '.' : '';
  if (ts.isConstructorDeclaration(n)) return cls + 'constructor';
  if (named) return cls + named;
  if (n.parent && ts.isVariableDeclaration(n.parent) && ts.isIdentifier(n.parent.name)) return n.parent.name.text;
  if (n.parent && ts.isPropertyDeclaration(n.parent) && n.parent.name && ts.isIdentifier(n.parent.name)) {
    const c2 = n.parent.parent && n.parent.parent.name ? n.parent.parent.name.text + '.' : '';
    return c2 + n.parent.name.text;
  }
  const o = owner && graph.functions[owner] ? graph.functions[owner].name : '?';
  return `${o}.<lambda:${lineOf(n)}>`;
}

// The declaration a value names when it is a function: an arrow or function written there, or a name bound to one.
function fnTarget(e) {
  while (e && (ts.isParenthesizedExpression(e) || ts.isAsExpression(e) || ts.isNonNullExpression(e))) e = e.expression;
  if (!e) return null;
  if (ts.isArrowFunction(e) || ts.isFunctionExpression(e)) return e;
  if (ts.isIdentifier(e) || ts.isPropertyAccessExpression(e)) {
    let s = checker.getSymbolAtLocation(e);
    if (s && s.flags & ts.SymbolFlags.Alias) s = checker.getAliasedSymbol(s);
    const d = s && s.valueDeclaration;
    if (!d) return null;
    if (isFn(d) && !ts.isArrowFunction(d)) return d;
    if ((ts.isVariableDeclaration(d) || ts.isPropertyDeclaration(d)) && d.initializer && (ts.isArrowFunction(d.initializer) || ts.isFunctionExpression(d.initializer))) return d.initializer;
  }
  return null;
}
function paramOf(e, fn) {
  while (e && (ts.isParenthesizedExpression(e) || ts.isAsExpression(e) || ts.isNonNullExpression(e))) e = e.expression;
  if (!e || !ts.isIdentifier(e) || !fn) return -1;
  const s = checker.getSymbolAtLocation(e);
  const d = s && s.valueDeclaration;
  if (!d || !ts.isParameter(d) || d.parent !== fn) return -1;
  return fn.parameters.indexOf(d);
}
function fieldOf(e) {
  if (!e || !ts.isPropertyAccessExpression(e)) return null;
  const s = checker.getSymbolAtLocation(e);
  const d = s && (s.valueDeclaration || (s.declarations && s.declarations[0]));
  if (!d) return null;
  if (!(ts.isPropertyDeclaration(d) || ts.isParameter(d) || ts.isPropertySignature(d) || ts.isPropertyAssignment(d) || ts.isShorthandPropertyAssignment(d))) return null;
  const sf = d.getSourceFile();
  const owner = d.parent && d.parent.name && d.parent.name.text ? d.parent.name.text : (ts.isParameter(d) && d.parent.parent && d.parent.parent.name ? d.parent.parent.name.text : path.basename(sf.fileName));
  return { name: `${owner}.${s.name}`, out: !mine(sf) };
}
const ASSIGN = new Set([ts.SyntaxKind.EqualsToken, ts.SyntaxKind.PlusEqualsToken, ts.SyntaxKind.MinusEqualsToken, ts.SyntaxKind.AsteriskEqualsToken,
  ts.SyntaxKind.SlashEqualsToken, ts.SyntaxKind.BarEqualsToken, ts.SyntaxKind.AmpersandEqualsToken, ts.SyntaxKind.QuestionQuestionEqualsToken,
  ts.SyntaxKind.BarBarEqualsToken, ts.SyntaxKind.AmpersandAmpersandEqualsToken, ts.SyntaxKind.PercentEqualsToken]);
const ITERATE = new Set(['forEach', 'map', 'filter', 'reduce', 'reduceRight', 'some', 'every', 'find', 'findIndex', 'flatMap']);

const endLine = (n) => n.getSourceFile().getLineAndCharacterOfPosition(n.end).line + 1;
const iterated = new Set();          // callbacks handed to a per-element method: their body is a loop's body
const initOf = (cls) => `${idOf(cls)}#init`;

function visit(n, fn, fnNode, guards, depth) {
  if (isFn(n)) {
    const id = idOf(n);
    const [f, l] = where(n);
    graph.functions[id] = { name: nameOf(n, fn), file: f, line: l, end: endLine(n),
      kind: ts.isArrowFunction(n) || ts.isFunctionExpression(n) ? 'lambda' : 'function', owner: fn };
    const base = iterated.has(n) ? 1 : 0;
    ts.forEachChild(n, (k) => visit(k, id, n, [], base));
    // a constructor runs the class's field initialisers
    if (ts.isConstructorDeclaration(n) && n.parent) graph.calls.push({ from: id, to: initOf(n.parent), name: null, line: l, guards: [], out: false });
    return;
  }
  if ((ts.isClassDeclaration(n) || ts.isClassExpression(n)) && n.members) {
    // the field initialisers run when an object of the class is made: a function of their own
    const init = initOf(n);
    const [f, l] = where(n);
    graph.functions[init] = { name: `${n.name ? n.name.text : '?'}.<fields>`, file: f, line: l, end: endLine(n), kind: 'function', owner: fn };
    for (const m of n.members) {
      if (ts.isPropertyDeclaration(m) && m.initializer && !isFn(m.initializer)) visit(m.initializer, init, null, [], 0);
      else visit(m, fn, fnNode, guards, depth);
    }
    for (const h of n.heritageClauses || []) visit(h, fn, fnNode, guards, depth);
    return;
  }
  if (!fn) { ts.forEachChild(n, (k) => visit(k, fn, fnNode, guards, depth)); return; }
  const line = lineOf(n);
  if (ts.isIfStatement(n)) {
    const t = text(n.expression);
    visit(n.expression, fn, fnNode, guards, depth);
    visit(n.thenStatement, fn, fnNode, [...guards, `${t} @${line}`], depth);
    if (n.elseStatement) visit(n.elseStatement, fn, fnNode, [...guards, `!(${t}) @${line}`], depth);
    return;
  }
  if (ts.isConditionalExpression(n)) {
    const t = text(n.condition);
    visit(n.condition, fn, fnNode, guards, depth);
    visit(n.whenTrue, fn, fnNode, [...guards, `${t} @${line}`], depth);
    visit(n.whenFalse, fn, fnNode, [...guards, `!(${t}) @${line}`], depth);
    return;
  }
  if (ts.isForStatement(n) || ts.isForOfStatement(n) || ts.isForInStatement(n) || ts.isWhileStatement(n) || ts.isDoStatement(n)) {
    const head = text(n).split('{')[0].slice(0, 160);
    graph.loops.push({ in: fn, line, end: endLine(n), depth: depth + 1, cond: head });
    ts.forEachChild(n, (k) => visit(k, fn, fnNode, [...guards, `loop ${head} @${line}`], depth + 1));
    return;
  }
  if (ts.isCaseClause(n)) {
    const label = text(n.expression);
    n.statements.forEach((k) => visit(k, fn, fnNode, [...guards, `case ${label} @${line}`], depth));
    return;
  }
  if (ts.isCallExpression(n) || ts.isNewExpression(n)) {
    const callee = n.expression;
    const pi = paramOf(callee, fnNode);
    if (pi >= 0) graph.invokes.push({ in: fn, param: pi, line, guards });
    else if (ts.isPropertyAccessExpression(callee)) {
      const fld = fieldOf(callee);
      if (fld && !fld.out) graph.field_calls.push({ in: fn, field: fld.name, line, guards });
    }
    const sig = checker.getResolvedSignature(n);
    let d = sig && sig.declaration;
    if (d && !isFn(d) && ts.isFunctionTypeNode && (ts.isFunctionTypeNode(d) || ts.isCallSignatureDeclaration(d) || ts.isMethodSignature(d))) {
      // a declared signature: the function itself when the callee names one
      const t2 = fnTarget(callee);
      if (t2) d = t2;
    }
    let to = null, name = text(callee).slice(0, 120), out = true;
    if (d && d.getSourceFile) {
      out = !mine(d.getSourceFile());
      to = out ? `out:${name}` : idOf(d);
      if (!out) name = null;
    } else to = `out:${name}`;
    if (ts.isPropertyAccessExpression(callee) && ITERATE.has(callee.name.text) && n.arguments && n.arguments[0]) {
      graph.loops.push({ in: fn, line, end: endLine(n), depth: depth + 1, cond: `${callee.name.text}(...)` });
      const cb = n.arguments[0];
      if (ts.isArrowFunction(cb) || ts.isFunctionExpression(cb)) iterated.add(cb);
    }
    graph.calls.push({ from: fn, to, name, line, guards, out });
    // `new` of a class of the program runs its field initialisers too (and its constructor, bound above when written)
    if (ts.isNewExpression(n)) {
      let s = checker.getSymbolAtLocation(n.expression);
      if (s && s.flags & ts.SymbolFlags.Alias) s = checker.getAliasedSymbol(s);
      const cls = s && s.valueDeclaration;
      if (cls && (ts.isClassDeclaration(cls) || ts.isClassExpression(cls)) && mine(cls.getSourceFile()))
        graph.calls.push({ from: fn, to: initOf(cls), name: null, line, guards, out: false });
    }
    const handedHere = [];
    (n.arguments || []).forEach((a, k) => {
      const t = fnTarget(a);
      if (t) { handedHere.push(t); graph.hands.push({ from: fn, fn: idOf(t), to, index: k, line, guards }); }
      const p = paramOf(a, fnNode);
      if (p >= 0 && !t) graph.forwards.push({ in: fn, param: p, to, index: k });
    });
    if (handedHere.length) {
      const named = (n.arguments || []).filter((a) => !fnTarget(a)).flatMap((a) => stringsIn(a, 3));
      for (const t of handedHere) if (named.length) graph.registers.push({ in: fn, name: named[0], fn: idOf(t), line });
    }
  }
  if (ts.isBinaryExpression(n) && ASSIGN.has(n.operatorToken.kind)) {
    const fld = fieldOf(n.left);
    if (fld && !fld.out) graph.fields.push({ in: fn, field: fld.name, write: true, line, guards });
    const t = fnTarget(n.right);
    if (t && fld && !fld.out) graph.stores.push({ in: fn, fn: idOf(t), field: fld.name, line });
    // a function kept in a field of an object the program does not define: the library runs it (by that field's name)
    if (t && ts.isPropertyAccessExpression(n.left) && (!fld || fld.out))
      graph.hands.push({ from: fn, fn: idOf(t), to: `out:field ${n.left.name.text}`, index: 0, line, guards });
    // a parameter kept in a field: what the callers hand through that parameter is run where the field is
    const pk = paramOf(n.right, fnNode);
    if (pk >= 0 && fld && !fld.out) graph.stores.push({ in: fn, param: pk, field: fld.name, line });
    if (ts.isPropertyAccessExpression(n.left)) ts.forEachChild(n.left.expression, (k) => visit(k, fn, fnNode, guards, depth));
    else visit(n.left, fn, fnNode, guards, depth);
    visit(n.right, fn, fnNode, guards, depth);
    return;
  }
  if ((ts.isPrefixUnaryExpression(n) || ts.isPostfixUnaryExpression(n)) &&
      (n.operator === ts.SyntaxKind.PlusPlusToken || n.operator === ts.SyntaxKind.MinusMinusToken)) {
    const fld = fieldOf(n.operand);
    if (fld && !fld.out) graph.fields.push({ in: fn, field: fld.name, write: true, line, guards });
  } else if (ts.isPropertyAccessExpression(n)) {
    const fld = fieldOf(n);
    if (fld && !fld.out) graph.fields.push({ in: fn, field: fld.name, write: false, line, guards });
    // reading a property with a getter runs it; writing one with a setter runs that
    const s = checker.getSymbolAtLocation(n);
    for (const d of (s && s.declarations) || []) {
      const writes = n.parent && ts.isBinaryExpression(n.parent) && n.parent.left === n && ASSIGN.has(n.parent.operatorToken.kind);
      if ((ts.isGetAccessorDeclaration(d) && !writes) || (ts.isSetAccessorDeclaration(d) && writes)) {
        const out = !mine(d.getSourceFile());
        graph.calls.push({ from: fn, to: out ? `out:${text(n).slice(0, 120)}` : idOf(d), name: out ? text(n).slice(0, 120) : null, line, guards, out });
      }
    }
  }
  if (ts.isStringLiteral(n) || ts.isNoSubstitutionTemplateLiteral(n)) graph.texts.push({ in: fn, text: n.text.slice(0, 200), line });
  if (ts.isNumericLiteral(n)) graph.numbers.push({ in: fn, value: n.text, line });
  if ((ts.isIdentifier(n) && !(n.parent && ts.isPropertyAccessExpression(n.parent) && n.parent.name === n)) || ts.isPropertyAccessExpression(n)) {
    const k = constOf(n);
    if (k) graph.consts.push({ in: fn, name: k.name, value: k.value, line });
  }
  ts.forEachChild(n, (k) => visit(k, fn, fnNode, guards, depth));
}

// A method of a class runs where the same member of a class it extends, or of an interface it implements, is called
// (ECMA-262 method lookup through the prototype chain; a call bound by the checker to the base member).
function overrides(cls) {
  const out = [];
  const bases = [];
  for (const h of cls.heritageClauses || []) for (const t of h.types) {
    const type = checker.getTypeAtLocation(t.expression);
    const sym = type && (type.symbol || (type.getSymbol && type.getSymbol()));
    for (const d of (sym && sym.declarations) || []) if (ts.isClassDeclaration(d) || ts.isInterfaceDeclaration(d) || ts.isClassExpression(d)) bases.push(d);
  }
  const seen = new Set();
  while (bases.length) {
    const b = bases.pop();
    if (seen.has(b)) continue;
    seen.add(b);
    for (const m of cls.members) {
      if (!m.name || !(ts.isMethodDeclaration(m) || ts.isGetAccessorDeclaration(m) || ts.isSetAccessorDeclaration(m) || ts.isPropertyDeclaration(m))) continue;
      const name = m.name.getText(m.getSourceFile());
      const impl = ts.isPropertyDeclaration(m) ? (m.initializer && isFn(m.initializer) ? m.initializer : null) : m;
      if (!impl) continue;
      for (const bm of b.members || []) if (bm.name && bm.name.getText(bm.getSourceFile()) === name) out.push({ fn: idOf(impl), base: idOf(bm) });
    }
    for (const h of b.heritageClauses || []) for (const t of h.types) {
      const type = checker.getTypeAtLocation(t.expression);
      const sym = type && type.symbol;
      for (const d of (sym && sym.declarations) || []) if (ts.isClassDeclaration(d) || ts.isInterfaceDeclaration(d)) bases.push(d);
    }
  }
  return out;
}
graph.overrides = [];

let files = 0;
for (const sf of program.getSourceFiles()) {
  if (!mine(sf)) continue;
  files++;
  const diags = program.getSyntacticDiagnostics(sf);
  if (diags.length) graph.errors[norm(sf.fileName)] = diags.slice(0, 5).map((d) => ts.flattenDiagnosticMessageText(d.messageText, ' '));
  visit(sf, null, null, [], 0);
  const classes = (n) => { if (ts.isClassDeclaration(n) || ts.isClassExpression(n)) graph.overrides.push(...overrides(n)); ts.forEachChild(n, classes); };
  classes(sf);
}
fs.writeFileSync(outFile, JSON.stringify(graph));
console.error(`${files} files, ${Object.keys(graph.functions).length} functions, ${graph.calls.length} calls, ${graph.hands.length} handed, ` +
  `${graph.fields.length} field uses, ${Object.keys(graph.errors).length} files with errors; ${((Date.now() - t0) / 1000).toFixed(0)} s, ` +
  `${(process.memoryUsage().rss / 1048576).toFixed(0)} MB`);
