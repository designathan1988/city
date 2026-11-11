# Values through `is` tests (log/trace-rules.md, "Narrowing at branch conditions": G2, R1).
extends RefCounted

class Crate:
	func open() -> void:
		pass

class Plank:
	func lay() -> void:
		pass

func gd_branches(x) -> void:
	if x is Crate:
		x.open()
	else:
		x.lay()

func gd_else_value(x):
	if x is Crate:
		pass
	else:
		return x

func gd_early_value(x):
	if not (x is Crate):
		return null
	return x

func gd_typed(x: Node):
	if x is Crate:
		return x
	return null

func gd_calls() -> void:
	gd_branches(Crate.new())
	gd_branches(Plank.new())
	gd_else_value(Crate.new())
	gd_else_value(Plank.new())
	gd_early_value(Crate.new())
	gd_early_value(Plank.new())
