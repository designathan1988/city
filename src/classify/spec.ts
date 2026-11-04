// The inputs of a classification: which source files a feature owns and which target subsystem will own it, and the
// mapping catalog that names, for each source concept a feature depends on, the target unit that already owns it.
import fs from 'node:fs';

export interface FeatureSpec {
  /** Short name, also the package's (`convert/<feature>`). */
  readonly feature: string;
  /** The target folder that will own the feature (e.g. `native/src/sim/transit`). */
  readonly subsystem: string;
  /** Source files the feature owns, relative to the source root. */
  readonly files: readonly string[];
}

/** A source concept (a unit id, or an external key) and the target unit that owns it. */
export interface ConceptMapping {
  readonly source: string;
  readonly target: string;
  /** Why the two are the same concept, in words: what each holds or does. */
  readonly reason: string;
}

export interface MappingCatalog {
  readonly concepts: readonly ConceptMapping[];
}

function readJson<T>(file: string): T {
  return JSON.parse(fs.readFileSync(file, 'utf8')) as T;
}

export function readFeatureSpec(file: string): FeatureSpec {
  const spec = readJson<FeatureSpec>(file);
  if (!spec.feature || !/^[a-z][a-z0-9_-]*$/.test(spec.feature)) throw new Error(`${file}: "feature" must be a short lower-case name`);
  if (!spec.subsystem) throw new Error(`${file}: "subsystem" must name the target folder that will own the feature`);
  if (!Array.isArray(spec.files) || spec.files.length === 0) throw new Error(`${file}: "files" must list the source files the feature owns`);
  return spec;
}

export function readMappingCatalog(file: string): MappingCatalog {
  const catalog = readJson<MappingCatalog>(file);
  if (!Array.isArray(catalog.concepts)) throw new Error(`${file}: "concepts" must be a list`);
  for (const c of catalog.concepts) {
    if (!c.source || !c.target || !c.reason) throw new Error(`${file}: every concept needs source, target and reason`);
  }
  return catalog;
}
