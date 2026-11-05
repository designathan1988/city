// What the feature uses of src/ui/materialSwatch.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/ui/materialSwatch.ts:5-5 variable BASE
/** A tile's picture for a finish: the surface itself, drawn rather than lettered. */

const BASE = Object.fromEntries(FINISHES.map((f) => [f, `#${FINISH_COLOUR[f].toString(16).padStart(6, '0')}`])) as Readonly<Record<Finish, string>>;

// src/ui/materialSwatch.ts:8-15 function shade
/** A darker shade of a hex colour, for mortar, seams and shadow. */
function shade(hex: string, amount: number): string;

// src/ui/materialSwatch.ts:17-192 function draw
function draw(ctx: CanvasRenderingContext2D, finish: Finish, size: number): void;

// src/ui/materialSwatch.ts:195-195 variable CACHE
/** Drawn once per finish: the tray asks for these on every redraw. */
const CACHE = new Map<string, string>();

// src/ui/materialSwatch.ts:197-211 function materialSwatch
export function materialSwatch(finish: Finish, size = 56, dpr = 2): string;
