// Classifies one use of a C++ name from its semantic token and the tokens around it: called, constructed, a field or
// global read or written (assigned, incremented, passed by mutable reference, mutated through a container method),
// used as a type, or referenced.
import type { EdgeKind } from '../graph/ir.ts';
import type { CppToken } from './lexer.ts';
import type { SemanticToken } from './session.ts';

const ASSIGN = new Set(['=', '+=', '-=', '*=', '/=', '%=', '&=', '|=', '^=', '<<=', '>>=']);
const MUTATORS = new Set(['push_back', 'emplace_back', 'emplace', 'insert', 'erase', 'clear', 'resize', 'pop_back', 'swap', 'assign', 'reserve', 'fill']);
const TYPES = new Set(['class', 'struct', 'interface', 'type', 'enum', 'concept']);
const STATE = new Set(['property', 'variable']);

/** Token types whose names lead to a declaration worth following (locals, parameters and namespaces do not). */
export function followed(token: SemanticToken): boolean {
  if (token.modifiers.includes('declaration')) return false;
  if (token.type === 'variable') return token.modifiers.includes('globalScope') || token.modifiers.includes('fileScope');
  return ['function', 'method', 'property', 'enumMember', ...TYPES].includes(token.type);
}

/** The index past a template argument list starting at `open` (`<`), or `open` when there is none. */
function pastTemplate(tokens: readonly CppToken[], open: number): number {
  if (tokens[open]?.text !== '<') return open;
  let level = 0;
  for (let j = open; j < tokens.length; j++) {
    const text = tokens[j]!.text;
    if (text === '<') level++;
    else if (text === '>') level--;
    else if (text === '>>') level -= 2;
    else if (text === ';' || text === '{') return open;
    if (level <= 0) return j + 1;
  }
  return open;
}

export function edgeKinds(token: SemanticToken, tokens: readonly CppToken[], index: number): EdgeKind[] {
  const after = pastTemplate(tokens, index + 1);
  const next = tokens[after]?.text;
  const prev = tokens[index - 1]?.text;
  if (token.type === 'function' || token.type === 'method') return [next === '(' ? 'call' : 'reference'];
  if (TYPES.has(token.type)) return prev === 'new' || next === '(' || next === '{' ? ['construct'] : ['typeRef'];
  if (!STATE.has(token.type)) return ['reference'];
  if (next !== undefined && ASSIGN.has(next)) return next === '=' ? ['write'] : ['read', 'write'];
  if (next === '++' || next === '--' || prev === '++' || prev === '--') return ['read', 'write'];
  if (token.modifiers.includes('usedAsMutableReference') || token.modifiers.includes('usedAsMutablePointer')) return ['read', 'write'];
  if ((next === '.' || next === '->') && MUTATORS.has(tokens[after + 1]?.text ?? '') && tokens[after + 2]?.text === '(') return ['read', 'write'];
  if (next === '[') {
    let depth = 0;
    for (let j = after; j < tokens.length; j++) {
      if (tokens[j]!.text === '[') depth++;
      else if (tokens[j]!.text === ']' && --depth === 0) return ASSIGN.has(tokens[j + 1]?.text ?? '') ? ['read', 'write'] : ['read'];
    }
  }
  return ['read'];
}
