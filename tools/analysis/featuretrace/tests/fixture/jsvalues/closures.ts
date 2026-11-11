// Closures and the enclosing function's variables (J7): a value a closure writes is its own (evaluated where it is
// written); a closure sees a name its enclosing function declares after it.
import { Box } from "./types";

export function collects(): Box[] {
	const all: Box[] = [];
	const add = (shown: Box) => { all[all.length] = shown; };
	add(new Box());
	return all;
}

export function laterDeclared(): void {
	const run = () => helper().open();
	const helper = () => new Box();
	run();
}
