// Which ECMAScript built-ins a TypeScript program calls, counted by receiver type and member: what a C++ runtime of
// the built-ins must cover so the translator writes each one the same way. Usage:
//   node scripts/builtins-census.ts --source-root <dir> --typescript <typescript package dir> [--out file.json]
import fs from 'node:fs';
import path from 'node:path';
import { loadTypeScript, openProgram } from '../src/graph/typescript.ts';

const args = new Map<string, string>();
for (let i = 2; i < process.argv.length; i += 2) args.set(process.argv[i]!.replace(/^--/, ''), process.argv[i + 1]!);
const root = path.resolve(args.get('source-root')!);
const ts = loadTypeScript(root, args.get('typescript'));
const sp = openProgram(ts, root, path.join(root, 'tsconfig.json'));
const checker = sp.checker;
const BUILTIN = /^(String|Number|Boolean|Array|ReadonlyArray|Map|ReadonlyMap|Set|ReadonlySet|RegExp|Math|JSON|Object|Float32Array|Float64Array|Int32Array|Uint32Array|Uint8Array|Uint16Array|Int16Array|Int8Array|ArrayLike|Date|Promise|Symbol)$/;
const count = new Map<string, number>();
const add = (k: string): void => { count.set(k, (count.get(k) ?? 0) + 1); };
for (const file of sp.program.getSourceFiles()) {
  if (file.isDeclarationFile || !file.fileName.replace(/\\/g, '/').includes('/src/')) continue;
  const visit = (n: import('typescript').Node): void => {
    if (ts.isPropertyAccessExpression(n)) {
      const sym = checker.getSymbolAtLocation(n.name);
      const decl = sym?.declarations?.[0];
      const lib = decl && /[\\/]typescript[\\/]lib[\\/]lib\./.test(decl.getSourceFile().fileName);
      if (lib) {
        const owner = decl.parent && (ts.isInterfaceDeclaration(decl.parent) || ts.isClassDeclaration(decl.parent)) ? decl.parent.name?.text ?? '?' : '?';
        const host = BUILTIN.test(owner) ? owner : owner.replace(/Constructor$/, '');
        add(`${host}.${n.name.text}`);
      }
    }
    if (ts.isBinaryExpression(n)) {
      const op = n.operatorToken.getText();
      if (['|', '&', '^', '<<', '>>', '>>>', '**', '|=', '&=', '^=', '<<=', '>>=', '>>>=', '**=', 'in', 'instanceof', '??', '??='].includes(op)) add(`op ${op}`);
    }
    if (ts.isTemplateExpression(n)) add('syntax template');
    if (ts.isSpreadElement(n) || ts.isSpreadAssignment(n)) add('syntax spread');
    if (n.kind === ts.SyntaxKind.RegularExpressionLiteral) add('syntax regex');
    if (ts.isTryStatement(n)) add('syntax try');
    if (ts.isThrowStatement(n)) add('syntax throw');
    if (ts.isClassDeclaration(n)) add('syntax class');
    if (ts.isAwaitExpression(n)) add('syntax await');
    if (ts.isOptionalChain(n)) add('syntax ?.');
    ts.forEachChild(n, visit);
  };
  visit(file);
}
const sorted = [...count].sort((a, b) => b[1] - a[1]);
const out = args.get('out');
if (out) fs.writeFileSync(out, JSON.stringify(Object.fromEntries(sorted), null, 1));
console.log(`${sorted.length} built-ins and syntax forms, ${sorted.reduce((a, [, n]) => a + n, 0)} uses`);
for (const [k, n] of sorted.slice(0, 120)) console.log(`${String(n).padStart(6)} ${k}`);
