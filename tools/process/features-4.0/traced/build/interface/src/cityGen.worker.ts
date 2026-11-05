// What the feature uses of src/cityGen.worker.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/cityGen.worker.ts:7-14 interface CityGenAsk
/** THE CITY MADE OFF THE PAGE (`main.ts` `cityData`): the game's own generator (`buildCityTable`) building a whole finite city in a job worker, so the page is not frozen for its seconds. */

export interface CityGenAsk {
  readonly id: number;
  /** The small city, or the same city made larger (`largeCity.ts`). */
  readonly kind: 'small' | 'large';
  readonly seed?: number;
  /** The large city over another radius, world units (the scale steps); absent, the large city's own. */
  readonly radius?: number;
}

// src/cityGen.worker.ts:17-17 type CityGenReply
/** `buildings`: the city's buildings as one table (`buildCityTable`, as `prepareCity` makes it), its `data` holding none: the page reads them from it (`preparedCity`). */
export type CityGenReply = { readonly id: number; readonly data?: SerializedDoc; readonly buildings?: ArrayBufferLike; readonly error?: string };

// src/cityGen.worker.ts:19-31 function serve
export function serve(scope: JobScope): void;
