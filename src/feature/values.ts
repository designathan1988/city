// The strings a value can be, by the flow of values through the whole program (string constant propagation,
// flow-insensitive, solved as a worklist fixpoint). A literal is itself; a variable, a parameter, a function's result
// is what flows into it (initializers, assignments, the arguments of every call that runs the function, its returns:
// the call graph says which calls run which function). A property is per object (pointsto.ts: Jelly's points-to sets):
// `o.p` reads the property p of each object o can be, written by object literals (and what they spread), assignments,
// class fields and parameter properties. A write whose key and value are the parameters of the function it is in
// (`this.values[key] = value` in a setter) is made at each call of that function with that call's key and value (a
// write merged over every call would give each key every call's value). Templates and `+` are the combinations of
// their parts; `?:`, `??`, `||` either side. What comes from outside is any string: a variable or parameter nothing
// in the program writes (a fact raised after the propagation, which then goes on from it), an object the analysis
// could not follow, a property of the platform's, a write with a key of any value (to every property of its objects).
// A property of an object made in the program that nothing writes does not exist: it holds no string. Where an
// expression's type is a finite union of literals, the type bounds it too: both are sound, their meet is kept.
import type * as TS from 'typescript';
import type { CallTargets } from './callgraph.ts';
import type { PointsTo } from './pointsto.ts';

/** Past this many combinations a template or a `+` is read as the prefixes it starts with. */
const MAX_COMBINATIONS = 256;
/** Past this many values (a loop building ever longer strings) a storage holds any string. */
const MAX_VALUES = 4096;
/** A flow running this often has stopped being monotone: an error of the analysis, reported. */
const MAX_RUNS = 1_000_000;
/** The property of an object a write with a key of any value goes to (every property reads it too). */
const ANY_KEY = '\u0000any';

/** The strings a value can be: exact ones, the prefixes of ones made with a part of any value, or any string. */
export interface Strings {
  readonly top: boolean;
  readonly exact: readonly string[];
  readonly prefixes: readonly string[];
}

class Value {
  top = false;
  readonly exact = new Set<string>();
  readonly prefixes = new Set<string>();

  static of(...xs: string[]): Value {
    const v = new Value();
    for (const x of xs) v.exact.add(x);
    return v;
  }

  static any(): Value {
    const v = new Value();
    v.top = true;
    return v;
  }

  /** Joins another value in; true when this one grew. */
  join(o: Value): boolean {
    if (this.top) return false;
    if (o.top) {
      this.top = true;
      this.exact.clear();
      this.prefixes.clear();
      return true;
    }
    let changed = false;
    for (const x of o.exact) if (!this.exact.has(x)) { this.exact.add(x); changed = true; }
    for (const p of o.prefixes) if (!this.prefixes.has(p)) { this.prefixes.add(p); changed = true; }
    // A prefix that is empty is any string.
    if (this.prefixes.has('') || this.exact.size + this.prefixes.size > MAX_VALUES) {
      this.top = true;
      this.exact.clear();
      this.prefixes.clear();
    }
    return changed;
  }
}

export class StringValues {
  readonly #ts: typeof TS;
  readonly #checker: TS.TypeChecker;
  readonly #calls: CallTargets;
  readonly #pts: PointsTo;
  readonly #slots = new Map<TS.Node | string, Value>();
  /** Object storage (`token|property`): not raised when nothing writes it (such a property does not exist). */
  readonly #heap = new Set<Value>();
  /** What an object literal spreads in, by its token: its properties are theirs too. */
  readonly #spreads = new Map<string, TS.Expression[]>();
  readonly #callers = new Map<TS.SignatureDeclaration, (TS.CallExpression | TS.NewExpression)[]>();
  readonly #flows: { readonly apply: () => void; readonly at: TS.Node }[] = [];
  /** Where a storage first became any string (a flow's place, or nothing in the program writing it). */
  readonly #anyCause = new Map<Value, string>();
  #at: TS.Node | null = null;
  readonly #readers = new Map<Value, Set<number>>();
  readonly #written = new Set<Value>();
  readonly #read = new Set<Value>();
  readonly #queue: number[] = [];
  readonly #queued = new Set<number>();
  #current = -1;

