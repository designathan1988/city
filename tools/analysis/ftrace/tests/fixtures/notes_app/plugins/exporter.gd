# Exports the note count to a text file on request.
extends RefCounted

func export_all(path: String) -> void:
	var total := NotesBridge.note_count()
	write_line(path, str(total))
