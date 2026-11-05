// The translation of one feature: the spec's units are the roots; every top-level function, constant and index they
// reach comes with them, in whatever file of the program it is written (shared code included: each goes to its own
// module, once). What the target already owns (the spec's mapped functions and memoizers) is not traced: the target's
// owner is called. Each function gets its C++ signature first (so
// any body can call any other), then its body. An edit is written twice: `edit_x` changes the state (edits call one
// another through these), and the public `x` calls it and rebuilds the indexes once. The indexes point into the
// state's vectors, which an edit may reallocate, so no edit may read an index: that is checked, not assumed.
import type * as TS from 'typescript';
import type { FunctionPlan, MemoPlan, TranslateContext, MemberOwner } from './context.ts';
import { holdsTypeParameter, paramType, render, type CppType } from './cpptype.ts';
import { pascal, snake, upper } from './names.ts';
import { Emitter } from './statements.ts';

export interface EmittedFunction {
  /** A class's member: its declaration is in the struct. */
  readonly member?: boolean;
  readonly plan: FunctionPlan;
  /** The modules (source files) whose structs, constants, indexes or functions its code uses. */
  readonly uses: Set<string>;
  /** The declaration, for the header (public) or the anonymous namespace (internal). */
  readonly signature: string;
  readonly lines: string[];
  readonly exported: boolean;
  /** The file it goes in: the edits or the queries. */
  readonly role: 'edit' | 'query';
}

export interface EmittedMemo {
  readonly plan: MemoPlan;
  readonly uses: Set<string>;
  readonly lines: string[];
}

export class Translator extends Emitter {
  readonly emitted: EmittedFunction[] = [];
  readonly builders: EmittedMemo[] = [];
  readonly #decls = new Map<TS.Symbol, TS.Node>();

  constructor(c: TranslateContext) {
    super(c);
    const ts = c.ts;
    // The target owns these: calls to them are written as the target's (spec), never traced into.
    const mapped = new Set([...Object.keys(c.spec.functions), ...c.spec.memoizers]);
    for (const file of c.sources) {
      for (const statement of file.statements) {
        if (ts.isFunctionDeclaration(statement) && statement.name && !mapped.has(statement.name.text)) this.#decls.set(this.symbolOf(statement.name)!, statement);
        if (ts.isClassDeclaration(statement) && statement.name && !c.spec.classes[statement.name.text]) this.#decls.set(this.symbolOf(statement.name)!, statement);
        if (ts.isVariableStatement(statement)) {
          for (const d of statement.declarationList.declarations) {
            if (ts.isIdentifier(d.name) && !mapped.has(d.name.text)) this.#decls.set(this.symbolOf(d.name)!, d);
          }
        }
      }
    }
  }

