// The translation of one feature: the spec's units are the roots; the file-level functions, constants and indexes
// they use come with them (the feature traced through its own file). Each function gets its C++ signature first (so
// any body can call any other), then its body. An edit is written twice: `edit_x` changes the state (edits call one
// another through these), and the public `x` calls it and rebuilds the indexes once. The indexes point into the
// state's vectors, which an edit may reallocate, so no edit may read an index: that is checked, not assumed.
import type * as TS from 'typescript';
import type { FunctionPlan, MemoPlan, TranslateContext } from './context.ts';
import { paramType, render, type CppType } from './cpptype.ts';
import { pascal, snake, upper } from './names.ts';
import { Emitter } from './statements.ts';

export interface EmittedFunction {
  readonly plan: FunctionPlan;
  /** The declaration, for the header (public) or the anonymous namespace (internal). */
  readonly signature: string;
  readonly lines: string[];
  readonly exported: boolean;
  /** The file it goes in: the edits or the queries. */
  readonly role: 'edit' | 'query';
}

export interface EmittedMemo {
  readonly plan: MemoPlan;
  readonly lines: string[];
}

export class Translator extends Emitter {
  readonly emitted: EmittedFunction[] = [];
  readonly builders: EmittedMemo[] = [];
  readonly #decls = new Map<TS.Symbol, TS.Node>();

  constructor(c: TranslateContext) {
    super(c);
    const ts = c.ts;
    for (const statement of c.file.statements) {
      if (ts.isFunctionDeclaration(statement) && statement.name) this.#decls.set(this.symbolOf(statement.name)!, statement);
      if (ts.isVariableStatement(statement)) {
        for (const d of statement.declarationList.declarations) if (ts.isIdentifier(d.name)) this.#decls.set(this.symbolOf(d.name)!, d);
      }
    }
  }

