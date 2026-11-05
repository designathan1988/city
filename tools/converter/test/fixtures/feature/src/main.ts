// Fixture: the program's root (the page's script). It plugs the feature in, keeps its values in state of its own and
// uses shared code of its own.
import { Ferry, type FerryHost } from './feature/ferry';
import { clamp, FULL, within } from './shared/math';
import { rows } from './shared/table';
import { t } from './shared/i18n';

const ferry = new Ferry();
ferry.sail(clamp(rows(), 0, 10));
const title = t('app.title');
const docked = ferry.at;
const doubled = docked * 2;

/** Shared code with the feature inside: its statements are the ones on the ferry and what depends on them. */
export function status(f: Ferry, n: number): string {
  let label = title;
  const extra = n * 3;
  if (f.at > 0) {
    label = f.hint();
  }
  const shown = label.toUpperCase();
  const ref = f;
  let total = 0;
  for (let i = 0; i < n; i++) {
    total += i;
    total += f.at;
  }
  return `${shown} ${extra} ${ref.at} ${total}`;
}

export const report = status(ferry, doubled);
export const frame = 'panel';
export const okText = t('common.ok');
/** A shared function given a value of the feature: what it returns is its own value, not the feature's. */
const boxed = clamp(docked, 0, 1);
export const level = boxed + 1;
/** The program's side of the interface: it looks the key up. */
const host: FerryHost = { say: (key) => void t(key) };
ferry.announce(host);
/** The program builds a key from a value its type calls a string: the values that reach it are what decide. */
interface Panel {
  readonly mode: string;
}
const panel: Panel = { mode: 'harbour' };
function modeOf(p: Panel): string {
  return p.mode;
}
export const panelText = t(`ferry.${modeOf(panel)}`);
/** A value nothing in the program writes comes from outside: any string, also through a copy of it. */
declare let outside: string;
const copied = outside;
export const fromOutside = `ferry.${copied}`;
/** The program keeps its state in a store: `set` writes the value of a key, at each call its own. */
class Store<V extends object> {
  private readonly current: V;
  constructor(initial: V) {
    this.current = { ...initial };
  }
  get values(): Readonly<V> {
    return this.current;
  }
  set<K extends keyof V & string>(key: K, value: V[K]): void {
    this.current[key] = value;
  }
}
declare const typed: string;
const store = new Store({ mode: 'harbour' as string, label: 'none' as string });
store.set('label', typed);
store.set('mode', 'dock');
/** Another object with a property of the same name, of any value: not the store's. */
export const other = { mode: typed };
export const storeKey = `ferry.${store.values.mode}`;
export const inRange = within(3, FULL);
/** A figure of this file alone, read by a slice: the feature uses it, though no declaration of its own names it. */
const PACE = 3;
function paced(f: Ferry): number {
  return f.at * PACE;
}
export const pace = paced(ferry);
