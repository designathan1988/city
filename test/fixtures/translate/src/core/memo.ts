// A value computed once per object (the target rebuilds it with every edit).
export function identityMemo<K extends object, V>(compute: (key: K) => V): (key: K) => V {
  const known = new WeakMap<K, V>();
  return (key) => {
    const kept = known.get(key);
    if (kept !== undefined) return kept;
    const value = compute(key);
    known.set(key, value);
    return value;
  };
}
