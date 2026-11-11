// `new` evaluates its constructor (N1): a function called as a constructor, a class through a variable.
export class Box { open() {} }

function Maker(a) { return new Box(); }

export function mk() {
	const m = new Maker(1);
	m.open();
}

export function viaVariable() {
	const C = Box;
	new C().open();
}

// A generator's iteration gives what it yields (J5).
function* made() { yield new Box(); }

export function fromGenerator() {
	for (const b of made()) b.open();
}
