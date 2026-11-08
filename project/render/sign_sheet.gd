## The sheets of plate pictures: the core asks for a cell the moment a chunk needs a plate (render/props/sign_atlas.h); here the
## cells waiting are painted, the words with the canvas's font, into a viewport, and the viewport's picture goes into its layer
## of the array the "prop" material reads (sign_atlas). Each layer's picture is also kept here, so the array is remade from these
## when the core opens a layer past its room (the room doubling each time) and a round on a layer begun lays what it holds under
## the new cells, with nothing read back from the GPU; the viewport's picture is the one read back, at most one a round
## (sheet_round_s). Between rounds the viewport shrinks to nothing. A new city (the core's sheets under a new generation) drops
## what was waiting and what the layers held.
class_name SignSheet
extends SubViewport

const LAYER_FORMAT := Image.FORMAT_RGBA8
const IDLE_SIZE := Vector2i.ONE
## A cell as the core gives it (sign_atlas.h take_pending): [type, words, rectangle in its layer, layer, generation].
const CELL_LAYER := 3
const CELL_GENERATION := 4

var layers := Texture2DArray.new()

var _paper := _Paper.new()
var _look: Dictionary
var _material: ShaderMaterial
var _waiting: Array = []
var _begun := -1 ## The last layer painted: the only one with cells to keep.
var _busy := false
var _since := 0.0
var _pictures: Array[Image] = [] ## What each layer of `layers` holds, kept as uploaded: its room is their count.
var _generation := -1 ## The core's generation of the sheets these layers hold.
var _painted := false ## Something of this generation was laid on the layers.


func _init(look: Dictionary, material: ShaderMaterial) -> void:
	_look = look
	_material = material
	size = IDLE_SIZE
	transparent_bg = false
	disable_3d = true
	render_target_clear_mode = SubViewport.CLEAR_MODE_ALWAYS
	render_target_update_mode = SubViewport.UPDATE_DISABLED
	_paper.look = look
	_paper.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
	add_child(_paper)
	_grow(1)


func _process(delta: float) -> void:
	_since += delta
	if _busy or _since < _look.sheet_round_s:
		return
	var fresh: Array = CityStore.query(&"sign_cells") # once a round, as they are drawn: an Array across the DLL otherwise
	if not fresh.is_empty() and int(fresh[0][CELL_GENERATION]) != _generation: # the core takes them all under its generation now
		_new_city(int(fresh[0][CELL_GENERATION]))
	_waiting.append_array(fresh)
	if _waiting.is_empty():
		_since = 0.0
		return
	var layer: int = _waiting[0][CELL_LAYER]
	var cut := 0
	while cut < _waiting.size() and _waiting[cut][CELL_LAYER] == layer:
		cut += 1
	var batch := _waiting.slice(0, cut)
	_waiting = _waiting.slice(cut)
	var kept: Image = _pictures[layer] if layer == _begun and layer < _pictures.size() else null
	_paper.begin(batch, ImageTexture.create_from_image(kept) if kept else null)
	_begun = layer
	size = _layer_size()
	_busy = true
	render_target_update_mode = SubViewport.UPDATE_ONCE
	RenderingServer.frame_post_draw.connect(_upload.bind(layer, _generation), CONNECT_ONE_SHOT)


## The viewport has drawn: its picture goes to its layer (unless the city changed meanwhile), and the fonts and the drawing go.
func _upload(layer: int, generation: int) -> void:
	if generation == _generation:
		var image := get_texture().get_image()
		image.convert(LAYER_FORMAT)
		if layer >= _pictures.size():
			_grow(layer + 1)
		_pictures[layer] = image
		layers.update_layer(image, layer)
		_painted = true
	_paper.end()
	size = IDLE_SIZE
	_busy = false
	_since = 0.0


## The core's sheets started again for another city: nothing waiting or laid belongs to it, and its first cells reuse the layers'
## places, so the layers are emptied before any of its plates reads them.
func _new_city(generation: int) -> void:
	_generation = generation
	_waiting = []
	_begun = -1
	if _painted:
		_painted = false
		_pictures = []
		_grow(1)


## Nothing waits to be painted and no round is on the canvas (the captures wait for this before they read the sheets).
func idle() -> bool:
	return not _busy and _waiting.is_empty()


func _layer_size() -> Vector2i:
	return Vector2i(int(_look.sheet_w_px), int(_look.sheet_h_px))


## The array with room for at least `count` layers, twice the room it had while that is less: a texture's layers are fixed when
## made, so it is made again from the pictures kept, the new layers empty.
func _grow(count: int) -> void:
	var room := maxi(_pictures.size(), 1)
	while room < count:
		room *= 2
	while _pictures.size() < room:
		_pictures.append(Image.create_empty(_layer_size().x, _layer_size().y, false, LAYER_FORMAT))
	layers = Texture2DArray.new()
	layers.create_from_images(_pictures)
	_material.set_shader_parameter("sign_atlas", layers)


## Draws each plate as a real one reads, in the proportions of the plate (a cell is stretched over it by the mesh's uv): the
## shape's colours on its cell, cut by the plate's own outline in the mesh (street_signs.cpp outline, the same polygon).
class _Paper:
	extends Node2D

	var look: Dictionary
	var cells: Array = []
	var under: Texture2D ## What the layer holds already.
	var font := FontVariation.new()
	var _origin := Vector2.ZERO # the cell being painted, and how a plate's proportions stretch over it
	var _stretch := 1.0
	var _face: FontFile # its own, for a batch only: the big glyphs of the shared font's cache would stay for good

	func begin(batch: Array, picture: Texture2D) -> void:
		cells = batch
		under = picture
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
		under = null
		queue_redraw()

	func _draw() -> void:
		if under:
			draw_texture(under, Vector2.ZERO)
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
			var q := Vector2.from_angle((shape.rotation + float(k) / n) * TAU)
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
