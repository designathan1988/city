// What the feature uses of src/ui/i18n/index.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/ui/i18n/index.ts:6-6 type LanguageCode
/** Interface translation. */

export type LanguageCode = 'en' | 'pt-BR';

// src/ui/i18n/index.ts:50-52 function language
export function language(): LanguageCode;

// src/ui/i18n/index.ts:55-64 function t
/** Looks up a key, substituting `{name}` placeholders from `params`. */
export function t(key: string, params?: Readonly<Record<string, string | number>>): string;

// src/ui/i18n/index.ts:68-70 function setGlobalParams
/** Placeholders any sentence may use without its caller passing them: the road tool's rebindable keys */
export function setGlobalParams(provider: () => Readonly<Record<string, string | number>>): void;

// src/ui/i18n/index.ts:73-75 function plural
/** Picks the singular or plural key by `count`, and passes it as `{count}`. */
export function plural(key: string, count: number): string;

// src/ui/i18n/index.ts:79-87 function formatDecimal
/** A measure with `digits` decimals, in the interface language's notation ("22,0" in Portuguese, "22.0" in English). */
export function formatDecimal(value: number, digits: number, lang: LanguageCode = current): string;

// src/ui/i18n/index.ts:114-133 function applyTranslations
/** Rewrites every element in `root` that declares a translation key. */
export function applyTranslations(root: ParentNode = document): void;
