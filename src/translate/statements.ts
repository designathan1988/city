// TypeScript statements → C++ statements, with the types the flow solved, and the two lowerings from collected
// objects to owned state:
// - an edit (a function that takes the state and returns a new one: `return { ...t, f: E }`) changes the state in
//   place. A field whose new value only reads that field is changed where it lies (push_back, remove_if, a loop);
//   otherwise every new value is computed first and assigned after, so no expression reads a half-changed state.
//   A loop over the state that edits it walks a snapshot (in the source each edit made a new object).
// - an index builder (identityMemo) fills the index's members in place of the locals it returned.
// A local that names another owner's object or container shares it, as in the source: a reference when it is never
// pointed elsewhere, else the pointer the flow gave it. Nothing is copied that the source shared.
import type * as TS from 'typescript';
import type { TypeScriptApi } from '../graph/typescript.ts';
import type { FunctionPlan } from './context.ts';
import { INT, STRING, declare, elementOf, isNumber, paramDecl, render, target, type CppType } from './cpptype.ts';
import { ExpressionEmitter, RUNTIME_INCLUDE } from './expressions.ts';
import { Own } from './flow.ts';
import { snake } from './names.ts';


/**
 * The names a destructuring binds and the field each reads: `{ a, b: c }` by name, `[t, y]` by position (a tuple's
 * field source is its position); a hole (`[, y]`) binds nothing.
 */
function bindings(ts: TypeScriptApi, pattern: TS.ObjectBindingPattern | TS.ArrayBindingPattern): { el: TS.BindingElement; prop: string }[] {
  if (ts.isObjectBindingPattern(pattern)) return pattern.elements.map((el) => ({ el, prop: (el.propertyName ?? el.name).getText() }));
  return pattern.elements.flatMap((el, i) => (ts.isBindingElement(el) ? [{ el, prop: String(i) }] : []));
}
export class Emitter extends ExpressionEmitter {
  /** The function being emitted (an edit lowers its returns). */
  protected fn: FunctionPlan | null = null;
  /** Inside an edit: the state parameter and the locals that stand for it. */
  protected state: { readonly symbols: Set<TS.Symbol>; readonly cpp: string } | null = null;
  /** Inside an index builder: source locals that are members of the index. */
  protected memoLocals = new Map<TS.Symbol, string>();
  /** Locals of an edit used once, as a new state field's value: that value is written where the field is. */
  protected inlined = new Map<TS.Symbol, TS.Expression>();

  protected functionBody(fn: TS.ArrowFunction | TS.FunctionExpression | TS.FunctionDeclaration, ret: CppType): string[] {
    const ts = this.c.ts;
    const saved = this.indent;
    this.indent += '\t';
    let lines: string[];
    if (fn.body && ts.isBlock(fn.body)) lines = this.block(fn.body.statements);
    else if (fn.body) {
      const code = this.expr(fn.body as TS.Expression, ret);
      lines = [...this.#flush(), `${this.indent}${ret.k === 'void' ? '' : 'return '}${code};`];
    } else lines = [];
    this.indent = saved;
    return lines;
  }

  block(statements: readonly TS.Statement[]): string[] {
    this.scopes.push(new Map());
    const out: string[] = [];
    for (let i = 0; i < statements.length; i++) {
      const merged = this.#getOrInsert(statements[i]!, statements[i + 1]);
      if (merged) {
        out.push(...merged);
        i++;
        continue;
      }
      out.push(...this.statement(statements[i]!));
    }
    this.scopes.pop();
    return out;
  }

  #flush(): string[] {
    const lines = this.prelude.map((l) => `${this.indent}${l}`);
    this.prelude = [];
    return lines;
  }

