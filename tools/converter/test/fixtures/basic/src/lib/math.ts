export function clamp(value: number, low: number, high: number): number {
  return value < low ? low : value > high ? high : value;
}

export const SPEED_LIMIT = 13.9;

export let counter = 0;

export function bump(): void {
  counter++;
}
