import path from 'node:path';
// Limits and file operations every stage shares, owned once: how much output a child process may write back, how a
// folder tree is removed on Windows (files briefly held by an indexer or antivirus are retried), and content hashes.
import { createHash } from 'node:crypto';
import fs from 'node:fs';

/** Output a child process (git, a build, a test run) may return to the converter. */
export const MAX_CHILD_OUTPUT_BYTES = 256 * 1024 * 1024;

const REMOVE_RETRIES = 5;
const REMOVE_RETRY_DELAY_MS = 200;

export function removeTree(dir: string): void {
  fs.rmSync(dir, { recursive: true, force: true, maxRetries: REMOVE_RETRIES, retryDelay: REMOVE_RETRY_DELAY_MS });
}

export function sha256(file: string): string {
  return createHash('sha256').update(fs.readFileSync(file)).digest('hex');
}

export function writeTree(root: string, files: ReadonlyMap<string, string>): void {
  for (const [file, text] of files) {
    const target = path.join(root, file);
    fs.mkdirSync(path.dirname(target), { recursive: true });
    fs.writeFileSync(target, text, 'utf8');
  }
}
