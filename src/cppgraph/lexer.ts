// A C++ tokenizer for facts and fingerprints: identifiers, numbers, strings and punctuation with their line, skipping
// comments, preprocessor lines and the contents of string, raw string and character literals.

export interface CppToken {
  readonly text: string;
  readonly kind: 'id' | 'number' | 'string' | 'punct';
  /** 0-based line and column, as LSP positions are. */
  readonly line: number;
  readonly column: number;
}

const PUNCTUATION = ['<<=', '>>=', '->*', '::', '->', '++', '--', '<<', '>>', '<=', '>=', '==', '!=', '&&', '||', '+=', '-=', '*=', '/=',
  '%=', '&=', '|=', '^=', '.*'];

export function tokenize(text: string): CppToken[] {
  const out: CppToken[] = [];
  let i = 0, line = 0, lineStart = 0, lineHasCode = false;
  const at = (k: number): string => text[k] ?? '';
  const push = (start: number, end: number, kind: CppToken['kind']): void => {
    out.push({ text: text.slice(start, end), kind, line, column: start - lineStart });
    lineHasCode = true;
  };
  const newline = (k: number): void => { line++; lineStart = k + 1; lineHasCode = false; };
  while (i < text.length) {
    const c = at(i);
    if (c === '\n') { newline(i); i++; continue; }
    if (c === ' ' || c === '\t' || c === '\r') { i++; continue; }
    if (c === '/' && at(i + 1) === '/') { while (i < text.length && at(i) !== '\n') i++; continue; }
    if (c === '/' && at(i + 1) === '*') {
      i += 2;
      while (i < text.length && !(at(i) === '*' && at(i + 1) === '/')) { if (at(i) === '\n') newline(i); i++; }
      i += 2;
      continue;
    }
    if (c === '#' && !lineHasCode) { // a preprocessor line, with its backslash continuations
      while (i < text.length && at(i) !== '\n') { if (at(i) === '\\' && at(i + 1) === '\n') { newline(i + 1); i++; } i++; }
      continue;
    }
    const raw = /^(?:u8|u|U|L)?R"([^()\\\s]{0,16})\(/.exec(text.slice(i, i + 24));
    if (raw) {
      const close = `)${raw[1]}"`;
      const start = i;
      const end = text.indexOf(close, i + raw[0].length);
      const stop = end < 0 ? text.length : end + close.length;
      push(start, stop, 'string');
      for (let k = start; k < stop; k++) if (at(k) === '\n') newline(k);
      i = stop;
      continue;
    }
    if (c === '"' || c === '\'') {
      const start = i;
      i++;
      while (i < text.length && at(i) !== c && at(i) !== '\n') i += at(i) === '\\' ? 2 : 1;
      i++;
      push(start, i, 'string');
      continue;
    }
    if (/[A-Za-z_]/.test(c)) {
      const start = i;
      while (/[A-Za-z0-9_]/.test(at(i))) i++;
      push(start, i, 'id');
      continue;
    }
    if (/[0-9]/.test(c) || (c === '.' && /[0-9]/.test(at(i + 1)))) {
      const start = i;
      while (/[0-9A-Za-z_.']/.test(at(i)) || ((at(i) === '+' || at(i) === '-') && /[eEpP]/.test(at(i - 1)))) i++;
      push(start, i, 'number');
      continue;
    }
    const op = PUNCTUATION.find((p) => text.startsWith(p, i)) ?? c;
    push(i, i + op.length, 'punct');
    i += op.length;
  }
  return out;
}
