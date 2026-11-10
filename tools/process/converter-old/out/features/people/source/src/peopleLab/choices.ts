// The code of src/peopleLab/choices.ts that is the feature's (the rest of the file is shared).

// src/peopleLab/choices.ts:11-11 slice of src/peopleLab/choices.ts#CLOTH (shared): the feature's statements; "…" marks code left out
export const CLOTH = [...CLOTH_COLOURS, 0xe05a8a, 0x2f9d8f, 0x3b6fd1, 0xffd166, 0x9b5de5, 0x00bbf9];

// src/peopleLab/choices.ts:15-15 slice of src/peopleLab/choices.ts#ROADCRAFT (shared): the feature's statements; "…" marks code left out
export const ROADCRAFT: CartoonStyle = { head: 1.12, eyes: 1, legs: 1, hands: 1, feet: 0.95 };

// src/peopleLab/choices.ts:17-23 slice of src/peopleLab/choices.ts#STYLE_PRESETS (shared): the feature's statements; "…" marks code left out
export const STYLE_PRESETS: readonly (readonly [string, CartoonStyle])[] = [
  ['Roadcraft', ROADCRAFT],
  ['Realista', REALISTIC],
  ['Desenho', { head: 1.2, eyes: 1.22, legs: 0.93, hands: 1.08, feet: 1.06 }],
  ['Pixar', PIXAR],
  ['Exagerado', { head: 1.65, eyes: 1.7, legs: 0.78, hands: 1.22, feet: 1.18 }],
];

// src/peopleLab/choices.ts:51-56 slice of src/peopleLab/choices.ts#READY_POSES (shared): the feature's statements; "…" marks code left out
export const READY_POSES: readonly (readonly [string, LibraryClipName, number])[] = [
  ['Em pé', 'idle', 0.5], ['Tchau', 'wave', 0.9], ['Vitória', 'cheer', 1.0], ['Dança', 'dance', 1.2],
  ['Celular', 'phone', 1.5], ['Foto', 'photo', 1.4], ['Leitura', 'read', 1.5], ['Brinde', 'drink', 1.1],
  ['Gargalhada', 'laugh', 0.8], ['Bronca', 'angry', 1.0], ['Discussão', 'argue', 1.2], ['Pensando', 'look', 1.6],
  ['Sentada', 'sitIdle', 0.5], ['Agachada', 'crouchIdle', 0.5], ['Corrida', 'run', 0.3], ['Passo', 'walkStroll', 0.4],
];

// src/peopleLab/choices.ts:59-68 slice of src/peopleLab/choices.ts#MOODS (shared): the feature's statements; "…" marks code left out
export const MOODS: readonly (readonly [string, string | undefined, PoseName, LibraryClipName])[] = [
  ['Parado', undefined, 'idle', 'idle'], ['Conversando', 'talk', 'idle', 'talk'], ['Ouvindo', 'listen', 'idle', 'listen'],
  ['Rindo', 'laugh', 'idle', 'laugh'], ['Acenando', 'wave', 'wave', 'wave'], ['Comemorando', 'cheer', 'cheer', 'cheer'],
  ['Dançando', 'dance', 'hips', 'dance'], ['Ao telefone', 'phone', 'idle', 'phone'], ['Fotografando', undefined, 'idle', 'photo'],
  ['Lendo', undefined, 'idle', 'read'], ['Bebendo', undefined, 'idle', 'drink'], ['Música', 'listen', 'idle', 'headphones'],
  ['Olhando', undefined, 'idle', 'look'], ['Nervoso', 'look', 'idle', 'nervous'], ['Bravo', 'angry', 'hips', 'angry'],
  ['Discutindo', 'argue', 'idle', 'argue'], ['Passeando', undefined, 'idle', 'walkStroll'], ['Andar estiloso', undefined, 'idle', 'walkCool'],
  ['Correndo', undefined, 'idle', 'run'], ['Sentado', 'talk', 'idle', 'sitIdle'], ['Agachado', undefined, 'idle', 'crouchIdle'],
  ['Chorando', 'cry', 'idle', 'idle'], ['Assustado', 'panic', 'idle', 'nervous'],
];
