// A minimal Language Server Protocol client over a child process's stdio (Content-Length framed JSON-RPC), enough to
// drive clangd: requests, notifications, server-to-client requests answered with null, and progress notifications.
import { spawn, type ChildProcess } from 'node:child_process';

type Handler = (params: Record<string, unknown>) => void;

/** How long a server may take to leave after `exit` before it is killed. */
const SHUTDOWN_GRACE_MS = 10_000;

export class LspClient {
  readonly #child: ChildProcess;
  readonly #pending = new Map<number, { resolve: (value: unknown) => void; reject: (error: Error) => void; method: string }>();
  readonly #handlers = new Map<string, Handler[]>();
  #buffer = Buffer.alloc(0);
  #sequence = 0;
  #stderr = '';

  constructor(executable: string, args: readonly string[]) {
    this.#child = spawn(executable, args, { stdio: ['pipe', 'pipe', 'pipe'] });
    this.#child.stdout!.on('data', (chunk: Buffer) => this.#receive(chunk));
    // A server that died leaves a broken pipe: its pending requests are rejected by the exit handler below.
    this.#child.stdin!.on('error', () => undefined);
    this.#child.stderr!.on('data', (chunk: Buffer) => { this.#stderr = (this.#stderr + chunk.toString()).slice(-20_000); });
    this.#child.on('exit', (code) => {
      for (const p of this.#pending.values()) p.reject(new Error(`${executable} exited (${code}) during ${p.method}:\n${this.#stderr}`));
      this.#pending.clear();
    });
  }

  request<T>(method: string, params: unknown): Promise<T> {
    const id = ++this.#sequence;
    return new Promise<T>((resolve, reject) => {
      this.#pending.set(id, { resolve: resolve as (value: unknown) => void, reject, method });
      this.#send({ jsonrpc: '2.0', id, method, params });
    });
  }

  notify(method: string, params: unknown): void {
    this.#send({ jsonrpc: '2.0', method, params });
  }

  on(method: string, handler: Handler): void {
    this.#handlers.set(method, [...(this.#handlers.get(method) ?? []), handler]);
  }

  /** The protocol's end: `shutdown`, then `exit`, then the server leaves by itself (killed only if it does not). */
  async shutdown(): Promise<void> {
    if (this.#child.exitCode !== null) return;
    const exited = new Promise<void>((resolve) => this.#child.once('exit', () => resolve()));
    await this.request('shutdown', null);
    this.notify('exit', null);
    this.#child.stdin!.end();
    const timer = setTimeout(() => this.#child.kill(), SHUTDOWN_GRACE_MS);
    await exited;
    clearTimeout(timer);
  }

  #send(message: unknown): void {
    const body = Buffer.from(JSON.stringify(message), 'utf8');
    this.#child.stdin!.write(`Content-Length: ${body.length}\r\n\r\n`);
    this.#child.stdin!.write(body);
  }

  #receive(chunk: Buffer): void {
    this.#buffer = Buffer.concat([this.#buffer, chunk]);
    for (;;) {
      const headerEnd = this.#buffer.indexOf('\r\n\r\n');
      if (headerEnd < 0) return;
      const match = /Content-Length: (\d+)/i.exec(this.#buffer.subarray(0, headerEnd).toString());
      if (!match) throw new Error('LSP message without Content-Length');
      const length = Number(match[1]);
      if (this.#buffer.length < headerEnd + 4 + length) return;
      const message = JSON.parse(this.#buffer.subarray(headerEnd + 4, headerEnd + 4 + length).toString('utf8')) as {
        id?: number; method?: string; params?: Record<string, unknown>; result?: unknown; error?: { message: string };
      };
      this.#buffer = this.#buffer.subarray(headerEnd + 4 + length);
      if (message.id !== undefined && message.method !== undefined) {
        this.#send({ jsonrpc: '2.0', id: message.id, result: null }); // a request from the server (progress tokens...)
      } else if (message.id !== undefined) {
        const pending = this.#pending.get(message.id);
        if (!pending) continue;
        this.#pending.delete(message.id);
        if (message.error) pending.reject(new Error(`${pending.method}: ${message.error.message}`));
        else pending.resolve(message.result);
      } else if (message.method !== undefined) {
        for (const handler of this.#handlers.get(message.method) ?? []) handler(message.params ?? {});
      }
    }
  }
}
