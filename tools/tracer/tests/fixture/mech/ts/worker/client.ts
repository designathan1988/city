// A worker made from a file's path: messages to it reach the file's handler, its answers the client's.
export class Client {
  worker: Worker | null = null;
  start(): void {
    const w = new Worker(new URL("./job.ts", import.meta.url));
    this.worker = w;
    w.onmessage = (e) => this.receive(e.data);
  }
  send(m: unknown): void { this.worker?.postMessage(m); }
  receive(d: unknown): void {}
}
