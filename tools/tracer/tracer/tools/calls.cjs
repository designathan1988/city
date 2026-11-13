// What the TypeScript compiler says each call of the files given runs (the tracer's `compilers.py` runs this).
//
//   node calls.cjs < [folder, [absolute paths]]
//
// Writes [[[file, line, column, end line, end column, answer, [[file, line, column, name], ...]], ...], problem]
// files by their place in the list given; lines and columns counted from 0, columns in UTF-16 units as the compiler
// counts them. An answer is "exact" (the function itself: a function declared, a static method, a method called
// through `super`), "dispatched" (a method run by the object's class: one below may run instead) or "outside" (a
// function of the language's own library or a package's). A call through a value holding a function (a variable, a
// parameter, a property), or one the compiler cannot resolve, has no answer: the tracer's flow says what it runs.
"use strict";
const path = require("path");

let ts;
try {
  ts = require(require.resolve("typescript", { paths: [__dirname] }));
} catch (e) {
  process.stdout.write(JSON.stringify([[], "the TypeScript compiler is not installed in " + __dirname]));
  process.exit(0);
}

let input = "";
process.stdin.setEncoding("utf8");
process.stdin.on("data", (chunk) => { input += chunk; });
process.stdin.on("end", () => {
  const [base, files] = JSON.parse(input);
  const norm = (p) => path.resolve(p).split(path.sep).join("/").toLowerCase();
  const index = new Map(files.map((f, i) => [norm(f), i]));

  let options = {
    allowJs: true, checkJs: false, noEmit: true, skipLibCheck: true, target: ts.ScriptTarget.ESNext,
    module: ts.ModuleKind.ESNext, moduleResolution: ts.ModuleResolutionKind.Bundler, jsx: ts.JsxEmit.Preserve,
    allowImportingTsExtensions: true, types: [],
  };
  const configPath = ts.findConfigFile(base, ts.sys.fileExists, "tsconfig.json")
    || ts.findConfigFile(base, ts.sys.fileExists, "jsconfig.json");
  let problem = "";
  if (configPath) {
    const config = ts.readConfigFile(configPath, ts.sys.readFile);
    if (!config.error) {
      const parsed = ts.parseJsonConfigFileContent(config.config, ts.sys, path.dirname(configPath));
      options = { ...parsed.options, allowJs: true, noEmit: true, skipLibCheck: true };
    } else {
      problem = configPath + ": unreadable";
    }
  }
  const program = ts.createProgram({ rootNames: files, options });
  const checker = program.getTypeChecker();

  const outsideFile = (sf) => program.isSourceFileDefaultLibrary(sf) || program.isSourceFileFromExternalLibrary(sf)
    || norm(sf.fileName).includes("/node_modules/");

  function place(d) {
    const sf = d.getSourceFile();
    const at = index.get(norm(sf.fileName));
    if (at === undefined) return null;
    const named = d.name || d;
    const start = sf.getLineAndCharacterOfPosition(named.getStart(sf));
    let name = d.name && d.name.getText ? d.name.getText(sf) : "";
    return [at, start.line, start.character, name];
  }

  function answer(call) {
    let callee = call.expression;
    while (ts.isParenthesizedExpression(callee) || ts.isNonNullExpression(callee)) callee = callee.expression;
    let name = callee, through = null;
    if (ts.isPropertyAccessExpression(callee)) {
      name = callee.name;
      through = callee.expression;
    } else if (!ts.isIdentifier(callee)) {
      return null;                         // a function written in place, an entry read by a key, a call's result
    }
    let sym = checker.getSymbolAtLocation(name);
    if (!sym) return null;
    if (sym.flags & ts.SymbolFlags.Alias) sym = checker.getAliasedSymbol(sym);
    const decls = sym.declarations || [];
    const fn = sym.flags & ts.SymbolFlags.Function, method = sym.flags & ts.SymbolFlags.Method;
    if (!decls.length || !(fn || method)) return null;     // a value holding a function: the flow's
    if (decls.every((d) => outsideFile(d.getSourceFile()))) return ["outside", []];
    if (decls.some((d) => outsideFile(d.getSourceFile()))) return null;
    let written = decls.filter((d) => d.body);
    if (!written.length && method) {
      written = decls.filter((d) => ts.isMethodDeclaration(d) && ts.isClassLike(d.parent));   // abstract: declared
    }
    if (!written.length) return null;     // declared alone (an interface's member, a declaration file): the flow's
    const targets = written.map(place);
    if (targets.some((t) => t === null)) return null;      // written in a file not read
    if (fn) return ["exact", targets];
    const fixed = written.every((d) => (ts.getCombinedModifierFlags(d) & ts.ModifierFlags.Static)
      || (d.name && ts.isPrivateIdentifier(d.name)));
    const viaSuper = through !== null && through.kind === ts.SyntaxKind.SuperKeyword;
    return [fixed || viaSuper ? "exact" : "dispatched", targets];
  }

  const calls = [];
  for (const sf of program.getSourceFiles()) {
    const at = index.get(norm(sf.fileName));
    if (at === undefined) continue;
    const visit = (node) => {
      if (ts.isCallExpression(node) && node.expression.kind !== ts.SyntaxKind.ImportKeyword
          && node.expression.kind !== ts.SyntaxKind.SuperKeyword) {
        let got = null;
        try { got = answer(node); } catch (e) { got = null; }
        if (got) {
          const start = sf.getLineAndCharacterOfPosition(node.getStart(sf));
          const end = sf.getLineAndCharacterOfPosition(node.getEnd());
          calls.push([at, start.line, start.character, end.line, end.character, got[0], got[1]]);
        }
      }
      ts.forEachChild(node, visit);
    };
    visit(sf);
  }
  process.stdout.write(JSON.stringify([calls, problem]));
});
