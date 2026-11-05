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
  /** Every file of the source program (tests left out): facts come from every caller and every use. */
  readonly #files: ReadonlySet<TS.SourceFile>;
  readonly #memoBodies: readonly TS.Node[];
  readonly #slots: Slot[] = [];
  readonly #bySymbol = new Map<TS.Node | TS.Symbol, Slot>();
  readonly #byNode = new Map<TS.Node, Slot>();
  /** Every flow; the worklist re-applies one only when a slot it read has changed. */
  readonly #flows: { readonly apply: () => boolean; readonly at: TS.Node }[] = [];
  /** The flow being applied (its reads are recorded), and the flows that read each slot. */
  #current = -1;
  readonly #readers = new Map<Slot, Set<number>>();
  readonly #queue: number[] = [];
  readonly #queued = new Set<number>();
  /** The node being gathered: each flow remembers where it comes from (named when the flows do not settle). */
  #at: TS.Node;
  readonly errors: string[] = [];

  constructor(ts: TypeScriptApi, checker: TS.TypeChecker, files: readonly TS.SourceFile[], spec: TranslationSpec, patterns: Patterns, memoBodies: readonly TS.Node[]) {
    this.#ts = ts;
    this.#checker = checker;
    this.#spec = spec;
    this.#patterns = patterns;
    this.#files = new Set(files);
    this.#memoBodies = memoBodies;
    this.#at = files[0]!;
    for (const file of files) this.#gather(file);
    this.#solve();
  }

  /** The node is in the source program (a declaration of the feature, of a feature it shares code with, of any). */
  inProgram(node: TS.Node): boolean {
    return this.#files.has(node.getSourceFile());
  }

  // ---- slots

  /** A slot's representative; during a flow, the read is recorded so the flow re-runs when the slot changes. */
  find(s: Slot): Slot {
    while (s.parent !== s) {
      s.parent = s.parent.parent;
      s = s.parent;
    }
    if (this.#current >= 0) {
      const readers = this.#readers.get(s);
      if (readers) readers.add(this.#current);
      else this.#readers.set(s, new Set([this.#current]));
    }
    return s;
  }

  /** A slot's facts rose: every flow that read it runs again. */
  #changed(s: Slot): void {
    for (const f of this.#readers.get(s) ?? []) {
      if (this.#queued.has(f)) continue;
      this.#queued.add(f);
      this.#queue.push(f);
    }
  }

  #raiseOwn(s: Slot, own: Own): boolean {
    const r = this.find(s);
    if (own <= r.own) return false;
    r.own = own;
    this.#changed(r);
    return true;
  }

  #markNullable(s: Slot): boolean {
    const r = this.find(s);
    if (r.nullable) return false;
    r.nullable = true;
    this.#changed(r);
    return true;
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
    // the shapes the merge brings together are joined pair by pair here, once (as #meetShape joins a new one)
    const pairs: [TS.Type, TS.Type][] = [];
    for (const [k, t] of drop.shapes) if (!keep.shapes.has(k)) { for (const o of keep.shapes.values()) pairs.push([t, o]); }
    for (const [k, t] of drop.shapes) keep.shapes.set(k, t);
    // Whoever read either slot reads the merged one now, and must see what the other brought.
    const readers = this.#readers.get(drop);
    if (readers) {
      const kept = this.#readers.get(keep);
      if (kept) for (const f of readers) kept.add(f);
      else this.#readers.set(keep, readers);
      this.#readers.delete(drop);
    }
    this.#changed(keep);
    for (const [t, o] of pairs) this.#joinFields(t, o);
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
    // a mapped expression holds the target's value: a slot of its own, its number kind the pattern's (numOf)
    if (!ts.isIdentifier(e) && this.#patterns.match(e)) { const s = this.nodeSlot(e, 'literal', null); s.num = this.numOf(e); return s; }
    if (ts.isParenthesizedExpression(e) || ts.isNonNullExpression(e) || ts.isAsExpression(e) || ts.isSatisfiesExpression(e)) return this.slotOfExpr(e.expression);
    if (ts.isIdentifier(e)) {
      const s = this.symbolOf(e);
      return s && this.#inFeature(s) ? this.symbolSlot(s) : null;
    }
    if (ts.isPropertyAccessExpression(e)) {
      const prop = this.#property(e);
      // Reading a getter runs it: the value is what it returns.
      const decl = prop?.valueDeclaration;
      // A member typed by a type parameter (V of a generic class) is one declaration for every instantiation: read
      // here, it is this instantiation's value (RerootMap<Id, Profile>: a Profile), a slot of its own
      if (prop && decl && this.#mentionsTypeParameter(this.#checker.getTypeOfSymbol(prop))) return this.nodeSlot(e, 'return', this.#typeAt(e));
      if (decl && ts.isGetAccessorDeclaration(decl) && this.inProgram(decl)) return this.returnSlot(decl);
      return prop && this.#isFeatureProperty(prop) ? this.symbolSlot(prop) : null;
    }
    if (ts.isElementAccessExpression(e)) {
      const c = this.slotOfExpr(e.expression);
      return c ? this.elemOf(c) : null;
    }
    if (ts.isArrayLiteralExpression(e) || ts.isObjectLiteralExpression(e) || ts.isNewExpression(e)) return this.nodeSlot(e, 'literal', this.#typeAt(e));
    if (ts.isConditionalExpression(e) || (ts.isBinaryExpression(e) && [ts.SyntaxKind.QuestionQuestionToken, ts.SyntaxKind.BarBarToken].includes(e.operatorToken.kind))) {
      return this.nodeSlot(e, 'literal', this.#typeAt(e));
    }
    if (ts.isCallExpression(e)) {
      const callee = e.expression;
      // A method of the program is a function like any other: its result is what it returns.
      const fn = this.calleeOf(e);
      // A generic function's or method's result typed by its type parameters is this call's instantiation (two
      // instantiations of one class must not meet in one return slot)
      if (fn && this.#genericResult(fn)) return this.nodeSlot(e, 'return', this.#typeAt(e));
      if (fn) return this.returnSlot(fn);
      if (ts.isPropertyAccessExpression(callee)) {
        const method = callee.name.text;
        const receiver = this.slotOfExpr(callee.expression);
        if (method === 'get' && receiver && this.#isMap(callee.expression)) return this.elemOf(receiver);
        if (SAME_ELEMENTS.has(method) && receiver) return receiver;
        if (method === 'map') return this.nodeSlot(e, 'literal', this.#typeAt(e));
        if (method === 'find' && receiver) return this.elemOf(receiver);
      }
      return null;
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
    return decl && ts.isFunctionLike(decl) && this.inProgram(decl) ? decl : null;
  }

  numOf(e: TS.Expression): Num {
    const ts = this.#ts;
    const match = this.#patterns.match(e);
    if (match) return { int: Num.Int, int64: Num.Int64, float: Num.Float, bool: Num.None }[match.pattern.type];
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
      const of = this.#checker.getNonNullableType(this.#typeAt(e.expression));
      if (this.#isContainer(of) || of.flags & ts.TypeFlags.StringLike) return Num.Int;
    }
    if (ts.isPropertyAccessExpression(e) && this.#isValueType(this.#typeAt(e.expression))) return Num.Float;
    if (ts.isCallExpression(e)) {
      const callee = e.expression;
      if (ts.isPropertyAccessExpression(callee) && ts.isIdentifier(callee.expression) && callee.expression.text === 'Math') {
        if (INT_MATH.has(callee.name.text)) return Num.Int;
        if (JOIN_MATH.has(callee.name.text)) return join<Num>(Num.None, ...e.arguments.map((a) => this.numOf(a)));
        return Num.Float;
      }
      if (ts.isPropertyAccessExpression(callee)) {
        const owner = this.#checker.getNonNullableType(this.#typeAt(callee.expression)).getSymbol()?.name ?? '';
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
      const t = this.#typeAt(e);
      return this.isObject(t) ? [t] : [];
    }
    const slot = this.slotOfExpr(e);
    if (slot) return [...this.find(slot).shapes.values()];
    const t = this.#checker.getNonNullableType(this.#typeAt(e));
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
      if (value.kind === ts.SyntaxKind.NullKeyword || (ts.isIdentifier(value) && value.text === 'undefined')) return this.#markNullable(r);
      changed = this.#raiseNum(r, this.numOf(value)) || changed;
      changed = this.#raiseOwn(r, this.ownOf(value)) || changed;
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
    this.#changed(r);
    return true;
  }

  /**
   * `a === b`: the two sides denote one quantity, so an integer on one side is evidence the other is an integer. Only
   * integer kinds are passed (a real may equal an integer value without being one), so the flow stays monotone.
   */
  #equality(target: TS.Expression, other: TS.Expression): void {
    this.#add(() => {
      const t = this.slotOfExpr(target);
      if (!t || !this.#isNumberSlot(this.find(t))) return false;
      const n = this.numOf(other);
      return n === Num.Int || n === Num.Int64 ? this.#raiseNum(t, n) : false;
    });
  }

  #isNumberSlot(s: Slot): boolean {
    return s.type !== null && (this.#checker.getNonNullableType(this.underlying(s.type)).flags & this.#ts.TypeFlags.NumberLike) !== 0;
  }

  /** A shape reaches a slot: kept, and its fields joined with the same-named fields of the shapes already there. */
  #meetShape(s: Slot, reached: TS.Type): boolean {
    const shape = this.underlying(reached);
    const key = this.#shapeKey(shape);
    // a shape the slot holds already was joined with every other shape when it (or the other) arrived: the joins
    // are union-find, done once per pair; only a new shape has pairs to join
    if (s.shapes.has(key)) return false;
    s.shapes.set(key, shape);
    this.#changed(s);
    let changed = true;
    for (const other of s.shapes.values()) if (other !== shape) changed = this.#joinFields(shape, other) || changed;
    return changed;
  }

  #gather(file: TS.SourceFile): void {
    const ts = this.#ts;
    const visit = (n: TS.Node): void => {
      this.#at = n;
      // an expression a pattern maps is the target's own value (ROAD_TUNING.grade.ramp → tn.grade_ramp): opaque, as
      // the emitter writes it, so nothing inside it (the outside object it reads) becomes a value of the feature
      const mapped = ts.isExpression(n) && !ts.isIdentifier(n) ? this.#patterns.match(n) : null;
      if (mapped) { for (const b of mapped.bound.values()) visit(b); return; } // its placeholders are the feature's values
      if (ts.isVariableDeclaration(n)) this.#declaration(n);
      if (ts.isBinaryExpression(n) && n.operatorToken.kind >= ts.SyntaxKind.FirstAssignment && n.operatorToken.kind <= ts.SyntaxKind.LastAssignment) this.#assignment(n);
      if (ts.isBinaryExpression(n) && [ts.SyntaxKind.EqualsEqualsEqualsToken, ts.SyntaxKind.ExclamationEqualsEqualsToken,
        ts.SyntaxKind.EqualsEqualsToken, ts.SyntaxKind.ExclamationEqualsToken].includes(n.operatorToken.kind)) {
        this.#equality(n.left, n.right);
        this.#equality(n.right, n.left);
      }
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
          if (ts.isSpreadElement(x)) this.#add(() => this.#sameElements(this.nodeSlot(n, 'literal', this.#typeAt(n)), x.expression));
          else this.#flow(() => this.elemOf(this.nodeSlot(n, 'literal', this.#typeAt(n))), x);
        }
      }
      if (ts.isObjectLiteralExpression(n)) this.#object(n);
      if (ts.isForOfStatement(n)) this.#forOf(n);
      if (ts.isConditionalExpression(n)) {
        this.#flow(() => this.nodeSlot(n, 'literal', this.#typeAt(n)), n.whenTrue);
        this.#flow(() => this.nodeSlot(n, 'literal', this.#typeAt(n)), n.whenFalse);
      }
      if (ts.isBinaryExpression(n) && [ts.SyntaxKind.QuestionQuestionToken, ts.SyntaxKind.BarBarToken].includes(n.operatorToken.kind)) {
        this.#flow(() => this.nodeSlot(n, 'literal', this.#typeAt(n)), n.left);
        this.#flow(() => this.nodeSlot(n, 'literal', this.#typeAt(n)), n.right);
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
      const t = this.#checker.getNonNullableType(this.#typeAt(source));
      const prop = t.getProperty(propName);
      if (!prop) return false;
      const target = this.symbolSlot(symbol);
      let changed = this.#raiseNum(target, this.find(this.symbolSlot(prop)).num);
      const propType = this.#checker.getNonNullableType(this.#checker.getTypeOfSymbol(prop));
      if (this.isObject(propType) || this.#isContainer(propType)) changed = this.#raiseOwn(target, Own.Ref) || changed;
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

  /** Parameters: an object parameter refers to its caller's object; a callback's parameters come from its receiver. */
  #parameters(fn: TS.SignatureDeclaration): void {
    const ts = this.#ts;
    fn.parameters.forEach((p, i) => {
      const symbol = this.symbolOf(p.name);
      if (!symbol) return;
      const slot = this.symbolSlot(symbol);
      if (this.isObject(this.#checker.getNonNullableType(this.#checker.getTypeOfSymbol(symbol)))) this.#raiseOwn(slot, Own.Ref);
      // A default value is a value the parameter takes (when the caller leaves it out).
      if (p.initializer) this.#flow(() => this.symbolSlot(symbol), p.initializer);
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
    if (elem.nullable) changed = this.#markNullable(r) || changed;
    for (const shape of elem.shapes.values()) changed = this.#meetShape(r, shape) || changed;
    if (this.#isContainerSlot(r) && this.#isContainerSlot(elem)) changed = this.#alias(r, elem) || changed;
    if (elem.shapes.size > 0 || elem.own > Own.None) changed = this.#raiseOwn(r, Own.Ref) || changed;
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
            changed = this.#raiseOwn(to, from.own) || changed;
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
    // A call of a function of the program, by name or as a method: each argument flows into its parameter.
    const fn = this.calleeOf(e);
    if (fn) {
      e.arguments.forEach((a, i) => {
        const p = fn.parameters[i];
        const symbol = p ? this.symbolOf(p.name) : undefined;
        if (symbol) this.#flow(() => this.symbolSlot(symbol), a);
      });
      return;
    }
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
            const out = this.elemOf(this.nodeSlot(e, 'literal', this.#typeAt(e)));
            const ret = this.find(this.returnSlot(fn));
            let changed = this.#raiseNum(out, ret.num);
            changed = this.#raiseOwn(out, ret.own) || changed;
            for (const shape of ret.shapes.values()) changed = this.#meetShape(this.find(out), shape) || changed;
            return changed;
          });
        }
      }
    }
  }

  /** An object literal: each property flows into the field of the type it is made as (and of its own type). */
  #object(o: TS.ObjectLiteralExpression): void {
    const ts = this.#ts;
    for (const p of o.properties) {
      if (ts.isSpreadAssignment(p)) {
        const source = p.expression;
        this.#add(() => {
          let changed = false;
          const from = this.#checker.getNonNullableType(this.#typeAt(source));
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
    const contextual = this.#contextualAt(o);
    const own = this.#typeAt(o);
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
    changed = this.#raiseOwn(a, b.own) || changed;
    for (const shape of b.shapes.values()) changed = this.#meetShape(a, shape) || changed;
    return changed;
  }

  /**
   * The worklist (Kildall): every flow once, then a flow again only when a slot it read has risen. Facts only rise in
   * finite lattices, so it ends; a flow that keeps running past the bound is not monotone, and the error names it.
   */
  #solve(): void {
    for (let i = 0; i < this.#flows.length; i++) {
      this.#queue.push(i);
      this.#queued.add(i);
    }
    const runs = new Map<number, number>();
    for (;;) {
      this.#drain(runs);
      // A number no flow of the program reaches comes from outside it (a parameter nobody in the program calls, a
      // field only an outside object fills): JavaScript's number, a real. It is a fact of the fixpoint, so whatever
      // was derived from the missing evidence is derived again from it.
      let raised = false;
      for (const s of this.#slots) {
        if (s.parent === s && s.num === Num.None && this.#isNumberSlot(s)) raised = this.#raiseNum(s, Num.Float) || raised;
      }
      if (!raised) return;
    }
  }

  /** The worklist run until it is empty. */
  #drain(runs: Map<number, number>): void {
    const bound = 64;
    for (let head = 0; head < this.#queue.length; head++) {
      const f = this.#queue[head]!;
      this.#queued.delete(f);
      const n = (runs.get(f) ?? 0) + 1;
      runs.set(f, n);
      if (n > bound) {
        const where = [...runs].filter(([, k]) => k > bound / 2).map(([i]) => this.#flows[i]!.at)
          .map((at) => `${at.getSourceFile().fileName}:${at.getSourceFile().getLineAndCharacterOfPosition(at.getStart()).line + 1}: ${at.getText().slice(0, 60)}`);
        throw new Error(`the flow of values did not settle; still changing:\n  ${[...new Set(where)].slice(0, 12).join('\n  ')}`);
      }
      this.#current = f;
      try {
        this.#flows[f]!.apply();
      } finally {
        this.#current = -1;
      }
      // The queue only grows at its end: compact what was processed, so a long solve does not hold it all.
      if (head > 65536) {
        this.#queue.splice(0, head + 1);
        head = -1;
      }
    }
    // Every queued flow ran: the next drain starts from an empty queue.
    this.#queue.length = 0;
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

  /** A number slot's final kind: what reached it in the whole program; with no evidence, a real (JavaScript's number). */
  numberOf(s: Slot): Num {
    const r = this.find(s);
    return r.num === Num.None ? Num.Float : r.num;
  }

  // ---- helpers

  /** The checker's answers for a node, asked once: the program is fixed, so they never change while the flow solves
   *  (each flow is applied many times; these queries were most of the solve). */
  readonly #typeAtCache = new Map<TS.Node, TS.Type>();
  #typeAt(n: TS.Node): TS.Type {
    let t = this.#typeAtCache.get(n);
    if (t === undefined) { t = this.#checker.getTypeAtLocation(n); this.#typeAtCache.set(n, t); }
    return t;
  }

  readonly #contextualCache = new Map<TS.Expression, TS.Type | undefined>();
  #contextualAt(e: TS.Expression): TS.Type | undefined {
    if (this.#contextualCache.has(e)) return this.#contextualCache.get(e);
    const t = this.#checker.getContextualType(e);
    this.#contextualCache.set(e, t);
    return t;
  }

  /** A type that is, or holds, a type parameter (T, V[], Map<K, V>, V | undefined...). */
  #mentionsTypeParameter(t: TS.Type, seen = new Set<TS.Type>()): boolean {
    const ts = this.#ts;
    if (seen.has(t)) return false;
    seen.add(t);
    if (t.flags & ts.TypeFlags.TypeParameter) return true;
    if (t.isUnionOrIntersection()) return t.types.some((x) => this.#mentionsTypeParameter(x, seen));
    const isReference = (t.flags & ts.TypeFlags.Object) !== 0 && ((t as TS.ObjectType).objectFlags & ts.ObjectFlags.Reference) !== 0;
    const args = isReference ? this.#checker.getTypeArguments(t as TS.TypeReference) : [];
    return args.some((x) => this.#mentionsTypeParameter(x, seen));
  }

  /** The function's declared result depends on type parameters (its own, or its class's). */
  #genericResult(fn: TS.SignatureDeclaration): boolean {
    const sig = this.#checker.getSignatureFromDeclaration(fn);
    return sig ? this.#mentionsTypeParameter(this.#checker.getReturnTypeOfSignature(sig)) : false;
  }

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

  /** Two shapes in one slot: their same-named fields are one slot (TypeScript is structural). */
  #joinFields(shape: TS.Type, other: TS.Type): boolean {
    let changed = false;
    for (const p of this.#checker.getPropertiesOfType(shape)) {
      const q = other.getProperty(p.name);
      if (q && this.#isFeatureProperty(p) && this.#isFeatureProperty(q)) changed = this.unify(this.symbolSlot(p), this.symbolSlot(q)) || changed;
    }
    return changed;
  }

  #isNamed(t: TS.Type): boolean {
    const name = (t.aliasSymbol ?? t.getSymbol())?.name;
    return name !== undefined && name !== '__type' && name !== '__object';
  }

  /** A type's key, once per type object (the checker's types are stable: typeToString was the solve's costliest step). */
  readonly #shapeKeys = new Map<TS.Type, string>();
  #shapeKey(t: TS.Type): string {
    let k = this.#shapeKeys.get(t);
    if (k === undefined) { k = this.#checker.typeToString(t, undefined, this.#ts.TypeFormatFlags.NoTruncation | this.#ts.TypeFormatFlags.InTypeAlias); this.#shapeKeys.set(t, k); }
    return k;
  }

  #property(e: TS.PropertyAccessExpression): TS.Symbol | undefined {
    const owner = this.#checker.getNonNullableType(this.#typeAt(e.expression));
    return owner.getProperty(e.name.text) ?? this.symbolOf(e.name);
  }

  /** A property of a type the feature declares or builds (not of a container, a string, or a target value type). */
  readonly #featureProps = new Map<TS.Symbol, boolean>();
  #isFeatureProperty(p: TS.Symbol): boolean {
    const known = this.#featureProps.get(p);
    if (known !== undefined) return known;
    const v = this.#isFeaturePropertyOf(p);
    this.#featureProps.set(p, v);
    return v;
  }

  #isFeaturePropertyOf(p: TS.Symbol): boolean {
    const decl = p.valueDeclaration ?? p.declarations?.[0];
    if (!decl || !this.inProgram(decl)) return false;
    const holder = decl.parent;
    const holderName = holder && (this.#ts.isInterfaceDeclaration(holder) || this.#ts.isTypeAliasDeclaration(holder)) ? holder.name.text : '';
    return !this.#spec.types[holderName];
  }

  #inFeature(s: TS.Symbol): boolean {
    const decl = s.valueDeclaration ?? s.declarations?.[0];
    return decl !== undefined && this.inProgram(decl);
  }

  #isValueType(t: TS.Type): boolean {
    const n = this.#checker.getNonNullableType(t);
    return Boolean(this.#spec.types[(n.aliasSymbol ?? n.getSymbol())?.name ?? '']);
  }

  #isMap(e: TS.Expression): boolean {
    const name = this.#checker.getNonNullableType(this.#typeAt(e)).getSymbol()?.name ?? '';
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
}
