// `converter package`: a translated feature as a conversion package (manifest, patch series, report), made on an
// isolated clone of the target, never in it. Three topics, one commit each: the subsystem's files; its registration
// where the target's gates look (the layer rule and the map); its state as a member of the state manager.
import fs from 'node:fs';
import path from 'node:path';
import { ArgsError, parseFlags, type FlagSpec } from '../common/args.ts';
import { targetGraph as derivedTargetGraph } from '../common/derived.ts';
import { createPackage, type Topic } from '../package/create.ts';
import { addLayer, addMapLine, addStateMember } from './registry.ts';
import { renderTranslation, translate } from './run.ts';
import { readTranslationSpec } from './spec.ts';

export const PACKAGE_FLAGS: readonly FlagSpec[] = [
  { name: 'source-root', required: true, help: 'root of the TypeScript source project' },
  { name: 'tsconfig', help: 'the source\'s tsconfig.json (default: <source-root>/tsconfig.json)' },
  { name: 'typescript', help: 'TypeScript package directory (default: the source project\'s own)' },
  { name: 'spec', required: true, help: 'translation spec JSON' },
  { name: 'target-root', required: true, help: 'root of the target game (read only; cloned to build the package)' },
  { name: 'out', required: true, help: 'package folder (outside the target; must be empty or absent)' },
  { name: 'author-name', required: true, help: 'name the package\'s commits are by' },
  { name: 'author-email', required: true, help: 'e-mail the package\'s commits are by' },
];

export function runPackageCommand(argv: readonly string[]): void {
  const flags = parseFlags(argv, PACKAGE_FLAGS);
  const sourceRoot = path.resolve(flags.require('source-root'));
  const targetRoot = path.resolve(flags.require('target-root'));
  const spec = readTranslationSpec(flags.require('spec'));
  const { registry, state } = spec;
  if (!registry || !state.anchor || !state.memberLine) throw new ArgsError('the spec needs registry (doc, mapLine) and state.anchor / state.memberLine to package');
  const result = translate({ sourceRoot, tsconfig: flags.one('tsconfig') ?? path.join(sourceRoot, 'tsconfig.json'), typescriptDir: flags.one('typescript'), spec, targetRoot, targetGraph: derivedTargetGraph({ targetRoot }) });
  if (result.errors.length > 0) throw new Error(`the translation has errors; nothing is packaged:\n${result.errors.join('\n')}`);
  const out = path.resolve(flags.require('out'));
  const rulesPath = spec.layer.rules;
  const manifest = createPackage({
    feature: spec.feature, sourceRoot, sourceFiles: result.sourcesRead, gameRoot: targetRoot, outDir: out,
    author: { name: flags.require('author-name'), email: flags.require('author-email') },
    topics: (read): Topic[] => {
      const need = (file: string): string => {
        const text = read(file);
        if (text === null) throw new Error(`the target has no ${file}`);
        return text;
      };
      const rulesText = need(rulesPath);
      const markers = (JSON.parse(rulesText) as { map_markers?: [string, string] }).map_markers;
      if (!markers) throw new Error(`${rulesPath} has no map_markers`);
      const summary = `${spec.feature} (${spec.subsystem}), translated from ${spec.source}`;
      return [
        { message: `Add ${spec.subsystem}: ${spec.summary.toLowerCase()}\n\nTranslated by the converter from ${spec.source}; every file is the owner's.`, files: result.files },
        {
          message: `Register ${spec.subsystem} in the layer rules and the map\n\n${summary}: it may include ${spec.layer.includes.join(', ')}.`,
          files: new Map([
            [rulesPath, addLayer(rulesText, spec.subsystem, spec.layer.includes)],
            [registry.doc, addMapLine(need(registry.doc), markers, spec.subsystem, registry.mapLine)],
          ]),
        },
        {
          message: `Hold ${state.target} in ${path.basename(state.root)}\n\n${summary}: its state is a member of the state manager.`,
          files: new Map([[state.root, addStateMember(need(state.root), result.stateHeader, state.anchor!, state.memberLine!)]]),
        },
      ];
    },
  });
  // The report: what the package holds, and what other owners still have to provide.
  fs.writeFileSync(path.join(out, 'translation.md'), renderTranslation(result));
  console.log(`package ${manifest.feature}: ${manifest.patches.length} patches on ${manifest.game.base.slice(0, 7)} -> ${out}`);
  for (const p of manifest.patches) console.log(`  ${p}`);
  if (result.pending.length > 0) console.log(`  left to other owners: ${result.pending.map((p) => p.owner).join(', ')}`);
}
