// A minimal Chrome DevTools Protocol client over one browser WebSocket in flat session mode: commands carry the
// target's sessionId, events arrive with it (Target.setAutoAttach / attachToTarget with flatten: true).

interface Pending {
  readonly resolve: (result: Record<string, unknown>) => void;
  readonly reject: (error: Error) => void;
  readonly method: string;
}

export type EventHandler = (params: Record<string, unknown>, sessionId: string | undefined) => void;

export class CdpClient {
  readonly #socket: WebSocket;
  readonly #pending = new Map<number, Pending>();
  readonly #handlers = new Map<string, EventHandler[]>();
  #sequence = 0;

  private constructor(socket: WebSocket) {
    this.#socket = socket;
    socket.addEventListener('message', (event) => this.#receive(String(event.data)));
    socket.addEventListener('close', () => {
      for (const pending of this.#pending.values()) pending.reject(new Error(`connection closed during ${pending.method}`));
      this.#pending.clear();
    });
  }

  static async connect(url: string): Promise<CdpClient> {
    const socket = new WebSocket(url);
    await new Promise<void>((resolve, reject) => {
      socket.addEventListener('open', () => resolve(), { once: true });
      socket.addEventListener('error', () => reject(new Error(`cannot connect to ${url}`)), { once: true });
    });
    return new CdpClient(socket);
  }

  send(method: string, params: Record<string, unknown> = {}, sessionId?: string): Promise<Record<string, unknown>> {
    const id = ++this.#sequence;
    return new Promise((resolve, reject) => {
      this.#pending.set(id, { resolve, reject, method });
      this.#socket.send(JSON.stringify({ id, method, params, ...(sessionId ? { sessionId } : {}) }));
    });
  }

  on(event: string, handler: EventHandler): void {
    this.#handlers.set(event, [...(this.#handlers.get(event) ?? []), handler]);
  }

  close(): void {
    this.#socket.close();
  }

  #receive(text: string): void {
    const message = JSON.parse(text) as {
      id?: number; method?: string; params?: Record<string, unknown>; sessionId?: string;
      result?: Record<string, unknown>; error?: { message: string; code: number };
    };
    if (message.id !== undefined) {
      const pending = this.#pending.get(message.id);
      if (!pending) return;
      this.#pending.delete(message.id);
      if (message.error) pending.reject(new Error(`${pending.method}: ${message.error.message} (${message.error.code})`));
      else pending.resolve(message.result ?? {});
      return;
    }
    if (message.method) for (const handler of this.#handlers.get(message.method) ?? []) handler(message.params ?? {}, message.sessionId);
  }
}
