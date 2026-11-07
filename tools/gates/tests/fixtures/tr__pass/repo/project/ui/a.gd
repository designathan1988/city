# Labels.
extends Label
func f(n):
	text = tr("A")
	$L.text = tr("A") % n
	$L.text = ""
	$L.text = tr("A") if n else tr("B")
	$O.add_item(tr("K"))
	$P.set_item_text(0, tr("B"))
	$L.text = "%d %%" % n
	# text = "Hello"
	$L.tooltip_text = TranslationServer.translate("T")
	$L.text = "%s: %s" % [tr("A"), tr("B")]
	$L.text = "%d" % [int(n)]
