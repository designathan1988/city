// What the feature uses of src/ui/i18n/index.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/ui/i18n/index.ts:6-6 type LanguageCode
/** Interface translation. */

export type LanguageCode = 'en' | 'pt-BR';

// src/ui/i18n/index.ts:8-12 interface LanguageSpec
export interface LanguageSpec {
  readonly code: LanguageCode;
  /** The language's own name, as its speakers write it. */
  readonly label: string;
}

// src/ui/i18n/index.ts:14-17 variable LANGUAGES
export const LANGUAGES: readonly LanguageSpec[] = [
  { code: 'en', label: 'English' },
  { code: 'pt-BR', label: 'Português' },
];

// src/ui/i18n/index.ts:50-52 function language
export function language(): LanguageCode;

// src/ui/i18n/index.ts:55-64 function t
/** Looks up a key, substituting `{name}` placeholders from `params`. */
export function t(key: string, params?: Readonly<Record<string, string | number>>): string;

// src/ui/i18n/index.ts:95-106 function setLanguage
export function setLanguage(next: LanguageCode): void;

// src/ui/i18n/index.ts:108-111 function onLanguageChange
export function onLanguageChange(listener: (language: LanguageCode) => void): () => void;
