// What objects an expression can be, from Jelly's points-to sets (`--tokens-json`: for each constraint variable, the
// tokens it holds). A variable is written as its declaration's name and place (`'name'[<pkg>:path:line:col]`, an import
// at its specifier), an expression as its range (`[<pkg>:path:l:c:l:c]`), `this` as its function's range
// (`This[...]`), a property of an object as `Object[...].name`; an object as where it is made (`Object[range]`: a literal,
// a `new` of a class is the class's range), and what Jelly could not follow as `@...` (an access path), `%...` (the
// platform's) or `*...` (a package's): objects from outside. Paths are relative to the directory Jelly ran in. A
// function's or a class's range starts where Babel starts it (after `export`), so those are matched by their end.
import fs from 'node:fs';
import path from 'node:path';
import type * as TS from 'typescript';

interface Entry {
  readonly var: string;
  readonly tokens?: readonly string[];
  readonly rep?: number;
}

/** The objects an expression can be; `outside` when it can also be one the analysis could not follow. */
export interface Objects {
  readonly objects: readonly string[];
  readonly outside: boolean;
}

const LOCATION = /^<[^>]*>:(.+?):(\d+):(\d+)(?::(\d+):(\d+))?$/;

export class PointsTo {
  readonly #ts: typeof TS;
  readonly #byKey = new Map<string, readonly string[]>();
  /** `This` variables by the end of their function: [start line, start column, tokens]. */
  readonly #thisByEnd = new Map<string, [number, number, readonly string[]][]>();
  /** Objects (tokens) by the end of their range, for classes matched by end. */
  readonly #objectsByEnd = new Map<string, [number, number, string][]>();

  /** `cwd`: the directory Jelly ran in. */
  constructor(ts: typeof TS, file: string, cwd: string) {
    this.#ts = ts;
    const entries = JSON.parse(fs.readFileSync(file, 'utf8')) as Entry[];
    const location = (inner: string): { file: string; sl: number; sc: number; el?: number; ec?: number } | null => {
      const m = LOCATION.exec(inner);
      if (!m) return null;
      return { file: norm(path.resolve(cwd, m[1]!)), sl: Number(m[2]), sc: Number(m[3]), ...(m[4] ? { el: Number(m[4]), ec: Number(m[5]) } : {}) };
    };
    // A token, its place made absolute: `Object[file:l:c:l:c]`; one from outside stays as written.
    const token = (t: string): string => {
      const m = /^(\w+)\[(.*)\]$/.exec(t);
      const at = m ? location(m[2]!) : null;
      if (!m || !at || at.el === undefined) return t;
      const id = `${m[1]}[${at.file}:${at.sl}:${at.sc}:${at.el}:${at.ec}]`;
      if (m[1] === 'Object') (this.#objectsByEnd.get(`${at.file}:${at.el}:${at.ec}`) ?? this.#objectsByEnd.set(`${at.file}:${at.el}:${at.ec}`, []).get(`${at.file}:${at.el}:${at.ec}`)!).push([at.sl, at.sc, id]);
      return id;
    };
    const tokensOf = entries.map((e) => (e.tokens ?? []).map(token));
    entries.forEach((e, i) => {
      const tokens = e.rep !== undefined ? tokensOf[e.rep]! : tokensOf[i]!;
      const key = this.#keyOf(e.var, location, token);
      if (key === null) return;
      if (key.startsWith('This|')) {
        const [, file, sl, sc, el, ec] = key.split('|');
        const k = `${file}:${el}:${ec}`;
        (this.#thisByEnd.get(k) ?? this.#thisByEnd.set(k, []).get(k)!).push([Number(sl), Number(sc), tokens]);
        return;
      }
      this.#byKey.set(key, [...(this.#byKey.get(key) ?? []), ...tokens]);
    });
  }

  /** A variable's key: `id|file:line:col|name`, `node|file:l:c:l:c`, `This|file|l|c|l|c`, `prop|token|name`. */
  #keyOf(v: string, location: (s: string) => ReturnType<LocationFn>, token: (t: string) => string): string | null {
    let m = /^'(.*)'\[(.*)\]$/.exec(v);
    if (m) {
      const at = location(m[2]!);
      return at ? `id|${at.file}:${at.sl}:${at.sc}|${m[1]}` : null;
    }
    m = /^\[(.*)\]$/.exec(v);
    if (m) {
      const at = location(m[1]!);
      return at && at.el !== undefined ? `node|${at.file}:${at.sl}:${at.sc}:${at.el}:${at.ec}` : null;
    }
    m = /^This\[(.*)\]$/.exec(v);
    if (m) {
      const at = location(m[1]!);
      return at && at.el !== undefined ? `This|${at.file}|${at.sl}|${at.sc}|${at.el}|${at.ec}` : null;
    }
    // A property of an object: the object is a token (`Object[...]`), the property what follows its `]`.
    m = /^((?:\w+)\[[^\]]*\])\.(.+)$/.exec(v);
    if (m) return `prop|${token(m[1]!)}|${m[2]}`;
    return null;
  }

