// TypeScript types → the target's C++ types, from the facts the flow solved for each slot (types.ts decides nothing on
// its own). Interfaces become structs (fields in declaration order, defaults the game's way); string-literal unions
// enum classes; `T | undefined` std::optional (a reference that may be absent: a pointer; an optional boolean: a bool,
// false when absent); arrays, typed arrays and Sets std::vector; Map std::unordered_map (number keys int64_t); function
// types std::function. A number is the flow's kind. An object a slot refers to (another owner's) is a pointer; one it
// owns, a value. Anonymous shapes get the spec's name, or the name of the type they are the same as (V2).
import type * as TS from 'typescript';
import type { TypeScriptApi } from '../graph/typescript.ts';
import { BOOL, STRING, VOID, type CppType, type IntName } from './cpptype.ts';
import { Num, Own, type FlowGraph, type Slot } from './flow.ts';
import { pascal, snake, upper } from './names.ts';
import type { TranslationSpec } from './spec.ts';

export interface CppField {
  readonly source: string;
  readonly name: string;
  readonly type: CppType;
  init: string;
  readonly comment: string | null;
}

export interface CppStruct {
  readonly name: string;
  readonly comment: string | null;
  readonly fields: CppField[];
  /** The source file that declares it (its module), null when it has no declaration of its own. */
  readonly module: string | null;
  readonly line: number;
  /** The source's name of the type it is made from (null: an anonymous shape, a tuple, a library type). */
  readonly declared?: string | null;
}

export interface CppEnum {
  readonly name: string;
  readonly module: string | null;
  readonly comment: string | null;
  /** The source's string values, in order. */
  readonly values: readonly string[];
}

/** Where a type is used: a field or parameter takes the spec's integer type, a local the plain int. */
export type Use = 'field' | 'param' | 'local' | 'return' | 'element';

/**
 * A reader the source declares itself: an object type whose members are only `get(key): V | undefined` and
 * `has(key): boolean` (methods, or functions held as fields), all of one key. Its key, and the value `get` gives
 * (null with `has` alone); null when the type is anything else.
 */
function readerOf(ts: TypeScriptApi, checker: TS.TypeChecker, type: TS.Type): { key: TS.Type; value: TS.Type | null } | null {
  if (!(type.flags & ts.TypeFlags.Object) || type.getCallSignatures().length > 0) return null;
  const props = checker.getPropertiesOfType(type);
  if (props.length === 0 || props.some((p) => p.name !== 'get' && p.name !== 'has')) return null;
  let key: TS.Type | null = null;
  let value: TS.Type | null = null;
  for (const p of props) {
    const sigs = checker.getSignaturesOfType(checker.getNonNullableType(checker.getTypeOfSymbol(p)), ts.SignatureKind.Call);
    if (sigs.length !== 1 || sigs[0]!.getParameters().length !== 1) return null;
    const k = checker.getTypeOfSymbol(sigs[0]!.getParameters()[0]!);
    if (key && !checker.isTypeAssignableTo(k, key)) return null;
    key ??= k;
    const ret = sigs[0]!.getReturnType();
    if (p.name === 'has' && !(ret.flags & ts.TypeFlags.BooleanLike)) return null;
    if (p.name === 'get') value = checker.getNonNullableType(ret);
  }
  return key ? { key, value } : null;
}

/** What a dictionary holds (an object type with a string index and no fields of its own); null for anything else. */
export function dictionaryOf(ts: TypeScriptApi, checker: TS.TypeChecker, type: TS.Type): TS.Type | null {
  if (!(type.flags & ts.TypeFlags.Object) || checker.isArrayType(type) || checker.getPropertiesOfType(type).length > 0) return null;
  return checker.getIndexInfoOfType(type, ts.IndexKind.String)?.type ?? null;
}

/** A reader with `get` (`interface NodeHeights { get(node: NodeId): number | undefined }`, a Map passed in): the map it is read as, [K, V]. */
export function lookupOf(ts: TypeScriptApi, checker: TS.TypeChecker, type: TS.Type): [TS.Type, TS.Type] | null {
  const reader = readerOf(ts, checker, type);
  return reader?.value ? [reader.key, reader.value] : null;
}

/**
 * A reader with `has` alone (`{ has(node: NodeId): boolean }`: a Set passed in, or a rule written in place): the test
 * it is, its key; null otherwise.
 */
export function membershipOf(ts: TypeScriptApi, checker: TS.TypeChecker, type: TS.Type): TS.Type | null {
  const reader = readerOf(ts, checker, type);
  return reader && !reader.value ? reader.key : null;
}

