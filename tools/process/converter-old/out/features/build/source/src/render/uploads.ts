// The code of src/render/uploads.ts that is the feature's (the rest of the file is shared).

// src/render/uploads.ts:418-420 function warmFull: the feature's own
export function warmFull(): boolean {
  return warmer && queuedBytes + reservedBytes > WARM_CEILING;
}

// src/render/uploads.ts:424-433 function reserveWarm: the feature's own
export function reserveWarm(bytes: number): () => void {
  const held = Math.max(0, bytes);
  reservedBytes += held;
  let released = false;
  return () => {
    if (released) return;
    released = true;
    reservedBytes -= held;
  };
}
