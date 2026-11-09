// Fixture: a file the program loads; its top-level code runs on load and uses a helper the feature uses too.
import { snap } from '../only/snap';

const cache: number[] = [];
cache.push(snap(2.5));

export function rows(): number {
  return cache.length;
}
