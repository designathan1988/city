## The small pictures the theme is made of, drawn once at boot: the slider's knob, a zone's swatch and the title screen's vignette.
class_name ThemeArt
extends RefCounted

const VIGNETTE_SIDE := 256 # the vignette texture's side: smooth at any window size, stretched


## The slider of the panels: a thin track, the part behind the knob in the accent colour.
static func slider(theme: Theme) -> void:
	var track := ThemeBuilder.box("raise_2", "pill_radius", "line")
	track.content_margin_top = ThemeBuilder.size("slider_h") * 0.5
	track.content_margin_bottom = ThemeBuilder.size("slider_h") * 0.5
	var knob := disc(ThemeBuilder.color("accent"), int(ThemeBuilder.size("knob")))
	for role in [&"grabber", &"grabber_highlight", &"grabber_disabled"]:
		theme.set_icon(role, &"HSlider", knob)
	var filled := ThemeBuilder.box("accent", "pill_radius")
	theme.set_stylebox(&"slider", &"HSlider", track)
	theme.set_stylebox(&"grabber_area", &"HSlider", filled)
	theme.set_stylebox(&"grabber_area_highlight", &"HSlider", filled)


## How wide each kind of button draws its icon.
static func icon_widths(theme: Theme) -> void:
	for name in [&"SegButton", &"DockButton", &"DockDanger", &"IconButton", &"Row", &"DrawerTab"]:
		theme.set_constant(&"icon_max_width", name, int(ThemeBuilder.size("cat_icon")))
	theme.set_constant(&"icon_max_width", &"IconButton", int(ThemeBuilder.size("bar_icon")))
	theme.set_constant(&"icon_max_width", &"SegButton", int(ThemeBuilder.size("seg_icon")))
	theme.set_constant(&"icon_max_width", &"Row", int(ThemeBuilder.size("row_icon")))
	theme.set_constant(&"icon_max_width", &"LayerRow", int(ThemeBuilder.size("row_icon")))
	theme.set_constant(&"icon_max_width", &"IconChoice", int(ThemeBuilder.size("bar_icon")))
	theme.set_constant(&"icon_max_width", &"Card", int(ThemeBuilder.size("card_glyph")))


## A round knob with a soft edge.
static func disc(colour: Color, px: int) -> Texture2D:
	var img := Image.create_empty(px, px, false, Image.FORMAT_RGBA8)
	var centre := Vector2.ONE * (px - 1) * 0.5
	for y in px:
		for x in px:
			var pixel := colour
			pixel.a *= clampf(px * 0.5 - Vector2(x, y).distance_to(centre), 0.0, 1.0)
			img.set_pixel(x, y, pixel)
	return ImageTexture.create_from_image(img)


## A rounded square with a light inner ring (.v2-zone-swatch).
static func swatch(colour: Color, px: int, radius: float, ring: float, ring_colour: Color) -> Texture2D:
	var img := Image.create_empty(px, px, false, Image.FORMAT_RGBA8)
	var half := px * 0.5
	var light := ring_colour
	light.a = 1.0
	for y in px:
		for x in px:
			var q := (Vector2(x + 0.5, y + 0.5) - Vector2.ONE * half).abs() - Vector2.ONE * (half - radius)
			var dist := Vector2(maxf(q.x, 0.0), maxf(q.y, 0.0)).length() + minf(maxf(q.x, q.y), 0.0) - radius
			var pixel := colour.lerp(light, clampf(dist + ring + 0.5, 0.0, 1.0) * ring_colour.a)
			pixel.a = colour.a * clampf(0.5 - dist, 0.0, 1.0)
			img.set_pixel(x, y, pixel)
	return ImageTexture.create_from_image(img)


## Dark at the edges and lighter round a point above the middle (.v2-start's radial gradient).
static func vignette(inner: Color, outer: Color, centre: Vector2, reach: float) -> Texture2D:
	var ramp := Gradient.new()
	ramp.set_color(0, inner)
	ramp.set_color(1, outer)
	var tex := GradientTexture2D.new()
	tex.gradient = ramp
	tex.width = VIGNETTE_SIDE
	tex.height = VIGNETTE_SIDE
	tex.fill = GradientTexture2D.FILL_RADIAL
	tex.fill_from = centre
	tex.fill_to = centre + Vector2(0.0, reach)
	return tex