  constructor(ts: typeof TS, checker: TS.TypeChecker, files: readonly TS.SourceFile[], calls: CallTargets, pts: PointsTo) {
    this.#ts = ts;
    this.#checker = checker;
    this.#calls = calls;
    this.#pts = pts;
    // Who calls each function: a write made per call needs them before the flows are gathered.
    for (const sf of files) {
      const visit = (n: TS.Node): void => {
        if (ts.isCallExpression(n) || ts.isNewExpression(n)) for (const f of calls.of(n)) (this.#callers.get(f) ?? this.#callers.set(f, []).get(f)!).push(n);
        ts.forEachChild(n, visit);
      };
      visit(sf);
    }
    for (const sf of files) this.#gather(sf);
    this.#solve();
  }

  /** The strings an expression can be, from the solved flow. */
  of(e: TS.Expression): Strings {
    const v = this.#valueOf(e);
    return { top: v.top, exact: [...v.exact].sort(), prefixes: [...v.prefixes].sort() };
  }

  /** Why an expression can be any string: where any string first flowed into what it reads (null when it cannot be). */
  whyAny(e: TS.Expression): string[] {
    const out: string[] = [];
    for (const s of this.#sources(e)) if (s.top) out.push(this.#anyCause.get(s) ?? 'unknown');
    return out;
  }

  // ---- storage

  #slot(key: TS.Node | string): Value {
    let s = this.#slots.get(key);
    if (!s) this.#slots.set(key, (s = new Value()));
    return s;
  }

  #heapSlot(token: string, property: string): Value {
    const s = this.#slot(`${token}|${property}`);
    this.#heap.add(s);
    return s;
  }

  #readSlot(s: Value): Value {
    if (this.#current >= 0) {
      if (!this.#heap.has(s)) this.#read.add(s);
      (this.#readers.get(s) ?? this.#readers.set(s, new Set()).get(s)!).add(this.#current);
    } else if (!this.#written.has(s) && !this.#heap.has(s)) {
      // Asked after the fixpoint, of a storage nothing in the program writes: it holds what comes from outside.
      return Value.any();
    }
    return s;
  }

  #write(s: Value, v: Value): void {
    this.#written.add(s);
    const wasAny = s.top;
    if (!s.join(v)) return;
    if (s.top && !wasAny) this.#anyCause.set(s, this.#current >= 0 ? place(this.#flows[this.#current]!.at) : 'nothing in the program writes it');
    for (const r of this.#readers.get(s) ?? []) {
      if (this.#queued.has(r)) continue;
      this.#queued.add(r);
      this.#queue.push(r);
    }
  }

  #flow(apply: () => void): void {
    this.#flows.push({ apply, at: this.#at! });
  }

