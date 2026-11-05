// Shared code that replaces a global of the package: every later use of the global runs the replacement.
export function patchRenderer(): void {
  globalThis.makeRenderer = function patched(): number { return 2; };
}
export function useRenderer(): number { return makeRenderer(); }
// The platform's class replaced by a subclass (as `window.Worker = WatchedWorker`).
export function watchEngines(): void {
  const Native = globalThis.Engine;
  class Watched extends Native { override run(): void { super.run(); } }
  globalThis.Engine = Watched;
}
export function startEngine(): void { new Engine().run(); }
