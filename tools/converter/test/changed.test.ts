// Tests of the audit of a port: only findings in lines the port added or changed since its base count.
import assert from 'node:assert/strict';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import { after, describe, it } from 'node:test';
import { changedLines, findingsInChanges } from '../src/audit/changed.ts';
import type { Finding } from '../src/audit/context.ts';
import { removeTree } from '../src/common/files.ts';
import { git } from '../src/common/git.ts';
import type { GraphIR } from '../src/graph/ir.ts';

const lines = (n: number, tag: string): string => Array.from({ length: n }, (_, i) => `${tag} ${i + 1}`).join('\n') + '\n';

describe('audit of a port', () => {
  const repo = fs.mkdtempSync(path.join(os.tmpdir(), 'converter-test-changed-'));
  after(() => removeTree(repo));
  git(repo, ['init', '--quiet', '-b', 'master']);
  fs.writeFileSync(path.join(repo, 'kept.cpp'), lines(10, 'kept'));
  fs.writeFileSync(path.join(repo, 'edited.cpp'), lines(12, 'old'));
  git(repo, ['add', '-A']);
  git(repo, ['-c', 'user.name=T', '-c', 'user.email=t@test.invalid', 'commit', '--quiet', '-m', 'base']);
  const base = git(repo, ['rev-parse', 'HEAD']).trim();
  // The port: line 5 of edited.cpp changed, line 10 removed, and a file of its own not committed yet.
  const edited = lines(12, 'old').split('\n');
  edited[4] = 'new 5';
  edited.splice(9, 1);
  fs.writeFileSync(path.join(repo, 'edited.cpp'), edited.join('\n'));
  fs.writeFileSync(path.join(repo, 'added.cpp'), lines(3, 'added'));
  const unit = (id: string, file: string, from: number, to: number): unknown => ({ id, file, start: { line: from, column: 1 }, end: { line: to, column: 1 } });
  const ir = { units: [unit('kept', 'kept.cpp', 1, 10), unit('before', 'edited.cpp', 1, 3), unit('around', 'edited.cpp', 4, 6),
    unit('removal', 'edited.cpp', 8, 11), unit('added', 'added.cpp', 1, 3)] } as unknown as GraphIR;
  const finding = (u: string | null, file: string, line: number): Finding => ({ rule: 'r', severity: 'high', unit: u, file, line, message: u ?? file });

  it('takes the lines from the diff against the base and untracked files whole', () => {
    const changed = changedLines(repo, base);
    // A removal is reported at the line before it (git: `@@ -10 +9,0 @@`).
    assert.deepEqual(changed.get('edited.cpp'), [[5, 5], [9, 9]]);
    assert.deepEqual(changed.get('added.cpp'), [[1, Number.MAX_SAFE_INTEGER]]);
    assert.equal(changed.has('kept.cpp'), false);
  });

  it('keeps the findings whose unit or line touches what the port changed', () => {
    const kept = findingsInChanges([
      finding('kept', 'kept.cpp', 1), finding('before', 'edited.cpp', 1), finding('around', 'edited.cpp', 4),
      finding('removal', 'edited.cpp', 8), finding('added', 'added.cpp', 1), finding(null, 'edited.cpp', 5), finding(null, 'edited.cpp', 7),
    ], ir, changedLines(repo, base));
    assert.deepEqual(kept.map((f) => `${f.unit ?? 'line'}:${f.line}`), ['around:4', 'removal:8', 'added:1', 'line:5']);
  });
});
