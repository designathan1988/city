// The measured profile: how often each unit ran per frame in a real run of the source, per variant (a quality tier,
// a scenario switch), on the hardware recorded with it.

export const PROFILE_VERSION = 1;

export interface UnitMeasure {
  readonly id: string;
  /** Calls of the unit's own function in the window. */
  readonly calls: number;
  readonly perFrame: number;
  /** Calls of closures inside the unit (callbacks it creates), counted apart. */
  readonly closureCalls: number;
  /** Context kinds it ran in: page, worker. */
  readonly contexts: string[];
}

export interface VariantProfile {
  readonly name: string;
  readonly setup: string;
  readonly frames: number;
  readonly seconds: number;
  readonly units: UnitMeasure[];
  readonly packages: { readonly name: string; readonly calls: number; readonly perFrame: number }[];
  readonly contexts: { readonly type: string; readonly url: string; readonly calls: number }[];
  readonly unmapped: { readonly functions: number; readonly calls: number };
}

export interface Profile {
  readonly version: number;
  readonly sourceRoot: string;
  readonly scenario: string;
  readonly frameUnit: string;
  readonly renderer: string;
  readonly chromiumArgs: readonly string[];
  readonly warmupSeconds: number;
  readonly windowSeconds: number;
  readonly graphUnits: number;
  readonly variants: VariantProfile[];
}
