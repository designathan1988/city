// A module using the global scope, and a class whose method writes an inner function (L1, L2).
export function use(): number {
	return shown() + x();
}

export class K {
	m(): () => number {
		function helper(): number { return 3; }
		return helper;
	}
}

export function viaMember(k: K): number {
	return k.helper();
}

export function make(): number {
	return viaMember(new K());
}
