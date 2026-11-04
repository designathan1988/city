// The strings a piece of code can produce: a literal is itself; a template is every combination its parts' types allow
// (`tool.${kind}` with kind: 'stop' | 'track' is 'tool.stop' and 'tool.track'), or, when a part can be any string,
// the prefix it starts with, written `prefix*`. These are the keys code looks things up by: dictionary entries,
// class names of style rules.
import type * as TS from 'typescript';

/** Past this many combinations a template is read as its prefix. */
const MAX_COMBINATIONS = 256;

export function stringsOf(ts: typeof TS, checker: TS.TypeChecker, node: TS.Node): string[] {
  if (ts.isStringLiteralLike(node)) return [node.text];
  if (!ts.isTemplateExpression(node)) return [];
  let combos = [node.head.text];
  for (const span of node.templateSpans) {
    const values = literalValues(checker.getTypeAtLocation(span.expression));
    if (!values || combos.length * values.length > MAX_COMBINATIONS) return [...new Set(combos.map((c) => `${c}*`))].filter((p) => p !== '*');
    combos = combos.flatMap((c) => values.map((v) => `${c}${v}${span.literal.text}`));
  }
  return combos;
}

/** The values a type allows when it is a finite set of literals; null when it allows any string or number. */
function literalValues(type: TS.Type): string[] | null {
  const parts = type.isUnion() ? type.types : [type];
  const out: string[] = [];
  for (const p of parts) {
    if (p.isStringLiteral()) out.push(p.value);
    else if (p.isNumberLiteral()) out.push(String(p.value));
    else return null;
  }
  return out;
}

/** Whether a key is one of the strings, exactly or by a prefix. */
export function matches(key: string, strings: ReadonlySet<string>, prefixes: readonly string[]): boolean {
  return strings.has(key) || prefixes.some((p) => key.startsWith(p));
}

/** Splits found strings into exact ones and prefixes (without the `*`). */
export function splitStrings(found: Iterable<string>): { exact: Set<string>; prefixes: string[] } {
  const exact = new Set<string>();
  const prefixes = new Set<string>();
  for (const s of found) if (s.endsWith('*')) prefixes.add(s.slice(0, -1)); else exact.add(s);
  return { exact, prefixes: [...prefixes].sort() };
}
