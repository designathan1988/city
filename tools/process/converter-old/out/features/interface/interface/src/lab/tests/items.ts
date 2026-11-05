// What the feature uses of src/lab/tests/items.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/lab/tests/items.ts:7-16 interface SweepItem
/** WHAT THE A/B SWEEP CAN FLIP: every switch of the effects panel (`effectsWiring.ts` snapshot, set with `setMany`) and every optimisation
 *  the optimisations' area exposes (`window.__labOpt`, when there). Each item knows its value now, the value it is flipped to and how to set it. */

export interface SweepItem {
  /** `switch.<key>` or `opt.<id>`. */
  readonly id: string;
  readonly label: string;
  readonly group: string;
  get(): unknown;
  /** The value the B window measures, from the value now (a boolean reversed; a choice the next one); null: not flippable. */
  flipped(now: unknown): unknown;
  set(value: unknown): void;
}

// src/lab/tests/items.ts:33-36 variable SWITCH_GROUPS
/** The groups of the switches, as the panel shows them (`effectsWiring.ts` GROUPS). */
const SWITCH_GROUPS: readonly (readonly [string, readonly string[]])[] = [
  ['post', POST_SWITCHES], ['light', LIGHT_SWITCHES], ['layers', LAYER_SWITCHES], ['buildings', BUILDING_SWITCHES],
  ['cpu', ['simulation', 'traffic', 'pedestrians', 'growth', 'onDemand']],
];

// src/lab/tests/items.ts:44-44 variable NOT_SWEPT
/** Left out: they change what is measured, not what it costs (the loop on demand: a frame every refresh is the measure itself). */
const NOT_SWEPT: readonly string[] = ['onDemand', 'simulation'];

// src/lab/tests/items.ts:52-102 function sweepItems
export function sweepItems(ctx: LabContext): SweepItem[];
