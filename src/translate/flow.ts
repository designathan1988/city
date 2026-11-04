// The one owner of what every value of the feature is in C++. Each place a value lives is a slot: a variable, a
// parameter, a field (one per property symbol), a function's return, a container's elements and keys, a literal.
// Every way a value moves is a flow between slots (assignment, initializer, argument to parameter, return to call,
// push / set / add into a container, spread, destructuring, element to loop variable). Three facts per slot, each a
// lattice with a bottom (no evidence yet), solved together by iterating every flow until nothing changes (a monotone
// framework: Kildall's worklist; it ends because every fact only rises in a finite lattice):
// - number:    none < int < int64 < float. int: integer literals, lengths, Math.floor..., integers; int64: the
//              target's cell keys; float: anything with a fraction, a division, the target's value types (V2).
// - ownership: none < ref < value. ref: a field, an element, a parameter, a loop variable, an object an index owns;
//              value: an object made here. A slot that receives both holds a value (a copy: a pointer could dangle).
// - shapes:    the object types that reach the slot (TypeScript is structural): the slot's struct is the widest one,
//              and every other must be a subset of it. Two shapes that meet have their same-named fields joined.
// A container that flows into another is the same container (JavaScript shares it): their elements are one slot
// (union-find). Facts come from the declared types (getTypeOfSymbol): the checker's narrowed type at a use ignores
// assignments made inside callbacks (TypeScript issue #9998), which this flow sees.
import type * as TS from 'typescript';
import type { TypeScriptApi } from '../graph/typescript.ts';
import type { Patterns } from './patterns.ts';
import type { TranslationSpec } from './spec.ts';

/** none < int < int64 < float. */
export const Num = { None: 0, Int: 1, Int64: 2, Float: 3 } as const;
export type Num = (typeof Num)[keyof typeof Num];
/** none < ref < value. */
export const Own = { None: 0, Ref: 1, Value: 2 } as const;
export type Own = (typeof Own)[keyof typeof Own];

/** The join of a lattice: the higher fact. */
export function join<T extends number>(...facts: T[]): T {
  return facts.reduce((a, b) => (b > a ? b : a), 0 as T);
}

export type SlotRole = 'field' | 'param' | 'local' | 'return' | 'element' | 'key' | 'literal';

export class Slot {
  parent: Slot = this;
  num: Num = Num.None;
  own: Own = Own.None;
  nullable = false;
  readonly shapes = new Map<string, TS.Type>();
  readonly id: number;
  readonly role: SlotRole;
  readonly name: string;
  readonly type: TS.Type | null;
  #elem: Slot | null = null;
  #key: Slot | null = null;
  constructor(id: number, role: SlotRole, name: string, type: TS.Type | null) {
    this.id = id;
    this.role = role;
    this.name = name;
    this.type = type;
  }
  get elem(): Slot | null {
    return this.#elem;
  }
  set elem(s: Slot | null) {
    this.#elem = s;
  }
  get key(): Slot | null {
    return this.#key;
  }
  set key(s: Slot | null) {
    this.#key = s;
  }
}

const INT_MATH = new Set(['floor', 'ceil', 'round', 'trunc', 'sign']);
const JOIN_MATH = new Set(['max', 'min', 'abs']);
const CALLBACK_METHODS = new Set(['forEach', 'map', 'filter', 'some', 'every', 'find', 'findIndex']);
const SAME_ELEMENTS = new Set(['filter', 'slice', 'sort', 'reverse', 'concat']);

export class FlowGraph {
  readonly #ts: TypeScriptApi;
  readonly #checker: TS.TypeChecker;
  readonly #spec: TranslationSpec;
  readonly #patterns: Patterns;
  readonly #file: TS.SourceFile;
  readonly #memoBodies: readonly TS.Node[];
  readonly #fieldNames: RegExp;
  readonly #paramNames: RegExp;
  readonly #slots: Slot[] = [];
  readonly #bySymbol = new Map<TS.Node | TS.Symbol, Slot>();
  readonly #byNode = new Map<TS.Node, Slot>();
  /** Every flow: re-applied until no fact changes. */
  readonly #flows: { readonly apply: () => boolean; readonly at: TS.Node }[] = [];
  /** The node being gathered: each flow remembers where it comes from (named when the flows do not settle). */
  #at: TS.Node;
  readonly errors: string[] = [];

  constructor(ts: TypeScriptApi, checker: TS.TypeChecker, file: TS.SourceFile, spec: TranslationSpec, patterns: Patterns, memoBodies: readonly TS.Node[]) {
    this.#ts = ts;
    this.#checker = checker;
    this.#spec = spec;
    this.#patterns = patterns;
    this.#file = file;
    this.#memoBodies = memoBodies;
    this.#at = file;
    this.#fieldNames = new RegExp(`^(${spec.integers.match})$`);
    this.#paramNames = new RegExp(`^(${spec.integers.params})$`);
    this.#gather(file);
    this.#solve();
  }

  // ---- slots

  find(s: Slot): Slot {
    while (s.parent !== s) {
      s.parent = s.parent.parent;
      s = s.parent;
    }
    return s;
  }

  #new(role: SlotRole, name: string, type: TS.Type | null): Slot {
    const s = new Slot(this.#slots.length, role, name, type);
    this.#slots.push(s);
    if (type) this.#declare(s, type);
    return s;
  }

