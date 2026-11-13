const scope = self as unknown as { onmessage: ((e: MessageEvent) => void) | null; postMessage(m: unknown): void };
scope.onmessage = (event) => { answer(event.data); };
function answer(d: unknown): void { scope.postMessage(d); }
