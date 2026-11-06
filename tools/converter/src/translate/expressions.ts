// TypeScript expressions → C++ expressions. Every type comes from the solved flow (through the type mapper): this
// file infers nothing. It writes what each construct is in the target: member access by what the value is (`.` or
// `->`), a value converted where it lands (a pointer taken or read, an optional filled, null as nullptr or nullopt),
// array methods as loops or <algorithm> (sort is stable in JavaScript since ES2019: std::stable_sort), Map.get as a
// pointer to the entry, Math with JavaScript's own rounding (halves toward +∞, not std::round's away from zero), a
// concept the target owns as the owner's call (spec.patterns), a literal the target does not allow bare as its name.
import type * as TS from 'typescript';
import type { FunctionPlan, Local, MemoPlan, TranslateContext } from './context.ts';
import {
  BOOL, INT, INT64, LITERAL, NULL, STRING, VOID, bindTypeParameters, byValue, declare, holdsTypeParameter, integerLiteral, elementOf, isInt, isNumber, paramDecl, realLiteral, render, same, target,
  substitute, widerNumber, type CppType, arrow, isHandle, DYNAMIC } from './cpptype.ts';
import { Num } from './flow.ts';
import { snake, upper } from './names.ts';

/** The header of the runtime of the ECMAScript built-ins, as the target includes it (files.ts writes it there). */
export const RUNTIME_INCLUDE = 'core/js_runtime.h';

/** Whether a number becomes another kind landing where braces refuse it: real and integer, or a wider integer in a narrower one. */
function narrows(from: CppType, to: CppType): boolean {
  if (!isNumber(from) || !isNumber(to) || render(from) === render(to)) return false;
  if (from.k === 'int' && to.k === 'int') return from.name === 'int64_t' && to.name !== 'int64_t';
  return true;
}

export interface Value {
  readonly code: string;
  readonly type: CppType;
}

const INT_MATH = new Set(['floor', 'ceil', 'trunc']);
const REAL_MATH = new Set(['hypot', 'sqrt', 'sin', 'cos', 'tan', 'atan2', 'atan', 'asin', 'acos', 'exp', 'log', 'pow']);

export abstract class ExpressionEmitter {
  /** The function being written. */
  protected fn: FunctionPlan | null = null;
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

  /** The unit being planned or written (a function, a constant, an index): what an error belongs to. */
  /** Whether what is being written called a function of the program (a constant that does cannot be constexpr). */
  protected calledFunction = false;
  #unit: string | null = null;
  protected get unit(): string | null {
    return this.#unit;
  }
  /** The unit being written: what the type mapper and the flow refuse from now on is its refusal. */
  protected set unit(name: string | null) {
    this.#unit = name;
    this.c.types.unit = name;
    this.c.flow.unit = name;
  }

  protected fail(node: TS.Node, why: string): Value {
    const file = node.getSourceFile();
    const { line } = file.getLineAndCharacterOfPosition(node.getStart());
    // the node's own file (a value read from another module is reported where it is) and the unit it belongs to
    // (`[unit X]`: a refusal is attributed to its root without translating each root alone)
    this.c.errors.push(`${this.c.relative(file)}:${line + 1}: ${why}: ${node.getText().slice(0, 80)}${this.unit ? ` [unit ${this.unit}]` : ''}`);
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

  /** A value put in a js::Value. */
  protected box(v: Value): string {
    this.c.includes.add(RUNTIME_INCLUDE);
    if (v.type.k === 'dynamic') return v.code;
    if (v.type.k === 'null') return 'js::Value(nullptr)';
    if (v.type.k === 'literal') return `js::Value(std::string(${v.code}))`;
    this.#boxing(v.type);
    return `js::Value(${v.code})`;
  }

  /** A js::Value read as a static type: what the source's own types say it is here. */
  protected unbox(code: string, want: CppType): string {
    this.c.includes.add(RUNTIME_INCLUDE);
    if (want.k === 'int' || want.k === 'real') return `${render(want)}(${code}.number())`;
    if (want.k === 'bool') return `${code}.truthy()`;
    if (want.k === 'string' || want.k === 'literal') return `${code}.text()`;
    this.#boxing(want);
    return `js::as<${render(want)}>(${code})`;
  }

  /** The structs and enums a boxed type holds: they get their conversions. */
  #boxing(t: CppType): void {
    if (t.k === 'struct' || t.k === 'enum') this.c.types.boxed.add(t.name);
    else if (t.k === 'optional' || t.k === 'vector' || t.k === 'set') this.#boxing(t.of);
    else if (t.k === 'ptr') this.#boxing(t.to);
    else if (t.k === 'map') this.#boxing(t.of);
  }

