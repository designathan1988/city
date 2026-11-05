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
