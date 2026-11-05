## The road catalogue card picture: the cross-section seen
## from above, strips top to bottom over grass, filling rc_fill + rc_fill_extra × (width / widest) of the height;
## a yellow centre line between opposite lanes, white dashes between lanes of one direction, rails on tracks.
class_name RoadSwatch
extends RefCounted

static var _cache: Dictionary = {}


static func texture(profile: Dictionary, widest: float) -> Texture2D:
	var key: String = profile.id
	if _cache.has(key):
		return _cache[key]
	var w := int(HudWidgets.px("rc_card_w"))
	var h := int(HudWidgets.px("rc_art_h"))
	var img := Image.create_empty(w, h, false, Image.FORMAT_RGBA8)
	img.fill(ThemeBuilder.color("rc_grass"))
	var total := 0.0
	for s in profile.strips:
		total += float(s.width)
	var span := h * (ThemeBuilder.size("rc_fill") + ThemeBuilder.size("rc_fill_extra") * minf(1.0, total / widest))
	var y := (h - span) * 0.5
	var prev: Dictionary = {}
	for s in profile.strips:
		var sh := float(s.width) / total * span
		img.fill_rect(Rect2i(0, int(y), w, maxi(1, int(ceil(sh)))), _colour(s))
		if s.get("rail", false):
			_rails(img, y, sh)
		if s.kind == "lane" and prev.get("kind", "") == "lane" and not s.get("rail", false):
			_line(img, int(y), prev.get("dir", 0) != s.get("dir", 0), profile.centre)
		prev = s
		y += sh
	var tex := ImageTexture.create_from_image(img)
	_cache[key] = tex
	return tex


static func _colour(s: Dictionary) -> Color:
	if s.get("rail", false):
		return ThemeBuilder.color("rc_ballast")
	if s.get("bus", false):
		return ThemeBuilder.color("rc_bus")
	var token := "rc_" + String(s.kind)
	if s.kind == "lane":
		token = "rc_asphalt"
	return ThemeBuilder.color(token)


static func _line(img: Image, y: int, opposite: bool, centre: String) -> void:
	var w := img.get_width()
	if opposite and centre == "none":
		return
	if opposite and centre == "double":
		img.fill_rect(Rect2i(0, y - 2, w, 1), ThemeBuilder.color("rc_yellow"))
		img.fill_rect(Rect2i(0, y + 1, w, 1), ThemeBuilder.color("rc_yellow"))
		return
	if opposite and centre != "dashed":
		img.fill_rect(Rect2i(0, y - 1, w, 2), ThemeBuilder.color("rc_yellow"))
		return
	var dash := int(ThemeBuilder.size("rc_dash"))
	var gap := int(ThemeBuilder.size("rc_gap"))
	var paint := ThemeBuilder.color("rc_yellow" if opposite else "rc_white")
	for x in range(0, w, dash + gap):
		img.fill_rect(Rect2i(x, y, dash, 1), paint)


static func _rails(img: Image, y: float, sh: float) -> void:
	var g := sh * ThemeBuilder.size("rc_rail_gauge")
	for ry in [y + sh * 0.5 - g, y + sh * 0.5 + g]:
		img.fill_rect(Rect2i(0, int(ry), img.get_width(), 1), ThemeBuilder.color("rc_rail"))
