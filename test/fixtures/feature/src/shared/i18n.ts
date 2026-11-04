// Fixture: the texts of the interface, a shared dictionary looked up by key.
export const EN: Record<string, string> = {
  'app.title': 'Harbour',
  'ferry.hint': 'Board here',
  'ferry.mode.fast': 'Fast',
  'ferry.mode.slow': 'Slow',
  'ferry.mode.other': 'Other',
  'common.ok': 'OK',
  'ferry.trip.one': 'one trip',
  'ferry.trip.other': 'trips',
  'ferry.board': 'Boarding',
  'ferry.harbour': 'Harbour',
};

/** Not looked up by anyone: a key equal to one the feature looks up elsewhere is not its entry. */
export const LEGACY: Record<string, string> = {
  'ferry.hint': 'old',
};

export function t(key: string): string {
  return EN[key] ?? key;
}

/** A lookup through another: the key is made from the parameter. */
export function plural(key: string, n: number): string {
  return t(n === 1 ? `${key}.one` : `${key}.other`);
}
