// Whether a key is among the strings code can produce (values.ts): one of the exact strings, or one starting with a
// prefix of strings made with a part of any value.
export function matches(key: string, exact: ReadonlySet<string>, prefixes: Iterable<string>): boolean {
  if (exact.has(key)) return true;
  for (const p of prefixes) if (key.startsWith(p)) return true;
  return false;
}