/** The longest name a shape's fields may give its struct; past it, the first fields and how many there are. */
const MAX_NAME = 48;
const FIELDS_IN_NAME = 3;

export class TypeMapper {
  readonly structs = new Map<string, CppStruct>();
  readonly enums = new Map<string, CppEnum>();
  readonly includes = new Set<string>(['cstdint']);
  /** Structs of the indexes: known for member access, laid out with the index. */
  readonly indexStructs = new Set<string>();
  readonly #ts: TypeScriptApi;
  readonly #checker: TS.TypeChecker;
  readonly #spec: TranslationSpec;
  readonly #flow: FlowGraph;
  readonly #errors: string[];
  readonly #relative: (file: TS.SourceFile) => string;
  /** Anonymous shapes by their printed form: two literals of the same fields are one struct. */
  readonly #anonymous = new Map<string, string>();
  /** Told the module of every struct and enum a type uses (the emitter records what each function needs). */
  observe: ((module: string) => void) | null = null;

  /** The target's own type names: a translated struct that would take one is named after its module too. */
  readonly #reserved: ReadonlySet<string>;
  /** The unit being written: its refusals name it. */
  unit: string | null = null;

  constructor(ts: TypeScriptApi, checker: TS.TypeChecker, spec: TranslationSpec, flow: FlowGraph, errors: string[], relative: (file: TS.SourceFile) => string, reserved: ReadonlySet<string> = new Set()) {
    this.#reserved = reserved;
    this.#ts = ts;
    this.#checker = checker;
    this.#spec = spec;
    this.#flow = flow;
    this.#errors = errors;
    this.#relative = relative;
  }