  /** The top-level declarations the roots reach inside the file, in source order. */
  trace(): TS.Node[] {
    const ts = this.c.ts;
    const byName = new Map([...this.#decls.keys()].map((s) => [s.name, s]));
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
    const pendingNames = new Set(this.c.spec.pending.map((p) => p.unit));
    for (let i = 0; i < queue.length; i++) {
      const visit = (n: TS.Node): void => {
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
    return [...this.#decls].filter(([s]) => reached.has(s)).map(([, d]) => d).sort((a, b) => a.pos - b.pos);
  }

  /** Signatures, constants and indexes of every traced unit (before any body). */
  plan(units: readonly TS.Node[]): void {
    const ts = this.c.ts;
    for (const node of units) {
      const line = this.c.file.getLineAndCharacterOfPosition(node.getStart()).line + 1;
      if (ts.isFunctionDeclaration(node)) {
        this.#planFunction(node, node.name!.text, line, node);
        continue;
      }
      const d = node as TS.VariableDeclaration;
      const name = (d.name as TS.Identifier).text;
      const init = d.initializer;
      if (init && (ts.isArrowFunction(init) || ts.isFunctionExpression(init))) this.#planFunction(init, name, line, d);
      else if (init && ts.isCallExpression(init) && ts.isIdentifier(init.expression) && this.c.spec.memoizers.includes(init.expression.text)) this.#planMemo(d, init, name, line);
      else this.#planConstant(d, name, line);
    }
    this.#checkEditsReadNoIndex();
  }

  /** The declaration's own comment: the last one before it (a section rule above it is not about it). */
  #doc(node: TS.Node): string | null {
    const ranges = this.c.ts.getLeadingCommentRanges(this.c.file.text, node.getFullStart()) ?? [];
    const last = ranges[ranges.length - 1];
    const text = last ? this.c.file.text.slice(last.pos, last.end) : '';
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

  #planFunction(fn: TS.SignatureDeclaration & { body?: TS.ConciseBody }, name: string, line: number, holder: TS.Node): void {
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
      return { name: pname, type: this.c.types.slot(flow.symbolSlot(symbol), 'param'), byValue };
    });
    const ret = edit ? edit.result ?? { k: 'void' as const } : this.c.types.slot(flow.returnSlot(fn), 'return');
    this.c.functions.set(this.symbolOf((holder as TS.VariableDeclaration).name ?? (fn as TS.FunctionDeclaration).name!)!, {
      source: name, cpp: snake(name), decl: fn, params, ret, edit, exported: this.#exported(holder), line,
      doc: this.#doc(this.c.ts.isVariableDeclaration(holder) ? holder.parent.parent : holder),
    });
  }

  #planMemo(d: TS.VariableDeclaration, call: TS.CallExpression, name: string, line: number): void {
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
      fields: fields.map(({ local, name: n, type }) => ({ local, name: n, type })), stores: new Map(), line, doc: this.#doc(d.parent.parent) };
    this.c.memos.set(this.symbolOf(d.name)!, plan);
    for (const f of fields) this.memoLocals.set(f.symbol, f.name);
    // Known with the structs so `stopIndex(t).byId` reads a member; laid out with the index, not as a struct.
    this.c.types.structs.set(plan.struct, { name: plan.struct, comment: null, line: 0,
      fields: plan.fields.map((f) => ({ source: f.local, name: f.name, type: f.type, init: '', comment: null })) });
    this.c.types.indexStructs.add(plan.struct);
  }

  #planConstant(d: TS.VariableDeclaration, name: string, line: number): void {
    const symbol = this.symbolOf(d.name)!;
    const type = this.c.types.slot(this.c.flow.symbolSlot(symbol), 'local');
    const value = d.initializer ? this.expr(d.initializer, type) : '{}';
    const scalar = type.k === 'int' || type.k === 'real' || type.k === 'bool';
    this.c.constants.set(symbol, { source: name, cpp: upper(name), type, value, line, doc: this.#doc(d.parent.parent), constexpr: scalar });
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

  /** Every planned function and index written out. */
  emit(): void {
    for (const plan of this.c.functions.values()) this.#emitFunction(plan);
    for (const memo of this.c.memos.values()) this.#emitMemo(memo);
  }

  #signature(plan: FunctionPlan, name: string): string {
    const params = plan.params.map((p, i) => {
      if (i === 0 && plan.params[0]!.type.k === 'struct' && (plan.params[0]!.type as { name: string }).name === this.c.spec.state.target) {
        return `${plan.edit ? '' : 'const '}${render(p.type)} &${p.name}`;
      }
      return p.byValue ? `${render(p.type)} ${p.name}` : `${paramType(p.type)}${paramType(p.type).endsWith('&') ? '' : ' '}${p.name}`;
    });
    return `${render(plan.ret)} ${name}(${params.join(', ')})`;
  }

  #emitFunction(plan: FunctionPlan): void {
    this.fn = plan;
    this.scopes.push(new Map());
    plan.decl.parameters.forEach((p, i) => {
      const symbol = this.symbolOf(p.name);
      if (symbol) this.bind(symbol, { cpp: plan.params[i]!.name, type: plan.params[i]!.type });
    });
    const first = plan.decl.parameters[0];
    this.state = plan.edit && first ? { symbols: new Set([this.symbolOf(first.name)!]), cpp: plan.params[0]!.name } : null;
    if (this.state) this.collectInlined(plan.decl);
    else this.inlined.clear();
    const body = this.functionBody(plan.decl as TS.FunctionDeclaration, plan.ret);
    this.scopes.pop();
    this.state = null;
    this.fn = null;
    if (plan.edit) {
      this.emitted.push({ plan, signature: this.#signature(plan, `edit_${plan.cpp}`), lines: body, exported: false, role: 'edit' });
      if (!plan.exported) return;
      const call = `edit_${plan.cpp}(${plan.params.map((p) => p.name).join(', ')})`;
      const state = plan.params[0]!.name;
      const wrapper = plan.ret.k === 'void' ? [`\t${call};`, `\treindex(${state});`]
        : [`\tconst ${render(plan.ret)} result = ${call};`, `\treindex(${state});`, '\treturn result;'];
      this.emitted.push({ plan, signature: this.#signature(plan, plan.cpp), lines: wrapper, exported: true, role: 'edit' });
      return;
    }
    this.emitted.push({ plan, signature: this.#signature(plan, plan.cpp), lines: body, exported: plan.exported, role: 'query' });
  }

  #emitMemo(plan: MemoPlan): void {
    this.memo = plan;
    this.scopes.push(new Map());
    const param = plan.fn.parameters[0];
    if (param) this.bind(this.symbolOf(param.name)!, { cpp: snake(param.name.getText()), type: { k: 'struct', name: this.c.spec.state.target } });
    const lines = this.functionBody(plan.fn, { k: 'void' });
    this.scopes.pop();
    this.memo = null;
    this.builders.push({ plan, lines });
  }
}

