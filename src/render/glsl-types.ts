// The GLSL type of an expression, as far as the Godot translation needs it: Godot's shading language takes no
// implicit vector–scalar broadcast for the integer operators (`uvec4 & uint`, `ivec2 % int`), which GLSL ES 3.0
// does, so the translation must know which side is the vector. Types come from the declarations in scope (globals,
// parameters, locals), constructors, literals, swizzles, indexing, the built-in functions' rules of the GLSL ES 3.00
// specification (section 8) and the user functions' return types. Unknown stays unknown (null): nothing is guessed.

/* eslint-disable @typescript-eslint/no-explicit-any */
type Node = { type: string; [key: string]: any };

export interface TypeEnv {
  /** Names in scope and their types (`vec3`, `float[4]`, a struct name). */
  readonly names: ReadonlyMap<string, string>;
  /** User functions and their return types. */
  readonly functions: ReadonlyMap<string, string>;
  /** Struct types and their fields' types. */
  readonly structs: ReadonlyMap<string, ReadonlyMap<string, string>>;
  /** Each reference node's type, from the parser's scopes (a name declared in two scopes of one function has two). */
  readonly refs?: WeakMap<object, string>;
}

const VECTOR = /^([biu]?)vec([234])$/;
const MATRIX = /^mat([234])$/;
const scalarOf = (prefix: string): string => (prefix === 'b' ? 'bool' : prefix === 'i' ? 'int' : prefix === 'u' ? 'uint' : 'float');
const vectorOf = (scalar: string, n: number): string => (n === 1 ? scalar : `${scalar === 'bool' ? 'b' : scalar === 'int' ? 'i' : scalar === 'uint' ? 'u' : ''}vec${n}`);
export const isVector = (t: string | null): boolean => !!t && VECTOR.test(t);
export const isScalar = (t: string | null): boolean => t === 'float' || t === 'int' || t === 'uint' || t === 'bool';
const componentsOf = (t: string): { scalar: string; n: number } | null => {
  const v = VECTOR.exec(t);
  if (v) return { scalar: scalarOf(v[1]!), n: Number(v[2]) };
  return isScalar(t) ? { scalar: t, n: 1 } : null;
};

/** Built-ins whose result has the type of their first (or, for step and smoothstep, last) argument. */
const SAME_AS_FIRST = new Set(['abs', 'sign', 'floor', 'ceil', 'fract', 'mod', 'min', 'max', 'clamp', 'mix', 'sqrt',
  'inversesqrt', 'pow', 'exp', 'exp2', 'log', 'log2', 'sin', 'cos', 'tan', 'asin', 'acos', 'atan', 'sinh', 'cosh', 'tanh',
  'normalize', 'reflect', 'refract', 'radians', 'degrees', 'round', 'roundEven', 'trunc', 'dFdx', 'dFdy', 'fwidth',
  'faceforward', 'transpose', 'inverse', 'bitfieldExtract', 'bitfieldInsert', 'bitfieldReverse']);
const SAME_AS_LAST = new Set(['step', 'smoothstep']);
const FLOAT_RESULT = new Set(['length', 'distance', 'dot', 'determinant']);

export function typeOf(n: Node | undefined, env: TypeEnv): string | null {
  if (!n) return null;
  switch (n.type) {
    case 'int_constant': return /[uU]$/.test(n.token) ? 'uint' : 'int';
    case 'uint_constant': return 'uint';
    case 'float_constant': return 'float';
    case 'bool_constant': return 'bool';
    case 'identifier': return (n.gtype as string | undefined) ?? env.refs?.get(n) ?? env.names.get(n.identifier) ?? null;
    case 'group': return typeOf(n.expression, env);
    case 'unary': return n.operator?.literal === '!' ? 'bool' : typeOf(n.expression, env);
    case 'ternary': return typeOf(n.left, env) ?? typeOf(n.right, env);
    case 'assignment': return typeOf(n.left, env);
    case 'postfix': return postfixType(n, env);
    case 'function_call': return callType(n, env);
    case 'binary': return binaryType(n, env);
    default: return null;
  }
}

function postfixType(n: Node, env: TypeEnv): string | null {
  let t = typeOf(n.expression, env);
  const ops: Node[] = [];
  const flat = (x: Node): void => { if (x.type === 'postfix') { flat(x.expression); flat(x.postfix); } else ops.push(x); };
  flat(n.postfix);
  for (const op of ops) {
    if (!t) return null;
    if (op.type === 'quantifier') {
      const arr = /^(.*)\[[^\]]*\]$/.exec(t);
      if (arr) { t = arr[1]!; continue; }
      const m = MATRIX.exec(t);
      if (m) { t = `vec${m[1]}`; continue; }
      const c = componentsOf(t);
      t = c ? c.scalar : null;
      continue;
    }
    if (op.type === 'field_selection') {
      const field = (op.selection?.identifier ?? op.selection?.literal) as string;
      const struct = env.structs.get(t);
      if (struct) { t = struct.get(field) ?? null; continue; }
      if (field === 'length') return 'int';
      const c = componentsOf(t);
      t = c && /^[xyzwrgbastpq]+$/.test(field) ? vectorOf(c.scalar, field.length) : null;
      continue;
    }
    if (op.type !== 'literal') return null; // ++, --
  }
  return t;
}

