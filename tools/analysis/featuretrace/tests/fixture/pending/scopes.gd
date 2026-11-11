# Block scope in GDScript: two locals of one name in sibling blocks.
extends RefCounted

class Box:
	func open() -> void:
		pass

class Can:
	func open() -> void:
		pass

func both(flag: bool) -> void:
	if flag:
		var it = Box.new()
		it.open()
	else:
		var it = Can.new()
		it.open()
