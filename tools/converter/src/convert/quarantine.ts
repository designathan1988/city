// The check on what a translation wrote, after each run and before anything reaches the target: translation
// validation (Pnueli, Siegel, Singerman, TACAS 1998): the translator is not trusted, each output is judged on its own.
// Nothing the ledger stopped was emitted, none of the catalog's wrong forms is in the C++, the target's size limits
// hold, and the code compiles against the target's own headers. Any finding refuses that module's whole output.
import fs from 'node:fs';
import path from 'node:path';
import { keyOf, logOf, mustRun, passed, runTask, textsKey, toolKey, treeKey } from '../common/tasks.ts';
import { isRuntimeFile } from '../translate/files.ts';

export interface QuarantineFinding {
  readonly file: string;
  readonly rule: string;
  readonly detail: string;
}

/** The catalog's wrong forms as they look in C++. */
const WRONG_FORMS: readonly { readonly rule: string; readonly form: RegExp; readonly detail: string }[] = [
  { rule: 'untranslated', form: /\/\*untranslated\*\//, detail: 'a construct left untranslated' },
  { rule: 'queue-shift', form: /\.erase\(\s*[\w.\->]+\.begin\(\)\s*\)/, detail: 'removal from the front of a vector' },
  { rule: 'per-object-draw', form: /\binstance_create\b/, detail: 'one render instance per item' },
  { rule: 'nondeterminism-in-simulation', form: /\b(std::rand|rand\(\)|std::random_device|std::chrono|time\(nullptr\))/, detail: 'wall clock or unseeded randomness' },
  { rule: 'hot-allocation', form: /(^|[^\w.])new\s+[A-Za-z_]|\bmalloc\(/, detail: 'a heap allocation by hand' },
];

export interface TargetLimits {
  readonly maxLines: Readonly<Record<string, number>>;
  readonly maxFunctionLines: Readonly<Record<string, number>>;
}

export function targetLimits(rulesFile: string): TargetLimits {
  const rules = JSON.parse(fs.readFileSync(rulesFile, 'utf8')) as { max_lines?: Record<string, number>; max_function_lines?: Record<string, number> };
  if (!rules.max_lines || !rules.max_function_lines) throw new Error(`${rulesFile} has no max_lines / max_function_lines`);
  return { maxLines: rules.max_lines, maxFunctionLines: rules.max_function_lines };
}

/** The translator writes each function from a head at column 0 to a `}` at column 0: the lines of the longest one. */
function longestFunction(text: string): number {
  let longest = 0;
  let start = -1;
  text.split('\n').forEach((line, i) => {
    if (start < 0 && /^[A-Za-z].*\) \{$/.test(line)) start = i;
    else if (start >= 0 && line === '}') {
      longest = Math.max(longest, i - start + 1);
      start = -1;
    }
  });
  return longest;
}

export function scanFiles(files: ReadonlyMap<string, string>, limits: TargetLimits): QuarantineFinding[] {
  const out: QuarantineFinding[] = [];
  for (const [file, text] of files) {
    // the runtime of the built-ins is the converter's own, checked against JavaScript by its differential
    if (isRuntimeFile(file)) continue;
    const code = text.replace(/"(?:[^"\\\n]|\\.)*"/g, '""').replace(/\/\/.*$/gm, '');
    for (const { rule, form, detail } of WRONG_FORMS) if (form.test(code)) out.push({ file, rule, detail });
    const extension = path.extname(file);
    const lines = text.split('\n').length;
    const maxLines = limits.maxLines[extension];
    if (maxLines !== undefined && lines > maxLines) out.push({ file, rule: 'god-file', detail: `${lines} lines (limit ${maxLines})` });
    const maxFn = limits.maxFunctionLines[extension];
    const longest = longestFunction(text);
    if (maxFn !== undefined && longest > maxFn) out.push({ file, rule: 'god-function', detail: `a function of ${longest} lines (limit ${maxFn})` });
  }
  return out;
}

/** What the translator traced beyond its roots: each source function it emitted must be one the ledger allows. */
export function scanUnits(stopOf: (id: string) => string | null, emitted: readonly { readonly source: string; readonly kind: string }[]): QuarantineFinding[] {
  const out: QuarantineFinding[] = [];
  for (const u of emitted) {
    if (u.kind !== 'edit' && u.kind !== 'query') continue;
    const stop = stopOf(u.source);
    if (stop) out.push({ file: u.source, rule: 'stopped-unit', detail: stop });
  }
  return out;
}

/** A syntax check of one translation unit against the generated and the target's headers; past this its process is stopped. */
const COMPILE_TIMEOUT_S = 300;
/** The same check of every generated header at once, which one compile reads together. */
const HEADERS_TIMEOUT_S = 900;
const SYNTAX_ONLY = ['-std=c++17', '-fsyntax-only', '-ferror-limit=0'];

/** Each compile context, read once per run: a conversion reads the target and never writes it (it writes only its --out). */
const contexts = new Map<string, string>();

/** What every compile's answer also depends on: the compiler and the target's headers (by content), the flags. */
function compileContext(cxx: string, targetRoot: string): string {
  const at = `${cxx}\0${path.resolve(targetRoot)}`;
  const known = contexts.get(at);
  if (known !== undefined) return known;
  const key = keyOf('syntax-check', cxx, toolKey(cxx), treeKey(targetRoot, ['native/src']), ...SYNTAX_ONLY);
  contexts.set(at, key);
  return key;
}

/**
 * The compile of `file` (relative to the overlay) as a task: run in the overlay with relative paths, so the same content
 * in another overlay is the same task. Its key is `content` (what the file reaches) with the compile's context; the
 * answer is the compiler's log.
 */
async function syntaxCheck(cxx: string, overlay: string, targetRoot: string, file: string, content: string, timeoutS: number): Promise<{ ok: boolean; log: string }> {
  const r = mustRun(`the compile of ${file}`, await runTask({ name: 'converter-syntax-check', cwd: overlay, timeoutS, extra: [content, compileContext(cxx, targetRoot)],
    argv: [cxx, ...SYNTAX_ONLY, '-I', 'native/src', '-I', path.join(targetRoot, 'native', 'src').split(path.sep).join('/'), file] }));
  return { ok: passed(r), log: logOf(r) };
}

/** Every generated header in one compile: one broken header would otherwise fail each file that includes it, compiled one by one. */
export async function compileHeaders(cxx: string, overlay: string, targetRoot: string, files: ReadonlyMap<string, string>): Promise<string[]> {
  const all = 'all_headers.cpp';
  const text = [...files.keys()].filter((f) => f.endsWith('.h')).sort().map((f) => `#include "${f.replace(/^native\/src\//, '')}"`).join('\n') + '\n';
  fs.writeFileSync(path.join(overlay, all), text);
  const headers = [...files].filter(([f]) => f.endsWith('.h'));
  const { log } = await syntaxCheck(cxx, overlay, targetRoot, all, textsKey([[all, text], ...headers]), HEADERS_TIMEOUT_S);
  return log.split(/\r?\n/).filter((l) => l.includes(': error:') || l.includes('requested here'));
}

/** Every source file compiled against the overlay first, then the target's own headers (C++17, the target's standard). */
export async function compileFiles(cxx: string, overlay: string, targetRoot: string, files: ReadonlyMap<string, string>): Promise<QuarantineFinding[]> {
  // what a file's result depends on: itself and the generated headers it reaches, so a change recompiles only who includes it
  const reached = (file: string): string => {
    const seen = new Set<string>([file]);
    const queue = [file];
    while (queue.length > 0) {
      for (const m of (files.get(queue.pop()!) ?? '').matchAll(/#include "([^"]+)"/g)) {
        const next = `native/src/${m[1]}`;
        if (files.has(next) && !seen.has(next)) { seen.add(next); queue.push(next); }
      }
    }
    return textsKey([...seen].map((f) => [f, files.get(f)!] as const));
  };
  // every .cpp at once, each its own task in the shared pool
  const results = await Promise.all([...files.keys()].filter((file) => file.endsWith('.cpp')).map(async (file): Promise<QuarantineFinding | null> => {
    const { ok, log } = await syntaxCheck(cxx, overlay, targetRoot, file, reached(file), COMPILE_TIMEOUT_S);
    if (ok) return null;
    const errors = log.split(/\r?\n/).filter((l) => l.includes('error:') || l.includes('requested here')).slice(0, 40).join(' | ');
    return { file, rule: 'does-not-compile', detail: errors || 'the compiler failed' };
  }));
  return results.filter((f): f is QuarantineFinding => f !== null);
}
