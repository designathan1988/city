// The code of src/render/people/proceduralCook.ts that is the feature's (the rest of the file is shared).

// src/render/people/proceduralCook.ts:11-32 function cookedNames: the feature's own
function cookedNames(): Promise<ReadonlySet<string> | null> {
  manifest ??= (async () => {
    if (!COOK_HASH || typeof fetch === 'undefined') return null;
    if (typeof location !== 'undefined' && new URLSearchParams(location.search).has('nocook')) return null;
    try {
      const response = await fetch(`${import.meta.env.BASE_URL}cooked/procedural/manifest.json`, { cache: 'no-cache' });
      const m = response.ok ? (await response.json()) as { hash?: string; names?: string[] } : null;
      if (!m || m.hash !== COOK_HASH || !Array.isArray(m.names)) {
        console.warn(`The procedural people are not cooked for this build (fingerprint ${COOK_HASH}): their classes and hairstyles are built during play. Run "npm run cook:people".`);
        // Looked for again later: a cook run meanwhile is read without reloading the game.
        if (typeof setTimeout === 'function') setTimeout(() => { manifest = null; }, 20_000);
        return null;
      }
      return new Set(m.names);
    } catch (error) {
      // Unreadable - an HTML page served in its place, as when the path missed the build's base
      console.error(`The procedural people's manifest could not be read (${String(error)}): their classes and hairstyles are built during play.`);
      return null;
    }
  })();
  return manifest;
}

// src/render/people/proceduralCook.ts:35-44 function loadProcedural: the feature's own
export async function loadProcedural(name: string): Promise<Record<string, PackValue> | null> {
  const names = await cookedNames();
  if (!names?.has(name)) return null;
  try {
    const response = await fetch(`${import.meta.env.BASE_URL}cooked/procedural/${name}.bin`);
    return response.ok ? unpackRecord(await response.arrayBuffer()) : null;
  } catch {
    return null;
  }
}

// src/render/people/proceduralCook.ts:59-72 function clipOf: the feature's own
export function clipOf(prefix: string, r: Record<string, PackValue>): ClipFrames {
  const head = r[`${prefix}.head`] as number[] | null;
  const clip: ClipFrames = {
    data: r[`${prefix}.data`] as Float32Array, frames: r[`${prefix}.frames`] as number, duration: r[`${prefix}.duration`] as number,
    stride: r[`${prefix}.stride`] as number, pelvisY: r[`${prefix}.pelvisY`] as number, pelvisX: r[`${prefix}.pelvisX`] as number,
    pelvisZ: r[`${prefix}.pelvisZ`] as number, loop: r[`${prefix}.loop`] as boolean,
  };
  const travel = r[`${prefix}.travel`], yaw = r[`${prefix}.yaw`], hands = r[`${prefix}.hands`];
  if (travel instanceof Float32Array) clip.travel = travel;
  if (yaw instanceof Float32Array) clip.yaw = yaw;
  if (hands instanceof Float32Array) clip.hands = hands;
  if (head) clip.head = new Matrix4().fromArray(head);
  return clip;
}
