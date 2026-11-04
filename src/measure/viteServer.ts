// Starts the source project's own Vite dev server (its version, its vite.config) through Vite's JavaScript API.
import fs from 'node:fs';
import path from 'node:path';
import { createRequire } from 'node:module';
import { pathToFileURL } from 'node:url';

export interface DevServer {
  readonly origin: string;
  close(): Promise<void>;
}

interface ViteModule {
  createServer(config: Record<string, unknown>): Promise<{
    listen(): Promise<unknown>;
    close(): Promise<void>;
    resolvedUrls: { local: string[] } | null;
  }>;
}

/** Vite is ESM-only: find its package from the source root and import the entry its `exports` names for `import`. */
async function importVite(sourceRoot: string): Promise<ViteModule> {
  const manifest = createRequire(path.join(sourceRoot, 'package.json')).resolve('vite/package.json');
  const pkg = JSON.parse(fs.readFileSync(manifest, 'utf8')) as { exports?: Record<string, unknown>; module?: string; main?: string };
  const root = pkg.exports?.['.'];
  const entry = typeof root === 'string' ? root
    : (root as { import?: string | { default?: string }; default?: string } | undefined)?.import ?? (root as { default?: string } | undefined)?.default ?? pkg.module ?? pkg.main;
  const file = typeof entry === 'string' ? entry : entry?.default;
  if (!file) throw new Error(`cannot find Vite's import entry in ${manifest}`);
  return (await import(pathToFileURL(path.join(path.dirname(manifest), file)).href)) as ViteModule;
}

export async function startDevServer(sourceRoot: string): Promise<DevServer> {
  const vite = await importVite(sourceRoot);
  const server = await vite.createServer({ root: sourceRoot, mode: 'development', server: { host: '127.0.0.1', port: 0, strictPort: false }, logLevel: 'warn' });
  await server.listen();
  const url = server.resolvedUrls?.local[0];
  if (!url) {
    await server.close();
    throw new Error('the dev server is not listening on any local URL');
  }
  return { origin: new URL(url).origin, close: () => server.close() };
}
