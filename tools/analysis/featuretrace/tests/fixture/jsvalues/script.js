// A classic script: its top-level declarations are global; a function written inside a function is its local (L2).
function shown() { return 1; }

function outer() {
	function x() { return 2; }
	return x;
}
