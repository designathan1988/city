// Fixture: the feature's entry file.
import { clamp } from '../shared/math';
import { dockNear } from '../only/docks';
import { snap } from '../only/snap';

export class Ferry {
  at = 0;
  sail(to: number): void {
    this.at = dockNear(clamp(snap(to), 0, 100));
  }
}
