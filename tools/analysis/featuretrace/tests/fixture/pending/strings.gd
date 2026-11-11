# A string is a value of the engine (log/trace-rules.md, T5): its methods, its index and an operator on it give the
# engine's String, never a value read from the text it carries.
extends RefCounted

func gd_text_method():
	return "a <common> b".to_upper()

func gd_text_index():
	return "a <common> b"[1]

func gd_text_plus(x):
	return "pre" + x