  #seen<T extends { readonly module: string | null }>(declared: T): T {
    if (declared.module !== null) this.observe?.(declared.module);
    return declared;
  }

  #moduleOf(decl: TS.Node | undefined): string | null {
    return decl && this.#flow.inProgram(decl) ? this.#relative(decl.getSourceFile()) : null;
  }

  /** The C++ type of a slot, used as `use`. */
  slot(s: Slot, use: Use): CppType {
    const r = this.#flow.find(s);
    if (!r.type) return this.number(this.#flow.numberOf(r), use);
    return this.ofType(r.type, use, r);
  }

  number(n: Num, use: Use): CppType {
    // What is stored or crosses the API has an explicit width; a local is the plain int, as the target writes it.
    if (n === Num.Int) return { k: 'int', name: use === 'field' || use === 'param' ? 'int32_t' : 'int' };
    if (n === Num.Int64) return { k: 'int', name: 'int64_t' };
    return { k: 'real', name: this.#spec.number };
  }

  /** A TypeScript type with the facts of the slot that holds it (null: no slot, the type's own defaults). */
  ofType(type: TS.Type, use: Use, slot: Slot | null): CppType {
    const ts = this.#ts;
    const checker = this.#checker;
    // Omit<T>, Readonly<T> and branded primitives are what the flow says they are underneath (one rule, one owner).
    const under = this.#flow.underlying(type);
    if (under !== type) return this.ofType(under, use, slot);
    const f = type.flags;
    // a function's own type parameter: the function is a template, T its caller's (a class's T has no such place)
    if (f & ts.TypeFlags.TypeParameter) {
      const decl = type.getSymbol()?.declarations?.[0];
      if (decl && ts.isTypeParameterDeclaration(decl) && ts.isFunctionLike(decl.parent)) return { k: 'tparam', name: type.getSymbol()!.name };
    }
    if (f & ts.TypeFlags.NumberLike) return this.number(slot ? this.#flow.numberOf(slot) : Num.Float, use);
    if (f & ts.TypeFlags.BooleanLike) return BOOL;
    if (f & ts.TypeFlags.Void) return VOID;
    if (type.isUnion()) {
      const present = type.types.filter((t) => !(t.flags & (ts.TypeFlags.Undefined | ts.TypeFlags.Null)));
      if (present.every((t) => t.flags & ts.TypeFlags.BooleanLike)) return BOOL;
      // numeric literals (a direction `1 | -1`, a sign, a code) are one number: its kind is the flow's (integers stay
      // int), optional when undefined or null may be in it
      if (present.length > 0 && present.every((t) => t.flags & ts.TypeFlags.NumberLike)) {
        const n = this.number(slot ? this.#flow.numberOf(slot) : Num.Float, use);
        return present.length === type.types.length ? n : { k: 'optional', of: n };
      }
      if (present.length > 0 && present.every((t) => t.flags & ts.TypeFlags.StringLiteral)) {
        const e: CppType = { k: 'enum', name: this.#enumOf(present, type.aliasSymbol, slot?.name ?? 'value').name };
        return present.length === type.types.length ? e : { k: 'optional', of: e };
      }
      if (present.length === type.types.length) return this.#unsupported(type, slot, 'a union of different types');
      const inner = this.ofType(present.length === 1 ? present[0]! : checker.getNonNullableType(type), use, slot);
      if (inner.k === 'ptr' || inner.k === 'bool' || inner.k === 'optional') return inner;
      this.includes.add('optional');
      return { k: 'optional', of: inner };
    }
    if (f & ts.TypeFlags.StringLiteral) return { k: 'enum', name: this.#enumOf([type], type.aliasSymbol, slot?.name ?? 'value').name };
    if (f & ts.TypeFlags.StringLike) {
      this.includes.add('string');
      return STRING;
    }
    const symbol = type.aliasSymbol ?? type.getSymbol();
    const name = symbol?.name ?? '';
    const known = this.#spec.types[name];
    if (known) {
      this.includes.add(known.include);
      return { k: 'struct', name: known.type };
    }
    const mapped = this.#spec.classes[name];
    if (mapped) {
      this.includes.add(mapped.include);
      return { k: 'class', name, text: mapped.type };
    }
    if (/^Float(32|64)Array$/.test(name)) return this.#vector({ k: 'real', name: this.#spec.number });
    const typed = /^(Ui|I)nt(8|16|32)Array$/.exec(name);
    if (typed) return this.#vector({ k: 'int', name: `${typed[1] === 'Ui' ? 'u' : ''}int${typed[2]}_t` as IntName });
    if (/^(Set|ReadonlySet)$/.test(name)) {
      const element = checker.getTypeArguments(type as TS.TypeReference)[0]!;
      return { k: 'set', of: this.ofType(element, 'element', slot ? this.#flow.elemOf(slot) : null) };
    }
    if (checker.isArrayType(type) || /^(ReadonlyArray|ArrayLike|Iterable|IterableIterator|Iterator|MapIterator|SetIterator|ArrayIterator)$/.test(name)) {
      const element = checker.getTypeArguments(type as TS.TypeReference)[0]!;
      return this.#vector(this.ofType(element, 'element', slot ? this.#flow.elemOf(slot) : null));
    }
    // a lookup the source declares itself (`interface NodeHeights { get(node: NodeId): number | undefined }`, a Map
    // passed in): the map it is read as
    const lookup = lookupOf(ts, checker, type);
    if (lookup || /^(Map|ReadonlyMap)$/.test(name)) {
      const [k, v] = lookup ?? checker.getTypeArguments(type as TS.TypeReference);
      this.includes.add('unordered_map');
      const key: CppType = k!.flags & ts.TypeFlags.NumberLike ? { k: 'int', name: 'int64_t' } : this.ofType(k!, 'element', slot ? this.#flow.keyOf(slot) : null);
      return { k: 'map', key, of: this.ofType(v!, 'element', slot ? this.#flow.elemOf(slot) : null) };
    }
    // a dictionary (Record<string, V>, { [key: string]: V }): a map of strings
    const entry = dictionaryOf(ts, checker, type);
    if (entry) {
      this.includes.add('string');
      return { k: 'map', key: STRING, of: this.ofType(checker.getNonNullableType(entry), 'element', slot ? this.#flow.elemOf(slot) : null) };
    }
    // a membership test (a Set passed in, or a rule written in place): the test, called with the key
    const member = membershipOf(ts, checker, type);
    if (member) {
      this.includes.add('functional');
      // a number key as a Map's (an id may pass 2^31: int64_t)
      const key: CppType = member.flags & ts.TypeFlags.NumberLike ? { k: 'int', name: 'int64_t' } : this.ofType(member, 'param', null);
      return { k: 'function', params: [key], ret: BOOL };
    }
    const call = type.getCallSignatures()[0];
    if (call) {
      this.includes.add('functional');
      const params = call.getParameters().map((p) => this.ofType(checker.getTypeOfSymbol(p), 'param', null));
      return { k: 'function', params, ret: this.ofType(call.getReturnType(), 'return', null) };
    }
    if (f & ts.TypeFlags.Object) {
      const shape = (slot ? this.#flow.shapeOf(slot) : null) ?? type;
      const struct = this.structFor(shape, slot?.name ?? 'value');
      if (struct.k !== 'struct' || Object.values(this.#spec.types).some((t) => t.type === struct.name)) return struct;
      // Another owner's object: a pointer (a parameter is a const reference, unless it may be absent).
      const ref = slot !== null && this.#flow.find(slot).own === Own.Ref;
      if (ref && use !== 'param') return { k: 'ptr', to: struct };
      return struct;
    }
    return this.#unsupported(type, slot, 'no C++ form');
  }

  /** The struct of an object type (or the target type it is the same as), declared on first use. */
  structFor(type: TS.Type, hint: string): CppType {
    if (this.#checker.isTupleType(type)) return this.#tuple(type as TS.TypeReference);
    const symbol = type.aliasSymbol ?? type.getSymbol();
    // a type TypeScript's own library declares (Pick, Partial, Record, Omit): no name of the program; named by its
    // shape, like an object literal's type
    const inLib = symbol?.declarations?.some((d) => /[\\/]typescript[\\/]lib[\\/]lib\.[^\\/]*\.d\.ts$/.test(d.getSourceFile().fileName)) ?? false;
    const declared = symbol && !inLib && symbol.name !== '__type' && symbol.name !== '__object' ? symbol.name : null;
    if (!declared) {
      const same = this.#sameAs(type);
      if (same) return same;
    }
    const shape = this.#checker.typeToString(type, undefined, this.#ts.TypeFormatFlags.NoTruncation | this.#ts.TypeFormatFlags.InTypeAlias);
    // a declared name the target already uses for its own type (city::Profile): the module's name in front
    // (ElevationProfile), so both live in one namespace; a name the spec gives (typeNames) wins
    const stem = pascal((symbol?.declarations?.[0]?.getSourceFile().fileName ?? '').replace(/^.*[\\/]/, '').replace(/\.tsx?$/, '').replace(/[^A-Za-z0-9]+/g, '_'));
    const own = declared && this.#reserved.has(declared) && declared !== this.#spec.state.source ? `${stem}${declared}` : declared;
    const name = declared === this.#spec.state.source ? this.#spec.state.target
      : (declared ? this.#spec.typeNames[declared] : undefined) ?? own ?? this.#anonymous.get(shape) ?? this.#spec.typeNames[shape] ?? this.#freshName(type, hint);
    const known = this.structs.get(name);
    if (known) {
      this.#seen(known);
      return { k: 'struct', name };
    }
    if (!declared) this.#anonymous.set(shape, name);
    // a library type has no declaration of the program: its module is where it is used, its line none
    const decl = inLib ? undefined : symbol?.declarations?.[0];
    const made: CppStruct = { name, comment: symbol && declared ? this.#doc(symbol) : null, fields: [], module: this.#moduleOf(decl),
      line: decl ? decl.getSourceFile().getLineAndCharacterOfPosition(decl.getStart()).line + 1 : 0, declared };
    this.#seen(made);
    // Registered before its fields: a type that holds itself stops here. The files order structs by what they hold.
    this.structs.set(name, made);
    for (const prop of this.#checker.getPropertiesOfType(type)) {
      const propType = this.#checker.getTypeOfSymbol(prop);
      if (propType.getCallSignatures().length > 0 && !(prop.flags & this.#ts.SymbolFlags.Property)) continue;
      let cpp = this.ofType(propType, 'field', this.#flow.symbolSlot(prop));
      if ((prop.flags & this.#ts.SymbolFlags.Optional) !== 0 && cpp.k !== 'bool' && cpp.k !== 'optional' && cpp.k !== 'ptr') cpp = { k: 'optional', of: cpp };
      if (cpp.k === 'optional') this.includes.add('optional');
      made.fields.push({ source: prop.name, name: snake(prop.name), type: cpp, init: this.#init(cpp), comment: this.#doc(prop) });
    }
    return { k: 'struct', name };
  }

  /**
   * A tuple (`[t: number, y: number]`): a struct of its positions, read by position. One struct for every tuple of the
   * same element types, labelled or not (the program passes one where the other is asked), named from them
   * (`NumberPair`); its fields `first`, `second`, ... (`v<i>` past four). A field's source is its position.
   */
  #tuple(type: TS.TypeReference): CppType {
    const elements = this.#checker.getTypeArguments(type);
    const word = (t: TS.Type): string => pascal(this.#checker.typeToString(this.#checker.getBaseTypeOfLiteralType(t)).replace(/[^A-Za-z0-9]+/g, ' ').trim().replace(/\s+/g, '_'));
    const words = elements.map(word);
    const arity = ['', 'Single', 'Pair', 'Triple', 'Quad'][elements.length] ?? `Tuple${elements.length}`;
    const name = this.#spec.typeNames[this.#checker.typeToString(type)] ?? `${words.every((w) => w === words[0]) ? words[0] : words.join('')}${arity}`;
    const known = this.structs.get(name);
    if (known) {
      this.#seen(known);
      return { k: 'struct', name };
    }
    const made: CppStruct = { name, comment: `A tuple of the source (${this.#checker.typeToString(type)}), read by position.`, fields: [], module: null, line: 0 };
    this.#seen(made);
    this.structs.set(name, made);
    const FIELDS = ['first', 'second', 'third', 'fourth'];
    elements.forEach((el, i) => {
      const prop = this.#checker.getPropertyOfType(type, String(i));
      const cpp = this.ofType(el, 'field', prop ? this.#flow.symbolSlot(prop) : null);
      made.fields.push({ source: String(i), name: FIELDS[i] ?? `v${i}`, type: cpp, init: this.#init(cpp), comment: null });
    });
    return { k: 'struct', name };
  }

  /**
   * The name of an anonymous shape, from what holds it. Two shapes under one hint (every function has a `return`) are
   * two structs: the second is named by its fields too, the same way for the same fields whatever the order they came.
   */
  #freshName(type: TS.Type, hint: string): string {
    const word = pascal(hint.replace(/[^A-Za-z0-9]+/g, ' ').trim().replace(/\s+/g, '_')).replace(/_/g, '') || 'Value';
    const fields = this.#checker.getPropertiesOfType(type).map((p) => p.name).sort();
    const known = this.structs.get(word);
    if (!known || known.fields.map((f) => f.source).sort().join() === fields.join()) return word;
    const named = `${word}${fields.map((f) => pascal(f.replace(/[^A-Za-z0-9]+/g, ''))).join('')}`;
    return named.length <= MAX_NAME ? named : `${word}${fields.slice(0, FIELDS_IN_NAME).map((f) => pascal(f.replace(/[^A-Za-z0-9]+/g, ''))).join('')}${fields.length}`;
  }

  /** The enum holding a string-literal union: its alias, or the declared enum holding all its values. */
  #enumOf(types: readonly TS.Type[], alias: TS.Symbol | undefined, hint: string): CppEnum {
    const values = types.map((t) => (t as TS.StringLiteralType).value);
    if (alias) {
      const existing = this.enums.get(alias.name);
      if (existing) return this.#seen(existing);
      const made = { name: alias.name, module: this.#moduleOf(alias.declarations?.[0]), comment: this.#doc(alias), values };
      this.enums.set(alias.name, made);
      return this.#seen(made);
    }
    for (const e of this.enums.values()) if (values.every((v) => e.values.includes(v))) return this.#seen(e);
    const made = { name: pascal(hint), module: null, comment: null, values };
    this.enums.set(made.name, made);
    return made;
  }

  enumValue(e: CppEnum, value: string): string {
    return `${e.name}::${upper(value.replace(/[^A-Za-z0-9]+/g, '_'))}`;
  }

  /** The target type an anonymous shape is (the same field names as a mapped or declared type), or null. */
  #sameAs(type: TS.Type): CppType | null {
    const names = this.#checker.getPropertiesOfType(type).map((p) => p.name).sort().join(',');
    for (const known of Object.values(this.#spec.types)) {
      if (known.fields && Object.keys(known.fields).sort().join(',') === names) {
        this.includes.add(known.include);
        return { k: 'struct', name: known.type };
      }
    }
    for (const s of this.structs.values()) {
      if (s.line > 0 && !this.indexStructs.has(s.name) && s.fields.map((f) => f.source).sort().join(',') === names) return { k: 'struct', name: s.name };
    }
    return null;
  }

  #init(t: CppType): string {
    if (t.k === 'int' || t.k === 'real') return ' = 0';
    if (t.k === 'bool') return ' = false';
    if (t.k === 'ptr') return ' = nullptr';
    if (t.k === 'enum') return '{}';
    return '';
  }

  #vector(of: CppType): CppType {
    this.includes.add('vector');
    return { k: 'vector', of };
  }

  #doc(symbol: TS.Symbol): string | null {
    const text = this.#ts.displayPartsToString(symbol.getDocumentationComment(this.#checker)).trim();
    return text.length > 0 ? text.replace(/\s+/g, ' ') : null;
  }

  #unsupported(type: TS.Type, slot: Slot | null, why: string): CppType {
    this.#errors.push(`type of ${slot?.name ?? 'a value'} (${this.#checker.typeToString(type)}): ${why}${this.unit ? ` [unit ${this.unit}]` : ''}`);
    return VOID;
  }
}
