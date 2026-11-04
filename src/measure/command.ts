// `converter measure`: runs the source project in a headless browser and writes the measured profile (calls per
// frame of every unit, per variant), which the audit uses to rate performance findings by what really runs.
import fs from 'node:fs';
import path from 'node:path';
import { ArgsError, parseFlags, type FlagSpec } from '../common/args.ts';
import { IR_VERSION, type GraphIR } from '../graph/ir.ts';
import { measure } from './run.ts';

export const MEASURE_FLAGS: readonly FlagSpec[] = [
  { name: 'source-root', required: true, help: 'root of the source project (its dev server is started from there)' },
  { name: 'graph', required: true, help: 'graph IR JSON of the same source (whole scope)' },
  { name: 'out', required: true, help: 'JSON file to write the profile to' },
  { name: 'chromium', required: true, help: 'Chromium or chrome-headless-shell executable' },
  { name: 'chromium-arg', repeatable: true, help: 'extra browser flag, e.g. the GPU backend (--use-angle=d3d11 on Windows)' },
  { name: 'scenario', required: true, help: 'path and query to open, e.g. /?city=small&seed=7' },
  { name: 'ready', required: true, help: 'JavaScript expression that is true once the scenario runs' },
  { name: 'frame-unit', required: true, help: 'graph unit id of the function called once per frame' },
  { name: 'variant', repeatable: true, help: 'name=expression evaluated before measuring that variant (e.g. a quality tier)' },
  { name: 'warmup', help: 'seconds to let each variant settle before its window (default 20)' },
  { name: 'window', help: 'seconds measured per variant (default 20)' },
  { name: 'ready-timeout', help: 'seconds to wait for --ready (default 600)' },
  { name: 'width', help: 'viewport width (default 1600)' },
  { name: 'height', help: 'viewport height (default 900)' },
  { name: 'allow-software-gl', help: '"yes" to accept a software renderer (counts only; not recommended)' },
];

function positive(value: string | undefined, name: string, fallback: number): number {
  if (value === undefined) return fallback;
  const n = Number(value);
  if (!Number.isFinite(n) || n <= 0) throw new ArgsError(`--${name} must be a positive number`);
  return n;
}

export async function runMeasureCommand(argv: readonly string[]): Promise<void> {
  const flags = parseFlags(argv, MEASURE_FLAGS);
  const ir = JSON.parse(fs.readFileSync(flags.require('graph'), 'utf8')) as GraphIR;
  if (ir.version !== IR_VERSION) throw new ArgsError(`the graph is IR version ${ir.version}; measure reads version ${IR_VERSION}: run \`converter graph\` again`);
  const variants = flags.all('variant').map((spec) => {
    const eq = spec.indexOf('=');
    if (eq <= 0) throw new ArgsError(`--variant ${spec}: expected name=expression`);
    return { name: spec.slice(0, eq), setup: spec.slice(eq + 1) };
  });
  const profile = await measure({
    sourceRoot: flags.require('source-root'), ir, chromium: flags.require('chromium'), chromiumArgs: flags.all('chromium-arg'),
    scenario: flags.require('scenario'), ready: flags.require('ready'), frameUnit: flags.require('frame-unit'), variants,
    warmupSeconds: positive(flags.one('warmup'), 'warmup', 20), windowSeconds: positive(flags.one('window'), 'window', 20),
    readyTimeoutSeconds: positive(flags.one('ready-timeout'), 'ready-timeout', 600),
    width: positive(flags.one('width'), 'width', 1600), height: positive(flags.one('height'), 'height', 900),
    allowSoftwareGl: flags.one('allow-software-gl') === 'yes',
  });
  const out = path.resolve(flags.require('out'));
  fs.mkdirSync(path.dirname(out), { recursive: true });
  fs.writeFileSync(out, JSON.stringify(profile), 'utf8');
  for (const v of profile.variants) {
    console.log(`measure ${v.name}: ${v.frames} frames in ${v.seconds} s, ${v.units.length} units ran, ` +
      `${v.unmapped.calls} unmapped calls; renderer ${profile.renderer}`);
  }
  console.log(`-> ${out}`);
}
