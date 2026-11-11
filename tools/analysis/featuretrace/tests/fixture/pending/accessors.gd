# A property read through its getter and written through its setter (log/trace-rules.md, G8, G8').
extends RefCounted

class Crate:
	func open() -> void:
		pass

var _stored
var held:
	get:
		return Crate.new()
var sink:
	set(value):
		_stored = value
var own = Crate.new():
	get:
		return own

func gd_getter():
	return held

func gd_setter():
	sink = Crate.new()
	return sink

func gd_stored():
	return _stored

func gd_own():
	return own