  protected coerce(v: Value, want: CppType | undefined, e?: TS.Expression): string {
    if (!want) return v.code;
    if (want.k === 'dynamic') return this.box(v);
    if (v.type.k === 'dynamic') return this.unbox(v.code, want);
    if (want.k === 'function' && want.ret.k === 'bool' && want.params.length === 1) {
      const from = target(v.type);
      const key = want.params[0]!;
      if (from.k === 'map' || from.k === 'set') return `[&](${paramDecl(key, 'k')}) { return ${v.code}.count(k) != 0; }`;
      if (from.k === 'vector') {
        this.c.includes.add('algorithm');
        return `[&](${paramDecl(key, 'k')}) { return std::find(${v.code}.begin(), ${v.code}.end(), k) != ${v.code}.end(); }`;
      }
    }
    if (v.type.k === 'null') return isHandle(want) ? 'nullptr' : want.k === 'optional' ? 'std::nullopt' : `${render(want)}{}`;
    if (want.k === 'ptr' && v.type.k !== 'ptr' && same(want.to, target(v.type))) return `&${this.#lvalue(v.code)}`;
    if (want.k !== 'ptr' && want.k !== 'optional' && v.type.k === 'ptr' && same(want, v.type.to)) return `*${v.code}`;
    if (want.k === 'optional' && v.type.k === 'ptr' && same(want.of, v.type.to)) return `(${v.code} ? std::optional<${render(want.of)}>(*${v.code}) : std::nullopt)`;
    if (want.k === 'string' && v.type.k === 'literal') return `std::string(${v.code})`;
    if (narrows(v.type, want) && !/^-?[\d.]+f?$/.test(v.code)) return `${render(want)}(${v.code})`;
    if (e && want.k === 'enum' && this.c.ts.isStringLiteral(e)) return this.#enumLiteral(want.name, e);
    const structural = this.#structural(v, want);
    if (structural) return structural;
    // a list of wider elements where a list of narrower ones is asked: each element read through the asked type
    const from = target(v.type);
    if (want.k === 'vector' && from.k === 'vector' && !same(want.of, from.of)) {
      const item = this.#structural({ code: 'item', type: from.of }, want.of);
      if (item) {
        const out = this.temp('read');
        return `[&] { ${render(want)} ${out}; ${out}.reserve(${v.code}${v.type.k === 'vector' ? '.' : '->'}size()); for (const auto &item : ${v.type.k === 'vector' ? v.code : `*${v.code}`}) ${out}.push_back(${item}); return ${out}; }()`;
      }
    }
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
    const fieldsOf = (name: string): { source: string; cpp: string; type?: CppType }[] | null => {
      const known = Object.values(this.c.spec.types).find((k) => k.type === name);
      if (known?.fields) return Object.entries(known.fields).map(([source, cpp]) => ({ source, cpp }));
      const s = this.c.types.structs.get(name);
      return s ? s.fields.map((f) => ({ source: f.source, cpp: f.name, type: f.type })) : null;
    };
    const wanted = fieldsOf(to.name);
    const given = fieldsOf(from.name);
    if (!wanted || !given) return null;
    const byName = new Map(given.map((f) => [f.source, f]));
    // a field the value lacks is one the asked type may leave absent (optional in the source)
    const absent = (t?: CppType): boolean => t !== undefined && (t.k === 'optional' || isHandle(t) || t.k === 'bool');
    if (!wanted.every((f) => byName.has(f.source) || absent(f.type))) return null;
    const access = arrow(v.type);
    const known = Object.values(this.c.spec.types).some((k) => k.type === to.name);
    const build = (base: string): string => {
      const args = wanted.map((f) => {
        const field = byName.get(f.source);
        if (!field) return '{}';
        const code = `${base}${access}${field.cpp}`;
        return field.type && f.type ? this.coerce({ code, type: field.type }, f.type) : code;
      }).join(', ');
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
      return { code: match.pattern.target.replace(/\$(\w+)/g, (_, n: string) => this.expr(match.bound.get(`$${n}`)!)),
        type: match.pattern.type === 'owned' ? this.#ownedType(e) : { int: INT, int64: INT64, bool: BOOL, void: VOID, float: this.c.types.number(Num.Float, 'local') }[match.pattern.type] };
    }
    if (ts.isParenthesizedExpression(e)) {
      const inner = this.value(e.expression, want);
      return { code: `(${inner.code})`, type: inner.type };
    }
    if (ts.isNonNullExpression(e)) {
      const inner = this.value(e.expression, want);
      return inner.type.k === 'optional' ? { code: `(*${inner.code})`, type: inner.type.of } : inner;
    }
    if (ts.isAsExpression(e) || ts.isSatisfiesExpression(e) || ts.isTypeAssertionExpression(e)) {
      const inner = this.value(e.expression, want);
      if (inner.type.k !== 'dynamic') return inner;
      // `raw as T`: the source's promise of what it is, read as that
      return !want || want.k === 'dynamic' || want.k === 'void' ? inner : { code: this.unbox(inner.code, want), type: want };
    }
    if (ts.isNumericLiteral(e)) return this.#number(e);
    if (ts.isStringLiteral(e) || ts.isNoSubstitutionTemplateLiteral(e)) {
      if (want?.k === 'enum') return { code: this.#enumLiteral(want.name, e as TS.StringLiteral), type: want };
      return { code: JSON.stringify(e.text), type: LITERAL };
    }
    if (ts.isTemplateExpression(e)) return this.#template(e);
    if (ts.isTypeOfExpression(e)) return this.#typeOf_(e);
    if (e.kind === ts.SyntaxKind.TrueKeyword) return { code: 'true', type: BOOL };
    if (e.kind === ts.SyntaxKind.FalseKeyword) return { code: 'false', type: BOOL };
    if (e.kind === ts.SyntaxKind.NullKeyword) return { code: 'nullptr', type: NULL };
    if (ts.isIdentifier(e)) return this.#narrowed(e, this.#identifier(e));
    // an enum's member is the value the checker knows
    if (ts.isPropertyAccessExpression(e) || ts.isElementAccessExpression(e)) {
      const member = this.symbolOf(ts.isPropertyAccessExpression(e) ? e.name : e.argumentExpression)?.valueDeclaration;
      const constant = member && ts.isEnumMember(member) ? this.c.checker.getConstantValue(member) : undefined;
      if (typeof constant === 'number') return Number.isInteger(constant) ? { code: String(constant), type: INT } : { code: realLiteral(String(constant), this.c.spec.number), type: this.c.types.number(Num.Float, 'local') };
      if (typeof constant === 'string') return { code: JSON.stringify(constant), type: LITERAL };
    }
    if (ts.isPropertyAccessExpression(e)) {
      const field = this.symbolOf(e.name);
      const decl = field?.valueDeclaration;
      if (field && decl && ts.isPropertyDeclaration(decl) && (ts.getCombinedModifierFlags(decl) & ts.ModifierFlags.Static) !== 0) {
        const owner = target(this.c.types.ofType(this.c.checker.getTypeAtLocation(decl.parent), 'local', null));
        return { code: `${owner.k === 'struct' ? owner.name : render(owner)}::${snake(e.name.text.replace(/^#/, ''))}`, type: this.c.types.slot(this.c.flow.symbolSlot(field), 'local') };
      }
      return this.#property(e);
    }
    if (ts.isElementAccessExpression(e)) {
      const obj = this.value(e.expression);
      const dictionary = target(obj.type);
      if (dictionary.k === 'map') {
        const key = this.expr(e.argumentExpression, dictionary.key);
        const written = ts.isBinaryExpression(e.parent) && e.parent.left === e && e.parent.operatorToken.kind >= ts.SyntaxKind.FirstAssignment && e.parent.operatorToken.kind <= ts.SyntaxKind.LastAssignment;
        if (written) return { code: `${obj.code}[${key}]`, type: dictionary.of };
        return this.#mapEntry(obj.code, dictionary, key);
      }
      if (target(obj.type).k === 'dynamic') {
        const key = this.value(e.argumentExpression);
        const code = key.type.k === 'string' || key.type.k === 'literal' ? `${obj.code}.get(${this.coerce(key, STRING)})` : `${obj.code}.at(${key.type.k === 'dynamic' ? `${key.code}.number()` : key.code})`;
        return { code, type: DYNAMIC };
      }
      const tuple = this.#tupleField(obj.type, e.argumentExpression);
      if (tuple) return { code: `${obj.code}${arrow(obj.type)}${tuple.name}`, type: tuple.type };
      const element = elementOf(obj.type);
      if (!element) return this.fail(e, `an element of ${render(obj.type)}`);
      return { code: `${obj.code}[${this.expr(e.argumentExpression)}]`, type: element };
    }
    if (ts.isCallExpression(e)) return this.call(e, want);
    if (e.kind === ts.SyntaxKind.ThisKeyword) return this.#self(e);
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
    // a separator (1_000.5) only groups the digits: C++ writes them without it
    const written = e.getText().replace(/_/g, '');
    // an integer written in any base (0x.., 0b.., 0o..): an int, or an int64_t past int32 (C++ would make it unsigned
    // or long); C++ has no 0o, so octal and binary are written in decimal
    const kind = integerLiteral(written);
    const int = kind !== null;
    const wide = kind === 'int64';
    const type = int ? (wide ? INT64 : INT) : this.c.types.number(Num.Float, 'local');
    const digits = /^0[bBoO]/.test(written) ? String(value) : written;
    const text = int ? (wide ? `int64_t(${digits}LL)` : digits) : realLiteral(written, this.c.spec.number);
    if (this.#inConstant(e) || this.c.allowedNumbers.includes(value)) return { code: text, type };
    // A number the target's gates do not allow bare: a constant of this module, named from what receives it.
    const named = this.c.literals.name(e, value, this.module);
    const name = this.c.spec.whole ? `${named}_${upper(this.module.replace(/^src\//, '').replace(/\.tsx?$/, '').replace(/[^A-Za-z0-9]+/g, '_'))}` : named;
    const key = `${this.module}|${name}`;
    if (!this.c.literalConstants.has(key)) {
      const line = e.getSourceFile().getLineAndCharacterOfPosition(e.getStart()).line + 1;
      this.c.literalConstants.set(key, { source: written, cpp: name, type, value: text, module: this.module, line,
        doc: `The ${written} the source writes here`, constexpr: true });
    }
    return { code: name, type };
  }

  /** A literal in the initializer of a named constant is that constant's value. */
  #inConstant(e: TS.Node): boolean {
    for (let n: TS.Node | undefined = e.parent; n; n = n.parent) {
      if (this.c.ts.isVariableDeclaration(n)) return this.c.ts.isIdentifier(n.name) && /^[A-Z][A-Z0-9_]*$/.test(n.name.text);
      if (this.c.ts.isPropertyDeclaration(n) && (this.c.ts.getCombinedModifierFlags(n) & this.c.ts.ModifierFlags.Static) !== 0) return true;
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
      else {
        this.c.includes.add(RUNTIME_INCLUDE);
        parts.push(`js::to_string(${v.code})`);
      }
      if (span.literal.text) parts.push(JSON.stringify(span.literal.text));
    }
    if (parts.length === 0) return { code: 'std::string()', type: STRING };
    if (!parts[0]!.startsWith('std::')) parts[0] = `std::string(${parts[0]})`;
    this.c.includes.add('string');
    return { code: parts.join(' + '), type: STRING };
  }

  #identifier(e: TS.Identifier): Value {
    if (e.text === 'undefined') return { code: 'nullptr', type: NULL };
    if (e.text === 'NaN') {
      this.c.includes.add('limits');
      return { code: `std::numeric_limits<${this.c.spec.number}>::quiet_NaN()`, type: this.c.types.number(Num.Float, 'local') };
    }
    if (e.text === 'Infinity') {
      this.c.includes.add('limits');
      return { code: `std::numeric_limits<${this.c.spec.number}>::infinity()`, type: this.c.types.number(Num.Float, 'local') };
    }
    const symbol = this.symbolOf(e);
    if (!symbol) return this.fail(e, 'an unresolved name');
    const local = this.lookup(symbol);
    if (local) return { code: local.cpp, type: local.type };
    const contextual = this.c.contextConstants.get(symbol);
    if (contextual) {
      this.c.includes.add(contextual.include);
      return { code: contextual.code, type: contextual.type };
    }
    const constant = this.c.constants.get(symbol);
    if (constant) {
      if (constant.module) this.uses?.add(constant.module);
      return { code: constant.calling ? `${constant.cpp}()` : constant.cpp, type: constant.type };
    }
    const fn = this.c.functions.get(symbol);
    if (fn) return { code: fn.cpp, type: { k: 'lambda', ret: fn.ret } };
    return this.fail(e, 'a name outside the feature that the spec does not map');
  }

  #property(e: TS.PropertyAccessExpression): Value {
    const ts = this.c.ts;
    const name = e.name.text;
    // Math.PI, Number.EPSILON: the runtime's constants, by ECMAScript's names
    if (ts.isIdentifier(e.expression) && /^(Math|Number)$/.test(e.expression.text) && this.#isBuiltin(e.expression)) {
      this.c.includes.add(RUNTIME_INCLUDE);
      const real = this.c.types.number(Num.Float, 'local');
      return { code: `${render(real)}(js::${e.expression.text === 'Number' ? 'NUMBER_' : ''}${name})`, type: real };
    }
    // xs.filter(p).length counts: no list is built.
    if (name === 'length' && ts.isCallExpression(e.expression) && ts.isPropertyAccessExpression(e.expression.expression)
      && e.expression.expression.name.text === 'filter') {
      const source = this.value(e.expression.expression.expression);
      const element = elementOf(source.type)!;
      this.c.includes.add('algorithm');
      return { code: `int(std::count_if(${source.code}.begin(), ${source.code}.end(), ${this.predicate(e.expression.arguments[0]!, element)}))`, type: INT };
    }
    const optional = e.questionDotToken !== undefined || ts.isOptionalChain(e);
    const obj = optional ? this.#mayBeAbsent(e.expression) : this.value(e.expression);
    if (optional && (isHandle(obj.type) || obj.type.k === 'optional')) return this.#optionalMember(e, obj);
    return this.#member(e, obj);
  }