function callType(n: Node, env: TypeEnv): string | null {
  const id = n.identifier as Node;
  const args = ((n.args ?? []) as Node[]).filter((a) => !(a.type === 'literal' && a.literal === ','));
  if (id.type === 'type_specifier') {
    const s = id.specifier as Node;
    const quant = id.quantifier ? '[]' : '';
    if (s.type === 'keyword') return `${s.token}${quant}`;
    const name = s.identifier as string;
    return env.functions.get(name) ?? (env.structs.has(name) ? name : null);
  }
  if (id.type === 'postfix') return null; // .length() and the like
  const name = id.identifier as string;
  if (env.functions.has(name)) return env.functions.get(name)!;
  if (FLOAT_RESULT.has(name)) return 'float';
  if (name === 'cross') return 'vec3';
  if (SAME_AS_FIRST.has(name)) return typeOf(args[0], env);
  if (SAME_AS_LAST.has(name)) return typeOf(args[args.length - 1], env);
  if (/^texture|^texelFetch/.test(name)) {
    if (name === 'textureSize') return 'ivec2';
    const sampler = typeOf(args[0], env) ?? '';
    return sampler.startsWith('usampler') ? 'uvec4' : sampler.startsWith('isampler') ? 'ivec4' : 'vec4';
  }
  if (/^(lessThan|lessThanEqual|greaterThan|greaterThanEqual|equal|notEqual)$/.test(name)) {
    const c = componentsOf(typeOf(args[0], env) ?? '');
    return c ? vectorOf('bool', c.n) : null;
  }
  if (name === 'any' || name === 'all') return 'bool';
  if (name === 'floatBitsToUint' || name === 'floatBitsToInt') {
    const c = componentsOf(typeOf(args[0], env) ?? '');
    return c ? vectorOf(name === 'floatBitsToUint' ? 'uint' : 'int', c.n) : null;
  }
  if (name === 'uintBitsToFloat' || name === 'intBitsToFloat') {
    const c = componentsOf(typeOf(args[0], env) ?? '');
    return c ? vectorOf('float', c.n) : null;
  }
  if (/^pack/.test(name)) return 'uint';
  if (name === 'unpackHalf2x16' || name === 'unpackSnorm2x16' || name === 'unpackUnorm2x16') return 'vec2';
  if (name === 'unpackSnorm4x8' || name === 'unpackUnorm4x8') return 'vec4';
  return null;
}

function binaryType(n: Node, env: TypeEnv): string | null {
  const op = n.operator?.literal as string;
  if (['==', '!=', '<', '>', '<=', '>=', '&&', '||', '^^'].includes(op)) return 'bool';
  const l = typeOf(n.left, env);
  const r = typeOf(n.right, env);
  if (!l || !r) return l ?? r;
  if (op === '*') {
    const lm = MATRIX.exec(l);
    const rm = MATRIX.exec(r);
    if (lm && isVector(r)) return r;
    if (rm && isVector(l)) return l;
    if (lm && rm) return l;
  }
  if (isScalar(l) && !isScalar(r)) return r;
  return l;
}

/** The declarations of a function (its parameters and every local), as name → type, over the globals given. */
export function functionScope(fn: Node, generate: (n: Node) => string, globals: ReadonlyMap<string, string>): Map<string, string> {
  const names = new Map(globals);
  const quant = (q: Node[] | null | undefined): string => (q && q.length > 0 ? q.map((x) => generate(x)).join('').replace(/\s+/g, '') : '');
  for (const p of (fn.prototype?.parameters ?? []) as Node[]) {
    if (!p.identifier) continue;
    names.set(p.identifier.identifier, `${generate(p.specifier).trim()}${quant(p.quantifier)}`);
  }
  const walk = (x: unknown): void => {
    if (!x || typeof x !== 'object') return;
    if (Array.isArray(x)) { for (const y of x) walk(y); return; }
    const node = x as Node;
    if (node.type === 'declarator_list') {
      const spec = node.specified_type?.specifier as Node | undefined;
      const base = spec?.specifier?.type === 'struct' ? spec.specifier.typeName?.identifier : spec ? generate(spec).trim() : null;
      if (base) for (const d of node.declarations as Node[]) names.set(d.identifier.identifier, `${base}${quant(d.quantifier)}`);
    }
    for (const [k, v] of Object.entries(node)) if (k !== 'scopes' && typeof v === 'object') walk(v);
  };
  walk(fn.body);
  return names;
}

/** Every reference's type, by the parser's scopes: the binding's declaration (a declarator or a parameter) gives it. */
export function referenceTypes(ast: Node, generate: (n: Node) => string): WeakMap<object, string> {
  const declType = new Map<object, string>();
  const quant = (q: Node[] | null | undefined): string => (q && q.length > 0 ? q.map((x) => generate(x)).join('').replace(/\s+/g, '') : '');
  const walk = (x: unknown): void => {
    if (!x || typeof x !== 'object') return;
    if (Array.isArray(x)) { for (const y of x) walk(y); return; }
    const node = x as Node;
    if (node.type === 'declarator_list') {
      const spec = node.specified_type?.specifier as Node | undefined;
      const base = spec?.specifier?.type === 'struct' ? spec.specifier.typeName?.identifier : spec ? generate(spec).trim() : null;
      if (base) for (const d of node.declarations as Node[]) { declType.set(d, `${base}${quant(d.quantifier)}`); declType.set(d.identifier, `${base}${quant(d.quantifier)}`); }
    }
    if (node.type === 'parameter_declaration' && node.identifier) {
      const ty = `${generate(node.specifier).trim()}${quant(node.quantifier)}`;
      declType.set(node, ty); declType.set(node.identifier, ty);
    }
    for (const [k, v] of Object.entries(node)) if (k !== 'scopes' && typeof v === 'object') walk(v);
  };
  walk(ast.program);
  const refs = new WeakMap<object, string>();
  for (const scope of (ast.scopes ?? []) as Node[]) {
    for (const binding of Object.values(scope.bindings ?? {}) as Node[]) {
      const ty = binding.declaration ? declType.get(binding.declaration) : undefined;
      if (!ty) continue;
      for (const r of (binding.references ?? []) as Node[]) refs.set(r, ty);
    }
  }
  return refs;
}
