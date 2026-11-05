// C++ types as data, rendered to text only when written. The translator asks a type what it is (a pointer, an
// optional, a container and what it holds) instead of reading it back out of text.

export type IntName = 'int' | 'int8_t' | 'int16_t' | 'int32_t' | 'int64_t' | 'uint8_t' | 'uint16_t' | 'uint32_t';

export type CppType =
  | { readonly k: 'int'; readonly name: IntName }
  | { readonly k: 'real'; readonly name: string }
  | { readonly k: 'bool' }
  | { readonly k: 'string' }
  | { readonly k: 'void' }
  | { readonly k: 'enum'; readonly name: string }
  | { readonly k: 'struct'; readonly name: string }
  /** A function's type parameter: the function is a template. */
  | { readonly k: 'tparam'; readonly name: string }
  | { readonly k: 'vector'; readonly of: CppType }
  | { readonly k: 'deque'; readonly of: CppType }
  | { readonly k: 'list'; readonly of: CppType }
  | { readonly k: 'map'; readonly key: CppType; readonly of: CppType }
  | { readonly k: 'set'; readonly of: CppType }
  | { readonly k: 'optional'; readonly of: CppType }
  | { readonly k: 'ptr'; readonly to: CppType }
  | { readonly k: 'function'; readonly params: readonly CppType[]; readonly ret: CppType }
  | { readonly k: 'class'; readonly name: string; readonly text: string }
  | { readonly k: 'lambda'; readonly ret: CppType }
  /** A string literal before it lands (a const char *, or a std::string, or an enum value). */
  | { readonly k: 'literal' }
  /** null or undefined before it lands (nullptr or std::nullopt). */
  | { readonly k: 'null' };

export const INT: CppType = { k: 'int', name: 'int' };
export const INT64: CppType = { k: 'int', name: 'int64_t' };
export const BOOL: CppType = { k: 'bool' };
export const STRING: CppType = { k: 'string' };
export const VOID: CppType = { k: 'void' };
export const NULL: CppType = { k: 'null' };
export const LITERAL: CppType = { k: 'literal' };

/**
 * The integer a number literal is, as written (`1_000`, `0x27d4eb2d`, `0b101`, `0o17`): 'int', 'int64' past int32,
 * or null for a real. The one place the flow and the emitter decide it.
 */
export function integerLiteral(written: string): 'int' | 'int64' | null {
  const text = written.replace(/_/g, '');
  if (!/^(\d+|0[xX][0-9a-fA-F]+|0[bB][01]+|0[oO][0-7]+)$/.test(text)) return null;
  return Number(text) > 2147483647 ? 'int64' : 'int';
}

export function render(t: CppType): string {
  switch (t.k) {
    case 'int':
    case 'real':
    case 'enum':
    case 'struct':
    case 'tparam':
      return t.name;
    case 'class':
      return t.text;
    case 'bool':
      return 'bool';
    case 'string':
    case 'literal':
      return 'std::string';
    case 'void':
      return 'void';
    case 'vector':
      return `std::vector<${render(t.of)}>`;
    case 'deque':
      return `std::deque<${render(t.of)}>`;
    case 'list':
      return `std::initializer_list<${render(t.of)}>`;
    case 'set':
      return `js::Set<${render(t.of)}>`;
    case 'map':
      return `js::Map<${render(t.key)}, ${render(t.of)}>`;
    case 'optional':
      return `std::optional<${render(t.of)}>`;
    case 'ptr':
      return `const ${render(t.to)} *`;
    case 'function':
      return `std::function<${render(t.ret)}(${t.params.map((p) => paramType(p)).join(', ')})>`;
    case 'lambda':
      return 'auto';
    case 'null':
      return 'std::nullptr_t';
  }
}

/** Whether a type holds a function's type parameter (it is known only in that function's template). */
export function holdsTypeParameter(t: CppType): boolean {
  switch (t.k) {
    case 'tparam': return true;
    case 'vector': case 'deque': case 'list': case 'optional': case 'set': return holdsTypeParameter(t.of);
    case 'ptr': return holdsTypeParameter(t.to);
    case 'map': return holdsTypeParameter(t.key) || holdsTypeParameter(t.of);
    case 'function': return holdsTypeParameter(t.ret) || t.params.some(holdsTypeParameter);
    default: return false;
  }
}

