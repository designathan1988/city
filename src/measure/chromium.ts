// Starts a headless Chromium with its DevTools endpoint on a free port and a throw-away profile, and stops it.
import { spawn, type ChildProcess } from 'node:child_process';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';

const STARTUP_TIMEOUT_MS = 30_000;

export interface Chromium {
  readonly endpoint: string;
  stop(): void;
}

export async function startChromium(executable: string, args: readonly string[], width: number, height: number): Promise<Chromium> {
  if (!fs.existsSync(executable)) throw new Error(`no Chromium at ${executable}`);
  const profile = fs.mkdtempSync(path.join(os.tmpdir(), 'converter-chromium-'));
  const child: ChildProcess = spawn(executable, [
    '--remote-debugging-port=0', `--user-data-dir=${profile}`, '--no-first-run', '--no-default-browser-check',
    `--window-size=${width},${height}`, ...args, 'about:blank',
  ], { stdio: ['ignore', 'ignore', 'pipe'] });
  const stop = (): void => {
    if (child.exitCode === null) child.kill();
    fs.rmSync(profile, { recursive: true, force: true, maxRetries: 5, retryDelay: 200 });
  };
  try {
    const endpoint = await new Promise<string>((resolve, reject) => {
      let output = '';
      const timer = setTimeout(() => reject(new Error(`Chromium gave no DevTools endpoint in ${STARTUP_TIMEOUT_MS} ms:\n${output}`)), STARTUP_TIMEOUT_MS);
      child.stderr?.on('data', (chunk: Buffer) => {
        output += chunk.toString();
        const match = /DevTools listening on (ws:\/\/\S+)/.exec(output);
        if (match) {
          clearTimeout(timer);
          resolve(match[1]!);
        }
      });
      child.on('exit', (code) => {
        clearTimeout(timer);
        reject(new Error(`Chromium exited with ${code} before listening:\n${output}`));
      });
    });
    return { endpoint, stop };
  } catch (error) {
    stop();
    throw error;
  }
}
