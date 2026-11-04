// Fixture: the program's root (the page's script). It plugs the feature in, keeps its values in state of its own and
// uses shared code of its own.
import { Ferry, type FerryHost } from './feature/ferry';
import { clamp } from './shared/math';
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
