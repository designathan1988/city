// Names a definition outside the analysed scope by its library and the class that declares it, so C++ externals get
// the same stable keys as TypeScript ones (`godot-cpp:RenderingServer.instance_create`, `std:vector.erase`).
import fs from 'node:fs';
import { relativePath } from '../common/paths.ts';
import { tokenize } from './lexer.ts';
import type { LspPosition } from './symbols.ts';

interface ClassSpan {
  readonly name: string;
  readonly start: LspPosition;
  readonly end: LspPosition;
}

export function moduleOfCpp(file: string, root: string): string {
  if (/\/godot-cpp\//i.test(file)) return 'godot-cpp';
  if (/\/(include\/c\+\+|lib\/clang|llvm-mingw[^/]*\/[^/]*-mingw32\/include)\//i.test(file)) return 'std';
  if (file.startsWith(root + '/')) return 'project:' + relativePath(root, file);
  return 'system';
}

function before(a: LspPosition, b: LspPosition): boolean {
  return a.line < b.line || (a.line === b.line && a.character <= b.character);
}

/** The class and struct bodies of a file, found by matching braces after `class`/`struct` heads. */
function classSpans(text: string): ClassSpan[] {
  const tokens = tokenize(text);
  const spans: ClassSpan[] = [];
  const stack: { name: string | null; start: LspPosition }[] = [];
  let pending: string | null = null;
  for (let i = 0; i < tokens.length; i++) {
    const t = tokens[i]!;
    if ((t.text === 'class' || t.text === 'struct') && tokens[i - 1]?.text !== 'enum') {
      // the name: the last identifier before ':' or '{' (skipping `final` and attributes)
      let name: string | null = null;
      for (let j = i + 1; j < tokens.length && tokens[j]!.text !== '{' && tokens[j]!.text !== ';' && tokens[j]!.text !== ':'; j++) {
        if (tokens[j]!.kind === 'id' && tokens[j]!.text !== 'final' && tokens[j]!.text !== 'alignas') name = tokens[j]!.text;
      }
      pending = name;
    } else if (t.text === ';') {
      pending = null;
    } else if (t.text === '{') {
      stack.push({ name: pending, start: { line: t.line, character: t.column } });
      pending = null;
    } else if (t.text === '}') {
      const open = stack.pop();
      if (open?.name) spans.push({ name: open.name, start: open.start, end: { line: t.line, character: t.column } });
    }
  }
  return spans;
}

export class ExternalNames {
  readonly #spans = new Map<string, ClassSpan[]>();

  /** The innermost class of `file` containing `at`, or null. */
  container(file: string, at: LspPosition): string | null {
    let spans = this.#spans.get(file);
    if (!spans) {
      spans = fs.existsSync(file) ? classSpans(fs.readFileSync(file, 'utf8')) : [];
      this.#spans.set(file, spans);
    }
    let best: ClassSpan | null = null;
    for (const span of spans) {
      if (!before(span.start, at) || !before(at, span.end)) continue;
      if (!best || before(best.start, span.start)) best = span;
    }
    return best?.name ?? null;
  }
}
