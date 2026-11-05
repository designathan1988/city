// The code of src/editor/workerParts.ts that is the feature's (the rest of the file is shared).

// src/editor/workerParts.ts:19-19 slice of src/editor/workerParts.ts#backend (shared): the feature's statements; "…" marks code left out
let backend: Promise<PartStore> | null = null;

// src/editor/workerParts.ts:20-20 slice of src/editor/workerParts.ts#give (shared): the feature's statements; "…" marks code left out
let give: ((store: PartStore) => void) | null = null;

// src/editor/workerParts.ts:25-32 slice of src/editor/workerParts.ts#takeStoreSetup (shared): the feature's statements; "…" marks code left out
export function takeStoreSetup(setup: StoreSetup): void {
  if (setup.byPort && !backend) backend = new Promise<PartStore>((resolve) => { give = resolve; });
// … 5 lines
}

// src/editor/workerParts.ts:35-42 slice of src/editor/workerParts.ts#workerParts (shared): the feature's statements; "…" marks code left out
export function workerParts(): PartStore {
// … 6 lines
}

// src/editor/workerParts.ts:36-36 function workerParts.open: the feature's own
  const open = (): Promise<PartStore> => (backend ??= Promise.resolve(indexedDbParts(openSaves, SAVES_STORE)));

// src/editor/workerParts.ts:38-38 method workerParts.get: the feature's own
    get: async (keys) => (await open()).get(keys),

// src/editor/workerParts.ts:39-39 method workerParts.write: the feature's own
    write: async (put, remove, marks) => (await open()).write(put, remove, marks),

// src/editor/workerParts.ts:45-77 slice of src/editor/workerParts.ts#portParts (shared): the feature's statements; "…" marks code left out
export function portParts(port: MessagePort): PartStore & { readonly drop: () => void } {
// … 31 lines
}

// src/editor/workerParts.ts:53-57 function portParts.call: the feature's own
  const call = (request: object): Promise<Answer | null> => new Promise((resolve) => {
    const id = next++;
    waiting.set(id, resolve);
    try { port.postMessage({ id, ...request }); } catch (error) { waiting.delete(id); console.error('[save] the request did not reach the saves writer', error); resolve(null); }
  });

// src/editor/workerParts.ts:59-63 method portParts.get: the feature's own
    async get(keys) {
      const answer = await call({ op: 'get', keys });
      const found = answer?.ok && Array.isArray(answer.result) ? (answer.result as unknown[]) : [];
      return keys.map((_, i) => { const bytes = found[i]; return bytes instanceof Uint8Array ? new Blob([bytes as Uint8Array<ArrayBuffer>]) : null; });
    },

// src/editor/workerParts.ts:64-67 method portParts.write: the feature's own
    async write(put, remove, marks) {
      const bytes = await Promise.all(put.map(async ([key, blob]) => [key, new Uint8Array(await blob.arrayBuffer())] as const));
      return (await call({ op: 'write', put: [...bytes, ...markBytes(marks)], remove }))?.ok === true;
    },

// src/editor/workerParts.ts:80-81 function markBytes: the feature's own
export const markBytes = (marks: readonly (readonly [string, string])[] = []): (readonly [string, Uint8Array])[] =>
  marks.map(([key, text]) => [key, new TextEncoder().encode(text)] as const);

// src/editor/workerParts.ts:84-92 function markTexts: the feature's own
export function markTexts(answer: { readonly ok: boolean; readonly result?: unknown } | null): (readonly [string, string])[] {
  if (!answer?.ok || !Array.isArray(answer.result)) return [];
  const out: [string, string][] = [];
  for (const entry of answer.result as unknown[]) {
    if (!Array.isArray(entry) || typeof entry[0] !== 'string' || !(entry[1] instanceof Uint8Array)) continue;
    out.push([entry[0], new TextDecoder().decode(entry[1])]);
  }
  return out;
}