  /** The receiver of `?.`: xs[i]?.y reads past the end of a list as absent, so the element is read as a pointer or null. */
  #mayBeAbsent(e: TS.Expression): Value {
    const ts = this.c.ts;
    if (ts.isElementAccessExpression(e)) {
      const list = this.value(e.expression);
      const element = target(list.type).k === 'vector' ? elementOf(list.type) : null;
      if (element && !isHandle(element)) {
        const held = this.temp('list');
        const at = this.temp('at');
        const type: CppType = { k: 'ptr', to: element };
        return { code: `[&]() -> ${render(type)} { const auto &${held} = ${list.type.k === 'vector' ? list.code : `*${list.code}`}; const auto ${at} = ${this.expr(e.argumentExpression, INT)}; return ${at} >= 0 && size_t(${at}) < ${held}.size() ? &${held}[size_t(${at})] : nullptr; }()`, type };
      }
    }
    return this.value(e);
  }

  /** a?.b: absent when a is, the receiver read once. What holds no absence of its own (a number) becomes optional. */
  #optionalMember(e: TS.PropertyAccessExpression, obj: Value): Value {
    return this.#unlessAbsent(obj, (held) => this.#member(e, { code: held, type: obj.type }));
  }

  /** `a?.b`, `a?.m()`: what `read` makes of a, or absent when a is; a itself read once. */
  #unlessAbsent(obj: Value, read: (held: string) => Value): Value {
    const held = this.temp('from');
    const made = read(held);
    if (made.type.k === 'void') return { code: `[&]() { const auto &${held} = ${obj.code}; if (${held}) ${made.code}; }()`, type: made.type };
    return this.#absentOr(obj, held, made);
  }

  #absentOr(obj: Value, held: string, read: Value): Value {
    const type: CppType = isHandle(read.type) || read.type.k === 'optional' || read.type.k === 'bool' ? read.type : { k: 'optional', of: read.type };
    const absent = isHandle(type) ? 'nullptr' : type.k === 'bool' ? 'false' : 'std::nullopt';
    this.c.includes.add('optional');
    return { code: `[&]() -> ${render(type)} { const auto &${held} = ${obj.code}; if (!${held}) return ${absent}; return ${read.code}; }()`, type };
  }

  /** `typeof x`: what the static type says, or what the running value is. */
  #typeOf_(e: TS.TypeOfExpression): Value {
    const v = this.value(e.expression);
    const named = (t: CppType): string => (t.k === 'int' || t.k === 'real' ? 'number' : t.k === 'string' || t.k === 'literal' || t.k === 'enum' ? 'string' : t.k === 'bool' ? 'boolean'
      : t.k === 'function' || t.k === 'lambda' ? 'function' : 'object');
    if (v.type.k === 'dynamic') return { code: `${v.code}.type_of()`, type: STRING };
    if (v.type.k === 'optional' || v.type.k === 'ptr') {
      const under = target(v.type);
      return { code: `(${v.code} ? std::string(${JSON.stringify(named(under))}) : std::string("undefined"))`, type: STRING };
    }
    return { code: `std::string(${JSON.stringify(named(v.type))})`, type: STRING };
  }

  /** `a instanceof C`: the running value's kind, or what the static type already says. */
  #instanceOf(e: TS.BinaryExpression, left: Value): Value {
    const name = e.right.getText();
    const t = target(left.type);
    if (t.k === 'dynamic') return { code: name === 'Array' ? `${left.code}.is_array()` : `(${left.code}.kind() == js::Value::Kind::Object)`, type: BOOL };
    const same = (t.k === 'vector' && /Array$/.test(name)) || (t.k === 'map' && name === 'Map') || (t.k === 'set' && name === 'Set') || (t.k === 'struct' && t.name === name);
    if (!same) return { code: 'false', type: BOOL };
    return { code: isHandle(left.type) || left.type.k === 'optional' ? `bool(${left.code})` : 'true', type: BOOL };
  }

  /** `k in o`: the key of a running value or a map; a struct's field by what the types say. */
  #inObject(e: TS.BinaryExpression): Value {
    const holder = this.value(e.right);
    const t = target(holder.type);
    if (t.k === 'dynamic') return { code: `${holder.code}.has(${this.expr(e.left, STRING)})`, type: BOOL };
    if (t.k === 'map') return { code: `(${holder.code}${arrow(holder.type)}count(${this.expr(e.left, t.key)}) != 0)`, type: BOOL };
    if (t.k === 'struct' && this.c.ts.isStringLiteralLike(e.left)) {
      const field = this.c.types.structs.get(t.name)?.fields.find((f) => f.source === (e.left as TS.StringLiteral).text);
      if (!field) return { code: 'false', type: BOOL };
      return { code: field.type.k === 'optional' || isHandle(field.type) ? `bool(${holder.code}${arrow(holder.type)}${field.name})` : 'true', type: BOOL };
    }
    return this.fail(e, '`in` on something with no keys');
  }

  /** A member of a value read: a list's length, a field, what a built-in has. */
  #member(e: TS.PropertyAccessExpression, obj: Value): Value {
    const name = e.name.text.replace(/^#/, '');
    const getter = this.#memberPlan(e.name);
    if (getter && this.c.dropped.has(getter)) return this.fail(e, `${getter.source} was left out of its class`);
    if (getter?.owner?.kind === 'getter') return { code: `${obj.code}->${getter.cpp}()`, type: holdsTypeParameter(getter.ret) ? this.#typeOf(e) : getter.ret };
    const t = target(obj.type);
    const access = arrow(obj.type);
    if (t.k === 'dynamic') return { code: `${obj.code}.get(${JSON.stringify(name)})`, type: DYNAMIC };
    if (name === 'length' && (t.k === 'vector' || t.k === 'string')) return { code: `int(${obj.code}${access}size())`, type: INT };
    // A Map's or a Set's size.
    if (name === 'size' && (t.k === 'map' || t.k === 'vector' || t.k === 'set')) return { code: `int(${obj.code}${access}size())`, type: INT };
    if (t.k === 'tparam') {
      if (name === 'length') return { code: `int(${obj.code}${access}size())`, type: INT };
      const slot = this.c.flow.slotOfExpr(e);
      return { code: `${obj.code}${access}${snake(name)}`, type: slot ? this.c.types.slot(slot, 'local') : this.#ownedType(e) };
    }
    if (t.k === 'class') return this.#classMember(e, t.name, obj, name, []);
    if (t.k === 'struct') {
      const known = Object.values(this.c.spec.types).find((k) => k.type === t.name);
      if (known?.fields) {
        const field = known.fields[name];
        if (!field) return this.fail(e, `${t.name} has no field for ${name} in the spec`);
        const flag = (this.c.checker.getTypeAtLocation(e).flags & this.c.ts.TypeFlags.BooleanLike) !== 0;
        return { code: `${obj.code}${access}${field}`, type: flag ? BOOL : this.c.types.number(Num.Float, 'local') };
      }
      let struct = this.c.types.structs.get(t.name);
      let field = struct?.fields.find((f) => f.source === name);
      // a field of what the class extends
      while (!field && struct?.base) {
        struct = this.c.types.structs.get(struct.base);
        field = struct?.fields.find((f) => f.source === name);
      }
      if (!field) return this.fail(e, `${t.name} has no field ${name}`);
      const bound = new Map((struct?.typeParams ?? []).flatMap((p, i) => (t.args?.[i] ? [[p, t.args[i]!] as const] : [])));
      return { code: `${obj.code}${access}${field.name}`, type: substitute(field.type, bound) };
    }
    if (this.#isBuiltin(e.name)) return this.builtin(e, obj);
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
    if (callee.kind === ts.SyntaxKind.SuperKeyword) return { code: '/*super*/', type: VOID };
    if (ts.isPropertyAccessExpression(callee)) {
      const receiver = callee.expression;
      const method = callee.name.text;
      if (ts.isIdentifier(receiver) && receiver.text === 'Math') return this.#math(e, method);
      // a static built-in (Number.isInteger, String.fromCharCode, Array.isArray): the runtime's
      if (ts.isIdentifier(receiver) && this.#isBuiltin(receiver) && !(receiver.text === 'Number' && method === 'isFinite')) return this.builtin(e, null);
      if (receiver.kind === ts.SyntaxKind.SuperKeyword) {
        const inherited = this.#memberPlan(callee.name);
        if (!inherited) return this.fail(e, 'a method of the base that was left out');
        return { code: `${inherited.owner!.struct}::${inherited.cpp}(${this.#argumentsOf(inherited, e.arguments, new Map()).join(', ')})`, type: inherited.ret };
      }
      if (ts.isIdentifier(receiver) && receiver.text === 'Number' && method === 'isFinite') {
        const checked = this.value(e.arguments[0]!);
        if (checked.type.k === 'dynamic') return { code: `(${checked.code}.kind() == js::Value::Kind::Number && std::isfinite(${checked.code}.number()))`, type: BOOL };
        this.c.includes.add('cmath');
        return { code: `std::isfinite(${this.expr(e.arguments[0]!)})`, type: BOOL };
      }
      if (method === 'test' && e.arguments.length === 1) {
        const regex = this.#regexOf(receiver);
        if (regex) return this.#regexTest(regex, e.arguments[0]!);
      }
      const member = this.#memberPlan(callee.name);
      const absentable = callee.questionDotToken !== undefined;
      if (member && !absentable) return this.#memberCall(e, member, receiver);
      const obj = this.value(receiver);
      if (member) return this.#unlessAbsent(obj, (held) => this.#memberCall(e, member, receiver, held));
      if (absentable && (isHandle(obj.type) || obj.type.k === 'optional')) {
        return this.#unlessAbsent(obj, (held) => this.#method(e, { code: `(*${held})`, type: target(obj.type) }, method, callee, want));
      }
      return this.#method(e, obj, method, callee, want);
    }
    return this.#plainCall(e, callee);
  }

  /** A method of a value of the target's or the runtime's: what it is called on decides. */
  #method(e: TS.CallExpression, held: Value, method: string, callee: TS.PropertyAccessExpression, want?: CppType): Value {
    {
      const t = target(held.type);
      // a container behind a pointer is read through it
      const obj: Value = held.type.k === 'ptr' && t.k !== 'class' ? { code: `(*${held.code})`, type: t } : held;
      if (t.k === 'class') return this.#classMember(e, t.name, obj, method, e.arguments);
      if (t.k === 'map') return this.#mapMethod(e, obj, method);
      if (t.k === 'function' && method === 'has' && t.params.length === 1 && e.arguments.length === 1) {
        return { code: `${obj.code}(${this.expr(e.arguments[0]!, t.params[0])})`, type: BOOL };
      }
      if (t.k === 'set') return this.#setMethod(e, obj, method);
      if (t.k === 'vector') return this.arrayMethod(e, obj, method, want);
      if (t.k === 'string' || t.k === 'literal') return this.#stringMethod(e, obj, method);
      if (this.#isBuiltin(callee.name)) return this.builtin(e, obj);
      return this.fail(e, `a method ${method} of ${render(obj.type)}`);
    }
  }

  /** A call of a name: a function of the feature, the target's, a local callback, a built-in. */
  #plainCall(e: TS.CallExpression, callee: TS.Expression): Value {
    const ts = this.c.ts;
    if (ts.isIdentifier(callee)) {
      const symbol = this.symbolOf(callee);
      const decl = symbol?.valueDeclaration;
      const external = symbol && !this.c.functions.has(symbol) ? this.c.spec.functions[callee.text] : undefined;
      if (external) {
        if (external.include) this.c.includes.add(external.include);
        const code = external.template.replace(/\$(\d)/g, (_, i: string) => this.expr(e.arguments[Number(i)]!));
        return { code, type: this.#ownedType(e) };
      }
      const memo = symbol ? this.c.memos.get(symbol) : undefined;
      if (memo) return this.#memoRead(e, memo);
      const plan = symbol ? this.c.functions.get(symbol) : undefined;
      if (plan) {
        if (this.c.dropped.has(plan)) return this.fail(e, `${plan.source} was left out`);
        this.uses?.add(plan.module);
        this.calledFunction = true;
        if (plan.edit) return this.fail(e, 'an edit used as a value (only as a statement or a return)');
        // a template: T is what the call's own types say, written out, so every argument lands as what T is
        const types = plan.typeParams.length > 0 ? this.#typeArguments(plan, e) : new Map<string, CppType>();
        const args = this.#argumentsOf(plan, e.arguments, types);
        const explicit = plan.typeParams.length > 0 && plan.typeParams.every((t) => types.has(t)) ? `<${plan.typeParams.map((t) => render(types.get(t)!)).join(', ')}>` : '';
        const ret = substitute(plan.ret, types);
        return { code: `${plan.cpp}${explicit}(${args.join(', ')})`, type: holdsTypeParameter(ret) ? this.#typeOf(e) : ret };
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
    if (ts.isIdentifier(callee) && this.#isBuiltin(callee)) return this.builtin(e, null);
    return this.fail(e, 'a call the translator does not know');
  }

  /** What a call passes: each argument as its parameter asks, what it leaves out as empty, then the context, by name (the caller takes it too: #propagateContext). */
  #argumentsOf(plan: FunctionPlan, given: readonly TS.Expression[], types: ReadonlyMap<string, CppType>): string[] {
    const args = plan.params.map((p, i) => {
      const asked = substitute(p.type, types);
      if (given[i]) return this.expr(given[i]!, holdsTypeParameter(asked) ? undefined : asked);
      return asked.k === 'optional' ? 'std::nullopt' : `${render(asked)}{}`;
    });
    for (const c of plan.context ?? []) args.push(c.name);
    return args;
  }

  /** The plan of a class's member a name reads (a method, a getter), or null. */
  #memberPlan(name: TS.Node): FunctionPlan | null {
    // of a generic class's instance the checker gives a symbol of its own: the plan is the declaration's
    const symbol = this.symbolOf(name);
    const declared = (symbol?.valueDeclaration as (TS.Node & { symbol?: TS.Symbol }) | undefined)?.symbol ?? symbol;
    const plan = declared ? this.c.functions.get(declared) : undefined;
    return plan?.owner ? plan : null;
  }

  /** A method called: on its instance, or on its class when it is static. */
  #memberCall(e: TS.CallExpression, plan: FunctionPlan, receiver: TS.Expression, on?: string): Value {
    if (this.c.dropped.has(plan)) return this.fail(e, `${plan.source} was left out of its class`);
    this.uses?.add(plan.module);
    this.calledFunction = true;
    const args = this.#argumentsOf(plan, e.arguments, new Map()).join(', ');
    const type = holdsTypeParameter(plan.ret) ? this.#typeOf(e) : plan.ret;
    if (plan.owner!.isStatic) return { code: `${plan.owner!.struct}::${plan.cpp}(${args})`, type };
    return { code: `${on ?? this.value(receiver).code}->${plan.cpp}(${args})`, type };
  }

  /** `this`: read through inside a member; as a value, the shared instance (never in a constructor: nothing holds it yet). */
  #self(e: TS.Expression): Value {
    const owner = this.fn?.owner;
    if (!owner) return this.fail(e, '`this` outside a class');
    const params = this.c.types.structs.get(owner.struct)?.typeParams ?? [];
    const type: CppType = { k: 'ref', to: { k: 'struct', name: owner.struct, ...(params.length > 0 ? { args: params.map((name) => ({ k: 'tparam' as const, name })) } : {}) } };
    if (this.c.ts.isPropertyAccessExpression(e.parent) && e.parent.expression === e) return { code: 'this', type };
    if (owner.kind === 'constructor') return this.fail(e, '`this` given away by a constructor');
    // of a subclass, the base's handle read as its own
    if (this.c.types.structs.get(owner.struct)?.base) return { code: `std::static_pointer_cast<${render(type.to)}>(this->shared_from_this())`, type };
    return { code: 'this->shared_from_this()', type };
  }

  /** An optional the source has already proved present here (`if (x === undefined) return;`) is its value. The checker knows; the written side of an assignment stays the optional. */
  #narrowed(e: TS.Identifier, v: Value): Value {
    const ts = this.c.ts;
    // a running value the source has proved a primitive (`typeof x === 'number'`) is one
    if (v.type.k === 'dynamic') {
      const at = this.c.checker.getTypeAtLocation(e);
      const left = ts.isBinaryExpression(e.parent) && e.parent.left === e && e.parent.operatorToken.kind === ts.SyntaxKind.EqualsToken;
      if (left || at.flags & (ts.TypeFlags.Any | ts.TypeFlags.Unknown)) return v;
      if (at.flags & ts.TypeFlags.NumberLike) return { code: `${v.code}.number()`, type: this.c.types.number(Num.Float, 'local') };
      if (at.flags & ts.TypeFlags.StringLike) return { code: `${v.code}.text()`, type: STRING };
      if (at.flags & ts.TypeFlags.BooleanLike) return { code: `${v.code}.truthy()`, type: BOOL };
      return v;
    }
    if (v.type.k !== 'optional') return v;
    const p = e.parent;
    if (ts.isBinaryExpression(p) && p.left === e && p.operatorToken.kind >= ts.SyntaxKind.FirstAssignment && p.operatorToken.kind <= ts.SyntaxKind.LastAssignment) return v;
    const here = this.c.checker.getTypeAtLocation(e);
    const absent = ts.TypeFlags.Undefined | ts.TypeFlags.Null;
    const mayBeAbsent = here.isUnion() ? here.types.some((t) => t.flags & absent) : (here.flags & (absent | ts.TypeFlags.Any | ts.TypeFlags.Unknown)) !== 0;
    return mayBeAbsent ? v : { code: `(*${v.code})`, type: v.type.of };
  }

  /** The type of an expression as the flow solved it (an expression with no slot: the form of its TypeScript type). */
  #typeOf(e: TS.Expression): CppType {
    const slot = this.c.flow.slotOfExpr(e);
    return slot ? this.c.types.slot(slot, 'local') : this.#ownedType(e);
  }

  /** What each type parameter of a template is at a call: its result first, as the value lands there, then its arguments. */
  #typeArguments(plan: FunctionPlan, e: TS.CallExpression): Map<string, CppType> {
    const out = new Map<string, CppType>();
    if (holdsTypeParameter(plan.ret)) bindTypeParameters(plan.ret, this.#typeOf(e), out);
    plan.params.forEach((p, i) => {
      const a = e.arguments[i];
      if (a && holdsTypeParameter(p.type) && p.type.k !== 'function') bindTypeParameters(p.type, this.#typeOf(a), out);
    });
    return out;
  }

  /** `{ ...a, x: 1, ...(c ? { y: 2 } : {}) }`: the struct filled in the order the source writes it. */
  #objectBySteps(e: TS.ObjectLiteralExpression, type: Extract<CppType, { k: 'struct' }>): Value {
    const ts = this.c.ts;
    const out = this.temp('made');
    const struct = this.c.types.structs.get(type.name);
    const put = (name: string, value: TS.Expression): string => {
      const field = struct?.fields.find((f) => f.source === name);
      return field ? `${out}.${field.name} = ${this.expr(value, field.type)};` : this.fail(value, `${type.name} has no field ${name}`).code;
    };
    const members = (o: TS.ObjectLiteralExpression): string => o.properties.map((p) => step(p)).join(' ');
    const step = (p: TS.ObjectLiteralElementLike): string => {
      if (ts.isPropertyAssignment(p)) return put(p.name.getText(), p.initializer);
      if (ts.isShorthandPropertyAssignment(p)) return put(p.name.text, p.name);
      if (!ts.isSpreadAssignment(p)) return this.fail(p, 'an object member the translator does not know').code;
      let s = p.expression;
      while (ts.isParenthesizedExpression(s)) s = s.expression;
      if (ts.isObjectLiteralExpression(s)) return members(s);
      if (ts.isConditionalExpression(s)) {
        let yes = s.whenTrue;
        let no = s.whenFalse;
        while (ts.isParenthesizedExpression(yes)) yes = yes.expression;
        while (ts.isParenthesizedExpression(no)) no = no.expression;
        if (ts.isObjectLiteralExpression(yes) && ts.isObjectLiteralExpression(no)) return `if (${this.expr(s.condition)}) { ${members(yes)} } else { ${members(no)} }`;
      }
      const from = this.value(s);
      const t = target(from.type);
      const read = isHandle(from.type) || from.type.k === 'optional' ? `*${from.code}` : from.code;
      if (t.k === 'struct' && t.name === type.name) return `${out} = ${read};`;
      const fields = t.k === 'struct' ? this.c.types.structs.get(t.name)?.fields ?? [] : [];
      const held = this.temp('from');
      return `{ const auto &${held} = ${read}; ${fields.filter((f) => struct?.fields.some((g) => g.source === f.source)).map((f) => `${out}.${struct!.fields.find((g) => g.source === f.source)!.name} = ${held}.${f.name};`).join(' ')} }`;
    };
    return { code: `[&] { ${type.name} ${out}; ${e.properties.map(step).join(' ')} return ${out}; }()`, type };
  }

  /** What a value the target owns is in C++: an object, the form of its source type (a type the spec maps); else a number. */
  #ownedType(e: TS.Expression): CppType {
    const t = this.c.checker.getNonNullableType(this.c.checker.getTypeAtLocation(e));
    if (t.flags & this.c.ts.TypeFlags.BooleanLike) return BOOL;
    if (t.flags & this.c.ts.TypeFlags.StringLike) return STRING;
    if (t.flags & (this.c.ts.TypeFlags.Void | this.c.ts.TypeFlags.Undefined)) return VOID;
    if (t.flags & this.c.ts.TypeFlags.Object) return this.c.types.ofType(t, 'local', null);
    return this.numberType(e);
  }

  /** Whether a name resolves to a declaration of TypeScript's own library (an ECMAScript built-in). */
  #isBuiltin(node: TS.Node): boolean {
    const decl = this.c.checker.getSymbolAtLocation(node)?.declarations?.[0];
    return decl !== undefined && /[\\/]typescript[\\/]lib[\\/]lib\.[^\\/]*\.d\.ts$/.test(decl.getSourceFile().fileName);
  }

  /**
   * A built-in the translator does not lower itself, written the one way every built-in is: the runtime's function
   * (cpp/js/js_runtime.h, each checked against JavaScript) named mechanically from ECMAScript's: x.toFixed(2) is
   * js::to_fixed(x, 2), Number.isInteger(x) js::number_is_integer(x), String(x) js::to_string(x). C++ overloads pick
   * the version; the value has the C++ form of its TypeScript type. A callback is a lambda; an absent argument is the
   * runtime's default.
   */
  protected builtin(e: TS.CallExpression | TS.PropertyAccessExpression, receiver: Value | null): Value {
    const ts = this.c.ts;
    const call = ts.isCallExpression(e) ? e : null;
    const callee = call ? call.expression : e;
    let name: string;
    if (ts.isPropertyAccessExpression(callee)) {
      const host = callee.expression;
      const statics = receiver === null && ts.isIdentifier(host) ? host.text : null;
      name = statics && statics !== 'Math' ? `${snake(statics)}_${snake(callee.name.text)}` : snake(callee.name.text);
    } else if (ts.isIdentifier(callee)) name = callee.text === 'String' ? 'to_string' : snake(callee.text);
    else return this.fail(e, 'a built-in called through an expression');
    name = name.replace(/_na_n$/, '_nan'); // isNaN is one word, however it is capitalised
    const args: string[] = receiver ? [receiver.type.k === 'literal' ? `std::string(${receiver.code})` : receiver.code] : [];
    for (const a of call?.arguments ?? []) {
      if (ts.isArrowFunction(a) || ts.isFunctionExpression(a)) args.push(this.lambda(a).code);
      else if (ts.isSpreadElement(a)) return this.fail(a, 'a spread argument to a built-in');
      else {
        const v = this.value(a);
        args.push(v.type.k === 'literal' ? `std::string(${v.code})` : v.code);
      }
    }
    this.c.includes.add(RUNTIME_INCLUDE);
    const type = this.#ownedType(call ?? e);
    // Float64Array.from(x), Array.from(x): the elements the result holds are the target's
    if (/array_from$/.test(name) && elementOf(type)) name = `array_from<${render(elementOf(type)!)}>`;
    const code = `js::${name}(${args.join(', ')})`;
    return { code: isNumber(type) ? `${render(type)}(${code})` : code, type };
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
    if (INT_MATH.has(method)) return isInt(args[0]!.type) ? { code: `int(std::${method}(${args[0]!.code}))`, type: INT } : { code: `int64_t(std::${method}(${args[0]!.code}))`, type: INT64 };
    if (method === 'round') {
      // JavaScript rounds halves toward +∞ (ECMAScript Math.round); std::round rounds them away from zero, and
      // floor(x + 0.5) is wrong just below a half (the sum rounds). x − floor(x) is exact, so this is JavaScript's.
      const x = this.temp('x');
      const number = this.c.spec.number;
      return { code: `[&] { const ${number} ${x} = ${args[0]!.code}; const ${number} down = std::floor(${x}); return int64_t(${x} - down >= ${realLiteral('0.5', number)} ? down + 1 : down); }()`, type: INT64 };
    }
    // the low 32 bits of the product of two int32 (wrapping: unsigned arithmetic in C++, signed overflow is undefined)
    // (an operand that is no integer is truncated first, as JavaScript's ToInt32 does)
    if (method === 'imul') {
      const u = (v: Value): string => (isInt(v.type) ? `uint32_t(${v.code})` : `uint32_t(int64_t(${v.code}))`);
      return { code: `int32_t(${u(args[0]!)} * ${u(args[1]!)})`, type: { k: 'int', name: 'int32_t' } };
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
      // reals keep ECMAScript's answer: NaN carries through, and +0 is above -0 (std::max drops a NaN on one side)
      const fold = type.k === 'real' ? `js::math_${method}` : `std::${method}`;
      return { code: code.reduce((acc, x) => `${fold}(${acc}, ${x})`), type };
    }
    return this.builtin(e, null);
  }

  /** A string's prefix, suffix and part tests (the same answer as JavaScript's for every string). */
  #stringMethod(e: TS.CallExpression, obj: Value, method: string): Value {
    if (method === 'replace' || method === 'replaceAll') return this.#replace(e, obj, method === 'replaceAll');
    if (e.arguments.length !== 1 || !['startsWith', 'endsWith', 'includes'].includes(method)) return this.builtin(e, obj);
    this.c.includes.add('string');
    const s = this.temp('s');
    const p = this.temp('p');
    const bind = `const std::string &${s} = ${obj.type.k === 'literal' ? `std::string(${obj.code})` : obj.code}; const std::string ${p} = ${this.expr(e.arguments[0]!, STRING)};`;
    if (method === 'startsWith') return { code: `[&] { ${bind} return ${s}.compare(0, ${p}.size(), ${p}) == 0; }()`, type: BOOL };
    if (method === 'endsWith') return { code: `[&] { ${bind} return ${s}.size() >= ${p}.size() && ${s}.compare(${s}.size() - ${p}.size(), ${p}.size(), ${p}) == 0; }()`, type: BOOL };
    if (method === 'includes') return { code: `[&] { ${bind} return ${s}.find(${p}) != std::string::npos; }()`, type: BOOL };
    return this.builtin(e, obj);
  }

  /**
   * s.replace(pattern, text): the first match replaced (every one with a g regex or replaceAll). A regex goes through
   * std::regex_replace with the ECMAScript grammar; a text pattern is found as text. A replacement with `$` (JavaScript's
   * $&, $1, ...) or a function is refused: its rules are not std::regex's format rules.
   */
  #replace(e: TS.CallExpression, obj: Value, all: boolean): Value {
    const ts = this.c.ts;
    const [pattern, replacement] = e.arguments;
    if (!pattern || !replacement || e.arguments.length !== 2) return this.fail(e, `String.replace with ${e.arguments.length} arguments`);
    if (!ts.isStringLiteral(replacement) && !ts.isNoSubstitutionTemplateLiteral(replacement)) return this.fail(replacement, 'a replacement that is not text written in place');
    if (replacement.text.includes('$')) return this.fail(replacement, 'a replacement with $ patterns');
    this.c.includes.add('string');
    const subject = obj.type.k === 'literal' ? `std::string(${obj.code})` : obj.code;
    const text = JSON.stringify(replacement.text);
    const regex = this.#regexOf(pattern);
    if (regex) {
      const slash = regex.text.lastIndexOf('/');
      const body = regex.text.slice(1, slash);
      const flags = regex.text.slice(slash + 1);
      if (/[^gi]/.test(flags)) return this.fail(regex, `a regular expression with the flags ${flags}`);
      if (body.includes(')"')) return this.fail(regex, 'a regular expression that ends a C++ raw string');
      this.c.includes.add('regex');
      const re = this.temp('re');
      const options = flags.includes('i') ? 'std::regex::ECMAScript | std::regex::icase' : 'std::regex::ECMAScript';
      const once = all || flags.includes('g') ? '' : ', std::regex_constants::format_first_only';
      return { code: `[&] { static const std::regex ${re}(R"(${body})", ${options}); return std::regex_replace(${subject}, ${re}, std::string(${text})${once}); }()`, type: STRING };
    }
    const s = this.temp('s');
    const p = this.temp('p');
    const at = this.temp('at');
    const find = `const std::string ${p} = ${this.expr(pattern, STRING)}; const std::string by = ${text};`;
    // JavaScript: an empty pattern matches at 0 (replace: find gives 0) and between every character (replaceAll)
    if (all) {
      return { code: `[&] { std::string ${s} = ${subject}; ${find} if (${p}.empty()) { std::string r = by; for (const char ch : ${s}) { r += ch; r += by; } return r; } for (size_t ${at} = ${s}.find(${p}); ${at} != std::string::npos; ${at} = ${s}.find(${p}, ${at} + by.size())) ${s}.replace(${at}, ${p}.size(), by); return ${s}; }()`, type: STRING };
    }
    return { code: `[&] { std::string ${s} = ${subject}; ${find} const size_t ${at} = ${s}.find(${p}); if (${at} != std::string::npos) ${s}.replace(${at}, ${p}.size(), by); return ${s}; }()`, type: STRING };
  }

  /** A regular expression written in place, or held by a constant written with one: its literal. */
  #regexOf(e: TS.Expression): TS.RegularExpressionLiteral | null {
    const ts = this.c.ts;
    if (e.kind === ts.SyntaxKind.RegularExpressionLiteral) return e as TS.RegularExpressionLiteral;
    if (!ts.isIdentifier(e)) return null;
    const decl = this.symbolOf(e)?.valueDeclaration;
    if (!decl || !ts.isVariableDeclaration(decl) || !decl.initializer || !ts.isVariableDeclarationList(decl.parent)
      || (decl.parent.flags & ts.NodeFlags.Const) === 0) return null;
    return decl.initializer.kind === ts.SyntaxKind.RegularExpressionLiteral ? decl.initializer as TS.RegularExpressionLiteral : null;
  }

  /**
   * re.test(s): std::regex_search with the ECMAScript grammar (std::regex's own default), compiled once. Flags other
   * than i (case) change what test answers (g and y keep a position between calls) and are refused.
   */
  #regexTest(regex: TS.RegularExpressionLiteral, arg: TS.Expression): Value {
    const text = regex.text;
    const slash = text.lastIndexOf('/');
    const body = text.slice(1, slash);
    const flags = text.slice(slash + 1);
    if (/[^i]/.test(flags)) return this.fail(regex, `a regular expression with the flags ${flags} (only i keeps test free of state)`);
    if (body.includes(')"')) return this.fail(regex, 'a regular expression that ends a C++ raw string');
    this.c.includes.add('regex');
    this.c.includes.add('string');
    const options = flags.includes('i') ? 'std::regex::ECMAScript | std::regex::icase' : 'std::regex::ECMAScript';
    const re = this.temp('re');
    return { code: `[&] { static const std::regex ${re}(R"(${body})", ${options}); return std::regex_search(${this.expr(arg, STRING)}, ${re}); }()`, type: BOOL };
  }

  /** The field a position of a tuple is (`p[0]`: its first), or null when the value is no tuple or the index no literal. */
  #tupleField(type: CppType, index: TS.Expression): { name: string; type: CppType } | null {
    const t = target(type);
    if (t.k !== 'struct' || !this.c.ts.isNumericLiteral(index)) return null;
    const field = this.c.types.structs.get(t.name)?.fields.find((f) => f.source === index.text);
    return field && /^\d+$/.test(field.source) ? { name: field.name, type: field.type } : null;
  }

  #setMethod(e: TS.CallExpression, obj: Value, method: string): Value {
    const element = elementOf(obj.type)!;
    const member = (): string => this.expr(e.arguments[0]!, element);
    if (method === 'add') return { code: `${obj.code}.insert(${member()})`, type: { k: 'void' } };
    if (method === 'has') return { code: `(${obj.code}.count(${member()}) != 0)`, type: BOOL };
    if (method === 'delete') return { code: `${obj.code}.remove(${member()})`, type: BOOL };
    if (method === 'clear') return { code: `${obj.code}.clear()`, type: { k: 'void' } };
    return this.builtin(e, obj);
  }

  /** The entry of a key, or null: a pointer to it (the entry itself when the map holds pointers). */
  #mapEntry(map: string, type: Extract<CppType, { k: 'map' }>, key: string): Value {
    const holdsPointers = isHandle(type.of);
    const ret: CppType = holdsPointers ? type.of : { k: 'ptr', to: type.of };
    return { code: `[&]() -> ${render(ret)} { auto it = ${map}.find(${key}); return it == ${map}.end() ? nullptr : ${holdsPointers ? 'it->second' : '&it->second'}; }()`, type: ret };
  }

  #mapMethod(e: TS.CallExpression, obj: Value, method: string): Value {
    const map = target(obj.type) as Extract<CppType, { k: 'map' }>;
    const key = (): string => this.expr(e.arguments[0]!, map.key);
    if (method === 'get') return this.#mapEntry(obj.code, map, key());
    if (method === 'set') return { code: `${obj.code}[${key()}] = ${this.expr(e.arguments[1]!, map.of)}`, type: { k: 'void' } };
    if (method === 'has') return { code: `(${obj.code}.count(${key()}) != 0)`, type: BOOL };
    if (method === 'delete') return { code: `${obj.code}.erase(${key()})`, type: { k: 'void' } };
    if (method === 'clear') return { code: `${obj.code}.clear()`, type: { k: 'void' } };
    if (method === 'keys') {
      this.c.includes.add(RUNTIME_INCLUDE);
      return { code: `js::keys_of(${obj.code})`, type: { k: 'vector', of: map.key } };
    }
    if (method === 'values') {
      this.c.includes.add(RUNTIME_INCLUDE);
      return { code: `js::values_of(${obj.code})`, type: { k: 'vector', of: map.of } };
    }
    return this.builtin(e, obj);
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
        const ret: CppType = isHandle(element) ? element : { k: 'ptr', to: element };
        const names = this.inlineParams(args[0]!, [element]);
        const x = names?.[0] ?? 'x';
        const test = names ? this.expr((args[0] as TS.ArrowFunction).body as TS.Expression) : `(${this.callback(args[0]!, [element], BOOL)})(x)`;
        if (names) this.endInline();
        return { code: `[&]() -> ${render(ret)} { for (${paramDecl(element, x)} : ${obj.code}) if (${test}) return ${isHandle(element) ? x : `&${x}`}; return nullptr; }()`, type: ret };
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
      case 'reduce': {
        // xs.reduce(f, start): the running value through every element in order (without a start JavaScript begins
        // at the first element and throws on an empty array: refused)
        if (args.length !== 2) return this.fail(e, 'Array.reduce without a start value');
        const literal = this.c.flow.slotOfExpr(e);
        const start = this.value(args[1]!, want);
        const type: CppType = want ?? (literal ? this.c.types.slot(literal, 'local') : start.type);
        const acc = this.temp('acc');
        const names = this.inlineParams(args[0]!, [type, element, INT]);
        if (names) {
          const x = names[1] ?? 'unused';
          const index = names[2] ? this.temp('i') : null;
          const body = this.expr((args[0] as TS.ArrowFunction).body as TS.Expression, type);
          this.endInline();
          const bindAcc = names[0] ? `const ${render(type)} ${names[0]} = ${acc}; ` : '';
          const loop = index
            ? `for (int ${index} = 0; ${index} < int(${obj.code}.size()); ${index}++) { ${paramDecl(element, x)} = ${obj.code}[${index}]; const int ${names[2]} = ${index}; ${bindAcc}${acc} = ${body}; }`
            : `for (${paramDecl(element, x)} : ${obj.code}) { ${bindAcc}${acc} = ${body}; }`;
          return { code: `[&] { ${render(type)} ${acc} = ${this.coerce(start, type, args[1])}; ${loop} return ${acc}; }()`, type };
        }
        const fold = this.callback(args[0]!, [type, element], type);
        return { code: `[&] { ${render(type)} ${acc} = ${this.coerce(start, type, args[1])}; for (const auto &x : ${obj.code}) ${acc} = (${fold})(${acc}, x); return ${acc}; }()`, type };
      }
      case 'slice': {
        // a copy (no arguments: the whole array, a working copy), or a range with JavaScript's rules: an index below
        // zero counts from the end, every index clamped to [0, length], an end before the start makes it empty
        const type = target(obj.type);
        if (args.length === 0) return { code: `${render(type)}(${obj.code})`, type };
        const src = this.temp('from');
        const n = this.temp('n');
        const at = this.temp('at');
        const a = this.temp('a');
        const b = this.temp('b');
        const end = args[1] ? `${at}(int(${this.expr(args[1]!, INT)}))` : n;
        this.c.includes.add('algorithm');
        return {
          code: `[&] { const auto &${src} = ${obj.code}; const int ${n} = int(${src}.size()); auto ${at} = [${n}](int i) { return i < 0 ? std::max(${n} + i, 0) : std::min(i, ${n}); }; `
            + `const int ${a} = ${at}(int(${this.expr(args[0]!, INT)})); const int ${b} = ${end}; return ${render(type)}(${src}.begin() + ${a}, ${src}.begin() + std::max(${a}, ${b})); }()`,
          type,
        };
      }
      default:
        return this.builtin(e, obj);
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
    const made = this.c.checker.getTypeAtLocation(e);
    if (this.c.types.isClass(made)) {
      const type = this.c.types.ofType(made, 'local', null);
      const decl = this.c.checker.getResolvedSignature(e)?.declaration as (TS.Node & { symbol?: TS.Symbol }) | undefined;
      const plan = decl?.symbol ? this.c.functions.get(decl.symbol) : undefined;
      if (plan) this.uses?.add(plan.module);
      return { code: `std::make_shared<${render(target(type))}>(${plan ? this.#argumentsOf(plan, e.arguments ?? [], new Map()).join(', ') : ''})`, type };
    }
    const slot = this.c.flow.slotOfExpr(e);
    const type = want && (want.k === 'vector' || want.k === 'map' || want.k === 'set') ? want : slot ? this.c.types.slot(slot, 'local') : want;
    if (!type) return this.fail(e, `new ${name}`);
    if (/^(Map|Set|WeakMap|WeakSet)$/.test(name)) {
      this.c.includes.add(RUNTIME_INCLUDE);
      // new Set(items), new Map(pairs): the runtime's constructor reads any range
      return { code: e.arguments?.length ? `${render(type)}(${this.expr(e.arguments[0]!)})` : `${render(type)}{}`, type };
    }
    if (/Array$/.test(name) && e.arguments?.length === 1) return { code: `${render(type)}(${this.expr(e.arguments[0]!, INT)})`, type };
    return this.fail(e, `new ${name}`);
  }

  #prefix(e: TS.PrefixUnaryExpression): Value {
    const ts = this.c.ts;
    const op = { [ts.SyntaxKind.ExclamationToken]: '!', [ts.SyntaxKind.MinusToken]: '-', [ts.SyntaxKind.PlusToken]: '+',
      [ts.SyntaxKind.TildeToken]: '~', [ts.SyntaxKind.PlusPlusToken]: '++', [ts.SyntaxKind.MinusMinusToken]: '--' }[e.operator];
    if (op === '-' && ts.isNumericLiteral(e.operand) && this.c.allowedNumbers.includes(-Number(e.operand.text))) {
      const text = e.operand.getText().replace(/_/g, '');
      return integerLiteral(text) === 'int' ? { code: `-${text}`, type: INT } : { code: `-${realLiteral(text, this.c.spec.number)}`, type: this.c.types.number(Num.Float, 'local') };
    }
    const v = this.value(e.operand);
    // ~x makes x an int32 first (ToInt32), as | and & do
    if (op === '~' && !(v.type.k === 'int' && v.type.name !== 'int64_t' && v.type.name !== 'uint32_t')) return { code: `~int32_t(int64_t(${v.code}))`, type: { k: 'int', name: 'int32_t' } };
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
      // xs[i] ?? d: past the end of a list JavaScript reads undefined
      if (ts.isElementAccessExpression(e.left)) {
        const list = this.value(e.left.expression);
        const element = target(list.type).k === 'vector' ? elementOf(list.type) : null;
        if (element) {
          const held = this.temp('list');
          const at = this.temp('at');
          const fallback = `${render(element)}(${this.expr(e.right, element)})`;
          // the list itself may be absent (`a?.b?.[i] ?? d`): read past nothing, as past its end
          const whole = this.temp('whole');
          const guard = list.type.k === 'vector' ? `const auto &${held} = ${list.code};` : `const auto &${whole} = ${list.code}; if (!${whole}) return ${fallback}; const auto &${held} = *${whole};`;
          return { code: `[&]() -> ${render(element)} { ${guard} const auto ${at} = ${this.expr(e.left.argumentExpression, INT)}; return ${at} >= 0 && size_t(${at}) < ${held}.size() ? ${held}[size_t(${at})] : ${fallback}; }()`, type: element };
        }
      }
      const left = this.value(e.left);
      // A pointer is absent as nullptr: `p ?? null` is p, `p ?? q` takes q where p is absent.
      if (left.type.k === 'ptr') {
        if (e.right.kind === ts.SyntaxKind.NullKeyword || (ts.isIdentifier(e.right) && e.right.text === 'undefined')) return left;
        // a value in place of what is pointed at (`table[k] ?? 0`): the result is the value, the left read once
        if (byValue(left.type.to) || left.type.to.k === 'string') {
          const held = this.temp('found');
          return { code: `[&] { const auto ${held} = ${left.code}; return ${held} ? *${held} : ${render(left.type.to)}(${this.expr(e.right, left.type.to)}); }()`, type: left.type.to };
        }
        return { code: `(${left.code} ? ${left.code} : ${this.expr(e.right, left.type)})`, type: left.type };
      }
      if (left.type.k === 'dynamic') {
        const held = this.temp('held');
        return { code: `[&] { const js::Value ${held} = ${left.code}; return ${held}.nullish() ? ${this.box(this.value(e.right))} : ${held}; }()`, type: DYNAMIC };
      }
      // what C++ holds as never absent cannot be: the left is the value
      if (left.type.k !== 'optional') return left;
      return { code: `(${left.code} ? *${left.code} : ${this.expr(e.right, left.type.of)})`, type: left.type.of };
    }
    if (k === ts.SyntaxKind.BarBarToken && ts.isIdentifier(e.right) && e.right.text === 'undefined') {
      return this.value(e.left); // `x || undefined` stores false as absent: a C++ bool is false either way
    }
    const left = this.value(e.left);
    const assigns = k >= ts.SyntaxKind.FirstAssignment && k <= ts.SyntaxKind.LastAssignment;
    if (e.operatorToken.kind === ts.SyntaxKind.InKeyword) return this.#inObject(e);
    // a member set on a running value, or through a setter
    if (e.operatorToken.kind === ts.SyntaxKind.EqualsToken && ts.isPropertyAccessExpression(e.left)) {
      const written = this.c.checker.getSymbolAtLocation(e.left.name)?.declarations?.find((d) => ts.isSetAccessorDeclaration(d));
      const setter = written ? this.c.setters.get(written) : undefined;
      if (setter) return { code: `${this.value(e.left.expression).code}->${setter.cpp}(${this.expr(e.right, setter.params[0]!.type)})`, type: VOID };
      const holder = this.value(e.left.expression);
      if (target(holder.type).k === 'dynamic') return { code: `${holder.code}.set(${JSON.stringify(e.left.name.text)}, ${this.box(this.value(e.right))})`, type: VOID };
    }
    const enumSide = left.type.k === 'enum' ? left.type : undefined;
    const right = this.value(e.right, enumSide ?? (assigns ? left.type : undefined));
    const text = e.operatorToken.getText();
    const op = text === '===' ? '==' : text === '!==' ? '!=' : text;
    if ((left.type.k === 'dynamic' || right.type.k === 'dynamic') && op !== '=' && op !== '&&' && op !== '||') {
      if (op === '==' || op === '!=') {
        // null and undefined are two values to ===, one to ==
        const nothing = (n: TS.Expression): string | null => (n.kind === ts.SyntaxKind.NullKeyword ? 'Null' : ts.isIdentifier(n) && n.text === 'undefined' ? 'Undefined' : null);
        const kind = nothing(e.right) ?? nothing(e.left);
        const held = left.type.k === 'dynamic' ? left : right;
        const not = op === '!=' ? '!' : '';
        if (kind) return { code: text.length === 3 ? `${not}(${held.code}.kind() == js::Value::Kind::${kind})` : `${not}${held.code}.nullish()`, type: BOOL };
        return { code: `${not}${this.box(left)}.same(${this.box(right)})`, type: BOOL };
      }
      const text2 = left.type.k === 'string' || left.type.k === 'literal' || right.type.k === 'string' || right.type.k === 'literal';
      const side = (v: Value): string => (v.type.k === 'dynamic' ? (text2 ? `${v.code}.text()` : `${v.code}.number()`) : v.type.k === 'literal' ? `std::string(${v.code})` : v.code);
      if (text2 && op === '+') return { code: `(${side(left)} + ${side(right)})`, type: STRING };
      const real = this.c.types.number(Num.Float, 'local');
      return { code: `(${side(left)} ${op} ${side(right)})`, type: /^[<>]=?$/.test(op) ? BOOL : real };
    }
    if (op === '=') return { code: `${left.code} = ${this.coerce(right, left.type, e.right)}`, type: left.type };
    if (['==', '!=', '<', '>', '<=', '>=', '&&', '||'].includes(op)) {
      const leftCode = ts.isStringLiteral(e.left) && right.type.k === 'enum' ? this.#enumLiteral(right.type.name, e.left) : left.code;
      // an optional is absent as nullopt, not as a null pointer
      if (right.type.k === 'null' && left.type.k === 'optional') return { code: `${leftCode} ${op} std::nullopt`, type: BOOL };
      if (left.type.k === 'null' && right.type.k === 'optional') return { code: `${right.code} ${op} std::nullopt`, type: BOOL };
      return { code: `${leftCode} ${op} ${this.coerce(right, enumSide, e.right)}`, type: BOOL };
    }
    if (op === 'instanceof') return this.#instanceOf(e, left);
    // a ** b is Math.pow(a, b): C++ has no such operator
    if (op === '**=' || op === '**') {
      this.c.includes.add('cmath');
      const pow = `std::pow(${left.code}, ${right.code})`;
      if (op === '**=') return { code: `${left.code} = ${isInt(left.type) ? `${render(left.type)}(${pow})` : pow}`, type: left.type };
      const type = this.numberType(e);
      return { code: isInt(type) ? `${render(type)}(${pow})` : pow, type };
    }
    // bitwise operators: JavaScript makes each operand an int32 (ToInt32: truncated, modulo 2^32), counts a shift
    // modulo 32 and gives >>> as an unsigned 32-bit value; C++ refuses them on a real, and a shift by 32 or a left
    // shift into the sign bit is undefined
    const bitwise = op.replace(/=$/, '');
    if (['|', '&', '^', '<<', '>>', '>>>'].includes(bitwise) && op !== '>=' && op !== '<=') {
      const i32 = (v: Value): string => (isInt(v.type) && v.type.k === 'int' && v.type.name !== 'int64_t' && v.type.name !== 'uint32_t' ? v.code : `int32_t(int64_t(${v.code}))`);
      const a = i32(left);
      const b = i32(right);
      const code = bitwise === '<<' ? `int32_t(uint32_t(${a}) << (uint32_t(${b}) & 31u))`
        : bitwise === '>>' ? `(${a} >> (uint32_t(${b}) & 31u))`
          : bitwise === '>>>' ? `int64_t(uint32_t(${a}) >> (uint32_t(${b}) & 31u))`
            : `(${a} ${bitwise} ${b})`;
      const type: CppType = bitwise === '>>>' ? INT64 : { k: 'int', name: 'int32_t' };
      if (assigns) return { code: `${left.code} = ${render(left.type) === render(type) ? code : `${render(left.type)}(${code})`}`, type: left.type };
      return { code, type };
    }
    if (assigns) return { code: `${left.code} ${op} ${right.code}`, type: left.type };
    if (op === '+' && (left.type.k === 'string' || left.type.k === 'literal' || right.type.k === 'string')) {
      return { code: `${left.type.k === 'literal' ? `std::string(${left.code})` : left.code} + ${right.code}`, type: STRING };
    }
    const type = this.numberType(e);
    // integers whose result is wider than they are: widened before the operator, so it cannot wrap
    if ((op === '*' || op === '+' || op === '-') && render(type) === 'int64_t' && isInt(left.type) && render(left.type) !== 'int64_t') {
      return { code: `(int64_t(${left.code}) ${op} ${right.code})`, type };
    }
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
    if (asked?.k === 'map') {
      const made = this.temp('table');
      const sets: string[] = [];
      for (const p of e.properties) {
        if (!ts.isPropertyAssignment(p) && !ts.isShorthandPropertyAssignment(p)) return this.fail(p, 'a member of a dictionary that is no key and value');
        const key = ts.isComputedPropertyName(p.name) ? this.expr(p.name.expression, asked.key) : `std::string(${JSON.stringify((p.name as TS.Identifier).text)})`;
        sets.push(`${made}.insert_or_assign(${key}, ${this.expr(ts.isPropertyAssignment(p) ? p.initializer : p.name, asked.of)});`);
      }
      this.c.includes.add(RUNTIME_INCLUDE);
      return { code: `[&] { ${render(asked)} ${made}; ${sets.join(' ')} return ${made}; }()`, type: asked };
    }
    if (asked?.k === 'function' && e.properties.length === 1) {
      const p = e.properties[0]!;
      const fn = ts.isPropertyAssignment(p) && p.name.getText() === 'has' ? p.initializer : undefined;
      if (fn && (ts.isArrowFunction(fn) || ts.isFunctionExpression(fn))) return { code: this.callback(fn, asked.params, asked.ret), type: asked };
      if (ts.isMethodDeclaration(p) && p.name.getText() === 'has') return this.fail(p, 'a has() written as a method (write it as has: (k) => ...)');
    }
    // an object that goes into a running value: built as one
    if (asked?.k === 'dynamic') {
      const made = this.temp('made');
      const sets = e.properties.map((p) => {
        if (ts.isPropertyAssignment(p)) return `${made}.set(${JSON.stringify(p.name.getText().replace(/^['"]|['"]$/g, ''))}, ${this.box(this.value(p.initializer))});`;
        if (ts.isShorthandPropertyAssignment(p)) return `${made}.set(${JSON.stringify(p.name.text)}, ${this.box(this.value(p.name))});`;
        return this.fail(p, 'a member of a running object').code;
      });
      this.c.includes.add(RUNTIME_INCLUDE);
      return { code: `[&] { js::Value ${made} = js::Value::object(); ${sets.join(' ')} return ${made}; }()`, type: DYNAMIC };
    }
    const slot = this.c.flow.slotOfExpr(e);
    const type: CppType = asked && asked.k === 'struct' ? asked
      : this.c.types.structFor((slot ? this.c.flow.shapeOf(slot) : null) ?? this.c.checker.getTypeAtLocation(e), 'value');
    if (type.k !== 'struct') return this.fail(e, 'an object that is no struct');
    if (e.properties.some((p, i) => ts.isSpreadAssignment(p) && (i > 0 || ts.isParenthesizedExpression(p.expression) || ts.isConditionalExpression(p.expression)))) return this.#objectBySteps(e, type);
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
    // a tuple asked (or the literal's own type a tuple): its positions in order
    const asTuple = want && target(want).k === 'struct' ? target(want) : null;
    const own = asTuple ?? (this.c.checker.isTupleType(this.c.checker.getContextualType(e) ?? this.c.checker.getTypeAtLocation(e))
      ? this.c.types.structFor(this.c.checker.getContextualType(e) ?? this.c.checker.getTypeAtLocation(e), 'tuple') : null);
    if (own && own.k === 'struct') {
      const struct = this.c.types.structs.get(own.name);
      if (struct && struct.fields.every((f, i) => f.source === String(i)) && struct.fields.length === e.elements.length && !e.elements.some((x) => ts.isSpreadElement(x))) {
        return { code: `${own.name}{${e.elements.map((x, i) => this.expr(x, struct.fields[i]!.type)).join(', ')}}`, type: own };
      }
    }
    const type: CppType = want?.k === 'vector' ? want : slot ? this.c.types.slot(slot, 'local') : { k: 'vector', of: INT };
    if (type.k === 'dynamic') {
      if (e.elements.some((x) => ts.isSpreadElement(x))) return this.fail(e, 'a spread into a running array');
      return { code: `js::Value(std::vector<js::Value>{${e.elements.map((x) => this.box(this.value(x))).join(', ')}})`, type: DYNAMIC };
    }
    const element = elementOf(type) ?? DYNAMIC;
    // [...src]: a copy of src.
    if (e.elements.length === 1 && ts.isSpreadElement(e.elements[0]!)) {
      const src = this.value(e.elements[0]!.expression);
      if (same(target(src.type), type)) return { code: `${render(type)}(${src.code})`, type };
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