/** What a template's type parameters are, read from a type that holds them against the type at a call (first found wins). */
export function bindTypeParameters(pattern: CppType, concrete: CppType, out: Map<string, CppType>): void {
  if (pattern.k === 'tparam') {
    if (!out.has(pattern.name) && concrete.k !== 'void' && concrete.k !== 'null' && concrete.k !== 'literal') out.set(pattern.name, concrete);
    return;
  }
  if ((pattern.k === 'vector' || pattern.k === 'deque' || pattern.k === 'list' || pattern.k === 'optional' || pattern.k === 'set') && concrete.k === pattern.k) bindTypeParameters(pattern.of, concrete.of, out);
  else if (pattern.k === 'optional') bindTypeParameters(pattern.of, concrete, out);
  else if (pattern.k === 'ptr' && concrete.k === 'ptr') bindTypeParameters(pattern.to, concrete.to, out);
  else if (pattern.k === 'map' && concrete.k === 'map') {
    bindTypeParameters(pattern.key, concrete.key, out);
    bindTypeParameters(pattern.of, concrete.of, out);
  }
}

/** A type with its type parameters replaced by what they are at a call. */
export function substitute(t: CppType, types: ReadonlyMap<string, CppType>): CppType {
  switch (t.k) {
    case 'tparam': return types.get(t.name) ?? t;
    case 'vector': case 'deque': case 'list': case 'optional': case 'set': return { ...t, of: substitute(t.of, types) };
    case 'ptr': return { k: 'ptr', to: substitute(t.to, types) };
    case 'map': return { k: 'map', key: substitute(t.key, types), of: substitute(t.of, types) };
    case 'function': return { k: 'function', params: t.params.map((p) => substitute(p, types)), ret: substitute(t.ret, types) };
    default: return t;
  }
}

/** Scalars, enums and pointers travel by value; everything else by const reference. */
export function byValue(t: CppType): boolean {
  return t.k === 'int' || t.k === 'real' || t.k === 'bool' || t.k === 'enum' || t.k === 'ptr';
}

export function paramType(t: CppType): string {
  return byValue(t) ? render(t) : `const ${render(t)} &`;
}

/** `T name`, with the pointer's star next to the name as the target's code writes it. */
export function declare(t: CppType, name: string): string {
  const text = render(t);
  return text.endsWith('*') ? `${text}${name}` : `${text} ${name}`;
}

export function paramDecl(t: CppType, name: string): string {
  return byValue(t) ? declare(t, name) : `const ${render(t)} &${name}`;
}

export const isInt = (t: CppType): boolean => t.k === 'int';
export const isNumber = (t: CppType): boolean => t.k === 'int' || t.k === 'real';
export const isPointer = (t: CppType): boolean => t.k === 'ptr';
export const isOptional = (t: CppType): boolean => t.k === 'optional';

/** The value under a pointer or an optional. */
export function target(t: CppType): CppType {
  return t.k === 'ptr' ? t.to : t.k === 'optional' ? t.of : t;
}

/** What a container holds (a map: its values); null for anything else. */
export function elementOf(t: CppType): CppType | null {
  const c = target(t);
  return c.k === 'vector' || c.k === 'deque' || c.k === 'list' || c.k === 'map' || c.k === 'set' ? c.of : null;
}

export function same(a: CppType, b: CppType): boolean {
  return render(a) === render(b) && a.k === b.k;
}

/** A real literal in the target's number type: `3.f`, `0.5f` for float; `3.0`, `0.5` for double. */
export function realLiteral(text: string, real: string): string {
  const t = text.replace(/^\./, '0.');
  const point = /[.eE]/.test(t) ? t : `${t}.${real === 'float' ? '' : '0'}`;
  return real === 'float' ? `${point}f` : point;
}

/** The wider of two numbers (int < int64 < real), for arithmetic and std::max. */
export function widerNumber(a: CppType, b: CppType): CppType {
  if (a.k === 'real') return a;
  if (b.k === 'real') return b;
  if (a.k === 'int' && b.k === 'int') return a.name === 'int64_t' || b.name === 'int64_t' ? INT64 : a;
  return a.k === 'int' ? b : a;
}
