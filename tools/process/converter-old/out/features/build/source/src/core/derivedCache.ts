// The code of src/core/derivedCache.ts that is the feature's (the rest of the file is shared).

// src/core/derivedCache.ts:107-126 function readDerivedAll: the feature's own
export async function readDerivedAll<T>(prefix: string): Promise<Map<string, T>> {
  const out = new Map<string, T>();
  const db = await database();
  if (!db) return out;
  return new Promise((resolve) => {
    try {
      const store = db.transaction(STORE, 'readonly').objectStore(STORE);
      const keys = store.getAllKeys(prefixRange(prefix));
      const values = store.getAll(prefixRange(prefix));
      values.onsuccess = () => {
        const list = keys.result ?? [];
        list.forEach((key, i) => out.set(String(key).slice(prefix.length), values.result[i] as T));
        resolve(out);
      };
      values.onerror = () => resolve(out);
    } catch {
      resolve(out);
    }
  });
}

// src/core/derivedCache.ts:162-173 function keepDerivedNow: the feature's own
export function keepDerivedNow(key: string, value: unknown): boolean {
  const db = opened;
  if (!db) { void database(); return false; }
  try {
    const tx = db.transaction(STORE, 'readwrite', RELAXED);
    tx.objectStore(STORE).put(value, key).onerror = (event) => event.preventDefault();
    return true;
  } catch (error) {
    console.error('[derived] not kept', error);
    return false;
  }
}
