// Fixture: the feature's entry file.
import { clamp } from '../shared/math';
import { plural, t } from '../shared/i18n';
import { dockNear } from '../only/docks';
import { snap } from '../only/snap';
import routes from './routes.json';

export type FerryMode = 'fast' | 'slow';

/** What the program gives the ferry: it shows a text by its key (an interface: the call reaches its implementations). */
export interface FerryHost {
  say(key: string): void;
}

export class Ferry {
  at = 0;
  readonly css = 'ferry-row panel';
  sail(to: number): void {
    this.at = dockNear(clamp(snap(to + routes.north), 0, 100));
  }
  hint(): string {
    return t('ferry.hint');
  }
  label(mode: FerryMode): string {
    return t(`ferry.mode.${mode}`);
  }
  ok(): string {
    return t('common.ok');
  }
  trips(n: number): string {
    return plural('ferry.trip', n);
  }
  announce(host: FerryHost): void {
    host.say('ferry.board');
  }
}
