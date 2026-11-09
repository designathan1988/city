// Tests of the package and sandbox stages on throw-away repositories: the package is a patch series made outside the
// game, the sandbox verifies the base and the package in an isolated clone, a port is written and verified in a
// workspace and packaged from its branch, and the game never changes.
import assert from 'node:assert/strict';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import { after, describe, it } from 'node:test';
import { removeTree, sha256 } from '../src/common/files.ts';
import { fingerprint, git } from '../src/common/git.ts';
import { createPackage, packageBranch } from '../src/package/create.ts';
import { readManifest } from '../src/package/manifest.ts';
import type { SandboxConfig } from '../src/sandbox/config.ts';
import { integrationCommands, runSandbox } from '../src/sandbox/run.ts';
import { createWorkspace, readWorkspace, verifyWorkspace } from '../src/sandbox/workspace.ts';

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
/** A test step's deadline: each is one Node process printing a line. */
const STEP_TIMEOUT_S = 120;

describe('package and sandbox', () => {
  const game = repo({ 'readme.txt': 'game\n', '.gitignore': 'vendor/\n' });
  fs.mkdirSync(path.join(game, 'vendor'));
  fs.writeFileSync(path.join(game, 'vendor', 'lib.txt'), 'outside version control\n');
  const source = repo({ 'feature.ts': 'export const x = 1;\n' });
  // The source's working tree may hold uncommitted changes: the package records what it read by content.
  fs.writeFileSync(path.join(source, 'feature.ts'), 'export const x = 2;\n');
  const converterRoot = repo({ 'converter.ts': '' });
  const out = fs.mkdtempSync(path.join(os.tmpdir(), 'converter-test-out-'));
  const pkg = path.join(out, 'transit');
  const gameBefore = fingerprint(game);
  after(() => {
    for (const dir of [game, source, converterRoot, out]) removeTree(dir);
  });
  const manifest = createPackage({
    feature: 'transit', sourceRoot: source, sourceFiles: ['feature.ts'], gameRoot: game, outDir: pkg, author, converterRoot,
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
    assert.deepEqual(manifest.source.files, [{ path: 'feature.ts', sha256: sha256(path.join(source, 'feature.ts')) }]);
  });

  it('refuses to package from a converter with uncommitted changes', () => {
    const dirty = repo({ 'converter.ts': '' });
    fs.writeFileSync(path.join(dirty, 'converter.ts'), 'changed');
    try {
      assert.throws(() => createPackage({
        feature: 'x', sourceRoot: source, sourceFiles: [], gameRoot: game, outDir: path.join(out, 'dirty'), author, converterRoot: dirty,
        topics: () => [{ message: 'x', files: new Map([['x.txt', 'x']]) }],
      }), /uncommitted changes/);
    } finally {
      removeTree(dirty);
    }
  });

  it('verifies the base and the package in an isolated clone, never touching the game', () => {
    const config: SandboxConfig = {
      copy: ['vendor'], hiddenRunner: 'unused.ps1',
      setup: [{ name: 'setup', timeoutSeconds: STEP_TIMEOUT_S, run: node("require('fs').writeFileSync('setup.txt', 'ready')") }],
      verify: [
        { name: 'vendor', timeoutSeconds: STEP_TIMEOUT_S, run: node("process.stdout.write(require('fs').readFileSync('vendor/lib.txt', 'utf8'))"), expect: { includes: ['outside version control'] } },
        { name: 'readme', timeoutSeconds: STEP_TIMEOUT_S, run: node("process.stdout.write(require('fs').readFileSync('readme.txt', 'utf8'))") },
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
    assert.equal(result.gameChanged, false);
    assert.ok(integrationCommands(result).some((line) => line.includes(`fetch "${result.sandbox}" convert/transit:convert/transit`)));
  });

  it('stops at the first expectation a step fails, saying which', () => {
    const config: SandboxConfig = {
      copy: [], setup: [],
      verify: [{ name: 'wants', timeoutSeconds: STEP_TIMEOUT_S, run: node("process.stdout.write('nothing')"), expect: { includes: ['something'] } }],
    };
    const pkgCopy = path.join(out, 'transit-copy');
    fs.cpSync(pkg, pkgCopy, { recursive: true });
    const result = runSandbox({ packageDir: pkgCopy, config, sandboxDir: path.join(out, 'sandbox-fail') });
    assert.equal(result.ok, false);
    assert.equal(result.steps.at(-1)!.problem, 'output lacks "something"');
    assert.equal(fingerprint(game), gameBefore);
  });

  it('refuses a package or a sandbox inside the game', () => {
    assert.throws(() => createPackage({
      feature: 'x', sourceRoot: source, sourceFiles: [], gameRoot: game, outDir: path.join(game, 'pkg'), author, converterRoot, topics: () => [],
    }), /outside the game/);
    assert.throws(() => runSandbox({ packageDir: pkg, config: { copy: [], setup: [], verify: [] }, sandboxDir: path.join(game, 'sb') }), /outside the game/);
  });
});

describe('workspace and pack', () => {
  const game = repo({ 'readme.txt': 'game\n', '.gitignore': 'vendor/\n' });
  fs.mkdirSync(path.join(game, 'vendor'));
  fs.writeFileSync(path.join(game, 'vendor', 'lib.txt'), 'outside version control\n');
  const source = repo({ 'src/feature.ts': 'export const x = 1;\n', 'src/other.ts': '' });
  const converterRoot = repo({ 'converter.ts': '' });
  const out = fs.mkdtempSync(path.join(os.tmpdir(), 'converter-test-out-'));
  // What `converter feature` wrote: the source files the port reads, under source/.
  const featureDir = path.join(out, 'feature');
  fs.mkdirSync(path.join(featureDir, 'source', 'src'), { recursive: true });
  fs.writeFileSync(path.join(featureDir, 'source', 'src', 'feature.ts'), 'export const x = 1;\n');
  const gameBefore = fingerprint(game);
  after(() => {
    for (const dir of [game, source, converterRoot, out]) removeTree(dir);
  });
  const config: SandboxConfig = {
    copy: ['vendor'], hiddenRunner: 'unused.ps1',
    setup: [{ name: 'setup', timeoutSeconds: STEP_TIMEOUT_S, run: node("process.stdout.write('configured')") }],
    verify: [
      { name: 'vendor', timeoutSeconds: STEP_TIMEOUT_S, run: node("process.stdout.write(require('fs').readFileSync('vendor/lib.txt', 'utf8'))"), expect: { includes: ['outside version control'] } },
      { name: 'readme', timeoutSeconds: STEP_TIMEOUT_S, run: node("process.stdout.write(require('fs').readFileSync('readme.txt', 'utf8'))") },
    ],
  };
  const port = path.join(out, 'port');
  const made = createWorkspace({ gameRoot: game, portDir: port, feature: 'transit', config });
  const ws = made.record.workspace;
  const pack = (dir: string): ReturnType<typeof packageBranch> => packageBranch({
    feature: 'transit', sourceRoot: source, sourceFiles: ['src/feature.ts'], gameRoot: game, repo: ws, base: made.record.base,
    branch: made.record.branch, outDir: path.join(out, dir), converterRoot,
  });
  const commit = (file: string, text: string, message: string): void => {
    fs.writeFileSync(path.join(ws, file), text);
    git(ws, ['add', '--', file]);
    git(ws, ['-c', `user.name=${author.name}`, '-c', `user.email=${author.email}`, 'commit', '--quiet', '-m', message]);
  };

  it('clones the game on a branch of its own, with what the build needs, and runs the setup there', () => {
    assert.equal(made.ok, true, JSON.stringify(made.steps));
    assert.equal(git(ws, ['rev-parse', '--abbrev-ref', 'HEAD']).trim(), 'convert/transit');
    assert.equal(made.record.base, git(game, ['rev-parse', 'HEAD']).trim());
    assert.equal(fs.readFileSync(path.join(ws, 'vendor', 'lib.txt'), 'utf8'), 'outside version control\n');
    assert.deepEqual(readWorkspace(port), made.record);
    assert.match(fs.readFileSync(path.join(port, 'evidence', 'setup', 'setup.log'), 'utf8'), /configured/);
    assert.equal(git(ws, ['status', '--porcelain']).trim(), '', 'the record and the evidence stay outside the clone');
    assert.equal(fingerprint(game), gameBefore);
  });

  it('refuses a port folder inside the game and a feature name that cannot be a branch', () => {
    assert.throws(() => createWorkspace({ gameRoot: game, portDir: path.join(game, 'port'), feature: 'transit', config }), /outside the game/);
    assert.throws(() => createWorkspace({ gameRoot: game, portDir: path.join(out, 'bad'), feature: 'Transit Lines', config }), /lower case/);
  });

  it('packages nothing before the first commit', () => {
    assert.throws(() => pack('empty'), /no commits/);
  });

  it('verifies the named steps, in the evidence of the commit they judged', () => {
    commit('readme.txt', 'game\ntransit\n', 'Add transit to the readme');
    const head = git(ws, ['rev-parse', 'HEAD']).trim().slice(0, 12);
    const run = verifyWorkspace(port, config, ['readme']);
    assert.equal(run.ok, true, JSON.stringify(run.steps));
    assert.deepEqual(run.steps.map((s) => s.name), ['readme']);
    assert.equal(run.evidence, path.join(port, 'evidence', head));
    assert.match(fs.readFileSync(path.join(run.evidence, 'readme.log'), 'utf8'), /transit/);
    assert.throws(() => verifyWorkspace(port, config, ['nope']), /no verify step nope/);
  });

  it('refuses to package changes that are not committed', () => {
    fs.writeFileSync(path.join(ws, 'stray.txt'), 'made by a verification\n');
    try {
      assert.throws(() => pack('dirty'), /not committed \(stray\.txt\)/);
      assert.match(verifyWorkspace(port, config, ['readme']).evidence, /-dirty$/);
    } finally {
      fs.rmSync(path.join(ws, 'stray.txt'));
    }
  });

  it('packages the branch, which a sandbox verifies from a fresh clone; the game never changes', () => {
    commit('notes.txt', 'stops and lines\n', 'Add the transit notes');
    const manifest = pack('package');
    assert.equal(manifest.patches.length, 2);
    assert.deepEqual(manifest.author, author);
    assert.deepEqual(manifest.source.files.map((f) => f.path), ['src/feature.ts']);
    const result = runSandbox({ packageDir: path.join(out, 'package'), config, sandboxDir: path.join(out, 'sandbox') });
    assert.equal(result.ok, true, JSON.stringify(result.steps));
    assert.equal(git(result.sandbox, ['show', 'convert/transit:notes.txt']), 'stops and lines\n');
    assert.equal(fingerprint(game), gameBefore, 'the game is exactly as it was');
  });

  it('applies a package that commits a file the base verification writes, recording what it left', () => {
    // As a Godot import writes a .uid the game lacks: the port commits it, and the base's verification writes it too.
    const writes: SandboxConfig = { ...config, verify: [...config.verify, { name: 'import', timeoutSeconds: STEP_TIMEOUT_S, run: node("require('fs').writeFileSync('made.uid', 'uid')") }] };
    commit('made.uid', 'uid', 'Commit the UID the game lacks');
    pack('package-made');
    const result = runSandbox({ packageDir: path.join(out, 'package-made'), config: writes, sandboxDir: path.join(out, 'sandbox-made') });
    assert.equal(result.ok, true, JSON.stringify(result.steps));
    assert.match(fs.readFileSync(path.join(out, 'package-made', 'evidence', 'before', 'leftovers.log'), 'utf8'), /\?\? made\.uid/);
    assert.equal(fingerprint(game), gameBefore);
  });
});
