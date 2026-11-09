// The symbols clangd gave each C++ file, kept by an exact key so a later graph parses only what changed. A file's symbols are
// what its parse yields, and its parse reads the file, every project header it includes (closed over their own includes), its
// compile command and the headers outside the project (godot-cpp, the standard library: fixed by the compilation database and the
// toolchain) through the clangd that parsed it: the key is all of that, so a header edited re-parses exactly the files that include it.
import { createHash } from 'node:crypto';
import fs from 'node:fs';
import path from 'node:path';
import { normalizePath } from '../common/paths.ts';
import type { DocumentSymbol } from './symbols.ts';

/** A quoted include (`#include "core/math.h"`): the project's own headers. Angle includes are the toolchain's and godot-cpp's. */
const QUOTED_INCLUDE = /^\s*#\s*include\s*"([^"]+)"/gm;

export class SymbolCache {
  readonly #dir: string;
  readonly #context: string;
  readonly #texts: ReadonlyMap<string, string>;
  readonly #includeDirs: readonly string[];
  readonly #closure = new Map<string, readonly string[]>();

  /**
   * `dir`: where the symbols are kept (outside the project, the same across graphs). `context`: what every parse shares (clangd's
   * version, the compilation database's content). `texts`: every file in scope by its path. `includeDirs`: where a quoted include
   * is looked for after the including file's own folder (the project's include roots).
   */
  constructor(dir: string, context: string, texts: ReadonlyMap<string, string>, includeDirs: readonly string[]) {
    this.#dir = dir;
    this.#context = context;
    this.#texts = texts;
    this.#includeDirs = includeDirs;
    fs.mkdirSync(dir, { recursive: true });
  }

  /** The project header a quoted include names, from `file`: its own folder first, then each include root; null outside the scope. */
  #resolve(file: string, name: string): string | null {
    for (const base of [path.dirname(file), ...this.#includeDirs]) {
      const candidate = normalizePath(path.resolve(base, name));
      if (this.#texts.has(candidate)) return candidate;
    }
    return null;
  }

  /** Every project file `file` reads, itself first then its includes closed over theirs, each once, in a fixed order. */
  #reads(file: string): readonly string[] {
    const known = this.#closure.get(file);
    if (known) return known;
    const seen = new Set<string>([file]);
    const order = [file];
    for (let i = 0; i < order.length; i++) {
      const text = this.#texts.get(order[i]!) ?? '';
      for (const match of text.matchAll(QUOTED_INCLUDE)) {
        const header = this.#resolve(order[i]!, match[1]!);
        if (header && !seen.has(header)) { seen.add(header); order.push(header); }
      }
    }
    const sorted = [file, ...order.slice(1).sort()];
    this.#closure.set(file, sorted);
    return sorted;
  }

  key(file: string): string {
    const h = createHash('sha256').update(this.#context).update('\0');
    for (const read of this.#reads(file)) h.update(read).update('\0').update(this.#texts.get(read) ?? '').update('\0');
    return h.digest('hex');
  }

  #path(file: string): string {
    return path.join(this.#dir, `${this.key(file)}.json`);
  }

  get(file: string): DocumentSymbol[] | null {
    const at = this.#path(file);
    return fs.existsSync(at) ? (JSON.parse(fs.readFileSync(at, 'utf8')) as DocumentSymbol[]) : null;
  }

  /** Written to a file of its own name first, then moved in place: a reader never sees half of it. */
  put(file: string, symbols: readonly DocumentSymbol[]): void {
    const at = this.#path(file);
    const part = `${at}.${process.pid}.part`;
    fs.writeFileSync(part, JSON.stringify(symbols));
    fs.renameSync(part, at);
  }
}