  #indented(f: () => string[]): string[] {
    const saved = this.indent;
    this.indent += '\t';
    const out = f();
    this.indent = saved;
    return out;
  }

  /** A nested statement (an if's or a loop's body): a block keeps its braces, one statement goes on its own line. */
  #body(s: TS.Statement, head: string): string[] {
    const ts = this.c.ts;
    const inner = this.#indented(() => (ts.isBlock(s) ? this.block(s.statements) : this.statement(s)));
    if (inner.length === 1 && !ts.isBlock(s)) return [`${this.indent}${head}`, inner[0]!];
    return [`${this.indent}${head} {`, ...inner, `${this.indent}}`];
  }

  statement(s: TS.Statement): string[] {
    const lines = this.#statement(s);
    return [...this.#flush(), ...lines];
  }

  #statement(s: TS.Statement): string[] {
    const ts = this.c.ts;
    const at = this.indent;
    if (ts.isBlock(s)) return [`${at}{`, ...this.#indented(() => this.block(s.statements)), `${at}}`];
    if (ts.isVariableStatement(s)) return s.declarationList.declarations.flatMap((d) => this.#declare(d, (s.declarationList.flags & ts.NodeFlags.Const) !== 0));
    if (ts.isExpressionStatement(s)) return this.#expressionStatement(s.expression);
    if (ts.isReturnStatement(s)) return this.#return(s);
    if (ts.isIfStatement(s)) {
      // What the head needs comes before the statement, never inside its body.
      const cond = this.expr(s.expression);
      const head = [...this.#flush(), ...this.#body(s.thenStatement, `if (${cond})`)];
      if (!s.elseStatement) return head;
      if (ts.isIfStatement(s.elseStatement)) {
        const rest = this.#statement(s.elseStatement);
        if (this.prelude.length > 0) return [this.fail(s.elseStatement.expression, 'an else-if condition that needs statements before it').code];
        rest[0] = `${at}else ${rest[0]!.trimStart()}`;
        return [...head, ...rest];
      }
      return [...head, ...this.#body(s.elseStatement, 'else')];
    }
    if (ts.isForStatement(s)) return this.#for(s);
    if (ts.isForOfStatement(s)) return this.#forOf(s);
    if (ts.isWhileStatement(s)) {
      const cond = this.expr(s.expression);
      if (this.prelude.length > 0) return [this.fail(s.expression, 'a loop condition that needs statements before it').code];
      return this.#body(s.statement, `while (${cond})`);
    }
    if (s.kind === ts.SyntaxKind.ContinueStatement) return [`${at}continue;`];
    if (s.kind === ts.SyntaxKind.BreakStatement) return [`${at}break;`];
    if (ts.isEmptyStatement(s)) return [];
    // throw: a library's pattern says what the target does instead (`new Error($m)`); without one, a broken promise of
    // the program stops it with the message (js::fail). C++ exceptions are not JavaScript's and are never written for one
    if (ts.isThrowStatement(s)) {
      if (this.c.patterns.match(s.expression)) return [`${at}${this.expr(s.expression)};`];
      const thrown = s.expression;
      if (ts.isNewExpression(thrown) && /Error$/.test(thrown.expression.getText())) {
        this.c.includes.add(RUNTIME_INCLUDE);
        const text = thrown.arguments?.[0];
        return [`${at}js::fail(${text ? this.expr(text, STRING) : 'std::string()'});`];
      }
      this.fail(s, 'a throw of what is no Error');
      return [];
    }
    if (ts.isSwitchStatement(s)) return this.#switch(s);
    this.fail(s, 'a statement the translator does not know');
    return [];
  }

  // ---- declarations

  #declare(d: TS.VariableDeclaration, isConst: boolean): string[] {
    const ts = this.c.ts;
    const at = this.indent;
    if (ts.isObjectBindingPattern(d.name) || ts.isArrayBindingPattern(d.name)) return this.#destructure(d.name, d.initializer!);
    if (!ts.isIdentifier(d.name)) return [this.fail(d, 'a declaration the translator does not know').code];
    const symbol = this.symbolOf(d.name)!;
    // A local constant keeps its UPPER_CASE name (the target's named-constant form).
    const name = /^[A-Z][A-Z0-9_]*$/.test(d.name.text) ? d.name.text : snake(d.name.text);
    const init = d.initializer;
    if (this.inlined.has(symbol)) return [];
    // An edit's local that holds the state (let out: State = { ...t, ... }) is the state itself, changed in place.
    if (this.state && init && this.#isStateType(this.c.checker.getTypeOfSymbol(symbol))) {
      this.state.symbols.add(symbol);
      return this.#applyState(init);
    }
    // An index builder's returned locals are the index's members.
    const member = this.memoLocals.get(symbol);
    if (member !== undefined) {
      this.bind(symbol, { cpp: `index.${member}`, type: this.memo!.fields.find((f) => f.name === member)!.type });
      return [];
    }
    if (init && (ts.isArrowFunction(init) || ts.isFunctionExpression(init))) {
      const lambda = this.lambda(init);
      this.bind(symbol, { cpp: name, type: lambda.type });
      return [`${at}auto ${name} = ${lambda.code};`];
    }
    const mapped = init && ts.isNewExpression(init) ? this.c.spec.classes[init.expression.getText()] : undefined;
    if (mapped) {
      const companion = mapped.companion ? `${name}${mapped.companion.suffix}` : undefined;
      this.bind(symbol, { cpp: name, type: { k: 'class', name: init!.getText().replace(/^new\s+|\(.*$/g, ''), text: mapped.type }, ...(companion ? { companion } : {}) });
      this.c.includes.add(mapped.include);
      return [`${at}${mapped.type} ${name};`, ...(mapped.companion ? [`${at}${mapped.companion.type} ${companion} = ${mapped.companion.init};`] : [])];
    }
    const slot = this.c.flow.find(this.c.flow.symbolSlot(symbol));
    const type = this.c.types.slot(slot, 'local');
    this.bind(symbol, { cpp: name, type });
    if (!init) return [`${at}${declare(type, name)}${this.#zero(type)};`];
    const v = this.value(init, type);
    const reassigned = this.#reassigned(symbol);
    // Another owner's object or container, never pointed elsewhere and never absent: a reference to it.
    const held = slot.own === Own.Ref && !reassigned && !slot.nullable && v.type.k !== 'ptr'
      && (target(type).k === 'struct' || target(type).k === 'vector' || target(type).k === 'map') && !this.#isValueType(target(type));
    if (held) {
      this.bind(symbol, { cpp: name, type: target(type) });
      return [`${at}${this.#mutated(symbol) ? 'auto' : 'const auto'} &${name} = ${this.coerce(v, target(type), init)};`];
    }
    const code = this.coerce(v, type, init);
    // An empty container needs no initializer.
    if (code === `${render(type)}{}`) return [`${at}${declare(type, name)};`];
    const constant = isConst && !this.#mutated(symbol) && (isNumber(type) || type.k === 'bool' || type.k === 'enum' || this.#isValueType(type) || type.k === 'ptr');
    return [`${at}${constant && type.k !== 'ptr' ? 'const ' : ''}${declare(type, name)} = ${code};`];
  }

  /** `const { a, b } = s`: each a reference to (or a copy of a scalar) field of s. */
  #destructure(pattern: TS.ObjectBindingPattern | TS.ArrayBindingPattern, init: TS.Expression): string[] {
    const at = this.indent;
    const source = this.value(init);
    const access = source.type.k === 'ptr' || source.type.k === 'optional' ? '->' : '.';
    const t = target(source.type);
    const struct = t.k === 'struct' ? this.c.types.structs.get(t.name) : undefined;
    const known = t.k === 'struct' ? Object.values(this.c.spec.types).find((k) => k.type === t.name) : undefined;
    return bindings(this.c.ts, pattern).map(({ el, prop }) => {
      const local = snake(el.name.getText());
      const field = struct?.fields.find((f) => f.source === prop);
      const fieldName = field?.name ?? known?.fields?.[prop];
      if (!fieldName) return this.fail(el, `${render(source.type)} has no field ${prop}`).code;
      const type: CppType = field?.type ?? this.c.types.number(3, 'local');
      const symbol = this.symbolOf(el.name)!;
      this.bind(symbol, { cpp: local, type });
      // an object or array the function changes through this name (h[i] = ...): a mutable reference into the source,
      // which is changed as JavaScript changes the shared array
      const decl = type.k === 'ptr' || isNumber(type) || type.k === 'bool' || type.k === 'enum' ? `${type.k === 'ptr' ? '' : 'const '}${declare(type, local)}`
        : this.#mutated(symbol) ? `${render(type)} &${local}` : `const ${render(type)} &${local}`;
      return `${at}${decl} = ${source.code}${access}${fieldName};`;
    });
  }

  #isValueType(t: CppType): boolean {
    return t.k === 'struct' && Object.values(this.c.spec.types).some((k) => k.type === t.name);
  }

  #zero(type: CppType): string {
    if (isNumber(type)) return ' = 0';
    if (type.k === 'bool') return ' = false';
    if (type.k === 'ptr') return ' = nullptr';
    return '';
  }

  /** The variable is given another value after its declaration (`let x = ...; x = ...`, `x++`). */
  #reassigned(symbol: TS.Symbol): boolean {
    const ts = this.c.ts;
    const decl = symbol.valueDeclaration;
    if (!decl || !ts.isVariableDeclaration(decl) || (decl.parent.flags & ts.NodeFlags.Const) !== 0) return false;
    let found = false;
    const visit = (n: TS.Node): void => {
      if (ts.isBinaryExpression(n) && ts.isIdentifier(n.left) && n.operatorToken.kind >= ts.SyntaxKind.FirstAssignment
        && n.operatorToken.kind <= ts.SyntaxKind.LastAssignment && this.symbolOf(n.left) === symbol) found = true;
      if ((ts.isPrefixUnaryExpression(n) || ts.isPostfixUnaryExpression(n)) && ts.isIdentifier(n.operand) && this.symbolOf(n.operand) === symbol) found = true;
      ts.forEachChild(n, visit);
    };
    visit(this.#scopeOf(decl));
    return found;
  }

  /** Something changes the local's contents after it is declared (a push, a field or element written). */
  #mutated(symbol: TS.Symbol): boolean {
    const ts = this.c.ts;
    const decl = symbol.valueDeclaration;
    if (!decl) return false;
    let found = false;
    const visit = (n: TS.Node): void => {
      if (found) return;
      if (ts.isIdentifier(n) && n !== (decl as TS.VariableDeclaration).name && this.symbolOf(n) === symbol) {
        const p = n.parent;
        if (ts.isPropertyAccessExpression(p) && p.expression === n) {
          const use = p.parent;
          if (ts.isCallExpression(use) && use.expression === p && ['push', 'set', 'add', 'delete', 'sort', 'reverse', 'fill', 'splice', 'pop', 'shift', 'clear'].includes(p.name.text)) found = true;
          if (ts.isBinaryExpression(use) && use.left === p && use.operatorToken.kind >= ts.SyntaxKind.FirstAssignment && use.operatorToken.kind <= ts.SyntaxKind.LastAssignment) found = true;
        }
        if (ts.isElementAccessExpression(p) && p.expression === n && ts.isBinaryExpression(p.parent) && p.parent.left === p) found = true;
      }
      ts.forEachChild(n, visit);
    };
    visit(this.#scopeOf(decl));
    return found;
  }

  #scopeOf(decl: TS.Node): TS.Node {
    let scope: TS.Node = decl;
    while (scope.parent && !this.c.ts.isBlock(scope) && !this.c.ts.isSourceFile(scope) && !this.c.ts.isFunctionLike(scope)) scope = scope.parent;
    return scope;
  }

  // ---- statements

  /**
   * `const l = m.get(k); if (l) l.push(v); else m.set(k, [v]);` is one lookup that makes the entry when absent:
   * `m[k].push_back(v)`.
   */
  #getOrInsert(first: TS.Statement, second: TS.Statement | undefined): string[] | null {
    const ts = this.c.ts;
    if (!second || !ts.isVariableStatement(first) || !ts.isIfStatement(second) || !second.elseStatement) return null;
    const d = first.declarationList.declarations[0];
    if (first.declarationList.declarations.length !== 1 || !d || !ts.isIdentifier(d.name) || !d.initializer) return null;
    const get = d.initializer;
    if (!ts.isCallExpression(get) || !ts.isPropertyAccessExpression(get.expression) || get.expression.name.text !== 'get' || get.arguments.length !== 1) return null;
    const name = d.name.text;
    const unwrap = (s: TS.Statement): TS.Expression | null => {
      const inner = ts.isBlock(s) && s.statements.length === 1 ? s.statements[0]! : s;
      return ts.isExpressionStatement(inner) ? inner.expression : null;
    };
    const push = unwrap(second.thenStatement);
    const set = unwrap(second.elseStatement);
    if (!ts.isIdentifier(second.expression) || second.expression.text !== name || !push || !set) return null;
    if (!ts.isCallExpression(push) || !ts.isPropertyAccessExpression(push.expression) || push.expression.name.text !== 'push'
      || !ts.isIdentifier(push.expression.expression) || push.expression.expression.text !== name || push.arguments.length !== 1) return null;
    if (!ts.isCallExpression(set) || !ts.isPropertyAccessExpression(set.expression) || set.expression.name.text !== 'set'
      || set.expression.expression.getText() !== get.expression.expression.getText() || set.arguments[0]!.getText() !== get.arguments[0]!.getText()) return null;
    const value = set.arguments[1]!;
    if (!ts.isArrayLiteralExpression(value) || value.elements.length !== 1 || value.elements[0]!.getText() !== push.arguments[0]!.getText()) return null;
    const map = this.value(get.expression.expression);
    const mapType = target(map.type);
    if (mapType.k !== 'map') return null;
    const code = `${this.indent}${map.code}[${this.expr(get.arguments[0]!, mapType.key)}].push_back(${this.expr(push.arguments[0]!, elementOf(mapType.of)!)});`;
    return [...this.#flush(), code];
  }

  #expressionStatement(e: TS.Expression): string[] {
    const ts = this.c.ts;
    const at = this.indent;
    // out = edit(out, ...): the edit changes the state in place.
    if (this.state && ts.isBinaryExpression(e) && e.operatorToken.kind === ts.SyntaxKind.EqualsToken && ts.isIdentifier(e.left)) {
      const symbol = this.symbolOf(e.left);
      if (symbol && this.state.symbols.has(symbol)) return this.#applyState(e.right);
    }
    if (ts.isCallExpression(e) && ts.isPropertyAccessExpression(e.expression) && e.expression.name.text === 'forEach') return this.#forEach(e);
    if (ts.isCallExpression(e) && ts.isIdentifier(e.expression)) {
      const plan = this.c.functions.get(this.symbolOf(e.expression)!);
      if (plan?.edit) return [`${at}${this.#editCall(e, plan)};`];
    }
    const code = this.expr(e);
    // A value alone (`chain.reverse();` once its statement is out) does nothing.
    if (/^[A-Za-z_][\w.]*$/.test(code)) return [];
    return [`${at}${code};`];
  }

  /** `items.forEach((x, i) => ...)`: an indexed loop with x a reference to the element. */
  /**
   * switch: an if chain on the value read once. Clauses without statements fall into the next (their tests join); a
   * clause with statements ends in break, return, throw or continue, or it falls through and is refused. A break
   * elsewhere in a clause leaves the switch from the middle: refused too.
   */
  #switch(s: TS.SwitchStatement): string[] {
    const ts = this.c.ts;
    const at = this.indent;
    const value = this.value(s.expression);
    const which = this.temp('which');
    const type = target(value.type);
    const leavesSwitch = (n: TS.Node): boolean => {
      if (n.kind === ts.SyntaxKind.BreakStatement) return (n as TS.BreakStatement).label === undefined;
      if (ts.isIterationStatement(n, false) || ts.isSwitchStatement(n) || ts.isFunctionLike(n)) return false;
      return ts.forEachChild(n, (c) => leavesSwitch(c) || undefined) ?? false;
    };
    const lines = [...this.#flush(), `${at}{`, `${at}\tconst auto &${which} = ${value.code};`];
    let tests: string[] = [];
    let isDefault = false;
    let last: string[] | null = null;
    let first = true;
    this.indent += '\t';
    const clauses = s.caseBlock.clauses;
    clauses.forEach((clause, i) => {
      if (ts.isDefaultClause(clause)) isDefault = true;
      else tests.push(`${which} == ${this.expr(clause.expression, type)}`);
      if (clause.statements.length === 0 && i < clauses.length - 1) return;
      const statements = [...clause.statements];
      const end = statements[statements.length - 1];
      if (end?.kind === ts.SyntaxKind.BreakStatement) statements.pop();
      else if (end && i < clauses.length - 1 && !(ts.isReturnStatement(end) || ts.isThrowStatement(end) || ts.isContinueStatement(end) || (ts.isBlock(end) && this.#endsFlow(end)))) this.fail(clause, 'a switch clause that falls through');
      if (statements.some((x) => leavesSwitch(x))) this.fail(clause, 'a break in the middle of a switch clause');
      const body = this.#indented(() => this.block(ts.factory.createNodeArray(statements) as unknown as TS.NodeArray<TS.Statement>));
      if (isDefault) last = body;
      else {
        lines.push(`${this.indent}${first ? '' : 'else '}if (${tests.join(' || ')}) {`, ...body, `${this.indent}}`);
        first = false;
      }
      tests = [];
      isDefault = false;
    });
    if (last) lines.push(...(first ? last : [`${this.indent}else {`, ...(last as string[]), `${this.indent}}`]));
    this.indent = at;
    lines.push(`${at}}`);
    return lines;
  }

  /** A block that cannot run past its end: its last statement leaves. */
  #endsFlow(b: TS.Block): boolean {
    const ts = this.c.ts;
    const end = b.statements[b.statements.length - 1];
    return end !== undefined && (ts.isReturnStatement(end) || ts.isThrowStatement(end) || ts.isContinueStatement(end) || end.kind === ts.SyntaxKind.BreakStatement);
  }

  #forEach(e: TS.CallExpression): string[] {
    const ts = this.c.ts;
    const at = this.indent;
    const receiver = this.value((e.expression as TS.PropertyAccessExpression).expression);
    const fn = e.arguments[0]!;
    if (!ts.isArrowFunction(fn) && !ts.isFunctionExpression(fn)) return [this.fail(fn, 'forEach of a function not written in place').code];
    const element = elementOf(receiver.type)!;
    const [item, index] = fn.parameters;
    const walked = target(receiver.type);
    if (walked.k === 'map' || walked.k === 'set') {
      // map.forEach((value, key) => ...), set.forEach((member) => ...): in insertion order, as the for-of below
      this.scopes.push(new Map());
      const entry = this.temp('entry');
      const value = item && ts.isIdentifier(item.name) ? snake(item.name.text) : this.temp('value');
      if (item && ts.isIdentifier(item.name)) this.bind(this.symbolOf(item.name)!, { cpp: value, type: element });
      const key = index && ts.isIdentifier(index.name) ? snake(index.name.text) : null;
      if (key && walked.k === 'map') this.bind(this.symbolOf(index!.name)!, { cpp: key, type: walked.key });
      const body = this.#indented(() => [
        `${this.indent}${paramDecl(element, value)} = ${walked.k === 'map' ? `${entry}.second` : entry};`,
        ...(key && walked.k === 'map' ? [`${this.indent}${paramDecl(walked.key, key)} = ${entry}.first;`] : []),
        ...(ts.isBlock(fn.body) ? this.block(fn.body.statements) : [`${this.indent}${this.expr(fn.body)};`]),
      ]);
      this.scopes.pop();
      return [...this.#flush(), `${at}for (const auto &${entry} : ${receiver.code}) {`, ...body, `${at}}`];
    }
    const i = index && ts.isIdentifier(index.name) ? snake(index.name.text) : this.temp('i');
    this.scopes.push(new Map());
    if (index && ts.isIdentifier(index.name)) this.bind(this.symbolOf(index.name)!, { cpp: i, type: INT });
    const itemName = item && ts.isIdentifier(item.name) ? snake(item.name.text) : this.temp('item');
    if (item && ts.isIdentifier(item.name)) this.bind(this.symbolOf(item.name)!, { cpp: itemName, type: element });
    const body = this.#indented(() => [
      `${this.indent}${paramDecl(element, itemName)} = ${receiver.code}[${i}];`,
      ...(ts.isBlock(fn.body) ? this.block(fn.body.statements) : [`${this.indent}${this.expr(fn.body)};`]),
    ]);
    this.scopes.pop();
    return [...this.#flush(), `${at}for (int ${i} = 0; ${i} < int(${receiver.code}.size()); ${i}++) {`, ...body, `${at}}`];
  }

  #for(s: TS.ForStatement): string[] {
    const ts = this.c.ts;
    this.scopes.push(new Map());
    let init = '';
    if (s.initializer && ts.isVariableDeclarationList(s.initializer)) {
      init = s.initializer.declarations.map((d, i) => {
        const line = this.#declare(d, false)[0]!.trim().replace(/;$/, '');
        return i === 0 ? line : line.replace(/^\S+\s+/, '');
      }).join(', ');
    } else if (s.initializer) init = this.expr(s.initializer as TS.Expression);
    const cond = s.condition ? this.expr(s.condition) : '';
    const step = s.incrementor ? this.expr(s.incrementor) : '';
    const pre = this.#flush();
    const out = this.#body(s.statement, `for (${init}; ${cond}; ${step})`);
    this.scopes.pop();
    return [...pre, ...out];
  }

  #forOf(s: TS.ForOfStatement): string[] {
    const ts = this.c.ts;
    const decl = (s.initializer as TS.VariableDeclarationList).declarations[0]!;
    const iterable = s.expression;
    // for (x of m.get(k) ?? []): only when the entry exists, walking it where it lies (one lookup).
    if (ts.isBinaryExpression(iterable) && iterable.operatorToken.kind === ts.SyntaxKind.QuestionQuestionToken
      && ts.isArrayLiteralExpression(iterable.right) && iterable.right.elements.length === 0) {
      const left = iterable.left;
      if (ts.isCallExpression(left) && ts.isPropertyAccessExpression(left.expression) && left.expression.name.text === 'get') {
        const map = this.value(left.expression.expression);
        const mapType = target(map.type);
        if (mapType.k === 'map') {
          const it = this.temp('it');
          const guard = `if (auto ${it} = ${map.code}.find(${this.expr(left.arguments[0]!, mapType.key)}); ${it} != ${map.code}.end())`;
          const pre = this.#flush();
          return [...pre, ...this.#loop(s, decl, { code: `${it}->second`, type: mapType.of }, guard)];
        }
      }
      return [this.fail(iterable, 'a loop over `?? []` of something other than a map entry').code];
    }
    let source = this.value(iterable);
    // The state walked while the body edits it: walk what it was when the loop began.
    if (this.state && this.#readsState(iterable) && this.#editsState(s.statement)) {
      const snapshot = this.temp('snapshot');
      this.prelude.push(`const ${render(target(source.type))} ${snapshot} = ${source.code};`);
      source = { code: snapshot, type: target(source.type) };
    }
    if (ts.isArrayLiteralExpression(iterable)) {
      const element = elementOf(source.type)!;
      source = { code: `{${iterable.elements.map((x) => this.expr(x, element)).join(', ')}}`, type: { k: 'list', of: element } };
    }
    const pre = this.#flush();
    return [...pre, ...this.#loop(s, decl, source, null)];
  }

  #loop(s: TS.ForOfStatement, decl: TS.VariableDeclaration, source: { code: string; type: CppType }, guard: string | null): string[] {
    const ts = this.c.ts;
    const at = this.indent;
    const element = elementOf(source.type)!;
    this.scopes.push(new Map());
    let head: string;
    let destructure: string[] = [];
    const inner = guard ? `${at}\t` : at;
    if (ts.isIdentifier(decl.name)) {
      const name = snake(decl.name.text);
      this.bind(this.symbolOf(decl.name)!, { cpp: name, type: element });
      head = `for (${paramDecl(element, name)} : ${source.code})`;
    } else if (target(source.type).k === 'map') {
      // for (const [key, value] of map): its entries, in insertion order
      const item = this.temp('item');
      const map = target(source.type) as Extract<CppType, { k: 'map' }>;
      head = `for (const auto &${item} : ${source.code})`;
      destructure = bindings(ts, decl.name as TS.ArrayBindingPattern).map(({ el, prop }) => {
        const local = snake(el.name.getText());
        const type = prop === '0' ? map.key : map.of;
        this.bind(this.symbolOf(el.name)!, { cpp: local, type });
        return `${inner}\t${paramDecl(type, local)} = ${item}.${prop === '0' ? 'first' : 'second'};`;
      });
    } else {
      const item = this.temp('item');
      head = `for (${paramDecl(element, item)} : ${source.code})`;
      const t = target(element);
      const struct = t.k === 'struct' ? this.c.types.structs.get(t.name) : undefined;
      const access = element.k === 'ptr' ? '->' : '.';
      destructure = bindings(ts, decl.name as TS.ObjectBindingPattern | TS.ArrayBindingPattern).map(({ el, prop }) => {
        const field = struct?.fields.find((f) => f.source === prop);
        if (!field) return this.fail(el, `${render(element)} has no field ${prop}`).code;
        const local = snake(el.name.getText());
        this.bind(this.symbolOf(el.name)!, { cpp: local, type: field.type });
        return `${inner}\t${paramDecl(field.type, local)} = ${item}${access}${field.name};`;
      });
    }
    const saved = this.indent;
    if (guard) this.indent += '\t';
    let body = this.#body(s.statement, head);
    this.indent = saved;
    this.scopes.pop();
    if (destructure.length > 0) {
      const inside = ts.isBlock(s.statement) ? body.slice(1, -1) : body.slice(1);
      body = [`${inner}${head} {`, ...destructure, ...inside, `${inner}}`];
    }
    return guard ? [`${at}${guard}`, ...body] : body;
  }

  #return(s: TS.ReturnStatement): string[] {
    const ts = this.c.ts;
    const at = this.indent;
    if (this.memo && this.fn === null && !this.#inNested(s)) return []; // the builder filled the index's members
    if (this.state && this.fn?.edit && !this.#inNested(s)) {
      if (!s.expression) return this.#isLast(s) ? [] : [`${at}return;`];
      const e = s.expression;
      const field = this.fn.edit.resultField;
      if (field !== null && ts.isObjectLiteralExpression(e)) {
        const data = e.properties.find((p) => p.name?.getText() === 'data');
        const result = e.properties.find((p) => p.name?.getText() === field);
        const resultExpr = result && ts.isPropertyAssignment(result) ? result.initializer : result && ts.isShorthandPropertyAssignment(result) ? result.name : undefined;
        const value = resultExpr ? this.expr(resultExpr, this.fn.edit.result!) : this.fail(e, 'an edit result without its value').code;
        const applied = data && ts.isPropertyAssignment(data) ? this.#applyState(data.initializer) : [];
        return [...applied, `${at}return ${value};`];
      }
      return [...this.#applyState(e), ...(this.#isLast(s) ? [] : [`${at}return;`])];
    }
    if (!s.expression) return this.#isLast(s) ? [] : [`${at}return;`];
    const fn = this.#enclosing(s);
    const ret = fn ? (this.fn && fn === this.fn.decl ? this.fn.ret : this.returnType(fn)) : undefined;
    const code = this.expr(s.expression, ret);
    return [...this.#flush(), `${at}return ${this.#moveLocals(s.expression, code)};`];
  }

  /** A struct returned from locals takes them: they end with the function (their containers are not copied). */
  #moveLocals(e: TS.Expression, code: string): string {
    const ts = this.c.ts;
    if (!ts.isObjectLiteralExpression(e)) return code;
    let out = code;
    for (const p of e.properties) {
      if (!ts.isShorthandPropertyAssignment(p)) continue;
      const local = this.lookup(this.c.checker.getShorthandAssignmentValueSymbol(p)!);
      const t = local ? target(local.type) : undefined;
      if (local && t && (t.k === 'vector' || t.k === 'map' || t.k === 'string')) out = out.replace(new RegExp(`([{,] ?)${local.cpp}(?=[,}])`), `$1std::move(${local.cpp})`);
    }
    return out;
  }

  /** The statement is the last of its function's body (a `return;` there says nothing). */
  #isLast(s: TS.Statement): boolean {
    const block = s.parent;
    return this.c.ts.isBlock(block) && this.c.ts.isFunctionLike(block.parent) && block.statements[block.statements.length - 1] === s;
  }

  #enclosing(n: TS.Node): TS.SignatureDeclaration | null {
    for (let p = n.parent; p; p = p.parent) if (this.c.ts.isFunctionLike(p)) return p;
    return null;
  }

  #inNested(s: TS.Node): boolean {
    const fn = this.#enclosing(s);
    return this.fn ? fn !== this.fn.decl : this.memo ? fn !== this.memo.fn : true;
  }

  // ---- edits: the new state written in place

  #isStateType(type: TS.Type | undefined): boolean {
    return type !== undefined && (type.aliasSymbol ?? type.getSymbol())?.name === this.c.spec.state.source;
  }

  #readsState(e: TS.Node): boolean {
    let found = false;
    const visit = (n: TS.Node): void => {
      if (this.c.ts.isIdentifier(n)) {
        const s = this.symbolOf(n);
        if (s && this.state!.symbols.has(s)) found = true;
      }
      this.c.ts.forEachChild(n, visit);
    };
    visit(e);
    return found;
  }

  #editsState(n: TS.Node): boolean {
    let found = false;
    const visit = (x: TS.Node): void => {
      if (this.c.ts.isCallExpression(x) && this.c.ts.isIdentifier(x.expression) && this.c.functions.get(this.symbolOf(x.expression)!)?.edit) found = true;
      this.c.ts.forEachChild(x, visit);
    };
    visit(n);
    return found;
  }

  #editCall(e: TS.CallExpression, plan: FunctionPlan): string {
    const args = e.arguments.map((a, i) => (i === 0 ? this.state!.cpp : this.expr(a, plan.params[i]?.type)));
    return `edit_${plan.cpp}(${args.join(', ')})`;
  }

  /** The locals of an edit used once, as the value of a field of the new state it returns: they are that value. */
  protected collectInlined(fn: TS.SignatureDeclaration & { body?: TS.ConciseBody }): void {
    const ts = this.c.ts;
    this.inlined.clear();
    if (!fn.body || !ts.isBlock(fn.body)) return;
    for (const st of fn.body.statements) {
      if (!ts.isVariableStatement(st) || (st.declarationList.flags & ts.NodeFlags.Const) === 0) continue;
      for (const d of st.declarationList.declarations) {
        if (!ts.isIdentifier(d.name) || !d.initializer) continue;
        const symbol = this.symbolOf(d.name)!;
        const uses: TS.Identifier[] = [];
        const visit = (n: TS.Node): void => {
          if (ts.isIdentifier(n) && n !== d.name && this.symbolOf(n) === symbol) uses.push(n);
          ts.forEachChild(n, visit);
        };
        visit(fn.body);
        const use = uses[0];
        if (uses.length !== 1 || !use) continue;
        const holder = use.parent;
        const literal = ts.isShorthandPropertyAssignment(holder) || (ts.isPropertyAssignment(holder) && holder.initializer === use) ? holder.parent : null;
        if (literal && ts.isObjectLiteralExpression(literal) && literal.properties.some((p) => ts.isSpreadAssignment(p)) && ts.isReturnStatement(literal.parent)) {
          this.inlined.set(symbol, d.initializer);
        }
      }
    }
  }

  /** The state becomes the value of `e`, in place. */
  #applyState(e: TS.Expression): string[] {
    const ts = this.c.ts;
    const at = this.indent;
    while (ts.isParenthesizedExpression(e) || ts.isAsExpression(e)) e = e.expression;
    if (ts.isIdentifier(e) && this.state!.symbols.has(this.symbolOf(e)!)) return [];
    if (ts.isCallExpression(e) && ts.isIdentifier(e.expression)) {
      const plan = this.c.functions.get(this.symbolOf(e.expression)!);
      if (plan?.edit) return [...this.#flush(), `${at}${this.#editCall(e, plan)};`];
    }
    if (!ts.isObjectLiteralExpression(e)) return [this.fail(e, 'a new state the translator cannot apply in place').code];
    const [first, ...rest] = e.properties;
    if (!first || !ts.isSpreadAssignment(first) || !this.#readsState(first.expression)) return [this.fail(e, 'a new state that does not start from the state').code];
    const state = this.c.types.structs.get(this.c.spec.state.target)!;
    const fields: { field: (typeof state.fields)[number]; value: TS.Expression }[] = [];
    for (const p of rest) {
      if (!ts.isPropertyAssignment(p) && !ts.isShorthandPropertyAssignment(p)) return [this.fail(p, 'a state member the translator does not know').code];
      const field = state.fields.find((f) => f.source === p.name.getText());
      if (!field) return [this.fail(p, `${state.name} has no field ${p.name.getText()}`).code];
      let value: TS.Expression = ts.isPropertyAssignment(p) ? p.initializer : p.name;
      const inline = ts.isIdentifier(value) ? this.inlined.get(this.symbolOf(value)!) : undefined;
      if (inline) value = inline;
      fields.push({ field, value });
    }
    const changed = new Set(fields.map((u) => u.field.source));
    const cpp = this.state!.cpp;
    const computed: string[] = [];
    const written: string[] = [];
    for (const { field, value } of fields) {
      const othersRead = fields.some((o) => o.field !== field && this.#stateFieldsRead(o.value).has(field.source));
      const inPlace = othersRead ? null : this.#inPlace(value, field.source, `${cpp}.${field.name}`, field.type);
      if (inPlace) {
        written.push(...inPlace);
        continue;
      }
      if ([...this.#stateFieldsRead(value)].some((r) => changed.has(r))) {
        const next = this.temp(`next_${field.name}`);
        const code = this.expr(value, field.type);
        computed.push(...this.#flush(), `${at}${declare(field.type, next)} = ${code};`);
        written.push(`${at}${cpp}.${field.name} = std::move(${next});`);
      } else {
        const code = this.expr(value, field.type);
        written.push(...this.#flush(), `${at}${cpp}.${field.name} = ${code};`);
      }
    }
    return [...this.#flush(), ...computed, ...written];
  }

  #stateFieldsRead(e: TS.Node): Set<string> {
    const out = new Set<string>();
    const visit = (n: TS.Node): void => {
      if (this.c.ts.isPropertyAccessExpression(n) && this.c.ts.isIdentifier(n.expression) && this.state!.symbols.has(this.symbolOf(n.expression)!)) out.add(n.name.text);
      this.c.ts.forEachChild(n, visit);
    };
    visit(e);
    return out;
  }

  /** The new value of one state field written where the field lies, when its form allows (else null). */
  #inPlace(value: TS.Expression, source: string, field: string, type: CppType): string[] | null {
    const ts = this.c.ts;
    const at = this.indent;
    const element = elementOf(type);
    if (!element) return null;
    const isField = (n: TS.Expression): boolean => ts.isPropertyAccessExpression(n) && n.name.text === source && this.#readsState(n.expression);
    // [...t.f, x]: appended.
    if (ts.isArrayLiteralExpression(value) && value.elements.length >= 1 && ts.isSpreadElement(value.elements[0]!) && isField(value.elements[0]!.expression)
      && value.elements.slice(1).every((x) => !ts.isSpreadElement(x))) {
      return value.elements.slice(1).flatMap((x) => {
        const code = this.expr(x as TS.Expression, element);
        return [...this.#flush(), `${at}${field}.push_back(${code});`];
      });
    }
    // t.f.map(...).filter(...): each step where the elements lie.
    const steps: TS.CallExpression[] = [];
    let n: TS.Expression = value;
    while (ts.isCallExpression(n) && ts.isPropertyAccessExpression(n.expression) && ['map', 'filter'].includes(n.expression.name.text)) {
      steps.unshift(n);
      n = n.expression.expression;
    }
    if (steps.length === 0 || !isField(n)) return null;
    const out: string[] = [];
    for (const step of steps) {
      const fn = step.arguments[0]!;
      if ((step.expression as TS.PropertyAccessExpression).name.text === 'filter') {
        this.c.includes.add('algorithm');
        out.push(`${at}${field}.erase(std::remove_if(${field}.begin(), ${field}.end(), ${this.predicate(fn, element, true)}), ${field}.end());`);
        continue;
      }
      const lines = ts.isArrowFunction(fn) ? this.#mapInPlace(fn, field, element) : null;
      if (!lines) return null;
      out.push(...lines);
    }
    return [...this.#flush(), ...out];
  }

  /** x => cond ? { ...x, f: v } : x, or x => ({ ...x, f: v }): the fields changed on the elements themselves. */
  #mapInPlace(fn: TS.ArrowFunction, field: string, element: CppType): string[] | null {
    const ts = this.c.ts;
    const param = fn.parameters[0]?.name;
    if (!param || !ts.isIdentifier(param) || ts.isBlock(fn.body)) return null;
    let body: TS.Expression = fn.body;
    while (ts.isParenthesizedExpression(body) || ts.isAsExpression(body)) body = body.expression;
    let cond: TS.Expression | null = null;
    if (ts.isConditionalExpression(body)) {
      let whenFalse = body.whenFalse;
      while (ts.isParenthesizedExpression(whenFalse)) whenFalse = whenFalse.expression;
      if (!ts.isIdentifier(whenFalse) || whenFalse.text !== param.text) return null;
      cond = body.condition;
      body = body.whenTrue;
      while (ts.isParenthesizedExpression(body) || ts.isAsExpression(body)) body = body.expression;
    }
    if (!ts.isObjectLiteralExpression(body)) return null;
    const [first, ...rest] = body.properties;
    if (!first || !ts.isSpreadAssignment(first) || !ts.isIdentifier(first.expression) || first.expression.text !== param.text) return null;
    const t = target(element);
    const struct = t.k === 'struct' ? this.c.types.structs.get(t.name) : undefined;
    if (!struct) return null;
    const item = snake(param.text);
    this.scopes.push(new Map());
    this.bind(this.symbolOf(param)!, { cpp: item, type: t });
    const sets: string[] = [];
    for (const p of rest) {
      if (!ts.isPropertyAssignment(p) && !ts.isShorthandPropertyAssignment(p)) return null;
      const f = struct.fields.find((x) => x.source === p.name.getText());
      if (!f) return null;
      const value = ts.isPropertyAssignment(p) ? p.initializer : p.name;
      const list = `${item}.${f.name}`;
      // { ...l, stops: l.stops.filter(f) }: the element's own list filtered where it lies.
      if (ts.isCallExpression(value) && ts.isPropertyAccessExpression(value.expression) && value.expression.name.text === 'filter'
        && ts.isPropertyAccessExpression(value.expression.expression) && value.expression.expression.name.text === f.source
        && ts.isIdentifier(value.expression.expression.expression) && value.expression.expression.expression.text === param.text) {
        this.c.includes.add('algorithm');
        sets.push(`${list}.erase(std::remove_if(${list}.begin(), ${list}.end(), ${this.predicate(value.arguments[0]!, elementOf(f.type)!, true)}), ${list}.end());`);
      } else sets.push(`${list} = ${this.expr(value, f.type)};`);
    }
    const test = cond ? this.expr(cond) : null;
    this.scopes.pop();
    const at = this.indent;
    return [`${at}for (${render(t)} &${item} : ${field})${test ? ` if (${test}) {` : ' {'} ${sets.join(' ')} }`];
  }
}
