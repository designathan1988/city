// What the feature uses of src/peopleLab/widgets.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/peopleLab/widgets.ts:8-15 function el
/** THE LAB'S CONTROLS: a slider, colour swatches, a segmented choice, a grid of items with their thumbnails */

export const el = <K extends keyof HTMLElementTagNameMap>(tag: K, props: Partial<HTMLElementTagNameMap[K]> & { cls?: string } = {}, ...kids: (Node | string)[]): HTMLElementTagNameMap[K] =>;

// src/peopleLab/widgets.ts:18-93 function widgets
/** The controls, redrawing the panel (`render`) after a pick. */
export function widgets(render: () => void);
