## The sheet of plate pictures: the core asks for a cell the moment a chunk needs a plate (render/props/sign_atlas.h); here
## the cells waiting are painted, the words with the canvas's font, into a viewport the "prop" material reads (sign_atlas).
## Only new cells are drawn: the viewport keeps what it had and updates once per batch.
class_name SignSheet
extends SubViewport

var drawn: Array = [] ## Every cell painted so far: [type, words, rectangle in the sheet].

var _paper := _Paper.new()
var _look: Dictionary


func _init(look: Dictionary) -> void:
	_look = look
	size = Vector2i(int(look.sheet_w_px), int(look.sheet_h_px))
	transparent_bg = false
	disable_3d = true
	render_target_clear_mode = SubViewport.CLEAR_MODE_ONCE
	render_target_update_mode = SubViewport.UPDATE_DISABLED
	_paper.look = look
	add_child(_paper)


func _process(_delta: float) -> void:
	var cells: Array = CityStore.query(&"sign_cells")
	if cells.is_empty():
		return
	drawn.append_array(cells)
	_paper.begin(cells)
	render_target_clear_mode = SubViewport.CLEAR_MODE_NEVER if _paper.painted else SubViewport.CLEAR_MODE_ONCE
	_paper.painted = true
	render_target_update_mode = SubViewport.UPDATE_ONCE
	if not RenderingServer.frame_post_draw.is_connected(_paper.end):
		RenderingServer.frame_post_draw.connect(_paper.end, CONNECT_ONE_SHOT)


## Draws each plate as a real one reads, in the proportions of the plate (a cell is stretched over it by the mesh's uv): the
## shape's colours on its cell, cut by the plate's own outline in the mesh (street_signs.cpp outline, the same polygon).
class _Paper:
	extends Node2D

	var look: Dictionary
	var cells: Array = []
	var painted := false
	var font := FontVariation.new()
	var _origin := Vector2.ZERO # the cell being painted, and how a plate's proportions stretch over it
	var _stretch := 1.0
	var _face: FontFile # its own, for a batch only: the big glyphs of the shared font's cache would stay for good

	func begin(batch: Array) -> void:
		cells = batch
		if _face == null:
			_face = ThemeDB.fallback_font.duplicate()
			font.base_font = _face
			font.variation_embolden = look.word_bold
		queue_redraw()

	## The sheet holds the picture now: neither the font and its glyphs nor the drawing need stay.
	func end() -> void:
		cells = []
		font.base_font = null
		_face = null
		queue_redraw()

	func _draw() -> void:
		for c: Array in cells:
			_plate(c[0], c[1], c[2])
		draw_set_transform(Vector2.ZERO)

	func _plate(type: int, text: String, cell: Rect2) -> void:
		var p: Dictionary = look.drawings[type]
		var shape: Dictionary = look.plates[p.plate]
		var wide: float = cell.size.y * shape.w_m / shape.h_m
		_stretch = cell.size.x / wide
		_origin = cell.position
		draw_set_transform(_origin, 0.0, Vector2(_stretch, 1.0))
		var r := Rect2(0.0, 0.0, wide, cell.size.y)
		var radius := minf(r.size.x, r.size.y) * 0.5
		var mid := r.get_center()
		draw_rect(r, Color(p.get("rim", p.field)))
		if p.has("rim"):
			_field(shape, r, p.inner_share, Color(p.field))
		if p.has("bar"):
			var bar := Vector2(radius * p.bar[0], radius * p.bar[1])
			draw_rect(Rect2(mid - bar * 0.5, bar), Color(p.ink))
		if p.has("figure"):
			_figure(p.figure, mid, radius, Color(p.ink))
		var words: String = text if not text.is_empty() else p.get("words", "")
		if p.has("arrow"):
			var a: Array = p.arrow
			mid.x -= r.size.x * (1.0 - float(a[1])) * 0.5
			draw_colored_polygon(PackedVector2Array([
				Vector2(r.size.x * a[0], r.size.y * 0.5), Vector2(r.size.x * a[1], r.size.y * (0.5 - a[2])),
				Vector2(r.size.x * a[1], r.size.y * (0.5 + a[2]))]), Color(p.ink))
		if not words.is_empty():
			_words(words, p, r, mid)
		if p.has("slash"):
			var reach: Vector2 = Vector2.ONE * radius * p.slash_reach
			draw_line(mid - reach, mid + reach, Color(p.slash), radius * p.slash_width)

	## The plate's field inside its rim: the outline of the mesh brought in to `share` round the plate's centre; a
	## rectangle by the same margin on its four sides.
	func _field(shape: Dictionary, r: Rect2, share: float, colour: Color) -> void:
		if int(shape.outline_points) == 0:
			draw_rect(r.grow(-minf(r.size.x, r.size.y) * (1.0 - share) * 0.5), colour)
			return
		var ring := PackedVector2Array()
		var low := Vector2.INF
		var high := -Vector2.INF
		var n := int(shape.outline_points)
		for k in n:
			var q := Vector2.from_angle((shape.turn + float(k) / n) * TAU)
			ring.append(q)
			low = low.min(q)
			high = high.max(q)
		var across := high - low
		var middle := (low + high) * 0.5
		var centre := _on_cell(-middle / across, r)
		for k in n:
			ring[k] = centre + (_on_cell((ring[k] - middle) / across, r) - centre) * share
		draw_colored_polygon(ring, colour)

	## A point of the plate, -0.5 .. 0.5 with y up, on the cell.
	func _on_cell(q: Vector2, r: Rect2) -> Vector2:
		return Vector2((q.x + 0.5) * r.size.x, (0.5 - q.y) * r.size.y)

	## A walking figure: its head and strokes in shares of the plate's radius, with round joints.
	func _figure(f: Dictionary, mid: Vector2, radius: float, ink: Color) -> void:
		var width: float = radius * f.width
		draw_circle(mid + Vector2(f.head[0], f.head[1]) * radius, radius * f.head[2], ink)
		for stroke: Array in f.strokes:
			for k in stroke.size():
				var a := mid + Vector2(stroke[k][0], stroke[k][1]) * radius
				draw_circle(a, width * 0.5, ink)
				if k + 1 < stroke.size():
					draw_line(a, mid + Vector2(stroke[k + 1][0], stroke[k + 1][1]) * radius, ink, width)

	## At the one size of its plate, drawn smaller when it must fit: a size per name would be a sheet of glyphs per size.
	func _words(words: String, p: Dictionary, r: Rect2, mid: Vector2) -> void:
		var px := int(r.size.y * p.word_height)
		var wide := font.get_string_size(words, HORIZONTAL_ALIGNMENT_LEFT, -1, px)
		var fit: float = minf(1.0, r.size.x * p.word_width / wide.x)
		draw_set_transform(_origin + mid * Vector2(_stretch, 1.0), 0.0, Vector2(_stretch, 1.0) * fit)
		draw_string(font, Vector2(-wide.x * 0.5, font.get_ascent(px) - wide.y * 0.5), words, HORIZONTAL_ALIGNMENT_LEFT, -1, px, Color(p.ink))
		draw_set_transform(_origin, 0.0, Vector2(_stretch, 1.0))
