// Facts of one C++ unit from its tokens, the same facts the TypeScript side records: loops (with each body's extent,
// for the depth), decisions, allocations inside loops (heap allocations, containers built each pass, render instances
// created per item), methodology patterns, numeric literals with their context, nondeterminism and fingerprints.
import { tokenHash, winnow } from '../graph/fingerprint.ts';
import type { AllocationSite, NumericLiteral, PatternSite } from '../graph/ir.ts';
import type { CppToken } from './lexer.ts';

export interface CppFacts {
  readonly loopAllocations: AllocationSite[];
  readonly numericLiterals: NumericLiteral[];
  readonly stringLiterals: number;
  readonly decisions: number;
  readonly loops: number;
  readonly bitOperations: number;
  readonly nondeterminism: string[];
  readonly patterns: PatternSite[];
  readonly fingerprints: [number, number][];
}

/** Standard containers whose local construction inside a loop allocates every pass. */
const CONTAINERS = new Set(['vector', 'string', 'unordered_map', 'map', 'set', 'unordered_set', 'deque', 'list', 'function']);
const DISTANCE = new Set(['hypot', 'sqrt', 'distance', 'distance_to', 'distance_squared_to', 'length', 'length_squared']);
/** RenderingServer calls that make a drawable instance: one per item of a loop is one draw per item. */
const INSTANCE_CREATE = new Set(['instance_create', 'instance_create2']);
const NONDETERMINISTIC = new Map([['rand', 'std:rand'], ['srand', 'std:srand'], ['random_device', 'std:random_device'], ['system_clock', 'std:system_clock']]);
const BIT_OPERATORS = new Set(['|', '^', '~', '|=', '&=', '^=', '<<=', '>>=']);
const CONSTANT_NAME = /^[A-Z][A-Z0-9_]*$/;
const STATEMENT_START = new Set([';', '{', '}']);

/** Token index ranges [open, close] of every loop body (braced or a single statement). */
function loopBodies(tokens: readonly CppToken[], match: Map<number, number>): [number, number][] {
  const bodies: [number, number][] = [];
  for (let i = 0; i < tokens.length; i++) {
    const t = tokens[i]!.text;
    let after = -1;
    if ((t === 'for' || t === 'while') && tokens[i + 1]?.text === '(') {
      // `} while (...)` closing a do-loop is not a loop of its own
      if (t === 'while' && tokens[i - 1]?.text === '}' && tokens[(match.get(i - 1) ?? 0) - 1]?.text === 'do') continue;
      after = (match.get(i + 1) ?? i + 1) + 1;
    } else if (t === 'do') {
      after = i + 1;
    }
    if (after < 0 || after >= tokens.length) continue;
    if (tokens[after]!.text === '{') bodies.push([after, match.get(after) ?? tokens.length - 1]);
    else {
      let end = after;
      while (end < tokens.length && tokens[end]!.text !== ';') end++;
      bodies.push([after, end]);
    }
  }
  return bodies;
}

function brackets(tokens: readonly CppToken[]): Map<number, number> {
  const match = new Map<number, number>();
  const stack: number[] = [];
  const pairs: Record<string, string> = { ')': '(', ']': '[', '}': '{' };
  tokens.forEach((t, i) => {
    if (t.text === '(' || t.text === '[' || t.text === '{') stack.push(i);
    else if (pairs[t.text]) {
      for (let k = stack.length - 1; k >= 0; k--) {
        if (tokens[stack[k]!]!.text === pairs[t.text]) {
          const open = stack.splice(k)[0]!;
          match.set(open, i);
          match.set(i, open);
          break;
        }
      }
    }
  });
  return match;
}

function literalContext(tokens: readonly CppToken[], k: number, inIndex: boolean): NumericLiteral['context'] {
  if (inIndex) return 'index';
  let s = k - 1;
  while (s >= 0 && !STATEMENT_START.has(tokens[s]!.text)) s--;
  const statement = tokens.slice(s + 1, k);
  const named = statement.some((t, j) => CONSTANT_NAME.test(t.text) && t.kind === 'id' && ['=', '{', '['].includes(statement[j + 1]?.text ?? ''));
  return named && statement.some((t) => t.text === 'constexpr' || t.text === 'const') ? 'constant' : 'value';
}

