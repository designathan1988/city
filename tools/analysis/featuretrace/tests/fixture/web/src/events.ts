// Events by type through the host: a bus that fires "ready".
export const bus = new EventTarget();
export function announce(): void {
	bus.dispatchEvent(new Event("ready"));
}
