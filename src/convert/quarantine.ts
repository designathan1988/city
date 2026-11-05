// The check on what a translation wrote, after each run and before anything reaches the target: translation
// validation (Pnueli, Siegel, Singerman, TACAS 1998): the translator is not trusted, each output is judged on its own.
// Nothing the ledger stopped was emitted, none of the catalog's wrong forms is in the C++, the target's size limits
// hold, and the code compiles against the target's own headers. Any finding refuses that module's whole output.
import { execFile } from 'node:child_process';
import { createHash } from 'node:crypto';
import fs from 'node:fs';
import path from 'node:path';
import { MAX_CHILD_OUTPUT_BYTES } from '../common/files.ts';

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
    for (const { rule, form, detail } of WRONG_FORMS) if (form.test(text)) out.push({ file, rule, detail });
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

/** Every source file compiled against the overlay first, then the target's own headers (C++17, the target's standard). */
/** Compile results by content: the .cpp, every header the overlay holds, the compiler and the target (an identical
 *  input gives the identical result: a file the judgments meet again is not compiled again). Lives for the run. */
const compiled = new Map<string, QuarantineFinding | null>();

export async function compileFiles(cxx: string, overlay: string, targetRoot: string, files: ReadonlyMap<string, string>): Promise<QuarantineFinding[]> {
  const headers = createHash('sha256');
  for (const [file, text] of [...files].sort(([a], [b]) => a.localeCompare(b))) if (!file.endsWith('.cpp')) headers.update(file).update('\0').update(text).update('\0');
  const headerKey = headers.update(cxx).update(targetRoot).digest('hex');
  // every .cpp at once, each its own compiler process
  const results = await Promise.all([...files].filter(([file]) => file.endsWith('.cpp')).map(async ([file, text]) => {
    const key = createHash('sha256').update(headerKey).update(file).update('\0').update(text).digest('hex');
    if (compiled.has(key)) return compiled.get(key)!;
    const run = await new Promise<{ status: number | null; stderr: string; error?: Error }>((resolve) => {
      execFile(cxx, ['-std=c++17', '-fsyntax-only', '-I', path.join(overlay, 'native', 'src'), '-I', path.join(targetRoot, 'native', 'src'), path.join(overlay, file)],
        { encoding: 'utf8', windowsHide: true, maxBuffer: MAX_CHILD_OUTPUT_BYTES }, (error, _stdout, stderr) => resolve({ status: error ? (typeof error.code === 'number' ? error.code : 1) : 0, stderr: String(stderr), error: error ?? undefined }));
    });
    let finding: QuarantineFinding | null = null;
    if (run.status !== 0) {
      const errors = run.stderr.split(/\r?\n/).filter((l) => l.includes('error:')).slice(0, 5).join(' | ');
      finding = { file, rule: 'does-not-compile', detail: errors || String(run.error ?? 'the compiler failed') };
    }
    compiled.set(key, finding);
    return finding;
  }));
  return results.filter((f): f is QuarantineFinding => f !== null);
}
