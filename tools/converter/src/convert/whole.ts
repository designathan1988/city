// The whole program in one translation: one flow, one emission, each file written once. What cannot be written leaves alone, and
// what uses it falls with it, round after round until nothing more falls: the translator's own errors first, then what the
// ledger stops, then what does not compile, each placed on its unit by file and line. No module is judged against another, so no
// two can write a shared file differently, and the fixed cost of a translation is paid per round, not per module.
import fs from 'node:fs';
import path from 'node:path';
import { removeTree, writeTree } from '../common/files.ts';
import { compileFiles, scanUnits } from './quarantine.ts';
import { translate, type PreparedProgram, type TranslationResult } from '../translate/run.ts';
import type { TranslationSpec } from '../translate/spec.ts';

const MOST_ROUNDS = 40;

export interface WholeInput {
  readonly sourceRoot: string;
  readonly typescript: string | undefined;
  readonly targetRoot: string;
  readonly targetGraph: string;
  readonly cxx: string;
  readonly candidate: string;
  readonly prepared: PreparedProgram;
  readonly stopOf: (id: string) => string | null;
}

export interface WholeResult {
  readonly result: TranslationResult;
  /** What was left out, `module#name`, with why. */
  readonly left: ReadonlyMap<string, string>;
  /** What still does not compile, by file: its units are not counted as written. */
  readonly failing: readonly string[];
}

type Unit = TranslationResult['units'][number];

/** The name a line of a generated file is in: a function's, a constant's, or a struct's with what the line declares. */
function placeOf(text: string, line: number): { name: string; struct: string | null; member: string | null } | null {
  const lines = text.split('\n');
  const at = lines[line - 1] ?? '';
  for (let i = Math.min(line, lines.length) - 1; i >= 0; i--) {
    const l = lines[i]!;
    const struct = /^(?:template <[^>]*> )?struct (\w+)/.exec(l);
    if (struct) {
      if (i === line - 1 || l.endsWith(';')) return { name: struct[1]!, struct: struct[1]!, member: null };
      const declared = /(\w+)\(/.exec(at) ?? /(\w+)(?:\{\}| = [^;]*)?;/.exec(at);
      return { name: struct[1]!, struct: struct[1]!, member: declared?.[1] ?? null };
    }
    const constant = /^(?:constexpr|inline const) .*?(\w+)(?:\(\) \{| = )/.exec(l);
    if (constant) return { name: constant[1]!, struct: null, member: null };
    if (/^};?$/.test(l) && i < line - 1) return null;
    const fn = /^(?!\s|\/\/|#|namespace\b|using\b)[^;]*?\b(?:(\w+)(?:<[^>]*>)?::)?(\w+)\s*\([^;]*$/.exec(l);
    if (fn) return { name: fn[2]!, struct: fn[1] ?? null, member: fn[1] ? fn[2]! : null };
  }
  return null;
}

/** Each unit of a file's module by its C++ name. */
function unitsOfFile(file: string, units: readonly Unit[], prepared: PreparedProgram): Map<string, Unit> {
  const out = new Map<string, Unit>();
  for (const u of units) {
    const placed = prepared.placement.of(u.source.slice(0, u.source.indexOf('#')));
    if ('error' in placed) continue;
    if (file.startsWith(`${placed.owner}/${placed.stem}`)) out.set(u.cpp, u);
  }
  return out;
}

/** The module `src/x.ts` of a generated file, or null. */
function moduleOfFile(file: string, result: TranslationResult, prepared: PreparedProgram): string | null {
  for (const id of result.declared) {
    const module = id.slice(0, id.indexOf('#'));
    const placed = prepared.placement.of(module);
    if (!('error' in placed) && file.startsWith(`${placed.owner}/${placed.stem}`)) return module;
  }
  return null;
}

/** What a compiler's detail places on units: `id → why`; the files it could place nothing of. */
function attribute(findings: readonly { file: string; detail: string }[], result: TranslationResult, prepared: PreparedProgram): { left: Map<string, string>; lost: string[] } {
  const left = new Map<string, string>();
  const lost: string[] = [];
  for (const f of findings) {
    let placed = 0;
    for (const m of f.detail.matchAll(/(native[\\/]src[\\/][^\s:|]+?\.(?:h|cpp)):(\d+):\d+: (?:error|note)/g)) {
      const file = m[1]!.replace(/\\/g, '/');
      const text = result.files.get(file);
      if (!text) continue;
      const place = placeOf(text, Number(m[2]));
      if (!place) continue;
      const units = unitsOfFile(file, result.units, prepared);
      const module = moduleOfFile(file, result, prepared);
      const why = `does not compile: ${f.detail.slice(0, 200)}`;
      if (place.struct && place.member && module) {
        left.set(`${module}#${place.struct}.${place.member}`, why);
        const member = units.get(place.member);
        if (member) left.set(member.source, why);
        placed++;
        continue;
      }
      const unit = units.get(place.name);
      if (unit) {
        left.set(unit.source, why);
        placed++;
      }
    }
    if (placed === 0) lost.push(f.file);
  }
  return { left, lost };
}

export async function convertWhole(specs: readonly TranslationSpec[], input: WholeInput, log: (text: string) => void): Promise<WholeResult> {
  const base = specs[0]!;
  const roots = specs.flatMap((s) => s.units.map((u) => `${s.source}#${u}`));
  const left = new Map<string, string>();
  const overlay = path.join(input.candidate, 'whole');
  let last: TranslationResult | null = null;
  let failing: string[] = [];
  for (let round = 1; round <= MOST_ROUNDS; round++) {
    const started = Date.now();
    const spec: TranslationSpec = { ...base, units: [], shared: roots, whole: true, left: [...left.keys()], state: { ...base.state, source: '', target: '', member: '' } };
    const result = translate({ sourceRoot: input.sourceRoot, tsconfig: path.join(input.sourceRoot, 'tsconfig.json'), typescriptDir: input.typescript, spec,
      targetRoot: input.targetRoot, targetGraph: input.targetGraph, prepared: input.prepared });
    last = result;
    const before = left.size;
    for (const [id, why] of result.fallen) left.set(id, why);
    for (const e of result.errors) {
      const unit = /\[unit ([^\]]+)\]$/.exec(e)?.[1];
      const file = /^(src\/\S+?\.tsx?):\d+:/.exec(e)?.[1];
      if (unit && file) left.set(`${file}#${unit}`, e);
    }
    for (const stopped of scanUnits(input.stopOf, result.units)) left.set(stopped.file, `${stopped.rule}: ${stopped.detail}`);
    const translated = Date.now();
    if (left.size > before) {
      log(`round ${round}: ${left.size} left out, ${result.units.length} written (${translated - started} ms)`);
      continue;
    }
    removeTree(overlay);
    writeTree(overlay, result.files);
    const findings = await compileFiles(input.cxx, overlay, input.targetRoot, result.files);
    const placed = attribute(findings, result, input.prepared);
    for (const [id, why] of placed.left) left.set(id, why);
    failing = placed.lost;
    log(`round ${round}: ${findings.length} files do not compile, ${left.size} left out, ${placed.lost.length} files unplaced (${translated - started} + ${Date.now() - translated} ms)`);
    if (findings.length === 0 || left.size === before) break;
  }
  fs.mkdirSync(input.candidate, { recursive: true });
  return { result: last!, left, failing };
}
