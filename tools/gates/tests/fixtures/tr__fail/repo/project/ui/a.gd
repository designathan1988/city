# Labels.
extends Label
func f(n):
	text = "Hello"
	$L.text = tr("A") + str(n)
	$B.tooltip_text = name
	$O.add_item("Raw")
	$P.set_item_text(0, "x " + tr("B"))
	$L.text = "%s" % [name]
