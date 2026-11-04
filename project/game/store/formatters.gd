## The only number/date formatting used by the UI (owners.json: money_format -> format_money).
class_name Formatters
extends RefCounted

const THOUSANDS := "."
const GROUP := 3
const PERCENT := 100.0


## "<n> <noun>": the singular or the plural key by the count (<base>.one, <base>.other).
static func count_label(base: String, count: int) -> String:
	return TranslationServer.translate(base + (".one" if count == 1 else ".other")).format({"count": format_int(count)})


static func format_int(v: float) -> String:
	var n := int(round(absf(v)))
	var digits := str(n)
	var out := ""
	while digits.length() > GROUP:
		out = THOUSANDS + digits.substr(digits.length() - GROUP) + out
		digits = digits.substr(0, digits.length() - GROUP)
	return ("-" if v < 0.0 and n != 0 else "") + digits + out


## One decimal, with the comma of the Portuguese text.
static func format_tenths(v: float) -> String:
	var text := "%.1f" % v
	return text.replace(".", ",") if TranslationServer.get_locale().begins_with("pt") else text


## The city's regional style as the interface shows it: its units and money (data/game/start.json).
static func style() -> Dictionary:
	var t: Dictionary = DataTables.game("start")
	return t.styles.get(CityStore.snapshot.get("style", ""), t.styles[t.default_style])


## Metres per second in a speed unit.
static func speed_in(ms: float, unit: String) -> float:
	var u: Dictionary = DataTables.game("start").units[unit]
	return ms * u.s / u.m


## Metres in a length unit.
static func length_in(metres: float, unit: String) -> float:
	return metres / DataTables.game("start").units[unit].m


static func format_speed(ms: float) -> String:
	var unit: String = style().speed_unit
	return "%d %s" % [int(round(speed_in(ms, unit))), DataTables.game("start").units[unit].label]


static func format_length(metres: float) -> String:
	var unit: String = style().length_unit
	return "%d %s" % [int(round(length_in(metres, unit))), DataTables.game("start").units[unit].label]


## With one decimal, as a road's length.
static func format_length_tenths(metres: float) -> String:
	var unit: String = style().length_unit
	return "%s %s" % [format_tenths(length_in(metres, unit)), DataTables.game("start").units[unit].label]


static func format_money(v: float) -> String:
	return "%s %s" % [style().currency, format_int(v)]


## A slider's value, already in percentage points.
static func format_points(v: float) -> String:
	return "%d %%" % int(v)


static func format_percent(v: float) -> String:
	return "%d%%" % int(round(v * PERCENT))


static func format_value(v: Variant, kind: String) -> String:
	match kind:
		"money":
			return format_money(float(v))
		"percent":
			return format_percent(float(v))
		_:
			return format_int(float(v))


## Game clock from the snapshot (day number and fractional hour of day).
static func format_clock(day: int, hour: float, minutes_per_hour: float) -> String:
	var h := int(hour)
	var m := int((hour - h) * minutes_per_hour)
	return tr_static("STAT_DATE") % [day, h, m]


static func tr_static(key: String) -> String:
	return TranslationServer.translate(key)
