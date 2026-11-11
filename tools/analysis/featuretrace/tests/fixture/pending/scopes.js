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
