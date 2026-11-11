// Block scope in JavaScript: two `const` of one name in sibling blocks; `var` is the function's.
class Box { open() {} }
class Can { open() {} }

export function both(flag) {
	if (flag) {
		const it = new Box();
		it.open();
	} else {
		const it = new Can();
		it.open();
	}
}

// A local of a name used twice, passed to a callback: the callback's parameter holds what that local holds.
export function each(rows) {
	for (const row of [new Box()]) row.open();
	for (const row of [new Can()]) {
		const pick = (r) => r.open();
		pick(row);
	}
}

// A `var` in a nested function is that function's, not the enclosing one's.
export function outer() {
	var it = new Box();
	const inner = () => {
		if (true) { var it = new Can(); }
		it.open();
	};
	inner();
	it.open();
}

// A function linked before its caller: its parameter has no value at first, then the caller's argument arrives.
function late(p) { return p.size; }
export function early() { late(new Box()); }
