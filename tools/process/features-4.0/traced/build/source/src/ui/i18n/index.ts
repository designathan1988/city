// The code of src/ui/i18n/index.ts that is the feature's (the rest of the file is shared).

// src/ui/i18n/index.ts:90-93 function parseDecimal: the feature's own
export function parseDecimal(text: string): number {
  const s = text.trim().replace(',', '.');
  return s === '' || !/^[-+]?(\d+\.?\d*|\.\d+)$/.test(s) ? Number.NaN : Number(s);
}