  /** What a slot's declared type says by itself: nullability, a named shape, typed array elements, map keys. */
  /**
   * What a type is in the target, before anything else is asked of it: `Omit<T, K>` and `Readonly<T>` are T (the
   * omitted fields are the function's to fill), a branded primitive (`number & { brand }`) is the primitive.
   */
  underlying(type: TS.Type): TS.Type {
    const ts = this.#ts;
    if (type.aliasSymbol && /^(Omit|Readonly)$/.test(type.aliasSymbol.name) && type.aliasTypeArguments?.[0]) return this.underlying(type.aliasTypeArguments[0]);
    if (type.isIntersection()) {
      const primitive = type.types.find((t) => t.flags & (ts.TypeFlags.NumberLike | ts.TypeFlags.StringLike | ts.TypeFlags.BooleanLike));
      if (primitive) return primitive;
    }
    return type;
  }

  #declare(s: Slot, type: TS.Type): void {
    const ts = this.#ts;
    if (type.isUnion() && type.types.some((t) => t.flags & (ts.TypeFlags.Null | ts.TypeFlags.Undefined))) s.nullable = true;
    const t = this.underlying(this.#checker.getNonNullableType(type));
    if (this.isObject(t)) s.shapes.set(this.#shapeKey(t), t);
    const name = t.getSymbol()?.name ?? '';
    if (/^Float(32|64)Array$/.test(name)) this.elemOf(s).num = Num.Float;
    if (/^(Ui|I)nt(8|16|32)Array$/.test(name)) this.elemOf(s).num = Num.Int;
    if (/^(Map|ReadonlyMap)$/.test(name)) {
      const k = this.#checker.getTypeArguments(t as TS.TypeReference)[0];
      if (k && k.flags & ts.TypeFlags.NumberLike) this.keyOf(s).num = Num.Int64;
    }
  }

  /**
   * The slot of a declared name. Keyed by the declaration, not the symbol: the checker makes new transient property
   * symbols each time it widens a fresh object literal's type, and all of them point at the same declaration.
   */
  symbolSlot(symbol: TS.Symbol): Slot {
    const decl = symbol.valueDeclaration ?? symbol.declarations?.[0];
    const identity: TS.Node | TS.Symbol = decl ?? symbol;
    const known = this.#bySymbol.get(identity);
    if (known) return this.find(known);
    const ts = this.#ts;
    const role: SlotRole = decl && ts.isParameter(decl) ? 'param'
      : decl && (ts.isPropertySignature(decl) || ts.isPropertyAssignment(decl) || ts.isShorthandPropertyAssignment(decl) || ts.isPropertyDeclaration(decl)) ? 'field' : 'local';
    const s = this.#new(role, symbol.name, this.#checker.getTypeOfSymbol(symbol));
    this.#bySymbol.set(identity, s);
    if (role === 'field' && this.#fieldNames.test(symbol.name)) this.#raiseNum(s, Num.Int);
    if (decl && (symbol.flags & ts.SymbolFlags.Optional)) s.nullable = true;
    return s;
  }

  /** The slot of a node that holds values of its own: a function's return, a literal, a call's result. */
  nodeSlot(node: TS.Node, role: SlotRole, type: TS.Type | null): Slot {
    const known = this.#byNode.get(node);
    if (known) return this.find(known);
    const s = this.#new(role, role, type);
    this.#byNode.set(node, s);
    return s;
  }

  returnSlot(fn: TS.SignatureDeclaration): Slot {
    const sig = this.#checker.getSignatureFromDeclaration(fn);
    return this.nodeSlot(fn, 'return', sig ? sig.getReturnType() : null);
  }

  elemOf(s: Slot): Slot {
    const r = this.find(s);
    if (!r.elem) {
      const t = r.type ? this.#elementType(r.type) : null;
      r.elem = this.#new('element', `${r.name}[]`, t);
    }
    return this.find(r.elem);
  }

  keyOf(s: Slot): Slot {
    const r = this.find(s);
    if (!r.key) r.key = this.#new('key', `${r.name}{}`, null);
    return this.find(r.key);
  }

  /** Two slots that hold the same container (or two shapes' same-named fields): one slot from now on. */
  unify(a: Slot, b: Slot): boolean {
    const x = this.find(a);
    const y = this.find(b);
    if (x === y) return false;
    const [keep, drop] = x.id < y.id ? [x, y] : [y, x];
    drop.parent = keep;
    keep.num = join(keep.num, drop.num);
    keep.own = join(keep.own, drop.own);
    keep.nullable ||= drop.nullable;
    for (const [k, t] of drop.shapes) keep.shapes.set(k, t);
    if (drop.elem) keep.elem ? this.unify(keep.elem, drop.elem) : (keep.elem = drop.elem);
    if (drop.key) keep.key ? this.unify(keep.key, drop.key) : (keep.key = drop.key);
    return true;
  }

  /**
   * Two names of one container (JavaScript shares it): its contents are one storage, so their elements and keys are
   * one slot. Each name keeps its own binding facts (whether it owns the container or refers to another's).
   */
  #alias(a: Slot, b: Slot): boolean {
    const x = this.find(a);
    const y = this.find(b);
    if (x === y) return false;
    let changed = this.unify(this.elemOf(x), this.elemOf(y));
    if (x.key || y.key) changed = this.unify(this.keyOf(x), this.keyOf(y)) || changed;
    return changed;
  }

  // ---- what an expression is

  /** The slot an expression reads its value from (null: a value made on the spot, with no slot of its own). */
  slotOfExpr(e: TS.Expression): Slot | null {
    const ts = this.#ts;
    if (ts.isParenthesizedExpression(e) || ts.isNonNullExpression(e) || ts.isAsExpression(e) || ts.isSatisfiesExpression(e)) return this.slotOfExpr(e.expression);
    if (ts.isIdentifier(e)) {
      const s = this.symbolOf(e);
      return s && this.#inFeature(s) ? this.symbolSlot(s) : null;
    }
    if (ts.isPropertyAccessExpression(e)) {
      const prop = this.#property(e);
      return prop && this.#isFeatureProperty(prop) ? this.symbolSlot(prop) : null;
    }
    if (ts.isElementAccessExpression(e)) {
      const c = this.slotOfExpr(e.expression);
      return c ? this.elemOf(c) : null;
    }
    if (ts.isArrayLiteralExpression(e) || ts.isObjectLiteralExpression(e) || ts.isNewExpression(e)) return this.nodeSlot(e, 'literal', this.#checker.getTypeAtLocation(e));
    if (ts.isConditionalExpression(e) || (ts.isBinaryExpression(e) && [ts.SyntaxKind.QuestionQuestionToken, ts.SyntaxKind.BarBarToken].includes(e.operatorToken.kind))) {
      return this.nodeSlot(e, 'literal', this.#checker.getTypeAtLocation(e));
    }
    if (ts.isCallExpression(e)) {
      const callee = e.expression;
      if (ts.isPropertyAccessExpression(callee)) {
        const method = callee.name.text;
        const receiver = this.slotOfExpr(callee.expression);
        if (method === 'get' && receiver && this.#isMap(callee.expression)) return this.elemOf(receiver);
        if (SAME_ELEMENTS.has(method) && receiver) return receiver;
        if (method === 'map') return this.nodeSlot(e, 'literal', this.#checker.getTypeAtLocation(e));
        if (method === 'find' && receiver) return this.elemOf(receiver);
        return null;
      }
      const fn = this.calleeOf(e);
      return fn ? this.returnSlot(fn) : null;
    }
    return null;
  }

  /** A call that reads an index (a value memoized by the spec's memoizers). */
  isMemoCall(e: TS.CallExpression): boolean {
    const ts = this.#ts;
    if (!ts.isIdentifier(e.expression)) return false;
    const decl = this.symbolOf(e.expression)?.valueDeclaration;
    return decl !== undefined && ts.isVariableDeclaration(decl) && decl.initializer !== undefined && ts.isCallExpression(decl.initializer)
      && ts.isIdentifier(decl.initializer.expression) && this.#spec.memoizers.includes(decl.initializer.expression.text);
  }

  /** The function a call runs, when it is the feature's (an index read runs its builder). */
  calleeOf(e: TS.CallExpression): TS.SignatureDeclaration | null {
    const ts = this.#ts;
    if (ts.isIdentifier(e.expression)) {
      const s = this.symbolOf(e.expression);
      const decl = s?.valueDeclaration;
      if (decl && ts.isVariableDeclaration(decl) && decl.initializer && ts.isCallExpression(decl.initializer)
        && ts.isIdentifier(decl.initializer.expression) && this.#spec.memoizers.includes(decl.initializer.expression.text)) {
        const builder = decl.initializer.arguments[0];
        return builder && (ts.isArrowFunction(builder) || ts.isFunctionExpression(builder)) ? builder : null;
      }
    }
    const decl = this.#checker.getResolvedSignature(e)?.getDeclaration();
    return decl && ts.isFunctionLike(decl) && decl.getSourceFile() === this.#file ? decl : null;
  }

  numOf(e: TS.Expression): Num {
    const ts = this.#ts;
    const match = this.#patterns.match(e);
    if (match) return Num.Int64;
    if (ts.isNumericLiteral(e)) return /^\d+$/.test(e.getText()) ? Num.Int : Num.Float;
    if (ts.isParenthesizedExpression(e) || ts.isNonNullExpression(e) || ts.isAsExpression(e)) return this.numOf(e.expression);
    if (ts.isPrefixUnaryExpression(e)) return e.operator === ts.SyntaxKind.TildeToken ? Num.Int : e.operator === ts.SyntaxKind.ExclamationToken ? Num.None : this.numOf(e.operand);
    if (ts.isPostfixUnaryExpression(e)) return this.numOf(e.operand);
    if (ts.isConditionalExpression(e)) return join(this.numOf(e.whenTrue), this.numOf(e.whenFalse));
    if (ts.isIdentifier(e) && e.text === 'Infinity') return Num.Float;
    if (ts.isBinaryExpression(e)) {
      const k = e.operatorToken.kind;
      if ([ts.SyntaxKind.AmpersandToken, ts.SyntaxKind.BarToken, ts.SyntaxKind.CaretToken, ts.SyntaxKind.LessThanLessThanToken,
        ts.SyntaxKind.GreaterThanGreaterThanToken, ts.SyntaxKind.GreaterThanGreaterThanGreaterThanToken].includes(k)) return Num.Int;
      if (k === ts.SyntaxKind.SlashToken || k === ts.SyntaxKind.SlashEqualsToken) return Num.Float;
      if (k === ts.SyntaxKind.EqualsToken) return this.numOf(e.right);
      return join(this.numOf(e.left), this.numOf(e.right));
    }
    if (ts.isPropertyAccessExpression(e) && (e.name.text === 'length' || e.name.text === 'size')) {
      const of = this.#checker.getNonNullableType(this.#checker.getTypeAtLocation(e.expression));
      if (this.#isContainer(of) || of.flags & ts.TypeFlags.StringLike) return Num.Int;
    }
    if (ts.isPropertyAccessExpression(e) && this.#isValueType(this.#checker.getTypeAtLocation(e.expression))) return Num.Float;
    if (ts.isCallExpression(e)) {
      const callee = e.expression;
      if (ts.isPropertyAccessExpression(callee) && ts.isIdentifier(callee.expression) && callee.expression.text === 'Math') {
        if (INT_MATH.has(callee.name.text)) return Num.Int;
        if (JOIN_MATH.has(callee.name.text)) return join<Num>(Num.None, ...e.arguments.map((a) => this.numOf(a)));
        return Num.Float;
      }
      if (ts.isPropertyAccessExpression(callee)) {
        const owner = this.#checker.getNonNullableType(this.#checker.getTypeAtLocation(callee.expression)).getSymbol()?.name ?? '';
        const mapped = this.#spec.classes[owner]?.returns[callee.name.text];
        if (mapped !== undefined) return /int64/.test(mapped) ? Num.Int64 : /int/.test(mapped) ? Num.Int : mapped === 'void' ? Num.None : Num.Float;
        if (['indexOf', 'findIndex', 'push'].includes(callee.name.text)) return Num.Int;
      }
      if (ts.isIdentifier(callee)) {
        const external = this.#spec.functions[callee.text];
        const s = this.symbolOf(callee);
        if (external && s && !this.#inFeature(s)) {
          const arg = /\$(\d)/.exec(external.template);
          return arg ? this.numOf(e.arguments[Number(arg[1])]!) : Num.Float;
        }
      }
    }
    const slot = this.slotOfExpr(e);
    return slot ? this.find(slot).num : Num.None;
  }

  ownOf(e: TS.Expression): Own {
    const ts = this.#ts;
    if (ts.isParenthesizedExpression(e) || ts.isNonNullExpression(e) || ts.isAsExpression(e)) return this.ownOf(e.expression);
    if (ts.isObjectLiteralExpression(e)) return this.inMemo(e) ? Own.Ref : Own.Value;
    if (ts.isArrayLiteralExpression(e) || ts.isNewExpression(e)) return Own.Value;
    if (ts.isConditionalExpression(e)) return join(this.ownOf(e.whenTrue), this.ownOf(e.whenFalse));
    if (ts.isBinaryExpression(e)) return join(this.ownOf(e.left), this.ownOf(e.right));
    if (ts.isPropertyAccessExpression(e) || ts.isElementAccessExpression(e)) return Own.Ref;
    if (ts.isCallExpression(e) && ts.isPropertyAccessExpression(e.expression) && ['get', 'find'].includes(e.expression.name.text)) return Own.Ref;
    // An index's value is held by the index: reading it is a reference.
    if (ts.isCallExpression(e) && this.isMemoCall(e)) return Own.Ref;
    if (e.kind === ts.SyntaxKind.NullKeyword || (ts.isIdentifier(e) && e.text === 'undefined')) return Own.None;
    const slot = this.slotOfExpr(e);
    return slot ? this.find(slot).own : Own.None;
  }

  shapesOf(e: TS.Expression): TS.Type[] {
    const ts = this.#ts;
    if (ts.isParenthesizedExpression(e) || ts.isNonNullExpression(e) || ts.isAsExpression(e)) return this.shapesOf(e.expression);
    if (ts.isObjectLiteralExpression(e)) {
      const t = this.#checker.getTypeAtLocation(e);
      return this.isObject(t) ? [t] : [];
    }
    const slot = this.slotOfExpr(e);
    if (slot) return [...this.find(slot).shapes.values()];
    const t = this.#checker.getNonNullableType(this.#checker.getTypeAtLocation(e));
    return this.isObject(t) ? [t] : [];
  }

  // ---- the flows

  /** A value moves into a slot: its number, its ownership, its shapes; a container is the same container. */
  #flow(target: () => Slot | null, value: TS.Expression): void {
    const ts = this.#ts;
    this.#add(() => {
      const t = target();
      if (!t) return false;
      let changed = false;
      const r = this.find(t);
      if (value.kind === ts.SyntaxKind.NullKeyword || (ts.isIdentifier(value) && value.text === 'undefined')) {
        if (!r.nullable) {
          r.nullable = true;
          changed = true;
        }
        return changed;
      }
      changed = this.#raiseNum(r, this.numOf(value)) || changed;
      const own = this.ownOf(value);
      if (own > r.own) {
        r.own = own;
        changed = true;
      }
      for (const shape of this.shapesOf(value)) changed = this.#meetShape(r, shape) || changed;
      const source = this.slotOfExpr(value);
      if (source && this.#isContainerSlot(r) && this.#isContainerSlot(this.find(source))) changed = this.#alias(r, source) || changed;
      return changed;
    });
  }

  #add(apply: () => boolean): void {
    this.#flows.push({ apply, at: this.#at });
  }

  #raiseNum(s: Slot, n: Num): boolean {
    const r = this.find(s);
    if (n <= r.num) return false;
    r.num = n;
    return true;
  }

  /** A shape reaches a slot: kept, and its fields joined with the same-named fields of the shapes already there. */
  #meetShape(s: Slot, reached: TS.Type): boolean {
    const shape = this.underlying(reached);
    const key = this.#shapeKey(shape);
    let changed = false;
    if (!s.shapes.has(key)) {
      s.shapes.set(key, shape);
      changed = true;
    }
    for (const other of s.shapes.values()) {
      if (other === shape) continue;
      for (const p of this.#checker.getPropertiesOfType(shape)) {
        const q = other.getProperty(p.name);
        if (q && this.#isFeatureProperty(p) && this.#isFeatureProperty(q)) changed = this.unify(this.symbolSlot(p), this.symbolSlot(q)) || changed;
      }
    }
    return changed;
  }

  #gather(file: TS.SourceFile): void {
    const ts = this.#ts;
    const visit = (n: TS.Node): void => {
      this.#at = n;
      if (ts.isVariableDeclaration(n)) this.#declaration(n);
      if (ts.isBinaryExpression(n) && n.operatorToken.kind >= ts.SyntaxKind.FirstAssignment && n.operatorToken.kind <= ts.SyntaxKind.LastAssignment) this.#assignment(n);
      if (ts.isReturnStatement(n) && n.expression) {
        const fn = this.#enclosingFunction(n);
        const value = n.expression;
        if (fn) this.#flow(() => this.returnSlot(fn), value);
      }
      if ((ts.isArrowFunction(n) || ts.isFunctionExpression(n)) && !ts.isBlock(n.body)) {
        const body = n.body;
        this.#flow(() => this.returnSlot(n), body);
      }
      if (ts.isFunctionLike(n)) this.#parameters(n);
      if (ts.isCallExpression(n)) this.#call(n);
      if (ts.isArrayLiteralExpression(n)) {
        for (const x of n.elements) {
          if (ts.isSpreadElement(x)) this.#add(() => this.#sameElements(this.nodeSlot(n, 'literal', this.#checker.getTypeAtLocation(n)), x.expression));
          else this.#flow(() => this.elemOf(this.nodeSlot(n, 'literal', this.#checker.getTypeAtLocation(n))), x);
        }
      }
      if (ts.isObjectLiteralExpression(n)) this.#object(n);
      if (ts.isForOfStatement(n)) this.#forOf(n);
      if (ts.isConditionalExpression(n)) {
        this.#flow(() => this.nodeSlot(n, 'literal', this.#checker.getTypeAtLocation(n)), n.whenTrue);
        this.#flow(() => this.nodeSlot(n, 'literal', this.#checker.getTypeAtLocation(n)), n.whenFalse);
      }
      if (ts.isBinaryExpression(n) && [ts.SyntaxKind.QuestionQuestionToken, ts.SyntaxKind.BarBarToken].includes(n.operatorToken.kind)) {
        this.#flow(() => this.nodeSlot(n, 'literal', this.#checker.getTypeAtLocation(n)), n.left);
        this.#flow(() => this.nodeSlot(n, 'literal', this.#checker.getTypeAtLocation(n)), n.right);
      }
      ts.forEachChild(n, visit);
    };
    visit(file);
  }

  #declaration(d: TS.VariableDeclaration): void {
    const ts = this.#ts;
    const init = d.initializer;
    if (ts.isIdentifier(d.name)) {
      const symbol = this.symbolOf(d.name);
      if (symbol && init) this.#flow(() => this.symbolSlot(symbol), init);
      return;
    }
    if (ts.isObjectBindingPattern(d.name) && init) {
      for (const el of d.name.elements) this.#binding(el, init);
    }
  }

  /** `const { a } = s`: a takes the field a of what s is. */
  #binding(el: TS.BindingElement, source: TS.Expression): void {
    const symbol = this.symbolOf(el.name);
    const propName = (el.propertyName ?? el.name).getText();
    if (!symbol) return;
    this.#add(() => {
      const t = this.#checker.getNonNullableType(this.#checker.getTypeAtLocation(source));
      const prop = t.getProperty(propName);
      if (!prop) return false;
      const target = this.symbolSlot(symbol);
      let changed = this.#raiseNum(target, this.find(this.symbolSlot(prop)).num);
      const propType = this.#checker.getNonNullableType(this.#checker.getTypeOfSymbol(prop));
      if ((this.isObject(propType) || this.#isContainer(propType)) && target.own < Own.Ref) {
        target.own = Own.Ref;
        changed = true;
      }
      for (const shape of this.find(this.symbolSlot(prop)).shapes.values()) changed = this.#meetShape(this.find(target), shape) || changed;
      if (this.#isContainerSlot(target)) changed = this.#alias(target, this.symbolSlot(prop)) || changed;
      return changed;
    });
  }