export function cppFacts(tokens: readonly CppToken[]): CppFacts {
  const match = brackets(tokens);
  const bodies = loopBodies(tokens, match);
  const depth = (i: number): number => bodies.reduce((n, [a, b]) => n + (i > a && i < b ? 1 : 0), 0);
  const facts = {
    loopAllocations: [] as AllocationSite[], numericLiterals: [] as NumericLiteral[], stringLiterals: 0, decisions: 0,
    loops: bodies.length, bitOperations: 0, nondeterminism: [] as string[], patterns: [] as PatternSite[],
  };
  const indexDepth: number[] = [];
  let inIndex = 0;
  for (let i = 0; i < tokens.length; i++) {
    const t = tokens[i]!;
    const next = tokens[i + 1]?.text;
    const prev = tokens[i - 1]?.text;
    const line = t.line + 1;
    if (t.text === '[') indexDepth.push(i);
    if (t.text === ']') indexDepth.pop();
    inIndex = indexDepth.length;
    if (['if', 'case', 'catch', '&&', '||', '?'].includes(t.text) && t.kind !== 'string') facts.decisions++;
    if (BIT_OPERATORS.has(t.text)) facts.bitOperations++;
    if (t.kind === 'string') facts.stringLiterals++;
    const nd = t.kind === 'id' ? NONDETERMINISTIC.get(t.text) : undefined;
    if (nd && (next === '(' || t.text === 'random_device' || t.text === 'system_clock')) facts.nondeterminism.push(nd);
    if (t.kind === 'number') {
      const negative = prev === '-' && ['=', '(', ',', '{', 'return', '?', ':', '['].includes(tokens[i - 2]?.text ?? '');
      facts.numericLiterals.push({ text: (negative ? '-' : '') + t.text, line, context: literalContext(tokens, i, inIndex > 0) });
    }
    const inLoop = depth(i) > 0;
    if (inLoop && t.kind === 'id') {
      if (t.text === 'new') facts.loopAllocations.push({ line, what: `new ${next ?? ''}` });
      if ((t.text === 'make_unique' || t.text === 'make_shared') && (next === '<' || next === '(')) facts.loopAllocations.push({ line, what: `std::${t.text}` });
      if (CONTAINERS.has(t.text) && prev === '::' && tokens[i - 2]?.text === 'std') {
        // a local `std::vector<...> name ...` declared in the loop body
        let j = i + 1;
        if (tokens[j]?.text === '<') j = (closingAngle(tokens, j) ?? j) + 1;
        if (tokens[j]?.kind === 'id' && ['=', ';', '(', '{'].includes(tokens[j + 1]?.text ?? '')) facts.loopAllocations.push({ line, what: `std::${t.text} local` });
      }
      if (INSTANCE_CREATE.has(t.text) && next === '(') facts.loopAllocations.push({ line, what: 'RenderingServer::instance_create' });
      // `queue.erase(queue.begin())`: every remaining element moves down one place
      if (t.text === 'erase' && next === '(' && tokens[i + 3]?.text === '.' && tokens[i + 4]?.text === 'begin') {
        facts.patterns.push({ id: 'queue-shift', line, detail: 'erase(begin()) inside a loop' });
      }
    }
    if (t.kind === 'id' && DISTANCE.has(t.text) && next === '(' && depth(i) > 1) {
      if (!facts.patterns.some((p) => p.id === 'quadratic-proximity' && p.line === line)) {
        facts.patterns.push({ id: 'quadratic-proximity', line, detail: 'distance computed in a nested loop' });
      }
    }
  }
  return { ...facts, fingerprints: winnow(tokens.map((t) => tokenHash(t.text)), tokens.map((t) => t.line + 1)) };
}

/** The `>` closing the template argument list opened at `open` (nested `<...>` counted). */
function closingAngle(tokens: readonly CppToken[], open: number): number | null {
  let level = 0;
  for (let j = open; j < tokens.length; j++) {
    const text = tokens[j]!.text;
    if (text === '<') level++;
    else if (text === '>') level--;
    else if (text === '>>') level -= 2;
    else if (text === ';' || text === '{') return null;
    if (level <= 0) return j;
  }
  return null;
}
