// The code of src/peopleLab/presets.ts that is the feature's (the rest of the file is shared).

// src/peopleLab/presets.ts:165-165 slice of src/peopleLab/presets.ts#BodyPreset (shared): the feature's statements; "…" marks code left out
export interface BodyPreset { readonly label: string; readonly body: Partial<MacroParams>; readonly features: Readonly<Record<string, number>> }

// src/peopleLab/presets.ts:183-188 slice of src/peopleLab/presets.ts#withBody (shared): the feature's statements; "…" marks code left out
export function withBody(body: MacroParams, features: Readonly<Record<string, number>>, preset: BodyPreset): { body: MacroParams; features: Record<string, number> } {
  // … 3 lines
  return { body: { ...body, ...preset.body }, features: { ...out, ...preset.features } };
}

// src/peopleLab/presets.ts:191-191 slice of src/peopleLab/presets.ts#LookPreset (shared): the feature's statements; "…" marks code left out
export interface LookPreset { readonly label: string; readonly female: Partial<PersonLook>; readonly male: Partial<PersonLook> }
