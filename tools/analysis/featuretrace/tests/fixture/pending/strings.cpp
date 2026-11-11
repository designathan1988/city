// A string literal is a value of the language (log/trace-rules.md, T5): an element of it is a char, never a value
// read from the text it carries.
char cpp_text_index() {
	const char *s = "a <common> b";
	return s[1];
}

auto cpp_text_element() {
	return "a <common> b"[1];
}
