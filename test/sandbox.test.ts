// Tests of the package and sandbox stages on throw-away repositories: the package is a patch series made outside the
// game, the sandbox verifies the base and the package in an isolated clone, and the game never changes.
import assert from 'node:assert/strict';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import { after, describe, it } from 'node:test';
import { fingerprint, git } from '../src/common/git.ts';
import { createPackage } from '../src/package/create.ts';
import { readManifest } from '../src/package/manifest.ts';
import type { SandboxConfig } from '../src/sandbox/config.ts';
import { integrationCommands, runSandbox } from '../src/sandbox/run.ts';

const author = { name: 'Converter Test', email: 'converter@test.invalid' };

function repo(files: Record<string, string>): string {
  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'converter-test-repo-'));
  git(dir, ['init', '--quiet', '-b', 'master']);
  for (const [name, content] of Object.entries(files)) {
    fs.mkdirSync(path.dirname(path.join(dir, name)), { recursive: true });
    fs.writeFileSync(path.join(dir, name), content);
  }
  git(dir, ['add', '-A']);
  git(dir, ['-c', `user.name=${author.name}`, '-c', `user.email=${author.email}`, 'commit', '--quiet', '-m', 'base']);
  return dir;
}

const node = (code: string): string[] => [process.execPath, '-e', code];

describe('package and sandbox', () => {
  const game = repo({ 'readme.txt': 'game\n', '.gitignore': 'vendor/\n' });
  fs.mkdirSync(path.join(game, 'vendor'));
  fs.writeFileSync(path.join(game, 'vendor', 'lib.txt'), 'outside version control\n');
  const source = repo({ 'feature.ts': 'export const x = 1;\n' });
  const out = fs.mkdtempSync(path.join(os.tmpdir(), 'converter-test-out-'));
  const pkg = path.join(out, 'transit');
  const gameBefore = fingerprint(game);
  after(() => {
    for (const dir of [game, source, out]) fs.rmSync(dir, { recursive: true, force: true, maxRetries: 5, retryDelay: 200 });
  });
  const manifest = createPackage({
    feature: 'transit', sourceRoot: source, gameRoot: game, outDir: pkg, author,
    topics: (read) => [
      { message: 'Add the transit state\n\nNew owner of stops and lines.', files: new Map([['native/src/sim/transit/transit_state.h', '#pragma once\n']]) },
      { message: 'Mention transit in the readme', files: new Map([['readme.txt', `${read('readme.txt')}transit\n`]]) },
    ],
  });

  it('stores the conversion as a patch series outside the game, one commit per topic', () => {
    assert.equal(manifest.patches.length, 2);
    assert.deepEqual(readManifest(pkg), manifest);
    assert.match(fs.readFileSync(path.join(pkg, 'patches', manifest.patches[1]!), 'utf8'), /\+transit/);
    assert.equal(fingerprint(game), gameBefore, 'making the package left the game as it was');
  });

  it('verifies the base and the package in an isolated clone, never touching the game', () => {
    const config: SandboxConfig = {
      copy: ['vendor'], hiddenRunner: 'unused.ps1',
      setup: [{ name: 'setup', run: node("require('fs').writeFileSync('setup.txt', 'ready')") }],
      verify: [
        { name: 'vendor', run: node("process.stdout.write(require('fs').readFileSync('vendor/lib.txt', 'utf8'))"), expect: { includes: ['outside version control'] } },
        { name: 'readme', run: node("process.stdout.write(require('fs').readFileSync('readme.txt', 'utf8'))") },
      ],
    };
    const sandbox = path.join(out, 'sandbox');
    const result = runSandbox({ packageDir: pkg, config, sandboxDir: sandbox });
    assert.equal(result.ok, true, JSON.stringify(result.steps));
    assert.deepEqual(result.steps.map((s) => `${s.phase}/${s.name}`), ['setup/setup', 'before/vendor', 'before/readme', 'after/vendor', 'after/readme']);
    assert.doesNotMatch(fs.readFileSync(path.join(pkg, 'evidence', 'before', 'readme.log'), 'utf8'), /transit/);
    assert.match(fs.readFileSync(path.join(pkg, 'evidence', 'after', 'readme.log'), 'utf8'), /transit/);
    assert.equal(git(sandbox, ['rev-list', '--count', `${manifest.game.base}..convert/transit`]).trim(), '2');
    assert.equal(fingerprint(game), gameBefore, 'the game is exactly as it was');
    assert.ok(integrationCommands(result).some((line) => line.includes(`fetch "${result.sandbox}" convert/transit:convert/transit`)));
  });

  it('stops at the first expectation a step fails, saying which', () => {
    const config: SandboxConfig = {
      copy: [], hiddenRunner: 'unused.ps1', setup: [],
      verify: [{ name: 'wants', run: node("process.stdout.write('nothing')"), expect: { includes: ['something'] } }],
    };
    const pkgCopy = path.join(out, 'transit-copy');
    fs.cpSync(pkg, pkgCopy, { recursive: true });
    const result = runSandbox({ packageDir: pkgCopy, config, sandboxDir: path.join(out, 'sandbox-fail') });
    assert.equal(result.ok, false);
    assert.equal(result.steps.at(-1)!.problem, 'output lacks "something"');
    assert.equal(fingerprint(game), gameBefore);
  });

  it('refuses a package or a sandbox inside the game', () => {
    assert.throws(() => createPackage({ feature: 'x', sourceRoot: source, gameRoot: game, outDir: path.join(game, 'pkg'), author, topics: () => [] }), /outside the game/);
    assert.throws(() => runSandbox({ packageDir: pkg, config: { copy: [], hiddenRunner: '', setup: [], verify: [] }, sandboxDir: path.join(game, 'sb') }), /outside the game/);
  });
});