  #assignment(e: TS.BinaryExpression): void {
    const ts = this.#ts;
    const left = e.left;
    const value = e.operatorToken.kind === ts.SyntaxKind.EqualsToken ? e.right : e;
    if (ts.isIdentifier(left) || ts.isPropertyAccessExpression(left)) this.#flow(() => this.slotOfExpr(left), value);
    if (ts.isElementAccessExpression(left)) this.#flow(() => {
      const c = this.slotOfExpr(left.expression);
      return c ? this.elemOf(c) : null;
    }, value);
  }

  /** Parameters: an exported function's come from outside (the spec's names say integers); a callback's from its receiver. */
  #parameters(fn: TS.SignatureDeclaration): void {
    const ts = this.#ts;
    fn.parameters.forEach((p, i) => {
      const symbol = this.symbolOf(p.name);
      if (!symbol) return;
      const slot = this.symbolSlot(symbol);
      if (this.isObject(this.#checker.getNonNullableType(this.#checker.getTypeOfSymbol(symbol)))) slot.own = join<Own>(slot.own, Own.Ref);
      if (this.#exported(fn)) {
        const t = this.#checker.getNonNullableType(this.#checker.getTypeOfSymbol(symbol));
        if (t.flags & ts.TypeFlags.NumberLike) this.#raiseNum(slot, this.#paramNames.test(symbol.name) ? Num.Int : Num.Float);
      }
      const call = fn.parent;
      if ((ts.isArrowFunction(fn) || ts.isFunctionExpression(fn)) && ts.isCallExpression(call) && ts.isPropertyAccessExpression(call.expression)) {
        const method = call.expression.name.text;
        const receiver = call.expression.expression;
        if (CALLBACK_METHODS.has(method) || method === 'sort') {
          if (i === 0 || method === 'sort') this.#add(() => this.#fromElements(slot, receiver));
          if (i === 1 && method !== 'sort') this.#raiseNum(slot, Num.Int);
        }
      }
    });
  }

  /** A callback's element parameter (or a loop variable) is a reference to an element of the receiver. */
  #fromElements(target: Slot, receiver: TS.Expression): boolean {
    const c = this.slotOfExpr(receiver);
    if (!c) return false;
    const elem = this.find(this.elemOf(c));
    const r = this.find(target);
    let changed = this.#raiseNum(r, elem.num);
    if (elem.nullable && !r.nullable) {
      r.nullable = true;
      changed = true;
    }
    for (const shape of elem.shapes.values()) changed = this.#meetShape(r, shape) || changed;
    if (this.#isContainerSlot(r) && this.#isContainerSlot(elem)) changed = this.#alias(r, elem) || changed;
    if (r.own < Own.Ref && (elem.shapes.size > 0 || elem.own > Own.None)) {
      r.own = Own.Ref;
      changed = true;
    }
    return changed;
  }

  #forOf(s: TS.ForOfStatement): void {
    const ts = this.#ts;
    const decl = (s.initializer as TS.VariableDeclarationList).declarations[0];
    if (!decl) return;
    let iterable = s.expression;
    if (ts.isBinaryExpression(iterable) && iterable.operatorToken.kind === ts.SyntaxKind.QuestionQuestionToken) iterable = iterable.left;
    const source = iterable;
    if (ts.isIdentifier(decl.name)) {
      const symbol = this.symbolOf(decl.name);
      if (symbol) this.#add(() => this.#fromElements(this.symbolSlot(symbol), source));
    } else if (ts.isObjectBindingPattern(decl.name)) {
      for (const el of decl.name.elements) {
        const symbol = this.symbolOf(el.name);
        const propName = (el.propertyName ?? el.name).getText();
        if (!symbol) continue;
        this.#add(() => {
          const c = this.slotOfExpr(source);
          if (!c) return false;
          const elem = this.find(this.elemOf(c));
          let changed = false;
          for (const shape of elem.shapes.values()) {
            const prop = shape.getProperty(propName);
            if (!prop) continue;
            const from = this.find(this.symbolSlot(prop));
            const to = this.symbolSlot(symbol);
            changed = this.#raiseNum(to, from.num) || changed;
            if (from.own > to.own) {
              to.own = from.own;
              changed = true;
            }
            for (const sh of from.shapes.values()) changed = this.#meetShape(this.find(to), sh) || changed;
          }
          return changed;
        });
      }
    }
  }

  #call(e: TS.CallExpression): void {
    const ts = this.#ts;
    const callee = e.expression;
    if (ts.isPropertyAccessExpression(callee)) {
      const method = callee.name.text;
      const receiver = callee.expression;
      const container = (): Slot | null => this.slotOfExpr(receiver);
      if (method === 'push' || method === 'add' || method === 'fill') {
        for (const a of e.arguments) this.#flow(() => (container() ? this.elemOf(container()!) : null), a);
      }
      if (method === 'set' && e.arguments.length === 2) {
        this.#flow(() => (container() ? this.keyOf(container()!) : null), e.arguments[0]!);
        this.#flow(() => (container() ? this.elemOf(container()!) : null), e.arguments[1]!);
      }
      if (method === 'map') {
        const fn = e.arguments[0];
        if (fn && (ts.isArrowFunction(fn) || ts.isFunctionExpression(fn))) {
          this.#add(() => {
            const out = this.elemOf(this.nodeSlot(e, 'literal', this.#checker.getTypeAtLocation(e)));
            const ret = this.find(this.returnSlot(fn));
            let changed = this.#raiseNum(out, ret.num);
            if (ret.own > out.own) {
              out.own = ret.own;
              changed = true;
            }
            for (const shape of ret.shapes.values()) changed = this.#meetShape(this.find(out), shape) || changed;
            return changed;
          });
        }
      }
      return;
    }
    const fn = this.calleeOf(e);
    if (!fn) return;
    e.arguments.forEach((a, i) => {
      const p = fn.parameters[i];
      const symbol = p ? this.symbolOf(p.name) : undefined;
      if (symbol) this.#flow(() => this.symbolSlot(symbol), a);
    });
  }

  /** An object literal: each property flows into the field of the type it is made as (and of its own type). */
  #object(o: TS.ObjectLiteralExpression): void {
    const ts = this.#ts;
    for (const p of o.properties) {
      if (ts.isSpreadAssignment(p)) {
        const source = p.expression;
        this.#add(() => {
          let changed = false;
          const from = this.#checker.getNonNullableType(this.#checker.getTypeAtLocation(source));
          for (const target of this.#targetsOf(o)) {
            for (const prop of this.#checker.getPropertiesOfType(from)) {
              const q = target.getProperty(prop.name);
              if (q && this.#isFeatureProperty(prop) && this.#isFeatureProperty(q)) changed = this.unify(this.symbolSlot(prop), this.symbolSlot(q)) || changed;
            }
          }
          return changed;
        });
        continue;
      }
      if (!ts.isPropertyAssignment(p) && !ts.isShorthandPropertyAssignment(p)) continue;
      const name = p.name.getText();
      const value = ts.isPropertyAssignment(p) ? p.initializer : p.name;
      for (const pick of [0, 1]) {
        this.#flow(() => {
          const target = this.#targetsOf(o)[pick];
          const prop = target?.getProperty(name);
          return prop && this.#isFeatureProperty(prop) ? this.symbolSlot(prop) : null;
        }, value);
      }
    }
  }

  /** The types an object literal is made as: what its place asks for (its contextual type), and its own. */
  #targetsOf(o: TS.ObjectLiteralExpression): TS.Type[] {
    const contextual = this.#checker.getContextualType(o);
    const own = this.#checker.getTypeAtLocation(o);
    const out: TS.Type[] = [];
    if (contextual) {
      const t = this.#checker.getNonNullableType(contextual);
      if (this.isObject(t)) out.push(t);
    }
    if (this.isObject(own) && !out.includes(own)) out.push(own);
    return out;
  }

  #sameElements(target: Slot, source: TS.Expression): boolean {
    const s = this.slotOfExpr(source);
    if (!s) return false;
    const a = this.find(this.elemOf(target));
    const b = this.find(this.elemOf(s));
    let changed = this.#raiseNum(a, b.num);
    if (b.own > a.own) {
      a.own = b.own;
      changed = true;
    }
    for (const shape of b.shapes.values()) changed = this.#meetShape(a, shape) || changed;
    return changed;
  }

  /**
   * Every flow applied until none changes a fact. Facts only rise in finite lattices, so this ends; if it does not,
   * a flow is not monotone, and the error names the flows still changing.
   */
  #solve(): void {
    const limit = 50 + this.#flows.length;
    for (let round = 0; ; round++) {
      const changing: TS.Node[] = [];
      for (const f of this.#flows) if (f.apply()) changing.push(f.at);
      if (changing.length === 0) return;
      if (round > limit) {
        const where = [...new Set(changing.map((n) => `${n.getSourceFile().getLineAndCharacterOfPosition(n.getStart()).line + 1}: ${n.getText().slice(0, 60)}`))];
        throw new Error(`the flow of values did not settle after ${round} rounds; still changing:\n  ${where.slice(0, 12).join('\n  ')}`);
      }
    }
  }

  // ---- reading the solution

  /** The struct type a slot holds: the widest shape that reached it; every other must be a subset of it. */
  shapeOf(s: Slot): TS.Type | null {
    const r = this.find(s);
    const shapes = [...r.shapes.values()];
    if (shapes.length === 0) return null;
    const named = shapes.filter((t) => this.#isNamed(t));
    const widest = (named.length > 0 ? named : shapes).reduce((a, b) => (this.#checker.getPropertiesOfType(b).length > this.#checker.getPropertiesOfType(a).length ? b : a));
    const fields = new Set(this.#checker.getPropertiesOfType(widest).map((p) => p.name));
    for (const t of shapes) {
      const extra = this.#checker.getPropertiesOfType(t).map((p) => p.name).filter((n) => !fields.has(n));
      if (extra.length > 0) {
        const msg = `${r.name}: values of different shapes meet (${this.#checker.typeToString(widest)} and ${this.#checker.typeToString(t)})`;
        if (!this.errors.includes(msg)) this.errors.push(msg);
      }
    }
    return widest;
  }

  /** A number slot's final kind: what reached it; with no evidence, the spec's rule for its name, else a float. */
  numberOf(s: Slot): Num {
    const r = this.find(s);
    if (r.num !== Num.None) return r.num;
    if (r.role === 'field' && this.#fieldNames.test(r.name)) return Num.Int;
    if (r.role === 'param' && this.#paramNames.test(r.name)) return Num.Int;
    return Num.Float;
  }

  // ---- helpers

  symbolOf(node: TS.Node): TS.Symbol | undefined {
    const ts = this.#ts;
    const parent = node.parent;
    if (parent && ts.isShorthandPropertyAssignment(parent) && parent.name === node) return this.#checker.getShorthandAssignmentValueSymbol(parent);
    const s = this.#checker.getSymbolAtLocation(node);
    return s && s.flags & ts.SymbolFlags.Alias ? this.#checker.getAliasedSymbol(s) : s;
  }

  inMemo(node: TS.Node): boolean {
    return this.#memoBodies.some((body) => node.pos >= body.pos && node.end <= body.end);
  }

  isObject(type: TS.Type): boolean {
    const ts = this.#ts;
    if (!(type.flags & ts.TypeFlags.Object) || this.#checker.isArrayType(type) || type.getCallSignatures().length > 0) return false;
    const name = (type.aliasSymbol ?? type.getSymbol())?.name ?? '';
    return !this.#spec.types[name] && !this.#spec.classes[name] && !/^(Map|Set|ReadonlyMap|ReadonlySet|ReadonlyArray|Promise)$/.test(name) && !/Array$/.test(name);
  }

  #isNamed(t: TS.Type): boolean {
    const name = (t.aliasSymbol ?? t.getSymbol())?.name;
    return name !== undefined && name !== '__type' && name !== '__object';
  }

  #shapeKey(t: TS.Type): string {
    return this.#checker.typeToString(t, undefined, this.#ts.TypeFormatFlags.NoTruncation | this.#ts.TypeFormatFlags.InTypeAlias);
  }

  #property(e: TS.PropertyAccessExpression): TS.Symbol | undefined {
    const owner = this.#checker.getNonNullableType(this.#checker.getTypeAtLocation(e.expression));
    return owner.getProperty(e.name.text) ?? this.symbolOf(e.name);
  }

  /** A property of a type the feature declares or builds (not of a container, a string, or a target value type). */
  #isFeatureProperty(p: TS.Symbol): boolean {
    const decl = p.valueDeclaration ?? p.declarations?.[0];
    if (!decl || decl.getSourceFile() !== this.#file) return false;
    const holder = decl.parent;
    const holderName = holder && (this.#ts.isInterfaceDeclaration(holder) || this.#ts.isTypeAliasDeclaration(holder)) ? holder.name.text : '';
    return !this.#spec.types[holderName];
  }

  #inFeature(s: TS.Symbol): boolean {
    const decl = s.valueDeclaration ?? s.declarations?.[0];
    return decl !== undefined && decl.getSourceFile() === this.#file;
  }

  #isValueType(t: TS.Type): boolean {
    const n = this.#checker.getNonNullableType(t);
    return Boolean(this.#spec.types[(n.aliasSymbol ?? n.getSymbol())?.name ?? '']);
  }

  #isMap(e: TS.Expression): boolean {
    const name = this.#checker.getNonNullableType(this.#checker.getTypeAtLocation(e)).getSymbol()?.name ?? '';
    return /^(Map|ReadonlyMap)$/.test(name);
  }

  #isContainer(t: TS.Type): boolean {
    const n = this.#checker.getNonNullableType(t);
    const name = n.getSymbol()?.name ?? '';
    return this.#checker.isArrayType(n) || /^(Map|Set|ReadonlyMap|ReadonlySet|ReadonlyArray)$/.test(name) || /Array$/.test(name);
  }

  #isContainerSlot(s: Slot): boolean {
    return s.type !== null && this.#isContainer(s.type);
  }

  #elementType(t: TS.Type): TS.Type | null {
    const n = this.#checker.getNonNullableType(t);
    const name = n.getSymbol()?.name ?? '';
    if (this.#checker.isArrayType(n) || /^(ReadonlyArray|Set|ReadonlySet)$/.test(name)) return this.#checker.getTypeArguments(n as TS.TypeReference)[0] ?? null;
    if (/^(Map|ReadonlyMap)$/.test(name)) return this.#checker.getTypeArguments(n as TS.TypeReference)[1] ?? null;
    return null;
  }

  #enclosingFunction(n: TS.Node): TS.SignatureDeclaration | null {
    for (let p = n.parent; p; p = p.parent) if (this.#ts.isFunctionLike(p)) return p;
    return null;
  }

  /**
   * The function is itself an exported declaration (`export function f`, `export const f = () => ...`): its callers
   * are outside. A function written inside one (a callback, a helper) is not, whatever contains it.
   */
  #exported(fn: TS.Node): boolean {
    const ts = this.#ts;
    const exported = (n: TS.Node): boolean => (ts.canHaveModifiers(n) ? ts.getModifiers(n) ?? [] : []).some((m) => m.kind === ts.SyntaxKind.ExportKeyword);
    if (ts.isFunctionDeclaration(fn)) return ts.isSourceFile(fn.parent) && exported(fn);
    const holder = fn.parent;
    if (holder && ts.isVariableDeclaration(holder) && holder.initializer === fn && ts.isVariableDeclarationList(holder.parent)) {
      const statement = holder.parent.parent;
      return ts.isVariableStatement(statement) && ts.isSourceFile(statement.parent) && exported(statement);
    }
    return false;
  }
}
