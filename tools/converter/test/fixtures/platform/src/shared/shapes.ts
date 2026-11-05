// Shared code typed by an interface the program's classes satisfy by their members (no `implements`).
export interface Shape { readonly area: number; grow(): void }
export function measure(s: Shape): number { s.grow(); return s.area; }
