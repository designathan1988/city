// Fixture: code nothing runs (no page loads it, nothing calls it).
import { dockNear } from './only/docks';

export function unused(): number {
  return dockNear(1);
}
