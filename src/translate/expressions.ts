// TypeScript expressions → C++ expressions. Every type comes from the solved flow (through the type mapper): this
// file infers nothing. It writes what each construct is in the target: member access by what the value is (`.` or
// `->`), a value converted where it lands (a pointer taken or read, an optional filled, null as nullptr or nullopt),
// array methods as loops or <algorithm> (sort is stable in JavaScript since ES2019: std::stable_sort), Map.get as a
// pointer to the entry, Math with JavaScript's own rounding (halves toward +∞, not std::round's away from zero), a
// concept the target owns as the owner's call (spec.patterns), a literal the target does not allow bare as its name.
import type * as TS from 'typescript';
import type { Local, MemoPlan, TranslateContext } from './context.ts';
import {
  BOOL, INT, INT64, LITERAL, NULL, STRING, declare, elementOf, isInt, isNumber, paramDecl, realLiteral, render, same, target,
  widerNumber, type CppType,
} from './cpptype.ts';
import { Num } from './flow.ts';
import { snake } from './names.ts';

export interface Value {
  readonly code: string;
  readonly type: CppType;
}

const INT_MATH = new Set(['floor', 'ceil', 'trunc']);
const REAL_MATH = new Set(['hypot', 'sqrt', 'sin', 'cos', 'tan', 'atan2', 'atan', 'asin', 'acos', 'exp', 'log', 'pow']);

export abstract class ExpressionEmitter {
  protected readonly c: TranslateContext;
  protected readonly scopes: Map<TS.Symbol, Local>[] = [new Map()];
  /** Statements the current statement needs first (a sort before a loop over its result). */
  protected prelude: string[] = [];
  protected indent = '';
  /** Inside an index builder: objects it files by reference are stored in the index. */
  protected memo: MemoPlan | null = null;
  /** The modules the code being written uses (a function, a constant, an index of another module). */
  protected uses: Set<string> | null = null;
  /** The module whose code is being written (where a literal's constant goes). */
  protected module = '';
  #temp = 0;

  constructor(c: TranslateContext) {
    this.c = c;
  }

  /** The lines of a function body (block or expression) returning `ret`. */
  protected abstract functionBody(fn: TS.ArrowFunction | TS.FunctionExpression | TS.FunctionDeclaration, ret: CppType): string[];

  protected temp(base: string): string {
    return `${base}_${++this.#temp}`;
  }

  protected bind(symbol: TS.Symbol, local: Local): void {
    this.scopes[this.scopes.length - 1]!.set(symbol, local);
  }

  protected lookup(symbol: TS.Symbol): Local | undefined {
    for (let i = this.scopes.length - 1; i >= 0; i--) {
      const found = this.scopes[i]!.get(symbol);
      if (found) return found;
    }
    return undefined;
  }

  protected symbolOf(node: TS.Node): TS.Symbol | undefined {
    return this.c.flow.symbolOf(node);
  }

  protected fail(node: TS.Node, why: string): Value {
    const { line } = node.getSourceFile().getLineAndCharacterOfPosition(node.getStart());
    this.c.errors.push(`${this.c.spec.source}:${line + 1}: ${why}: ${node.getText().slice(0, 80)}`);
    return { code: '/*untranslated*/', type: NULL };
  }

  /** The number type the flow solved for an expression. */
  protected numberType(e: TS.Expression): CppType {
    const n = this.c.flow.numOf(e);
    return this.c.types.number(n === Num.None ? Num.Float : n, 'local');
  }

  // ---- conversion where a value lands

  /** The expression converted to `want` (a pointer taken or read, an optional filled, null as nullptr or nullopt). */
  expr(e: TS.Expression, want?: CppType): string {
    return this.coerce(this.value(e, want), want, e);
  }

  protected coerce(v: Value, want: CppType | undefined, e?: TS.Expression): string {
    if (!want) return v.code;
    if (v.type.k === 'null') return want.k === 'ptr' ? 'nullptr' : want.k === 'optional' ? 'std::nullopt' : `${render(want)}{}`;
    if (want.k === 'ptr' && v.type.k !== 'ptr' && same(want.to, target(v.type))) return `&${this.#lvalue(v.code)}`;
    if (want.k !== 'ptr' && want.k !== 'optional' && v.type.k === 'ptr' && same(want, v.type.to)) return `*${v.code}`;
    if (want.k === 'optional' && v.type.k === 'ptr' && same(want.of, v.type.to)) return `(${v.code} ? std::optional<${render(want.of)}>(*${v.code}) : std::nullopt)`;
    if (want.k === 'string' && v.type.k === 'literal') return `std::string(${v.code})`;
    if (e && want.k === 'enum' && this.c.ts.isStringLiteral(e)) return this.#enumLiteral(want.name, e);
    const structural = this.#structural(v, want);
    if (structural) return structural;
    return v.code;
  }

