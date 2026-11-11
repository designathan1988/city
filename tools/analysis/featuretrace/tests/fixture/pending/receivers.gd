# `self` is the object a method runs on (log/trace-rules.md, J2'): an inherited method returns its receiver.
extends RefCounted

class Base2:
	func me():
		return self

	func again():
		return me()

class Leaf2 extends Base2:
	func leaf_only() -> void:
		pass

func gd_self_receiver():
	return Leaf2.new().me()

func gd_implicit_receiver():
	return Leaf2.new().again()
