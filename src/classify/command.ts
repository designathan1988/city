// `converter classify`: what a feature of the source becomes in the target, and what still blocks its translation.
import fs from 'node:fs';
import path from 'node:path';
import { ArgsError, parseFlags, type FlagSpec } from '../common/args.ts';
import { IR_VERSION, type GraphIR } from '../graph/ir.ts';
import { classifyFeature, renderClassification } from './run.ts';
import { readFeatureSpec, readMappingCatalog } from './spec.ts';

export const CLASSIFY_FLAGS: readonly FlagSpec[] = [
  { name: 'graph', required: true, help: 'graph IR of the source (`converter graph`, whole scope)' },
  { name: 'target-graph', required: true, help: 'graph IR of the target\'s C++ (`converter cppgraph`)' },
  { name: 'feature', required: true, help: 'feature spec JSON: feature, subsystem, files' },
  { name: 'mapping', required: true, help: 'mapping catalog JSON: source concept → target owner' },
  { name: 'out', required: true, help: 'JSON file to write the classification to' },
  { name: 'markdown', help: 'also write the readable report to this file' },
];

function readGraph(file: string): GraphIR {
  const ir = JSON.parse(fs.readFileSync(file, 'utf8')) as GraphIR;
  if (ir.version !== IR_VERSION) throw new ArgsError(`${file} is IR version ${ir.version}; this converter reads ${IR_VERSION}: build it again`);
  return ir;
}

export function runClassifyCommand(argv: readonly string[]): void {
  const flags = parseFlags(argv, CLASSIFY_FLAGS);
  const source = readGraph(flags.require('graph'));
  const target = readGraph(flags.require('target-graph'));
  if (source.language !== 'ts' || target.language !== 'cpp') throw new ArgsError('--graph must be the TypeScript source, --target-graph the C++ target');
  const result = classifyFeature(source, target, readFeatureSpec(flags.require('feature')), readMappingCatalog(flags.require('mapping')));
  const out = path.resolve(flags.require('out'));
  fs.mkdirSync(path.dirname(out), { recursive: true });
  fs.writeFileSync(out, JSON.stringify(result, null, 1), 'utf8');
  const markdown = flags.one('markdown');
  if (markdown !== undefined) fs.writeFileSync(markdown, renderClassification(result), 'utf8');
  console.log(`classify ${result.feature}: ${result.units.length} units, ${result.dependencies.length} dependencies ` +
    `(${result.dependencies.filter((d) => !d.problem).length} mapped), ${result.rewrites.length} rewrites, ${result.constants.length} constants; ` +
    `${result.ready ? 'ready' : `blocked by ${result.blocked.length}`} -> ${out}`);
  process.exitCode = result.ready ? 0 : 1;
}