  /**
   * TypeScript is structural: a value of a wider shape is passed where a narrower one is asked (a post with x and y
   * where a point is asked). In C++ the asked type is built from the value's same-named fields.
   */
  #structural(v: Value, want: CppType): string | null {
    const to = want.k === 'optional' ? want.of : want;
    const from = target(v.type);
    if (to.k !== 'struct' || from.k !== 'struct' || to.name === from.name) return null;
    const fieldsOf = (name: string): { source: string; cpp: string }[] | null => {
      const known = Object.values(this.c.spec.types).find((k) => k.type === name);
      if (known?.fields) return Object.entries(known.fields).map(([source, cpp]) => ({ source, cpp }));
      const s = this.c.types.structs.get(name);
      return s ? s.fields.map((f) => ({ source: f.source, cpp: f.name })) : null;
    };
    const wanted = fieldsOf(to.name);
    const given = fieldsOf(from.name);
    if (!wanted || !given) return null;
    const byName = new Map(given.map((f) => [f.source, f.cpp]));
    if (!wanted.every((f) => byName.has(f.source))) return null;
    const access = v.type.k === 'ptr' || v.type.k === 'optional' ? '->' : '.';
    const known = Object.values(this.c.spec.types).some((k) => k.type === to.name);
    const build = (base: string): string => {
      const args = wanted.map((f) => `${base}${access}${byName.get(f.source)}`).join(', ');
      return known ? `${to.name}(${args})` : `${to.name}{${args}}`;
    };
    // A value read once: a name is read in place, anything else through a temporary.
    if (/^[A-Za-z_][\w.]*$/.test(v.code)) return build(v.code);
    const tmp = this.temp('from');
    return `[&] { const auto &${tmp} = ${v.code}; return ${build(tmp)}; }()`;
  }

  #lvalue(code: string): string {
    return /^[\w.\->[\]]+$/.test(code) ? code : `(${code})`;
  }

  #enumLiteral(name: string, e: TS.StringLiteral): string {
    const en = this.c.types.enums.get(name);
    if (!en || !en.values.includes(e.text)) return this.fail(e, `'${e.text}' is not a value of ${name}`).code;
    return this.c.types.enumValue(en, e.text);
  }

  // ---- expressions

  value(e: TS.Expression, want?: CppType): Value {
    const ts = this.c.ts;
    const match = this.c.patterns.match(e);
    if (match) {
      this.c.includes.add(match.pattern.include);
      return { code: match.pattern.target.replace(/\$(\w+)/g, (_, n: string) => this.expr(match.bound.get(`$${n}`)!)), type: INT64 };
    }
    if (ts.isParenthesizedExpression(e)) {
      const inner = this.value(e.expression, want);
      return { code: `(${inner.code})`, type: inner.type };
    }
    if (ts.isNonNullExpression(e)) {
      const inner = this.value(e.expression, want);
      return inner.type.k === 'optional' ? { code: `(*${inner.code})`, type: inner.type.of } : inner;
    }
    if (ts.isAsExpression(e) || ts.isSatisfiesExpression(e) || ts.isTypeAssertionExpression(e)) return this.value(e.expression, want);
    if (ts.isNumericLiteral(e)) return this.#number(e);
    if (ts.isStringLiteral(e) || ts.isNoSubstitutionTemplateLiteral(e)) {
      if (want?.k === 'enum') return { code: this.#enumLiteral(want.name, e as TS.StringLiteral), type: want };
      return { code: JSON.stringify(e.text), type: LITERAL };
    }
    if (ts.isTemplateExpression(e)) return this.#template(e);
    if (e.kind === ts.SyntaxKind.TrueKeyword) return { code: 'true', type: BOOL };
    if (e.kind === ts.SyntaxKind.FalseKeyword) return { code: 'false', type: BOOL };
    if (e.kind === ts.SyntaxKind.NullKeyword) return { code: 'nullptr', type: NULL };
    if (ts.isIdentifier(e)) return this.#identifier(e);
    if (ts.isPropertyAccessExpression(e)) return this.#property(e);
    if (ts.isElementAccessExpression(e)) {
      const obj = this.value(e.expression);
      const element = elementOf(obj.type);
      if (!element) return this.fail(e, `an element of ${render(obj.type)}`);
      return { code: `${obj.code}[${this.expr(e.argumentExpression)}]`, type: element };
    }
    if (ts.isCallExpression(e)) return this.call(e, want);
    if (ts.isNewExpression(e)) return this.#construct(e, want);
    if (ts.isPrefixUnaryExpression(e)) return this.#prefix(e);
    if (ts.isPostfixUnaryExpression(e)) {
      const v = this.value(e.operand as TS.Expression);
      return { code: `${v.code}${e.operator === ts.SyntaxKind.PlusPlusToken ? '++' : '--'}`, type: v.type };
    }
    if (ts.isBinaryExpression(e)) return this.#binary(e, want);
    if (ts.isConditionalExpression(e)) return this.#conditional(e, want);
    if (ts.isObjectLiteralExpression(e)) return this.object(e, want);
    if (ts.isArrayLiteralExpression(e)) return this.#array(e, want);
    if (ts.isArrowFunction(e) || ts.isFunctionExpression(e)) return this.lambda(e);
    return this.fail(e, 'no C++ form for this expression');
  }

  #number(e: TS.NumericLiteral): Value {
    const value = Number(e.text);
    const int = /^\d+$/.test(e.getText());
    const type = int ? INT : this.c.types.number(Num.Float, 'local');
    const text = int ? e.getText() : realLiteral(e.getText(), this.c.spec.number);
    if (this.#inConstant(e) || this.c.allowedNumbers.includes(value)) return { code: text, type };
    // A number the target's gates do not allow bare: a constant of this module, named from what receives it.
    const name = this.c.literals.name(e, value, this.module);
    const key = `${this.module}|${name}`;
    if (!this.c.literalConstants.has(key)) {
      const line = e.getSourceFile().getLineAndCharacterOfPosition(e.getStart()).line + 1;
      this.c.literalConstants.set(key, { source: e.getText(), cpp: name, type, value: text, module: this.module, line,
        doc: `The ${e.getText()} the source writes here`, constexpr: true });
    }
    return { code: name, type };
  }

  /** A literal in the initializer of a named constant is that constant's value. */
  #inConstant(e: TS.Node): boolean {
    for (let n: TS.Node | undefined = e.parent; n; n = n.parent) {
      if (this.c.ts.isVariableDeclaration(n)) return this.c.ts.isIdentifier(n.name) && /^[A-Z][A-Z0-9_]*$/.test(n.name.text);
      if (this.c.ts.isFunctionLike(n) || this.c.ts.isBlock(n)) return false;
    }
    return false;
  }

  #template(e: TS.TemplateExpression): Value {
    const parts: string[] = [];
    if (e.head.text) parts.push(`std::string(${JSON.stringify(e.head.text)})`);
    for (const span of e.templateSpans) {
      const v = this.value(span.expression, STRING);
      if (isInt(v.type)) parts.push(`std::to_string(${v.code})`);
      else if (v.type.k === 'string') parts.push(v.code);
      else if (v.type.k === 'literal') parts.push(`std::string(${v.code})`);
      else return this.fail(span.expression, 'a template part that is not an integer or a string (JavaScript formats numbers its own way)');
      if (span.literal.text) parts.push(JSON.stringify(span.literal.text));
    }
    if (parts.length === 0) return { code: 'std::string()', type: STRING };
    if (!parts[0]!.startsWith('std::')) parts[0] = `std::string(${parts[0]})`;
    this.c.includes.add('string');
    return { code: parts.join(' + '), type: STRING };
  }

  #identifier(e: TS.Identifier): Value {
    if (e.text === 'undefined') return { code: 'nullptr', type: NULL };
    if (e.text === 'Infinity') {
      this.c.includes.add('limits');
      return { code: `std::numeric_limits<${this.c.spec.number}>::infinity()`, type: this.c.types.number(Num.Float, 'local') };
    }
    const symbol = this.symbolOf(e);
    if (!symbol) return this.fail(e, 'an unresolved name');
    const local = this.lookup(symbol);
    if (local) return { code: local.cpp, type: local.type };
    const constant = this.c.constants.get(symbol);
    if (constant) {
      if (constant.module) this.uses?.add(constant.module);
      return { code: constant.cpp, type: constant.type };
    }
    const fn = this.c.functions.get(symbol);
    if (fn) return { code: fn.cpp, type: { k: 'lambda', ret: fn.ret } };
    return this.fail(e, 'a name outside the feature that the spec does not map');
  }

  #property(e: TS.PropertyAccessExpression): Value {
    const ts = this.c.ts;
    const name = e.name.text;
    // xs.filter(p).length counts: no list is built.
    if (name === 'length' && ts.isCallExpression(e.expression) && ts.isPropertyAccessExpression(e.expression.expression)
      && e.expression.expression.name.text === 'filter') {
      const source = this.value(e.expression.expression.expression);
      const element = elementOf(source.type)!;
      this.c.includes.add('algorithm');
      return { code: `int(std::count_if(${source.code}.begin(), ${source.code}.end(), ${this.predicate(e.expression.arguments[0]!, element)}))`, type: INT };
    }
    const obj = this.value(e.expression);
    const t = target(obj.type);
    if (name === 'length' && (t.k === 'vector' || t.k === 'string')) return { code: `int(${obj.code}.size())`, type: INT };
    // A Map's or a Set's size (a Set is a vector here: its members in order).
    if (name === 'size' && (t.k === 'map' || t.k === 'vector')) return { code: `int(${obj.code}.size())`, type: INT };
    const access = obj.type.k === 'ptr' || obj.type.k === 'optional' ? '->' : '.';
    if (t.k === 'class') return this.#classMember(e, t.name, obj, name, []);
    if (t.k === 'struct') {
      const known = Object.values(this.c.spec.types).find((k) => k.type === t.name);
      if (known?.fields) {
        const field = known.fields[name];
        if (!field) return this.fail(e, `${t.name} has no field for ${name} in the spec`);
        return { code: `${obj.code}${access}${field}`, type: this.c.types.number(Num.Float, 'local') };
      }
      const struct = this.c.types.structs.get(t.name);
      const field = struct?.fields.find((f) => f.source === name);
      if (!field) return this.fail(e, `${t.name} has no field ${name}`);
      return { code: `${obj.code}${access}${field.name}`, type: field.type };
    }
    return this.fail(e, `a member of ${render(obj.type)} the translator does not know`);
  }

  #classMember(e: TS.Node, className: string, obj: Value, member: string, args: readonly TS.Expression[]): Value {
    const mapping = this.c.spec.classes[className]!;
    const template = mapping.members[member];
    if (!template) return this.fail(e, `the spec maps no member ${member} of ${className}`);
    const code = template.replace(/\$self/g, obj.code).replace(/\$seq/g, `${obj.code}${mapping.companion?.suffix ?? ''}`)
      .replace(/\$(\d)/g, (_, i: string) => this.expr(args[Number(i)]!));
    this.c.includes.add(mapping.include);
    const ret = mapping.returns[member] ?? 'void';
    const type: CppType = /int64/.test(ret) ? INT64 : /int/.test(ret) ? INT : ret === 'void' ? { k: 'void' } : this.c.types.number(Num.Float, 'local');
    return { code, type };
  }

  protected call(e: TS.CallExpression, want?: CppType): Value {
    const ts = this.c.ts;
    const callee = e.expression;
    if (ts.isPropertyAccessExpression(callee)) {
      const receiver = callee.expression;
      const method = callee.name.text;
      if (ts.isIdentifier(receiver) && receiver.text === 'Math') return this.#math(e, method);
      if (ts.isIdentifier(receiver) && receiver.text === 'Number' && method === 'isFinite') {
        this.c.includes.add('cmath');
        return { code: `std::isfinite(${this.expr(e.arguments[0]!)})`, type: BOOL };
      }
      const obj = this.value(receiver);
      const t = target(obj.type);
      if (t.k === 'class') return this.#classMember(e, t.name, obj, method, e.arguments);
      if (t.k === 'map') return this.#mapMethod(e, obj, method);
      if (t.k === 'vector') return this.arrayMethod(e, obj, method, want);
      return this.fail(e, `a method ${method} of ${render(obj.type)}`);
    }
    if (ts.isIdentifier(callee)) {
      const symbol = this.symbolOf(callee);
      const decl = symbol?.valueDeclaration;
      const external = symbol && !this.c.functions.has(symbol) ? this.c.spec.functions[callee.text] : undefined;
      if (external) {
        return { code: external.template.replace(/\$(\d)/g, (_, i: string) => this.expr(e.arguments[Number(i)]!)), type: this.numberType(e) };
      }
      const memo = symbol ? this.c.memos.get(symbol) : undefined;
      if (memo) return this.#memoRead(e, memo);
      const plan = symbol ? this.c.functions.get(symbol) : undefined;
      if (plan) {
        this.uses?.add(plan.module);
        if (plan.edit) return this.fail(e, 'an edit used as a value (only as a statement or a return)');
        const args = plan.params.map((p, i) => (e.arguments[i] ? this.expr(e.arguments[i]!, p.type) : p.type.k === 'optional' ? 'std::nullopt' : `${render(p.type)}{}`));
        return { code: `${plan.cpp}(${args.join(', ')})`, type: plan.ret };
      }
      const local = symbol ? this.lookup(symbol) : undefined;
      if (local && local.type.k === 'lambda') {
        const fn = decl && ts.isVariableDeclaration(decl) ? decl.initializer : undefined;
        const params = fn && (ts.isArrowFunction(fn) || ts.isFunctionExpression(fn)) ? fn.parameters : undefined;
        const args = e.arguments.map((a, i) => {
          const p = params?.[i] ? this.symbolOf(params[i]!.name) : undefined;
          return this.expr(a, p ? this.c.types.slot(this.c.flow.symbolSlot(p), 'param') : undefined);
        });
        return { code: `${local.cpp}(${args.join(', ')})`, type: local.type.ret };
      }
      if (local && local.type.k === 'function') {
        const fnType = local.type;
        return { code: `${local.cpp}(${e.arguments.map((a, i) => this.expr(a, fnType.params[i])).join(', ')})`, type: fnType.ret };
      }
    }
    return this.fail(e, 'a call the translator does not know');
  }

  #memoRead(e: TS.CallExpression, memo: MemoPlan): Value {
    this.uses?.add(this.c.relative(this.c.file));
    const base = `${this.expr(e.arguments[0]!)}.index.${memo.member}`;
    // A builder that returns one container is read as that container; one that returns several, as the index.
    if (memo.single) return { code: `${base}.${memo.fields[0]!.name}`, type: memo.fields[0]!.type };
    return { code: base, type: { k: 'struct', name: memo.struct } };
  }

  #math(e: TS.CallExpression, method: string): Value {
    this.c.includes.add('cmath');
    const args = e.arguments.map((a) => this.value(a));
    const real = this.c.types.number(Num.Float, 'local');
    if (INT_MATH.has(method)) return { code: `int(std::${method}(${args[0]!.code}))`, type: INT };
    if (method === 'round') {
      // JavaScript rounds halves toward +∞ (ECMAScript Math.round); std::round rounds them away from zero, and
      // floor(x + 0.5) is wrong just below a half (the sum rounds). x − floor(x) is exact, so this is JavaScript's.
      const x = this.temp('x');
      const number = this.c.spec.number;
      return { code: `[&] { const ${number} ${x} = ${args[0]!.code}; const ${number} down = std::floor(${x}); return int(${x} - down >= ${realLiteral('0.5', number)} ? down + 1 : down); }()`, type: INT };
    }
    if (method === 'abs') return isInt(args[0]!.type) ? { code: `std::abs(${args[0]!.code})`, type: INT } : { code: `std::fabs(${args[0]!.code})`, type: real };
    if (REAL_MATH.has(method)) return { code: `std::${method}(${args.map((a) => a.code).join(', ')})`, type: real };
    if (method === 'max' || method === 'min') {
      this.c.includes.add('algorithm');
      const type = args.map((a) => a.type).reduce((a, b) => widerNumber(a, b));
      // std::max takes one type: every argument becomes the widest.
      const code = args.map((a) => {
        if (same(a.type, type)) return a.code;
        // A number written in place takes the real form; a name (a literal's constant) is converted.
        if (/^\d+$/.test(a.code) && type.k === 'real') return realLiteral(a.code, type.name);
        return `${render(type)}(${a.code})`;
      });
      return { code: code.reduce((acc, x) => `std::${method}(${acc}, ${x})`), type };
    }
    return this.fail(e, `Math.${method}`);
  }

  #mapMethod(e: TS.CallExpression, obj: Value, method: string): Value {
    const map = target(obj.type) as Extract<CppType, { k: 'map' }>;
    const key = (): string => this.expr(e.arguments[0]!, map.key);
    if (method === 'get') {
      // The entry, or null: a pointer to it (the entry itself when the map holds pointers).
      const holdsPointers = map.of.k === 'ptr';
      const ret: CppType = holdsPointers ? map.of : { k: 'ptr', to: map.of };
      return { code: `[&]() -> ${render(ret)} { auto it = ${obj.code}.find(${key()}); return it == ${obj.code}.end() ? nullptr : ${holdsPointers ? 'it->second' : '&it->second'}; }()`, type: ret };
    }
    if (method === 'set') return { code: `${obj.code}[${key()}] = ${this.expr(e.arguments[1]!, map.of)}`, type: { k: 'void' } };
    if (method === 'has') return { code: `(${obj.code}.count(${key()}) != 0)`, type: BOOL };
    if (method === 'delete') return { code: `${obj.code}.erase(${key()})`, type: { k: 'void' } };
    return this.fail(e, `Map.${method}`);
  }

  // ---- callbacks

  /**
   * A callback written in place with an expression body: its parameters become names in scope (the caller's loop
   * variables); endInline() closes them. Null when it is not that simple: it becomes a lambda.
   */
  protected inlineParams(fn: TS.Expression, types: readonly CppType[]): (string | null)[] | null {
    const ts = this.c.ts;
    if (!ts.isArrowFunction(fn) || ts.isBlock(fn.body) || fn.parameters.some((p) => !ts.isIdentifier(p.name))) return null;
    this.scopes.push(new Map());
    return types.map((type, i) => {
      const p = fn.parameters[i];
      if (!p) return null;
      const name = snake((p.name as TS.Identifier).text);
      this.bind(this.symbolOf(p.name)!, { cpp: name, type });
      return name;
    });
  }

  protected endInline(): void {
    this.scopes.pop();
  }

  /** `[&](const T &x) { return <body>; }` for a callback called on each element (negated: what it rejects). */
  protected predicate(fn: TS.Expression, element: CppType, negate = false): string {
    const names = this.inlineParams(fn, [element]);
    if (names) {
      const x = names[0] ?? 'unused';
      const body = this.expr((fn as TS.ArrowFunction).body as TS.Expression);
      this.endInline();
      return `[&](${paramDecl(element, x)}) { return ${negate ? this.#negate(body) : body}; }`;
    }
    const lambda = this.callback(fn, [element], BOOL);
    return negate ? `[&](${paramDecl(element, 'x')}) { return !(${lambda})(x); }` : lambda;
  }

  /** The opposite of a test: one top-level comparison flipped, anything else under `!`. */
  #negate(test: string): string {
    const flips: [string, string][] = [[' != ', ' == '], [' == ', ' != '], [' >= ', ' < '], [' <= ', ' > '], [' < ', ' >= '], [' > ', ' <= ']];
    if (!/&&|\|\||\?|\(/.test(test.replace(/^[^(]*\(/, ''))) {
      const found = flips.filter(([a]) => test.includes(a));
      if (found.length === 1 && test.split(found[0]![0]).length === 2) return test.replace(found[0]![0], found[0]![1]);
    }
    return `!(${test})`;
  }

  /** A callback as a lambda, its parameters typed as given. */
  protected callback(fn: TS.Expression, params: readonly CppType[], ret: CppType): string {
    const ts = this.c.ts;
    if (!ts.isArrowFunction(fn) && !ts.isFunctionExpression(fn)) return this.fail(fn, 'a callback that is not written in place').code;
    this.scopes.push(new Map());
    const names = fn.parameters.map((p, i) => {
      if (!ts.isIdentifier(p.name)) return this.fail(p, 'a destructured callback parameter').code;
      const symbol = this.symbolOf(p.name)!;
      const type = params[i] ?? this.c.types.slot(this.c.flow.symbolSlot(symbol), 'param');
      const cpp = snake(p.name.text);
      this.bind(symbol, { cpp, type });
      return paramDecl(type, cpp);
    });
    const body = this.functionBody(fn, ret);
    this.scopes.pop();
    return `[&](${names.join(', ')}) -> ${render(ret)} {\n${body.join('\n')}\n${this.indent}}`;
  }

  protected arrayMethod(e: TS.CallExpression, obj: Value, method: string, want?: CppType): Value {
    const element = elementOf(obj.type)!;
    const args = e.arguments;
    this.c.includes.add('algorithm');
    switch (method) {
      case 'push':
        return { code: args.map((a) => `${obj.code}.push_back(${this.expr(a, element)})`).join(', '), type: { k: 'void' } };
      case 'includes':
        return { code: `(std::find(${obj.code}.begin(), ${obj.code}.end(), ${this.expr(args[0]!, element)}) != ${obj.code}.end())`, type: BOOL };
      case 'indexOf':
        return { code: `[&]() -> int { auto it = std::find(${obj.code}.begin(), ${obj.code}.end(), ${this.expr(args[0]!, element)}); return it == ${obj.code}.end() ? -1 : int(it - ${obj.code}.begin()); }()`, type: INT };
      case 'some':
        return { code: `std::any_of(${obj.code}.begin(), ${obj.code}.end(), ${this.predicate(args[0]!, element)})`, type: BOOL };
      case 'every':
        return { code: `std::all_of(${obj.code}.begin(), ${obj.code}.end(), ${this.predicate(args[0]!, element)})`, type: BOOL };
      case 'filter': {
        const out = this.temp('kept');
        const type = target(obj.type);
        this.c.includes.add('iterator');
        return { code: `[&] { ${render(type)} ${out}; std::copy_if(${obj.code}.begin(), ${obj.code}.end(), std::back_inserter(${out}), ${this.predicate(args[0]!, element)}); return ${out}; }()`, type };
      }
      case 'map': {
        const fn = args[0]!;
        const literal = this.c.flow.slotOfExpr(e);
        const type: CppType = want?.k === 'vector' ? want : literal ? this.c.types.slot(literal, 'local') : { k: 'vector', of: element };
        const of = elementOf(type)!;
        const out = this.temp('mapped');
        const names = this.inlineParams(fn, [element, INT]);
        if (names) {
          const x = names[0] ?? 'unused';
          const body = this.expr((fn as TS.ArrowFunction).body as TS.Expression, of);
          this.endInline();
          const index = names[1] ? `const int ${names[1]} = int(${out}.size()); ` : '';
          return { code: `[&] { ${render(type)} ${out}; ${out}.reserve(${obj.code}.size()); for (${paramDecl(element, x)} : ${obj.code}) { ${index}${out}.push_back(${body}); } return ${out}; }()`, type };
        }
        const mapper = this.callback(fn, [element, INT], of);
        const call = (fn as TS.ArrowFunction).parameters.length > 1 ? `(x, int(${out}.size()))` : '(x)';
        return { code: `[&] { ${render(type)} ${out}; ${out}.reserve(${obj.code}.size()); for (const auto &x : ${obj.code}) ${out}.push_back((${mapper})${call}); return ${out}; }()`, type };
      }
      case 'find': {
        const ret: CppType = element.k === 'ptr' ? element : { k: 'ptr', to: element };
        const names = this.inlineParams(args[0]!, [element]);
        const x = names?.[0] ?? 'x';
        const test = names ? this.expr((args[0] as TS.ArrowFunction).body as TS.Expression) : `(${this.callback(args[0]!, [element], BOOL)})(x)`;
        if (names) this.endInline();
        return { code: `[&]() -> ${render(ret)} { for (${paramDecl(element, x)} : ${obj.code}) if (${test}) return ${element.k === 'ptr' ? x : `&${x}`}; return nullptr; }()`, type: ret };
      }
      case 'sort': {
        const sorted = this.#named(obj, 'sorted');
        const names = this.inlineParams(args[0]!, [element, element]);
        let compare: string;
        if (names) {
          const body = this.expr((args[0] as TS.ArrowFunction).body as TS.Expression);
          this.endInline();
          // A comparator's sign orders (negative: a first); zero or NaN keeps their order, as a stable sort does.
          compare = `[&](${paramDecl(element, names[0] ?? 'a')}, ${paramDecl(element, names[1] ?? 'b')}) { return ${body} < 0; }`;
        } else {
          const lambda = this.callback(args[0]!, [element, element], this.c.types.number(Num.Float, 'local'));
          compare = `[&](${paramDecl(element, 'a')}, ${paramDecl(element, 'b')}) { return (${lambda})(a, b) < 0; }`;
        }
        this.prelude.push(`std::stable_sort(${sorted.code}.begin(), ${sorted.code}.end(), ${compare});`);
        return sorted;
      }
      case 'reverse': {
        const reversed = this.#named(obj, 'reversed');
        this.prelude.push(`std::reverse(${reversed.code}.begin(), ${reversed.code}.end());`);
        return reversed;
      }
      case 'fill': {
        // new Float64Array(n).fill(v): n elements of v at once.
        const receiver = (e.expression as TS.PropertyAccessExpression).expression;
        if (this.c.ts.isNewExpression(receiver) && receiver.arguments?.length === 1) {
          return { code: `${render(target(obj.type))}(${this.expr(receiver.arguments[0]!, INT)}, ${this.expr(args[0]!, element)})`, type: target(obj.type) };
        }
        this.prelude.push(`std::fill(${obj.code}.begin(), ${obj.code}.end(), ${this.expr(args[0]!, element)});`);
        return obj;
      }
      default:
        return this.fail(e, `Array.${method}`);
    }
  }

  /** A receiver a statement changes in place: itself when it is a name, else a named copy first. */
  #named(obj: Value, base: string): Value {
    if (/^[A-Za-z_][\w.]*$/.test(obj.code)) return obj;
    const name = this.temp(base);
    this.prelude.push(`${declare(target(obj.type), name)} = ${obj.code};`);
    return { code: name, type: target(obj.type) };
  }

  #construct(e: TS.NewExpression, want?: CppType): Value {
    const name = e.expression.getText();
    const mapped = this.c.spec.classes[name];
    if (mapped) return { code: `${mapped.type}{}`, type: { k: 'class', name, text: mapped.type } };
    const slot = this.c.flow.slotOfExpr(e);
    const type = want && (want.k === 'vector' || want.k === 'map') ? want : slot ? this.c.types.slot(slot, 'local') : want;
    if (!type) return this.fail(e, `new ${name}`);
    if (/^(Map|Set)$/.test(name) && (!e.arguments || e.arguments.length === 0)) return { code: `${render(type)}{}`, type };
    if (/Array$/.test(name) && e.arguments?.length === 1) return { code: `${render(type)}(${this.expr(e.arguments[0]!, INT)})`, type };
    return this.fail(e, `new ${name}`);
  }

  #prefix(e: TS.PrefixUnaryExpression): Value {
    const ts = this.c.ts;
    const op = { [ts.SyntaxKind.ExclamationToken]: '!', [ts.SyntaxKind.MinusToken]: '-', [ts.SyntaxKind.PlusToken]: '+',
      [ts.SyntaxKind.TildeToken]: '~', [ts.SyntaxKind.PlusPlusToken]: '++', [ts.SyntaxKind.MinusMinusToken]: '--' }[e.operator];
    if (op === '-' && ts.isNumericLiteral(e.operand) && this.c.allowedNumbers.includes(-Number(e.operand.text))) {
      const text = e.operand.getText();
      return /^\d+$/.test(text) ? { code: `-${text}`, type: INT } : { code: `-${realLiteral(text, this.c.spec.number)}`, type: this.c.types.number(Num.Float, 'local') };
    }
    const v = this.value(e.operand);
    return { code: `${op}${v.code}`, type: op === '!' ? BOOL : v.type };
  }

  #conditional(e: TS.ConditionalExpression, want?: CppType): Value {
    const a = this.value(e.whenTrue, want);
    const b = this.value(e.whenFalse, want);
    let type: CppType;
    if (want) type = want;
    else if (isNumber(a.type) || isNumber(b.type)) type = this.numberType(e);
    else if (a.type.k === 'literal' || b.type.k === 'literal' || a.type.k === 'string') type = STRING;
    else if (a.type.k === 'null') {
      const slot = this.c.flow.slotOfExpr(e);
      type = slot ? this.c.types.slot(slot, 'local') : b.type;
    } else type = a.type;
    return { code: `(${this.expr(e.condition)} ? ${this.coerce(a, type, e.whenTrue)} : ${this.coerce(b, type, e.whenFalse)})`, type };
  }

  #binary(e: TS.BinaryExpression, want?: CppType): Value {
    const ts = this.c.ts;
    const k = e.operatorToken.kind;
    if (k === ts.SyntaxKind.QuestionQuestionToken) {
      const left = this.value(e.left);
      // A pointer is absent as nullptr: `p ?? null` is p, `p ?? q` takes q where p is absent.
      if (left.type.k === 'ptr') {
        if (e.right.kind === ts.SyntaxKind.NullKeyword || (ts.isIdentifier(e.right) && e.right.text === 'undefined')) return left;
        return { code: `(${left.code} ? ${left.code} : ${this.expr(e.right, left.type)})`, type: left.type };
      }
      if (left.type.k !== 'optional') return this.fail(e, '?? on a value that is never absent in C++');
      return { code: `(${left.code} ? *${left.code} : ${this.expr(e.right, left.type.of)})`, type: left.type.of };
    }
    if (k === ts.SyntaxKind.BarBarToken && ts.isIdentifier(e.right) && e.right.text === 'undefined') {
      return this.value(e.left); // `x || undefined` stores false as absent: a C++ bool is false either way
    }
    const left = this.value(e.left);
    const assigns = k >= ts.SyntaxKind.FirstAssignment && k <= ts.SyntaxKind.LastAssignment;
    const enumSide = left.type.k === 'enum' ? left.type : undefined;
    const right = this.value(e.right, enumSide ?? (assigns ? left.type : undefined));
    const text = e.operatorToken.getText();
    const op = text === '===' ? '==' : text === '!==' ? '!=' : text;
    if (op === '=') return { code: `${left.code} = ${this.coerce(right, left.type, e.right)}`, type: left.type };
    if (['==', '!=', '<', '>', '<=', '>=', '&&', '||'].includes(op)) {
      const leftCode = ts.isStringLiteral(e.left) && right.type.k === 'enum' ? this.#enumLiteral(right.type.name, e.left) : left.code;
      return { code: `${leftCode} ${op} ${this.coerce(right, enumSide, e.right)}`, type: BOOL };
    }
    if (op === 'instanceof' || op === 'in') return this.fail(e, op);
    if (assigns) return { code: `${left.code} ${op} ${right.code}`, type: left.type };
    if (op === '+' && (left.type.k === 'string' || left.type.k === 'literal' || right.type.k === 'string')) {
      return { code: `${left.type.k === 'literal' ? `std::string(${left.code})` : left.code} + ${right.code}`, type: STRING };
    }
    const type = this.numberType(e);
    if (op === '/' && isInt(left.type) && isInt(right.type)) return { code: `${this.c.spec.number}(${left.code}) / ${right.code}`, type };
    if (op === '%' && !(isInt(left.type) && isInt(right.type))) {
      this.c.includes.add('cmath');
      return { code: `std::fmod(${left.code}, ${right.code})`, type };
    }
    void want;
    return { code: `${left.code} ${op} ${right.code}`, type };
  }

  /** An object literal: a struct value (every field in declaration order), or one the index owns. */
  protected object(e: TS.ObjectLiteralExpression, want?: CppType): Value {
    const ts = this.c.ts;
    const asked = want ? target(want) : undefined;
    const slot = this.c.flow.slotOfExpr(e);
    const type: CppType = asked && asked.k === 'struct' ? asked
      : this.c.types.structFor((slot ? this.c.flow.shapeOf(slot) : null) ?? this.c.checker.getTypeAtLocation(e), 'value');
    if (type.k !== 'struct') return this.fail(e, 'an object that is no struct');
    const props = new Map<string, TS.Expression>();
    let spread: TS.Expression | null = null;
    for (const p of e.properties) {
      if (ts.isPropertyAssignment(p)) props.set(p.name.getText(), p.initializer);
      else if (ts.isShorthandPropertyAssignment(p)) props.set(p.name.text, p.name);
      else if (ts.isSpreadAssignment(p) && spread === null && e.properties.indexOf(p) === 0) spread = p.expression;
      else return this.fail(p, 'an object member the translator does not know');
    }
    const known = Object.values(this.c.spec.types).find((k) => k.type === type.name);
    if (known?.fields) {
      const real = this.c.types.number(Num.Float, 'local');
      return { code: `${type.name}(${Object.keys(known.fields).map((f) => this.expr(props.get(f)!, real)).join(', ')})`, type };
    }
    const struct = this.c.types.structs.get(type.name);
    if (!struct) return this.fail(e, `an object of ${type.name}, which is no struct`);
    for (const name of props.keys()) if (!struct.fields.some((f) => f.source === name)) return this.fail(e, `${type.name} has no field ${name}`);
    let made: string;
    if (spread) {
      const copy = this.temp('copy');
      const sets = [...props].map(([name, v]) => {
        const field = struct.fields.find((f) => f.source === name)!;
        return `${copy}.${field.name} = ${this.expr(v, field.type)};`;
      });
      made = `[&] { ${type.name} ${copy} = ${this.expr(spread, type)}; ${sets.join(' ')} return ${copy}; }()`;
    } else {
      made = `${type.name}{${struct.fields.map((f) => (props.has(f.source) ? this.expr(props.get(f.source)!, f.type) : '{}')).join(', ')}}`;
    }
    if (this.memo && this.c.flow.inMemo(e)) {
      const store = `store_${snake(type.name)}`;
      this.memo.stores.set(type.name, store);
      return { code: `&index.${store}.emplace_back(${made})`, type: { k: 'ptr', to: type } };
    }
    return { code: made, type };
  }

  #array(e: TS.ArrayLiteralExpression, want?: CppType): Value {
    const ts = this.c.ts;
    const slot = this.c.flow.slotOfExpr(e);
    const type: CppType = want?.k === 'vector' ? want : slot ? this.c.types.slot(slot, 'local') : { k: 'vector', of: INT };
    const element = elementOf(type)!;
    // [...src]: a copy of src.
    if (e.elements.length === 1 && ts.isSpreadElement(e.elements[0]!)) {
      const src = this.value(e.elements[0]!.expression);
      if (same(target(src.type), type)) return { code: src.code, type };
    }
    if (!e.elements.some((x) => ts.isSpreadElement(x))) return { code: `${render(type)}{${e.elements.map((x) => this.expr(x, element)).join(', ')}}`, type };
    const out = this.temp('items');
    const parts = e.elements.map((x) => {
      if (!ts.isSpreadElement(x)) return `${out}.push_back(${this.expr(x, element)});`;
      const src = this.value(x.expression);
      return `${out}.insert(${out}.end(), ${src.code}.begin(), ${src.code}.end());`;
    });
    return { code: `[&] { ${render(type)} ${out}; ${parts.join(' ')} return ${out}; }()`, type };
  }

  /** A function written in place: a lambda with the parameter and return types the flow solved. */
  protected lambda(fn: TS.ArrowFunction | TS.FunctionExpression): Value {
    const ret = this.returnType(fn);
    const params = fn.parameters.map((p) => this.c.types.slot(this.c.flow.symbolSlot(this.symbolOf(p.name)!), 'param'));
    return { code: this.callback(fn, params, ret), type: { k: 'lambda', ret } };
  }

  protected returnType(fn: TS.SignatureDeclaration): CppType {
    return this.c.types.slot(this.c.flow.returnSlot(fn), 'return');
  }
}
