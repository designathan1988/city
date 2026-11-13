// Oracle for TypeScript: the compiler's own answer to "what does this call run?".
// For every call and `new` in the project's own files, the declaration the type checker resolves it to.
// Not part of the tracer: the bench compares the tracer's links with these.
//
//   node ts_calls.cjs <project folder with tsconfig.json> <out.jsonl> [subfolder to keep]
//
// Each line: {"file", "line", "callee", "targets": [{"file", "line", "name"}]}; a call whose declaration lies
// outside the project (the platform's or a package's) has "outside": true and no targets.
"use strict";
const path = require("path");
const fs = require("fs");

const root = path.resolve(process.argv[2]);
const out = process.argv[3];
const keep = process.argv[4] ? path.resolve(root, process.argv[4]) : root;
const ts = require(require.resolve("typescript", { paths: [root] }));

const configPath = ts.findConfigFile(root, ts.sys.fileExists, "tsconfig.json");
const config = ts.readConfigFile(configPath, ts.sys.readFile);
const parsed = ts.parseJsonConfigFileContent(config.config, ts.sys, path.dirname(configPath));
const program = ts.createProgram({ rootNames: parsed.fileNames, options: { ...parsed.options, noEmit: true } });
const checker = program.getTypeChecker();

const norm = (p) => path.resolve(p).split(path.sep).join("/");
const keepN = norm(keep) + "/";
const rootN = norm(root) + "/";
const own = (file) => norm(file).startsWith(keepN) && !norm(file).includes("/node_modules/");
const rel = (file) => norm(file).slice(keepN.length);
const lineOf = (sf, pos) => sf.getLineAndCharacterOfPosition(pos).line + 1;

function declOf(node) {
  // The declaration a call or `new` runs: the resolved signature's, else the symbol of the callee's.
  let decl;
  try {
    const sig = checker.getResolvedSignature(node);
    decl = sig && sig.declaration;
  } catch (e) { decl = undefined; }
  if (!decl || decl.kind === ts.SyntaxKind.JSDocSignature) {
    const callee = node.expression;
    if (!callee) return [];
    let sym = checker.getSymbolAtLocation(ts.isPropertyAccessExpression(callee) ? callee.name : callee);
    if (sym && sym.flags & ts.SymbolFlags.Alias) sym = checker.getAliasedSymbol(sym);
    return sym && sym.declarations ? sym.declarations : [];
  }
  return [decl];
}

function nameOf(decl) {
  if (decl.name && decl.name.getText) return decl.name.getText();
  if (ts.isConstructorDeclaration(decl)) return "constructor";
  if (decl.parent && ts.isVariableDeclaration(decl.parent)) return decl.parent.name.getText();
  if (decl.parent && ts.isPropertyAssignment(decl.parent)) return decl.parent.name.getText();
  return "";
}

// The code a call is written in: the nearest function (a lambda counts), else the class, else the file.
const RUNS = new Set([ts.SyntaxKind.FunctionDeclaration, ts.SyntaxKind.FunctionExpression, ts.SyntaxKind.ArrowFunction,
  ts.SyntaxKind.MethodDeclaration, ts.SyntaxKind.Constructor, ts.SyntaxKind.GetAccessor, ts.SyntaxKind.SetAccessor]);
function callerOf(node, sf) {
  let up = node.parent, cls = null;
  while (up && up.kind !== ts.SyntaxKind.SourceFile) {
    if (RUNS.has(up.kind)) return { file: rel(sf.fileName), line: lineOf(sf, up.getStart(sf)), name: nameOf(up) };
    if (!cls && (ts.isClassDeclaration(up) || ts.isClassExpression(up))) cls = up;
    up = up.parent;
  }
  if (cls) return { file: rel(sf.fileName), line: lineOf(sf, cls.getStart(sf)), name: nameOf(cls) };
  return { file: rel(sf.fileName), line: 1, name: "" };
}

const lines = [];
let calls = 0;
for (const sf of program.getSourceFiles()) {
  if (sf.isDeclarationFile || !own(sf.fileName)) continue;
  const visit = (node) => {
    if (ts.isCallExpression(node) || ts.isNewExpression(node)) {
      if (!(ts.isCallExpression(node) && node.expression.kind === ts.SyntaxKind.ImportKeyword)) {
        calls++;
        const decls = declOf(node);
        const targets = [];
        // through a value of a function type (a parameter, a property): the compiler names the type, not a function
        const indirect = decls.some((d) => d.kind === ts.SyntaxKind.FunctionType || d.kind === ts.SyntaxKind.CallSignature
          || d.kind === ts.SyntaxKind.ConstructorType || d.kind === ts.SyntaxKind.Parameter
          || (d.kind === ts.SyntaxKind.PropertySignature && d.type && d.type.kind === ts.SyntaxKind.FunctionType));
        let outside = decls.length > 0;
        for (const d of decls) {
          const dsf = d.getSourceFile();
          if (!own(dsf.fileName)) continue;
          outside = false;
          targets.push({ file: rel(dsf.fileName), line: lineOf(dsf, d.getStart(dsf)), name: nameOf(d) });
        }
        lines.push(JSON.stringify({
          file: rel(sf.fileName), line: lineOf(sf, node.getStart(sf)), end: [lineOf(sf, node.getEnd()), sf.getLineAndCharacterOfPosition(node.getEnd()).character],
          callee: node.expression ? node.expression.getText(sf).slice(0, 80) : "", targets, caller: callerOf(node, sf),
          outside: outside && targets.length === 0, unresolved: decls.length === 0, indirect,
        }));
      }
    }
    ts.forEachChild(node, visit);
  };
  visit(sf);
}
fs.writeFileSync(out, lines.join("\n") + "\n");
console.error(`${calls} calls written to ${out}`);
