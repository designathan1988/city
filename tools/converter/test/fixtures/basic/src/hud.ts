import type { World } from './world';

export function showTotal(world: World, label: HTMLElement): void {
  label.textContent = String(world.total);
}

export function resetFromHud(world: World, label: HTMLElement): void {
  world.total = 0;
  label.textContent = '0';
}