  /** The objects an expression can be (null when the analysis has nothing for it). */
  objectsOf(expr: TS.Expression, checker: TS.TypeChecker): Objects | null {
    const ts = this.#ts;
    let e = expr;
    while (ts.isParenthesizedExpression(e) || ts.isAsExpression(e) || ts.isNonNullExpression(e) || ts.isSatisfiesExpression(e) || ts.isTypeAssertionExpression(e)) e = e.expression;
    const sf = e.getSourceFile();
    const file = norm(sf.fileName);
    const at = (pos: number): string => {
      const lc = sf.getLineAndCharacterOfPosition(pos);
      return `${lc.line + 1}:${lc.character + 1}`;
    };
    if (e.kind === ts.SyntaxKind.ThisKeyword) return this.#thisOf(e);
    if (ts.isIdentifier(e)) {
      // The name as declared where the code binds it (an import binds it at its specifier).
      const decl = checker.getSymbolAtLocation(e)?.declarations?.[0];
      const name = decl ? (ts as typeof TS).getNameOfDeclaration(decl as TS.Declaration) : undefined;
      if (name && ts.isIdentifier(name)) {
        const dsf = name.getSourceFile();
        const lc = dsf.getLineAndCharacterOfPosition(name.getStart(dsf));
        const found = this.#byKey.get(`id|${norm(dsf.fileName)}:${lc.line + 1}:${lc.character + 1}|${name.text}`);
        if (found) return split(found);
      }
    }
    const node = this.#byKey.get(`node|${file}:${at(e.getStart(sf))}:${at(e.end)}`);
    if (node) return split(node);
    if (ts.isPropertyAccessExpression(e)) {
      const of = this.objectsOf(e.expression, checker);
      if (!of) return null;
      const out: string[] = [];
      let outside = of.outside;
      for (const t of of.objects) {
        const p = this.#byKey.get(`prop|${t}|${e.name.text}`);
        if (!p) continue;
        const s = split(p);
        out.push(...s.objects);
        outside ||= s.outside;
      }
      return { objects: [...new Set(out)], outside };
    }
    return null;
  }

  /** The objects a class's instances are (a `new` of it is where it is made): the token of the class's range. */
  instancesOf(cls: TS.ClassLikeDeclaration): string[] {
    const sf = cls.getSourceFile();
    const end = sf.getLineAndCharacterOfPosition(cls.end);
    const start = sf.getLineAndCharacterOfPosition(cls.getStart(sf));
    return (this.#objectsByEnd.get(`${norm(sf.fileName)}:${end.line + 1}:${end.character + 1}`) ?? [])
      .filter(([l, c]) => l > start.line + 1 || (l === start.line + 1 && c >= start.character + 1)).map(([, , id]) => id);
  }

  /** The token an object literal is (where it is made). */
  literalToken(o: TS.ObjectLiteralExpression): string {
    const sf = o.getSourceFile();
    const at = (pos: number): string => {
      const lc = sf.getLineAndCharacterOfPosition(pos);
      return `${lc.line + 1}:${lc.character + 1}`;
    };
    return `Object[${norm(sf.fileName)}:${at(o.getStart(sf))}:${at(o.end)}]`;
  }

  /** `this` in code: its function's (an arrow's is its enclosing function's; a field initializer's, its class's). */
  #thisOf(e: TS.Node): Objects | null {
    const ts = this.#ts;
    let fn: TS.Node | undefined = e.parent;
    while (fn && !(ts.isFunctionLike(fn) && !ts.isArrowFunction(fn)) && !ts.isClassLike(fn)) fn = fn.parent;
    return fn ? this.#thisVar(fn) : null;
  }

  /**
   * The objects `this` is in a function or a class (a class is its own constructor: `this` in its fields' initializers
   * and in a constructor Jelly does not model apart is the class's), made in the program.
   */
  thisOf(container: TS.Node): string[] {
    const found = this.#thisVar(container) ?? (this.#ts.isConstructorDeclaration(container) ? this.#thisVar(container.parent) : null);
    return found ? [...found.objects] : [];
  }

  /** A function's or a class's `This` variable, matched by its end and the nearest start. */
  #thisVar(fn: TS.Node): Objects | null {
    const sf = fn.getSourceFile();
    const end = sf.getLineAndCharacterOfPosition(fn.end);
    const start = sf.getLineAndCharacterOfPosition(fn.getStart(sf));
    const candidates = (this.#thisByEnd.get(`${norm(sf.fileName)}:${end.line + 1}:${end.character + 1}`) ?? [])
      .filter(([l, c]) => l > start.line + 1 || (l === start.line + 1 && c >= start.character + 1));
    if (candidates.length === 0) return null;
    return split(candidates.flatMap(([, , t]) => t));
  }
}

type LocationFn = (inner: string) => { file: string; sl: number; sc: number; el?: number; ec?: number } | null;

/** Objects made in the program, apart from what comes from outside. */
function split(tokens: readonly string[]): Objects {
  const objects = tokens.filter((t) => /^(Object|Array|Function|Prototype)\[/.test(t));
  return { objects: [...new Set(objects)], outside: objects.length < tokens.length };
}

/** A path as compared here: absolute, forward slashes, lower case (Windows). */
function norm(p: string): string {
  return path.resolve(p).split(path.sep).join('/').toLowerCase();
}
