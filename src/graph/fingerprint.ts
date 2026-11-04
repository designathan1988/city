// Winnowing fingerprints (Schleimer, Wilkerson and Aiken, "Winnowing: Local Algorithms for Document
// Fingerprinting", SIGMOD 2003): hashes of every k-gram of the exact token sequence, keeping the minimum of each
// window of w consecutive hashes. Any identical run of at least w + k - 1 tokens shares a fingerprint, so k = 40 and
// w = 41 find every duplicate of 80 tokens or more: the threshold of the target project's clone gate.

export const GRAM_TOKENS = 40;
export const WINDOW_GRAMS = 41;
const BASE = 0x01000193;

export function tokenHash(text: string): number {
  let h = 0x811c9dc5;
  for (let i = 0; i < text.length; i++) h = Math.imul(h ^ text.charCodeAt(i), 0x01000193);
  return h | 0;
}

/** `hashes[i]` and `lines[i]` describe token i; returns [fingerprint, line of the k-gram's first token] pairs. */
export function winnow(hashes: readonly number[], lines: readonly number[]): [number, number][] {
  const grams: number[] = [];
  let power = 1;
  for (let i = 0; i < GRAM_TOKENS - 1; i++) power = Math.imul(power, BASE);
  let h = 0;
  for (let i = 0; i < hashes.length; i++) {
    if (i >= GRAM_TOKENS) h = (h - Math.imul(hashes[i - GRAM_TOKENS]!, power)) | 0;
    h = (Math.imul(h, BASE) + hashes[i]!) | 0;
    if (i >= GRAM_TOKENS - 1) grams.push(h >>> 0);
  }
  // Fewer than w + k - 1 tokens cannot hold a duplicate of the threshold length.
  if (grams.length < WINDOW_GRAMS) return [];
  const out: [number, number][] = [];
  let last = -1;
  for (let start = 0; start + WINDOW_GRAMS <= grams.length; start++) {
    let min = start;
    for (let j = start; j < start + WINDOW_GRAMS; j++) if (grams[j]! <= grams[min]!) min = j;
    if (min !== last) {
      out.push([grams[min]!, lines[min]!]);
      last = min;
    }
  }
  return out;
}