  /** A unit the translator itself cannot write refuses that unit, never its module. */
  #guarded(node: TS.Node, write: () => void): void {
    try {
      write();
    } catch (error) {
      this.fail(node, `the translator stopped: ${(error as Error).message.split('\n')[0]}`);
    }
  }

  /** The top-level declarations the roots reach in the whole program, in file and source order. */
  trace(): TS.Node[] {
    const ts = this.c.ts;
    const byName = new Map([...this.#decls].filter(([, d]) => d.getSourceFile() === this.c.file).map(([s]) => [s.name, s]));
    const reached = new Set<TS.Symbol>();
    const queue: TS.Symbol[] = [];
    for (const name of this.c.spec.units) {
      const s = byName.get(name);
      if (!s) {
        this.c.errors.push(`${this.c.spec.source}: no top-level declaration named ${name}`);
        continue;
      }
      reached.add(s);
      queue.push(s);
    }
    for (const id of this.c.spec.shared ?? []) {
      const at = id.indexOf('#');
      const module = id.slice(0, at);
      const name = id.slice(at + 1);
      const found = [...this.#decls].find(([s, d]) => s.name === name && this.c.relative(d.getSourceFile()) === module);
      if (found && !reached.has(found[0])) {
        reached.add(found[0]);
        queue.push(found[0]);
      }
    }
    const pendingNames = new Set(this.c.spec.pending.map((p) => p.unit));
    for (let i = 0; i < queue.length; i++) {
      const visit = (n: TS.Node): void => {
        // an expression a pattern maps is the target's own value (ROAD_TUNING.grade.ramp → tn.grade_ramp): what it
        // names is not a unit to translate; only its placeholders are the feature's values
        const mapped = ts.isExpression(n) && !ts.isIdentifier(n) ? this.c.patterns.match(n) : null;
        if (mapped) { for (const b of mapped.bound.values()) visit(b); return; }
        if (ts.isIdentifier(n)) {
          const s = this.symbolOf(n);
          if (s && this.#decls.has(s) && !reached.has(s)) {
            if (pendingNames.has(s.name)) this.fail(n, `${s.name} is left to its owner (spec.pending) but a translated unit uses it`);
            reached.add(s);
            queue.push(s);
          }
        }
        ts.forEachChild(n, visit);
      };
      visit(this.#decls.get(queue[i]!)!);
    }
    return [...this.#decls].filter(([s]) => reached.has(s)).map(([, d]) => d)
      .sort((a, b) => this.c.relative(a.getSourceFile()).localeCompare(this.c.relative(b.getSourceFile())) || a.pos - b.pos);
  }

  /** Signatures, constants and indexes of every traced unit (before any body). */
  plan(units: readonly TS.Node[]): void {
    const ts = this.c.ts;
    for (const node of units) {
      const line = node.getSourceFile().getLineAndCharacterOfPosition(node.getStart()).line + 1;
      this.#guarded(node, () => {
        if (ts.isFunctionDeclaration(node)) {
          this.#planFunction(node, node.name!.text, line, node);
          return;
        }
        if (ts.isClassDeclaration(node)) {
          this.#planClass(node, line);
          return;
        }
        const d = node as TS.VariableDeclaration;
        const name = (d.name as TS.Identifier).text;
        const init = d.initializer;
        if (init && (ts.isArrowFunction(init) || ts.isFunctionExpression(init))) this.#planFunction(init, name, line, d);
        else if (init && ts.isCallExpression(init) && ts.isIdentifier(init.expression) && this.c.spec.memoizers.includes(init.expression.text)) this.#planMemo(d, init, name, line);
        else this.#planConstant(d, name, line);
      });
    }
    this.#propagateMutability();
    this.#propagateContext();
    this.#constantValues();
    this.#checkEditsReadNoIndex();
  }

  /**
   * The context parameters each function takes: a context pattern it uses (in its body, or through a module constant
   * mapped to one), or a function it calls that takes one, to the fixed point. They follow its own parameters, by
   * name, and every call passes them on (the caller has them as its own).
   */
  #propagateContext(): void {
    const ts = this.c.ts;
    type Ctx = { readonly type: string; readonly name: string };
    const direct = new Map<TS.Symbol, Map<string, Ctx>>();
    const calls = new Map<TS.Symbol, TS.Symbol[]>();
    for (const [key, plan] of this.c.functions) {
      const own = new Map<string, Ctx>();
      const callees: TS.Symbol[] = [];
      const visit = (n: TS.Node): void => {
        if (ts.isIdentifier(n)) {
          const s = this.symbolOf(n);
          const k = s ? this.c.contextConstants.get(s) : undefined;
          if (k) own.set(k.context.name, k.context);
        }
        if (ts.isExpression(n)) {
          const m = this.c.patterns.match(n);
          if (m?.pattern.context) own.set(m.pattern.context.name, m.pattern.context);
        }
        if (ts.isCallExpression(n) && ts.isIdentifier(n.expression)) {
          const s = this.symbolOf(n.expression);
          if (s && this.c.functions.has(s)) callees.push(s);
          const owned = s && !this.c.functions.has(s) ? this.c.spec.functions[n.expression.text]?.context : undefined;
          if (owned) own.set(owned.name, owned);
        }
        ts.forEachChild(n, visit);
      };
      const body = (plan.decl as { body?: TS.Node }).body;
      if (body) visit(body);
      direct.set(key, own);
      calls.set(key, callees);
    }
    for (let changed = true; changed;) {
      changed = false;
      for (const [key, own] of direct) for (const c of calls.get(key) ?? []) for (const [name, ctx] of direct.get(c) ?? []) {
        if (!own.has(name)) { own.set(name, ctx); changed = true; }
      }
    }
    for (const [key, own] of direct) {
      if (own.size === 0) continue;
      const plan = this.c.functions.get(key)!;
      this.c.functions.set(key, { ...plan, context: [...own.values()].sort((a, b) => (a.name < b.name ? -1 : 1)) });
    }
  }

  /**
   * A parameter passed on (whole, or a part of it: `p.h`) to a parameter the callee changes is changed too: taken by
   * mutable reference as well, to the fixed point (JavaScript shares the array all the way down).
   */
  #propagateMutability(): void {
    const ts = this.c.ts;
    const rootOf = (e: TS.Expression): TS.Expression => {
      let x = e;
      while (ts.isPropertyAccessExpression(x) || ts.isElementAccessExpression(x) || ts.isParenthesizedExpression(x) || ts.isNonNullExpression(x)) x = x.expression;
      return x;
    };
    for (let changed = true; changed;) {
      changed = false;
      for (const [key, plan] of this.c.functions) {
        const fn = plan.decl as TS.SignatureDeclaration & { body?: TS.Node };
        if (!fn.body) continue;
        const params = plan.params.map((p, i) => { const symbol = fn.parameters[i] ? this.symbolOf(fn.parameters[i]!.name) : undefined; return { p, symbol, aliases: symbol ? this.#aliasesOf(fn, symbol) : new Set<TS.Symbol>() }; });
        let next = plan.params;
        const visit = (n: TS.Node): void => {
          if (ts.isCallExpression(n) && ts.isIdentifier(n.expression)) {
            const callee = this.symbolOf(n.expression);
            const target = callee ? this.c.functions.get(callee) : undefined;
            if (target) n.arguments.forEach((arg, j) => {
              if (!target.params[j]?.mutable) return;
              const root = rootOf(arg);
              if (!ts.isIdentifier(root)) return;
              const s = this.symbolOf(root);
              const i = params.findIndex((x) => s !== undefined && x.aliases.has(s));
              if (i < 0 || next[i]!.mutable || next[i]!.byValue) return;
              // the state parameter keeps its own rule (an edit changes it, a query does not)
              const q = next[i]!.type;
              if (i === 0 && q.k === 'struct' && q.name === this.c.spec.state.target) return;
              next = next.map((q, k) => (k === i ? { ...q, mutable: true } : q));
            });
          }
          ts.forEachChild(n, visit);
        };
        visit(fn.body);
        if (next !== plan.params) { this.c.functions.set(key, { ...plan, params: next }); changed = true; }
      }
    }
  }

  /** The declaration's own comment: the last one before it (a section rule above it is not about it). */
  /** The context an expression reads inside (a context pattern, or a constant that is one), with its include; null if none. */
  #contextIn(e: TS.Expression): { context: { type: string; name: string }; include: string } | null {
    const ts = this.c.ts;
    let found: { context: { type: string; name: string }; include: string } | null = null;
    const visit = (n: TS.Node): void => {
      if (found) return;
      if (ts.isExpression(n)) {
        const match = this.c.patterns.match(n);
        if (match?.pattern.context) {
          found = { context: match.pattern.context, include: match.pattern.include };
          return;
        }
        const s = ts.isIdentifier(n) ? this.symbolOf(n) : undefined;
        const k = s ? this.c.contextConstants.get(s) : undefined;
        if (k) {
          found = { context: k.context, include: k.include };
          return;
        }
      }
      ts.forEachChild(n, visit);
    };
    visit(e);
    return found;
  }

  #doc(node: TS.Node): string | null {
    const source = node.getSourceFile().text;
    const ranges = this.c.ts.getLeadingCommentRanges(source, node.getFullStart()) ?? [];
    const last = ranges[ranges.length - 1];
    const text = last ? source.slice(last.pos, last.end) : '';
    if (/^\/\/\s*-{4,}/.test(text)) return null;
    const clean = text.replace(/\/\*\*|\*\/|^\s*\*|\/\//gm, ' ').replace(/\s+/g, ' ').trim();
    return clean.length > 0 ? clean : null;
  }

  #isState(type: TS.Type): boolean {
    return (type.aliasSymbol ?? type.getSymbol())?.name === this.c.spec.state.source;
  }

  #exported(holder: TS.Node): boolean {
    const ts = this.c.ts;
    let n: TS.Node | undefined = holder;
    while (n && !ts.isFunctionDeclaration(n) && !ts.isVariableStatement(n)) n = n.parent;
    return n !== undefined && (n.modifiers ?? []).some((m) => m.kind === ts.SyntaxKind.ExportKeyword);
  }

  /**
   * A class: a struct of its fields that declares its members, its instances shared as references are in the source. A subclass, a
   * generic class, a setter and a static field are refused by their unit.
   */
  #planClass(node: TS.ClassDeclaration, line: number): void {
    const ts = this.c.ts;
    const source = node.name!.text;
    this.unit = source;
    if (node.heritageClauses?.some((h) => h.token === ts.SyntaxKind.ExtendsKeyword)) { this.fail(node, 'a class that extends another'); return; }
    if (node.typeParameters?.length) { this.fail(node, 'a generic class'); return; }
    const type = this.c.types.ofType(this.c.checker.getTypeAtLocation(node), 'local', null);
    if (type.k !== 'ref') { this.fail(node, 'a class with no C++ form'); return; }
    const struct = render(type.to);
    const owner = (kind: MemberOwner['kind'], m: TS.ClassElement): MemberOwner => ({
      struct, source, kind, node, isStatic: (ts.getCombinedModifierFlags(m) & ts.ModifierFlags.Static) !== 0 });
    let made = false;
    for (const m of node.members) {
      const at = m.getSourceFile().getLineAndCharacterOfPosition(m.getStart()).line + 1;
      if (ts.isConstructorDeclaration(m) && m.body) {
        made = true;
        this.#planFunction(m, 'constructor', at, m, owner('constructor', m));
      } else if (ts.isMethodDeclaration(m) && m.body) this.#planFunction(m, m.name.getText().replace(/^#/, ''), at, m, owner('method', m));
      else if (ts.isGetAccessorDeclaration(m)) this.#planFunction(m, m.name.getText().replace(/^#/, ''), at, m, owner('getter', m));
      else if (ts.isSetAccessorDeclaration(m)) this.fail(m, 'a setter');
      else if (ts.isClassStaticBlockDeclaration(m)) this.fail(m, 'a static block');
      else if (ts.isPropertyDeclaration(m) && owner('method', m).isStatic) this.fail(m, 'a static field');
    }
    this.unit = source;
    if (made) return;
    // what fills the fields when the source writes no constructor
    const empty = ts.factory.createConstructorDeclaration(undefined, [], ts.factory.createBlock([]));
    (empty as { parent: TS.Node }).parent = node;
    const plan: FunctionPlan = { source: `${source}.constructor`, cpp: struct, decl: empty, params: [], ret: { k: 'void' }, typeParams: [], edit: null, exported: true,
      owner: { struct, source, kind: 'constructor', isStatic: false, node }, module: this.c.relative(node.getSourceFile()), line, doc: null };
    this.c.functions.set(this.symbolOf(node.name!)!, plan);
  }

  #planFunction(fn: TS.SignatureDeclaration & { body?: TS.ConciseBody }, name: string, line: number, holder: TS.Node, owner?: MemberOwner): void {
    this.unit = owner?.source ?? name;
    const checker = this.c.checker;
    const flow = this.c.flow;
    const retType = checker.getSignatureFromDeclaration(fn)!.getReturnType();
    const first = fn.parameters[0];
    const takesState = first !== undefined && this.#isState(checker.getTypeAtLocation(first));
    let edit: FunctionPlan['edit'] = null;
    if (takesState && this.#isState(retType)) edit = { result: null, resultField: null };
    else if (takesState && retType.getProperty('data') && this.#isState(checker.getTypeOfSymbol(retType.getProperty('data')!))) {
      const others = checker.getPropertiesOfType(retType).filter((p) => p.name !== 'data');
      if (others.length !== 1) {
        this.fail(fn, 'an edit that returns more than the new state and one value');
        return;
      }
      const other = others[0]!;
      edit = { result: this.c.types.ofType(checker.getTypeOfSymbol(other), 'param', flow.symbolSlot(other)), resultField: other.name };
    }
    const state: CppType = { k: 'struct', name: this.c.spec.state.target };
    const params = fn.parameters.map((p, i) => {
      const pname = snake(p.name.getText());
      if (i === 0 && takesState) return { name: pname, type: state, byValue: false };
      const symbol = this.symbolOf(p.name)!;
      const declared = checker.getTypeOfSymbol(symbol);
      // An Omit<T> parameter is filled in by the function: taken by value.
      const byValue = declared.aliasSymbol?.name === 'Omit';
      return { name: pname, type: this.c.types.slot(flow.symbolSlot(symbol), 'param'), byValue, mutable: !byValue && this.#contentsChanged(fn, symbol) };
    });
    const ret = edit ? edit.result ?? { k: 'void' as const } : this.c.types.slot(flow.returnSlot(fn), 'return');
    const key = owner?.kind === 'constructor' ? (fn as unknown as { symbol: TS.Symbol }).symbol : this.symbolOf((holder as TS.VariableDeclaration).name ?? (fn as TS.FunctionDeclaration).name!)!;
    this.c.functions.set(key, {
      source: owner ? `${owner.source}.${name}` : name, cpp: owner?.kind === 'constructor' ? owner.struct : snake(name), ...(owner ? { owner } : {}), decl: fn, params, ret, typeParams: (fn.typeParameters ?? []).map((t) => t.name.text), edit, exported: owner !== undefined || this.#exported(holder), module: this.c.relative(fn.getSourceFile()), line,
      doc: this.#doc(this.c.ts.isVariableDeclaration(holder) ? holder.parent.parent : holder),
    });
  }

  #planMemo(d: TS.VariableDeclaration, call: TS.CallExpression, name: string, line: number): void {
    this.unit = name;
    const ts = this.c.ts;
    const fn = call.arguments[0];
    if (!fn || (!ts.isArrowFunction(fn) && !ts.isFunctionExpression(fn))) {
      this.fail(call, 'an index whose builder is not written in place');
      return;
    }
    const returns: TS.Expression[] = [];
    const visit = (n: TS.Node): void => {
      if (n !== fn && ts.isFunctionLike(n)) return;
      if (ts.isReturnStatement(n) && n.expression) returns.push(n.expression);
      ts.forEachChild(n, visit);
    };
    if (ts.isBlock(fn.body)) visit(fn.body);
    if (returns.length !== 1) {
      this.fail(fn, 'an index builder with other than one return');
      return;
    }
    const ret = returns[0]!;
    const locals: TS.Identifier[] = ts.isIdentifier(ret) ? [ret]
      : ts.isObjectLiteralExpression(ret) ? ret.properties.map((p) => (ts.isShorthandPropertyAssignment(p) ? p.name : null)).filter((x): x is TS.Identifier => x !== null) : [];
    if (locals.length === 0 || (ts.isObjectLiteralExpression(ret) && locals.length !== ret.properties.length)) {
      this.fail(ret, 'an index builder must return its locals (a container, or { a, b })');
      return;
    }
    const fields = locals.map((id) => {
      const symbol = this.c.flow.symbolOf(id)!;
      const local = (symbol.valueDeclaration as TS.VariableDeclaration).name as TS.Identifier;
      const localSymbol = this.symbolOf(local)!;
      return { local: id.text, name: snake(id.text), type: this.c.types.slot(this.c.flow.symbolSlot(localSymbol), 'local'), symbol: localSymbol };
    });
    const plan: MemoPlan = { source: name, member: snake(name), struct: pascal(name), fn, single: ts.isIdentifier(ret),
      fields: fields.map(({ local, name: n, type }) => ({ local, name: n, type })), stores: new Map(), writtenIn: this.c.relative(d.getSourceFile()),
      line, doc: this.#doc(d.parent.parent) };
    this.c.memos.set(this.symbolOf(d.name)!, plan);
    for (const f of fields) this.memoLocals.set(f.symbol, f.name);
    // Known with the structs so `stopIndex(t).byId` reads a member; laid out with the index, not as a struct.
    // The indexes of the state belong to the state's owner, wherever the source wrote them.
    this.c.types.structs.set(plan.struct, { name: plan.struct, comment: null, line: 0, module: this.c.relative(this.c.file),
      fields: plan.fields.map((f) => ({ source: f.local, name: f.name, type: f.type, init: '', comment: null })) });
    this.c.types.indexStructs.add(plan.struct);
  }

  #planConstant(d: TS.VariableDeclaration, name: string, line: number): void {
    this.unit = name;
    this.module = this.c.relative(d.getSourceFile());
    // a regular expression: no C++ constant; each use compiles it once where it is tested (#regexTest)
    if (d.initializer?.kind === this.c.ts.SyntaxKind.RegularExpressionLiteral) return;
    // `declare const brand: unique symbol`: a type's mark, no value of the program
    if (!d.initializer) return;
    const symbol = this.symbolOf(d.name)!;
    const type = this.c.types.slot(this.c.flow.symbolSlot(symbol), 'local');
    // a constant whose value is a context pattern (RAMP_GRADE = ROAD_TUNING.grade.ramp, the game's tuning): no C++
    // constant; each reference reads the target expression through the context parameter
    const contextual = d.initializer ? this.c.patterns.match(d.initializer) : null;
    if (contextual?.pattern.context) {
      this.c.contextConstants.set(symbol, { code: this.expr(d.initializer!, type), type, include: contextual.pattern.include, context: contextual.pattern.context });
      return;
    }
    // a value built from a context inside (CLEAR = ROAD_TUNING.grade.clear * 2): a number, a flag or a name is read
    // the same at each use; a table built from it would be a new one at each use (a pointer into it dangles), so the
    // target must own it: the library maps the constant by name to the target's table
    const inner = d.initializer ? this.#contextIn(d.initializer) : null;
    if (inner) {
      const scalar = type.k === 'int' || type.k === 'real' || type.k === 'bool' || type.k === 'enum';
      if (!scalar) {
        this.fail(d, `${name} is a ${render(type)} built from ${inner.context.name} (${inner.context.type}): the target owns it; map ${name} in the library to the target's table`);
        return;
      }
      this.c.contextConstants.set(symbol, { code: `(${this.expr(d.initializer!, type)})`, type, include: inner.include, context: inner.context });
      return;
    }
    // its value is written when every name exists: a constant may read one of a module planned later, or call a function
    this.c.constants.set(symbol, { source: name, cpp: upper(name), type, value: '{}', module: this.c.relative(d.getSourceFile()), line, doc: this.#doc(d.parent.parent), constexpr: false });
    this.#valued.push({ symbol, d, name });
  }

  readonly #valued: { readonly symbol: TS.Symbol; readonly d: TS.VariableDeclaration; readonly name: string }[] = [];

  /** Every constant's value, once all the units are planned; one that calls a function, or reads one that does, comes after the functions. */
  #constantValues(): void {
    for (const { symbol, d, name } of this.#valued) {
      if (!d.initializer) continue;
      this.#guarded(d, () => {
        this.unit = name;
        this.module = this.c.relative(d.getSourceFile());
        this.calledFunction = false;
        const k = this.c.constants.get(symbol)!;
        let value = '{}';
        const uses = this.#recording(() => { value = this.expr(d.initializer!, k.type); });
        const scalar = k.type.k === 'int' || k.type.k === 'real' || k.type.k === 'bool';
        // at namespace scope there is nothing to capture
        this.c.constants.set(symbol, { ...k, value: value.replaceAll('[&]', '[]'), constexpr: scalar && !this.calledFunction && !/\b(std|js)::/.test(value), calling: this.calledFunction, uses });
      });
    }
    // the order C++ initializes constants of different files in is not the source's (the static initialization order
    // problem): one that calls a function, or reads a constant that is not constexpr, is made on first use, by a function
    const named = (value: string, cpp: string): boolean => new RegExp(`\\b${cpp}\\b`).test(value);
    for (let changed = true; changed;) {
      changed = false;
      const dynamic = [...this.c.constants.values()].filter((k) => !k.constexpr);
      for (const [symbol, k] of this.c.constants) {
        if (k.calling || !dynamic.some((d) => d !== k && named(k.value, d.cpp))) continue;
        this.c.constants.set(symbol, { ...k, calling: true, constexpr: false });
        changed = true;
      }
    }
    const made = [...this.c.constants.values()].filter((k) => k.calling).map((k) => k.cpp);
    for (const [symbol, k] of this.c.constants) {
      const value = made.reduce((text, cpp) => text.replace(new RegExp(`\\b${cpp}\\b(?!\\()`, 'g'), `${cpp}()`), k.value);
      if (value !== k.value) this.c.constants.set(symbol, { ...k, value });
    }
  }

  /**
   * An edit must not read an index: the index points into the state's vectors, which the edit may have just
   * reallocated (cppreference: a vector that changes capacity invalidates every pointer to its elements). Every
   * function an edit calls is followed.
   */
  #checkEditsReadNoIndex(): void {
    const ts = this.c.ts;
    const reads = new Map<TS.Node, boolean>();
    const readsIndex = (fn: TS.Node): boolean => {
      const known = reads.get(fn);
      if (known !== undefined) return known;
      reads.set(fn, false);
      let found = false;
      const visit = (n: TS.Node): void => {
        if (found) return;
        if (ts.isCallExpression(n)) {
          if (this.c.flow.isMemoCall(n)) found = true;
          const callee = this.c.flow.calleeOf(n);
          if (callee && !this.c.flow.isMemoCall(n) && readsIndex(callee)) found = true;
        }
        ts.forEachChild(n, visit);
      };
      visit(fn);
      reads.set(fn, found);
      return found;
    };
    for (const plan of this.c.functions.values()) {
      if (plan.edit && readsIndex(plan.decl)) this.fail(plan.decl, `the edit ${plan.source} reads an index, whose pointers its own changes may invalidate`);
    }
  }

  /** Every planned function and index written out, each with the modules its code uses. */
  emit(): void {
    for (const plan of this.c.functions.values()) {
      const from = this.emitted.length;
      const uses = this.#recording(() => this.#guarded(plan.decl, () => this.#emitFunction(plan)));
      for (const e of this.emitted.slice(from)) for (const u of uses) e.uses.add(u);
    }
    for (const memo of this.c.memos.values()) {
      const from = this.builders.length;
      const uses = this.#recording(() => this.#emitMemo(memo));
      for (const b of this.builders.slice(from)) for (const u of uses) b.uses.add(u);
    }
  }

  /** What a piece of emission uses: every struct or enum the types meet, every function, constant or index it calls. */
  #recording(emit: () => void): Set<string> {
    const uses = new Set<string>();
    this.uses = uses;
    this.c.types.observe = (m) => uses.add(m);
    try {
      emit();
    } finally {
      this.uses = null;
      this.c.types.observe = null;
    }
    return uses;
  }

  /** The function changes the contents of a parameter: an element or a field written (=, op=, ++, --), or a mutating
   *  method called on it. Reading it, or passing it on, is not a change. */
  /** A parameter and the names that alias a part of it (`const { h } = profile`, `const h = profile.h`): a change
   *  through any of them is a change of the parameter (JavaScript shares the object). */
  #aliasesOf(fn: TS.SignatureDeclaration, symbol: TS.Symbol): Set<TS.Symbol> {
    const ts = this.c.ts;
    const tracked = new Set<TS.Symbol>([symbol]);
    const rootOf = (e: TS.Expression): TS.Expression => {
      let x = e;
      while (ts.isPropertyAccessExpression(x) || ts.isElementAccessExpression(x) || ts.isParenthesizedExpression(x) || ts.isNonNullExpression(x)) x = x.expression;
      return x;
    };
    const collect = (n: TS.Node): void => {
      if (ts.isVariableDeclaration(n) && n.initializer) {
        const root = rootOf(n.initializer);
        const from = ts.isIdentifier(root) ? this.symbolOf(root) : undefined;
        if (from && tracked.has(from)) {
          if (ts.isObjectBindingPattern(n.name)) for (const el of n.name.elements) { const s = this.symbolOf(el.name); if (s) tracked.add(s); }
          else if (ts.isIdentifier(n.name) && root !== n.initializer) { const s = this.symbolOf(n.name); if (s) tracked.add(s); }
        }
      }
      ts.forEachChild(n, collect);
    };
    const fnBody = (fn as { body?: TS.Node }).body;
    for (let size = -1; fnBody && size !== tracked.size;) { size = tracked.size; collect(fnBody); }
    return tracked;
  }

  #contentsChanged(fn: TS.SignatureDeclaration, symbol: TS.Symbol): boolean {
    const ts = this.c.ts;
    const MUTATORS = ['push', 'set', 'add', 'delete', 'sort', 'reverse', 'fill', 'splice', 'pop', 'shift', 'unshift', 'clear', 'copyWithin'];
    const assigns = (k: TS.SyntaxKind): boolean => k >= ts.SyntaxKind.FirstAssignment && k <= ts.SyntaxKind.LastAssignment;
    const written = (target: TS.Node): boolean => {
      const p = target.parent;
      if (ts.isBinaryExpression(p) && p.left === target && assigns(p.operatorToken.kind)) return true;
      return (ts.isPrefixUnaryExpression(p) || ts.isPostfixUnaryExpression(p)) && (p.operator === ts.SyntaxKind.PlusPlusToken || p.operator === ts.SyntaxKind.MinusMinusToken);
    };
    const tracked = this.#aliasesOf(fn, symbol);
    let found = false;
    const visit = (n: TS.Node): void => {
      if (found) return;
      const s = ts.isIdentifier(n) ? this.symbolOf(n) : undefined;
      if (s && tracked.has(s) && !(ts.isParameter(n.parent) && n.parent.name === n)) {
        const p = n.parent;
        if ((ts.isElementAccessExpression(p) || ts.isPropertyAccessExpression(p)) && p.expression === n) {
          if (written(p)) found = true;
          if (ts.isPropertyAccessExpression(p) && ts.isCallExpression(p.parent) && p.parent.expression === p && MUTATORS.includes(p.name.text)) found = true;
        }
      }
      ts.forEachChild(n, visit);
    };
    const body = (fn as { body?: TS.Node }).body;
    if (body) visit(body);
    return found;
  }

  #signature(plan: FunctionPlan, name: string): string {
    const params = plan.params.map((p, i) => {
      if (i === 0 && plan.params[0]!.type.k === 'struct' && (plan.params[0]!.type as { name: string }).name === this.c.spec.state.target) {
        return `${plan.edit ? '' : 'const '}${render(p.type)} &${p.name}`;
      }
      if (p.mutable) return `${render(p.type)} &${p.name}`;
      // a callback says nothing of T: T comes from the other arguments (a lambda is not a std::function to deduce from)
      if (p.type.k === 'function' && holdsTypeParameter(p.type)) return `const js::same<${render(p.type)}> &${p.name}`;
      return p.byValue ? `${render(p.type)} ${p.name}` : `${paramType(p.type)}${paramType(p.type).endsWith('&') ? '' : ' '}${p.name}`;
    });
    // context parameters after its own (const Tuning &tn)
    for (const c of plan.context ?? []) params.push(`${c.type}${c.type.endsWith('&') || c.type.endsWith('*') ? '' : ' '}${c.name}`);
    const head = plan.typeParams.length > 0 ? `template <${plan.typeParams.map((t) => `class ${t}`).join(', ')}> ` : '';
    if (plan.owner?.kind === 'constructor') return `${name}(${params.join(', ')})`;
    return `${head}${render(plan.ret)} ${name}(${params.join(', ')})`;
  }

  #emitFunction(plan: FunctionPlan): void {
    this.unit = plan.owner?.source ?? plan.source;
    this.fn = plan;
    this.module = plan.module;
    this.scopes.push(new Map());
    plan.decl.parameters.forEach((p, i) => {
      const symbol = this.symbolOf(p.name);
      if (symbol) this.bind(symbol, { cpp: plan.params[i]!.name, type: plan.params[i]!.type });
    });
    const first = plan.decl.parameters[0];
    this.state = plan.edit && first ? { symbols: new Set([this.symbolOf(first.name)!]), cpp: plan.params[0]!.name } : null;
    if (this.state) this.collectInlined(plan.decl);
    else this.inlined.clear();
    const fields = plan.owner?.kind === 'constructor' ? this.#fieldLines(plan.owner, plan) : [];
    const body = [...fields, ...this.functionBody(plan.decl as TS.FunctionDeclaration, plan.ret)];
    this.scopes.pop();
    this.state = null;
    this.fn = null;
    if (plan.owner) {
      this.#emitMember(plan, plan.owner, body);
      return;
    }
    if (plan.edit) {
      this.emitted.push({ plan, signature: this.#signature(plan, `edit_${plan.cpp}`), lines: body, exported: false, role: 'edit', uses: new Set() });
      if (!plan.exported) return;
      const call = `edit_${plan.cpp}(${plan.params.map((p) => p.name).join(', ')})`;
      const state = plan.params[0]!.name;
      const wrapper = plan.ret.k === 'void' ? [`\t${call};`, `\treindex(${state});`]
        : [`\tconst ${render(plan.ret)} result = ${call};`, `\treindex(${state});`, '\treturn result;'];
      this.emitted.push({ plan, signature: this.#signature(plan, plan.cpp), lines: wrapper, exported: true, role: 'edit', uses: new Set() });
      return;
    }
    this.emitted.push({ plan, signature: this.#signature(plan, plan.cpp), lines: body, exported: plan.exported, role: 'query', uses: new Set() });
  }

  /** The fields a constructor fills before its own body: what each is declared with, and what its parameters declare. */
  #fieldLines(owner: MemberOwner, plan: FunctionPlan): string[] {
    const ts = this.c.ts;
    const struct = this.c.types.structs.get(owner.struct)!;
    const lines: string[] = [];
    const put = (name: string, value: (type: CppType) => string): void => {
      const field = struct.fields.find((f) => f.source === name);
      if (field) lines.push(`\tthis->${field.name} = ${value(field.type)};`);
    };
    for (const m of owner.node.members) {
      if (!ts.isPropertyDeclaration(m) || !m.initializer || (m.modifiers ?? []).some((x) => x.kind === ts.SyntaxKind.StaticKeyword)) continue;
      const init = m.initializer;
      put(m.name.getText().replace(/^#/, ''), (type) => this.expr(init, type));
    }
    plan.decl.parameters.forEach((p, i) => {
      if ((p.modifiers ?? []).length > 0) put(p.name.getText(), () => plan.params[i]!.name);
    });
    return lines;
  }

  /** A member: declared in its struct, defined by its qualified name. */
  #emitMember(plan: FunctionPlan, owner: MemberOwner, body: string[]): void {
    const struct = this.c.types.structs.get(owner.struct)!;
    (struct.methods ??= []).push(`${owner.isStatic ? 'static ' : ''}${this.#signature(plan, plan.cpp)};`);
    (struct.methodTypes ??= []).push(...plan.params.map((p) => p.type), plan.ret);
    this.emitted.push({ plan, signature: this.#signature(plan, `${owner.struct}::${plan.cpp}`), lines: body, exported: false, role: 'query', uses: new Set(), member: true });
  }

  #emitMemo(plan: MemoPlan): void {
    this.unit = plan.source;
    this.memo = plan;
    this.module = this.c.relative(this.c.file);
    this.scopes.push(new Map());
    const param = plan.fn.parameters[0];
    if (param) this.bind(this.symbolOf(param.name)!, { cpp: snake(param.name.getText()), type: { k: 'struct', name: this.c.spec.state.target } });
    const lines = this.functionBody(plan.fn, { k: 'void' });
    this.scopes.pop();
    this.memo = null;
    this.builders.push({ plan, lines, uses: new Set() });
  }
}