  #solve(): void {
    for (let i = 0; i < this.#flows.length; i++) { this.#queue.push(i); this.#queued.add(i); }
    const runs = new Map<number, number>();
    for (;;) {
      for (let head = 0; head < this.#queue.length; head++) {
        const f = this.#queue[head]!;
        this.#queued.delete(f);
        const n = (runs.get(f) ?? 0) + 1;
        runs.set(f, n);
        if (n > MAX_RUNS) throw new Error(`the flow of string values did not settle at ${place(this.#flows[f]!.at)}`);
        this.#current = f;
        try { this.#flows[f]!.apply(); } finally { this.#current = -1; }
      }
      this.#queue.length = 0;
      // What is read and nothing in the program writes comes from outside it: any string, then the flow goes on.
      let raised = false;
      for (const s of this.#read) if (!this.#written.has(s)) { this.#write(s, Value.any()); raised = true; }
      if (!raised) return;
    }
  }

  // ---- where values flow

  #gather(sf: TS.SourceFile): void {
    const ts = this.#ts;
    const visit = (n: TS.Node): void => {
      this.#at = n;
      if (ts.isVariableDeclaration(n) && n.initializer) {
        const init = n.initializer;
        if (ts.isIdentifier(n.name)) { const s = this.#slot(n); this.#flow(() => this.#write(s, this.#valueOf(init))); }
        this.#bindings(n.name, init);
      }
      if (ts.isParameter(n)) {
        const param = n;
        if (param.initializer) { const s = this.#slot(param); const init = param.initializer; this.#flow(() => this.#write(s, this.#valueOf(init))); }
        this.#bindings(param.name, null);
        // A parameter property (`constructor(readonly mode: string)`) is that property of the objects built.
        if (ts.isIdentifier(param.name) && ts.isParameterPropertyDeclaration(param, param.parent)) {
          const name = param.name.text;
          const s = this.#slot(param);
          const objects = this.#pts.thisOf(param.parent);
          this.#flow(() => { for (const t of objects) this.#write(this.#heapSlot(t, name), this.#readSlot(s)); });
        }
      }
      if (ts.isBinaryExpression(n) && n.operatorToken.kind >= ts.SyntaxKind.FirstAssignment && n.operatorToken.kind <= ts.SyntaxKind.LastAssignment) this.#assignment(n);
      if (ts.isObjectLiteralExpression(n)) this.#objectLiteral(n);
      if (ts.isPropertyDeclaration(n) && n.initializer && !ts.getModifiers(n)?.some((m) => m.kind === ts.SyntaxKind.StaticKeyword)) {
        // A field's initializer is that property of every object the class builds (what `this` is in it).
        const name = staticName(ts, n.name);
        const init = n.initializer;
        if (name !== null) {
          const objects = this.#pts.thisOf(n.parent);
          this.#flow(() => { const v = this.#valueOf(init); for (const t of objects) this.#write(this.#heapSlot(t, name), v); });
        }
      }
      if (ts.isReturnStatement(n) && n.expression) {
        const fn = enclosingFunction(ts, n);
        const e = n.expression;
        if (fn) { const s = this.#slot(fn); this.#flow(() => this.#write(s, this.#valueOf(e))); }
      }
      if (ts.isArrowFunction(n) && !ts.isBlock(n.body)) { const s = this.#slot(n); const body = n.body; this.#flow(() => this.#write(s, this.#valueOf(body))); }
      if (ts.isCallExpression(n) || ts.isNewExpression(n)) this.#call(n);
      if (ts.isForOfStatement(n) && ts.isVariableDeclarationList(n.initializer)) {
        const decl = n.initializer.declarations[0];
        const iterable = n.expression;
        if (decl && ts.isIdentifier(decl.name)) {
          const s = this.#slot(decl);
          this.#flow(() => this.#write(s, this.#elementsOf(iterable) ?? Value.any()));
        }
      }
      ts.forEachChild(n, visit);
    };
    visit(sf);
  }

  /** `const { a, b: c } = v`: each name is that property of what v is (of a parameter: of what calls give it). */
  #bindings(name: TS.BindingName, source: TS.Expression | null): void {
    const ts = this.#ts;
    if (!ts.isObjectBindingPattern(name)) return;
    for (const el of name.elements) {
      const prop = el.propertyName ? staticName(ts, el.propertyName) : ts.isIdentifier(el.name) ? el.name.text : null;
      if (prop !== null && ts.isIdentifier(el.name)) {
        const s = this.#slot(el);
        this.#flow(() => this.#write(s, source ? this.#property(source, prop) : Value.any()));
      }
      if (el.initializer) { const s = this.#slot(el); const init = el.initializer; this.#flow(() => this.#write(s, this.#valueOf(init))); }
      this.#bindings(el.name, null);
    }
  }

  #objectLiteral(o: TS.ObjectLiteralExpression): void {
    const ts = this.#ts;
    const token = this.#pts.literalToken(o);
    for (const p of o.properties) {
      if (ts.isSpreadAssignment(p)) {
        (this.#spreads.get(token) ?? this.#spreads.set(token, []).get(token)!).push(p.expression);
        continue;
      }
      if (!ts.isPropertyAssignment(p) && !ts.isShorthandPropertyAssignment(p)) continue;
      const name = staticName(ts, p.name);
      if (name === null) continue;
      const s = this.#heapSlot(token, name);
      this.#flow(() => this.#write(s, ts.isShorthandPropertyAssignment(p) ? this.#identifier(p.name, this.#checker.getShorthandAssignmentValueSymbol(p)) : this.#valueOf(p.initializer)));
    }
  }

  #assignment(n: TS.BinaryExpression): void {
    const ts = this.#ts;
    const compound = n.operatorToken.kind !== ts.SyntaxKind.EqualsToken;
    const left = n.left;
    // `x += y` is the old value with y after it.
    const value = (): Value => (compound ? this.#concat(this.#valueOf(left), this.#valueOf(n.right)) : this.#valueOf(n.right));
    if (ts.isIdentifier(left)) {
      const decl = this.#declarationOf(left);
      if (decl) { const s = this.#slot(decl); this.#flow(() => this.#write(s, value())); }
    } else if (ts.isPropertyAccessExpression(left)) {
      const name = left.name.text;
      const receiver = left.expression;
      this.#flow(() => { const v = value(); for (const t of this.#madeIn(receiver)) this.#write(this.#heapSlot(t, name), v); });
    } else if (ts.isElementAccessExpression(left)) {
      const key = left.argumentExpression;
      const receiver = left.expression;
      const fn = enclosingFunction(ts, n);
      const ik = fn ? paramIndex(ts, this.#checker, key, fn) : -1;
      const iv = fn && !compound ? paramIndex(ts, this.#checker, n.right, fn) : -1;
      if (fn && ik >= 0 && iv >= 0 && (this.#callers.get(fn)?.length ?? 0) > 0) this.#keyedWrite(fn, ik, iv, receiver, new Set([fn]));
      else this.#flow(() => this.#writeKeyed(receiver, this.#valueOf(key), value()));
    }
  }

  /**
   * A write whose key and value are parameters i and j of `fn`, made at each call of `fn` with that call's arguments; a
   * call that passes on parameters of its own function, called in the program, is followed to that function's calls.
   */
  #keyedWrite(fn: TS.SignatureDeclaration, ik: number, iv: number, receiver: TS.Expression, seen: Set<TS.SignatureDeclaration>): void {
    for (const c of this.#callers.get(fn) ?? []) {
      const args = c.arguments ?? [];
      const key = args[ik];
      const value = args[iv];
      if (!key) continue;
      const g = enclosingFunction(this.#ts, c);
      const jk = g ? paramIndex(this.#ts, this.#checker, key, g) : -1;
      const jv = g && value ? paramIndex(this.#ts, this.#checker, value, g) : -1;
      if (g && jk >= 0 && jv >= 0 && !seen.has(g) && (this.#callers.get(g)?.length ?? 0) > 0) {
        this.#keyedWrite(g, jk, jv, receiver, new Set([...seen, g]));
        continue;
      }
      this.#flow(() => this.#writeKeyed(receiver, this.#valueOf(key), value ? this.#valueOf(value) : new Value()));
    }
  }

  /** `o[k] = v`: the properties k can name of each object o can be (a key of any value: every property). */
  #writeKeyed(receiver: TS.Expression, key: Value, v: Value): void {
    for (const t of this.#madeIn(receiver)) {
      if (key.top || key.prefixes.size > 0) this.#write(this.#heapSlot(t, ANY_KEY), v);
      for (const k of key.exact) this.#write(this.#heapSlot(t, k), v);
    }
  }

  /** A call: each argument flows into the parameter of every function the call runs. */
  #call(c: TS.CallExpression | TS.NewExpression): void {
    const ts = this.#ts;
    const args = c.arguments ?? [];
    for (const fn of this.#calls.of(c)) {
      fn.parameters.forEach((p, i) => {
        const s = this.#slot(p);
        const given = p.dotDotDotToken ? args.slice(i) : args[i] ? [args[i]!] : [];
        for (const a of given) {
          if (ts.isSpreadElement(a)) { this.#flow(() => this.#write(s, Value.any())); continue; }
          this.#flow(() => this.#write(s, this.#valueOf(a)));
        }
      });
    }
  }

  // ---- objects

  /** The objects an expression can be, made in the program; null when it can be one from outside. */
  #objects(e: TS.Expression): readonly string[] | null {
    const of = this.#pts.objectsOf(e, this.#checker);
    return of && !of.outside && of.objects.length > 0 ? of.objects : null;
  }

  /** The objects made in the program an expression can be (where a write to it goes; one from outside is not modelled). */
  #madeIn(e: TS.Expression): readonly string[] {
    return this.#pts.objectsOf(e, this.#checker)?.objects ?? [];
  }

  /** The property `name` of what `receiver` can be. */
  #property(receiver: TS.Expression, name: string, seen = new Set<string>()): Value {
    const objects = this.#objects(receiver);
    if (!objects) return Value.any();
    const out = new Value();
    for (const t of objects) out.join(this.#ofObject(t, name, seen));
    return out;
  }

  /** A property of one object: what is written to it, to any of its keys, and what it spreads in. */
  #ofObject(token: string, name: string, seen: Set<string>): Value {
    const out = new Value();
    if (seen.has(token)) return out;
    seen.add(token);
    out.join(this.#readSlot(this.#heapSlot(token, name)));
    out.join(this.#readSlot(this.#heapSlot(token, ANY_KEY)));
    for (const s of this.#spreads.get(token) ?? []) {
      const objects = this.#objects(s);
      if (!objects) return Value.any();
      for (const t of objects) out.join(this.#ofObject(t, name, seen));
    }
    return out;
  }

  // ---- what an expression can be

  #valueOf(expr: TS.Expression): Value {
    const flow = this.#flowOf(expr);
    const typed = this.#typeValues(expr);
    if (!typed) return flow;
    if (flow.top) return Value.of(...typed);
    // Both bound the value: their meet.
    return Value.of(...typed.filter((t) => flow.exact.has(t) || [...flow.prefixes].some((p) => t.startsWith(p))));
  }

  #flowOf(expr: TS.Expression): Value {
    const ts = this.#ts;
    let e = expr;
    while (ts.isParenthesizedExpression(e) || ts.isNonNullExpression(e) || ts.isSatisfiesExpression(e) || ts.isTypeAssertionExpression(e) || ts.isAsExpression(e)) e = e.expression;
    if (ts.isStringLiteralLike(e)) return Value.of(e.text);
    if (ts.isNumericLiteral(e)) return Value.of(String(Number(e.text)));
    if (!this.#stringish(e)) return new Value();
    if (ts.isTemplateExpression(e)) return this.#template(e);
    if (ts.isIdentifier(e)) return this.#identifier(e, this.#checker.getSymbolAtLocation(e));
    if (ts.isPropertyAccessExpression(e)) {
      const symbol = this.#checker.getSymbolAtLocation(e.name);
      const decl = symbol?.valueDeclaration;
      if (decl && ts.isEnumMember(decl)) return this.#enumValue(decl);
      // A property of the platform's or a package's: what it holds comes from outside.
      if (!symbol || !(symbol.declarations ?? []).some((d) => this.#inProgram(d))) return Value.any();
      // A getter is what it returns.
      const getter = (symbol.declarations ?? []).find((d) => ts.isGetAccessorDeclaration(d));
      if (getter) return this.#readSlot(this.#slot(getter));
      return this.#property(e.expression, e.name.text);
    }
    if (ts.isElementAccessExpression(e)) {
      const elements = this.#elementsOf(e.expression);
      if (elements && !this.#stringish(e.argumentExpression)) return elements;
      const k = this.#valueOf(e.argumentExpression);
      if (k.top || k.prefixes.size > 0) return Value.any();
      const out = new Value();
      for (const name of k.exact) out.join(this.#property(e.expression, name));
      return out;
    }
    if (ts.isCallExpression(e) || ts.isNewExpression(e)) {
      const fns = this.#calls.of(e).filter((f) => (f as TS.FunctionLikeDeclaration).body !== undefined);
      if (fns.length === 0) return Value.any();
      const out = new Value();
      for (const f of fns) out.join(this.#readSlot(this.#slot(f)));
      return out;
    }
    if (ts.isConditionalExpression(e)) return this.#either(e.whenTrue, e.whenFalse);
    if (ts.isBinaryExpression(e)) {
      const k = e.operatorToken.kind;
      if (k === ts.SyntaxKind.QuestionQuestionToken || k === ts.SyntaxKind.BarBarToken || k === ts.SyntaxKind.AmpersandAmpersandToken) return this.#either(e.left, e.right);
      if (k === ts.SyntaxKind.PlusToken) return this.#concat(this.#partOf(e.left), this.#partOf(e.right));
      if (k >= ts.SyntaxKind.FirstAssignment && k <= ts.SyntaxKind.LastAssignment) return this.#valueOf(e.right);
    }
    return Value.any();
  }

  /** The storages an expression reads (to say why it can be any string). */
  #sources(expr: TS.Expression): Value[] {
    const ts = this.#ts;
    let e = expr;
    while (ts.isParenthesizedExpression(e) || ts.isNonNullExpression(e) || ts.isAsExpression(e)) e = e.expression;
    if (ts.isIdentifier(e)) {
      const decl = this.#declarationOf(e);
      return decl ? [this.#slot(decl)] : [];
    }
    if (ts.isPropertyAccessExpression(e)) return (this.#objects(e.expression) ?? []).map((t) => this.#heapSlot(t, e.name.text));
    if (ts.isCallExpression(e)) return this.#calls.of(e).map((f) => this.#slot(f));
    if (ts.isTemplateExpression(e)) return e.templateSpans.flatMap((s) => this.#sources(s.expression));
    return [];
  }

  #either(a: TS.Expression, b: TS.Expression): Value {
    const out = new Value();
    out.join(this.#valueOf(a));
    out.join(this.#valueOf(b));
    return out;
  }

  #identifier(id: TS.Identifier, symbol: TS.Symbol | undefined): Value {
    const ts = this.#ts;
    if (id.text === 'undefined') return new Value();
    const s = symbol && symbol.flags & ts.SymbolFlags.Alias ? this.#checker.getAliasedSymbol(symbol) : symbol;
    const decl = s?.valueDeclaration;
    if (!decl) return Value.any();
    if (ts.isEnumMember(decl)) return this.#enumValue(decl);
    if (ts.isVariableDeclaration(decl) || ts.isParameter(decl) || ts.isBindingElement(decl)) return this.#readSlot(this.#slot(decl));
    return Value.any();
  }

  #declarationOf(id: TS.Identifier): TS.Node | undefined {
    const ts = this.#ts;
    const symbol = this.#checker.getSymbolAtLocation(id);
    const s = symbol && symbol.flags & ts.SymbolFlags.Alias ? this.#checker.getAliasedSymbol(symbol) : symbol;
    return s?.valueDeclaration;
  }

  #enumValue(member: TS.EnumMember): Value {
    const v = this.#checker.getConstantValue(member);
    return v === undefined ? Value.any() : Value.of(String(v));
  }

  /** A part of a template or a `+`: a number of a finite type is its digits, of any value any digits. */
  #partOf(e: TS.Expression): Value {
    if (this.#stringish(e)) return this.#valueOf(e);
    const typed = this.#typeValues(e);
    return typed ? Value.of(...typed) : Value.any();
  }

  #template(e: TS.TemplateExpression): Value {
    let combos = [e.head.text];
    for (const span of e.templateSpans) {
      const part = this.#partOf(span.expression);
      if (part.top || part.prefixes.size > 0) {
        // What follows a part of any value is unknown: the strings start with what came before.
        const out = new Value();
        for (const c of combos) {
          if (part.top) out.prefixes.add(c);
          for (const p of part.prefixes) out.prefixes.add(c + p);
          for (const x of part.exact) out.prefixes.add(c + x);
        }
        return out.prefixes.has('') ? Value.any() : out;
      }
      const next = combos.flatMap((c) => [...part.exact].map((x) => c + x + span.literal.text));
      if (next.length > MAX_COMBINATIONS) {
        const out = new Value();
        for (const c of combos) out.prefixes.add(c);
        return out.prefixes.has('') ? Value.any() : out;
      }
      combos = next;
    }
    return Value.of(...combos);
  }

  #concat(a: Value, b: Value): Value {
    if (a.top) return Value.any();
    const out = new Value();
    for (const p of a.prefixes) out.prefixes.add(p);
    if (b.top || b.prefixes.size > 0) {
      for (const x of a.exact) {
        if (b.top) out.prefixes.add(x);
        for (const p of b.prefixes) out.prefixes.add(x + p);
        for (const y of b.exact) out.prefixes.add(x + y);
      }
    } else if (a.exact.size * b.exact.size > MAX_COMBINATIONS) {
      for (const x of a.exact) out.prefixes.add(x);
    } else {
      for (const x of a.exact) for (const y of b.exact) out.exact.add(x + y);
    }
    return out.prefixes.has('') ? Value.any() : out;
  }

  /** The elements of an array literal an expression is (through `const` variables and `as const`); null when not one. */
  #elementsOf(expr: TS.Expression, seen = new Set<TS.Node>()): Value | null {
    const ts = this.#ts;
    let e = expr;
    while (ts.isParenthesizedExpression(e) || ts.isAsExpression(e) || ts.isSatisfiesExpression(e) || ts.isNonNullExpression(e)) e = e.expression;
    if (seen.has(e)) return null;
    seen.add(e);
    if (ts.isArrayLiteralExpression(e)) {
      const out = new Value();
      for (const x of e.elements) {
        if (ts.isSpreadElement(x)) { const inner = this.#elementsOf(x.expression, seen); out.join(inner ?? Value.any()); }
        else out.join(this.#valueOf(x));
      }
      return out;
    }
    if (ts.isIdentifier(e)) {
      const decl = this.#declarationOf(e);
      if (decl && ts.isVariableDeclaration(decl) && decl.initializer && (ts.getCombinedNodeFlags(decl) & ts.NodeFlags.Const)) return this.#elementsOf(decl.initializer, seen);
    }
    return null;
  }

  /** The values a finite union of literal types allows (null when the type allows any string or number). */
  #typeValues(e: TS.Expression): string[] | null {
    const ts = this.#ts;
    const t = this.#checker.getTypeAtLocation(e);
    const parts = (t.isUnion() ? t.types : [t]).filter((p) => !(p.flags & (ts.TypeFlags.Undefined | ts.TypeFlags.Null | ts.TypeFlags.Void)));
    if (parts.length === 0) return null;
    const out: string[] = [];
    for (const p of parts) {
      if (p.isStringLiteral()) out.push(p.value);
      else if (p.isNumberLiteral()) out.push(String(p.value));
      else return null;
    }
    return out;
  }

  /** Whether an expression can hold a string (its type is a string, any, unknown, or generic). */
  #stringish(e: TS.Expression): boolean {
    const ts = this.#ts;
    const can = (t: TS.Type): boolean => {
      if (t.isUnion() || t.isIntersection()) return t.types.some(can);
      return (t.flags & (ts.TypeFlags.StringLike | ts.TypeFlags.Any | ts.TypeFlags.Unknown | ts.TypeFlags.TypeParameter | ts.TypeFlags.IndexedAccess)) !== 0;
    };
    return can(this.#checker.getTypeAtLocation(e));
  }

  #inProgram(d: TS.Node): boolean {
    const f = d.getSourceFile();
    return !f.isDeclarationFile && !/[\\/]node_modules[\\/]/.test(f.fileName);
  }
}

/** A member name written as a name, a string or a number (null when computed). */
function staticName(ts: typeof TS, name: TS.PropertyName | TS.BindingName): string | null {
  if (ts.isIdentifier(name) || ts.isStringLiteralLike(name) || ts.isNumericLiteral(name) || ts.isPrivateIdentifier(name)) return name.text;
  return null;
}

function enclosingFunction(ts: typeof TS, n: TS.Node): TS.SignatureDeclaration | null {
  for (let p = n.parent; p; p = p.parent) if (ts.isFunctionLike(p)) return p;
  return null;
}

/** Which parameter of `fn` an expression is, as written (−1 when it is not one). */
function paramIndex(ts: typeof TS, checker: TS.TypeChecker, e: TS.Expression, fn: TS.SignatureDeclaration): number {
  let x = e;
  while (ts.isParenthesizedExpression(x) || ts.isAsExpression(x) || ts.isNonNullExpression(x)) x = x.expression;
  if (!ts.isIdentifier(x)) return -1;
  const decl = checker.getSymbolAtLocation(x)?.valueDeclaration;
  return decl && ts.isParameter(decl) && decl.parent === fn ? fn.parameters.indexOf(decl) : -1;
}

/** A node's place: file and line. */
function place(n: TS.Node): string {
  const sf = n.getSourceFile();
  return `${sf.fileName}:${sf.getLineAndCharacterOfPosition(n.getStart(sf)).line + 1}`;
}
