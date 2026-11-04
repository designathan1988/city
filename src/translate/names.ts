// The target's naming: snake_case for functions, fields and variables, PascalCase for types, UPPER_CASE for constants
// and enum values (the game's headers), and words of C++ that a source name may not take.

const CPP_RESERVED = new Set(['auto', 'class', 'default', 'delete', 'new', 'operator', 'register', 'template', 'this', 'union',
  'unsigned', 'signed', 'short', 'long', 'int', 'float', 'double', 'char', 'bool', 'struct', 'enum', 'namespace', 'friend',
  'private', 'protected', 'public', 'virtual', 'inline', 'static', 'extern', 'typename', 'mutable', 'explicit', 'and', 'or',
  'not', 'xor', 'goto', 'asm', 'export', 'concept', 'requires']);

export function snake(name: string): string {
  const out = name.replace(/([a-z0-9])([A-Z])/g, '$1_$2').replace(/([A-Z]+)([A-Z][a-z])/g, '$1_$2').toLowerCase();
  return CPP_RESERVED.has(out) ? `${out}_` : out;
}

export function upper(name: string): string {
  return /^[A-Z0-9_]+$/.test(name) ? name : snake(name).toUpperCase();
}

export function pascal(name: string): string {
  return name.replace(/(^|[_\s-])([a-z])/g, (_, __, c: string) => c.toUpperCase()).replace(/^[a-z]/, (c) => c.toUpperCase());
}
