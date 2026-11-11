## The owner of shown values in this fixture: its class is named by owners.json's shown_values concept.
class_name Formatters
extends RefCounted


static func count_label(base: String, count: int) -> String:
	return TranslationServer.translate(base).format({"count": count})
