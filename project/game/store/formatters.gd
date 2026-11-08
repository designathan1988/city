## The only number/date formatting used by the UI (owners.json: money_format -> format_money).
class_name Formatters
extends RefCounted

const GROUP := 3
## Below this a measure is whole: the float32 of the catalogue carries a few millionths.
const WHOLE_EPS := 0.0001


## "<n> <noun>": the singular or the plural key by the count (<base>.one, <base>.other).
static func count_label(base: String, count: int) -> String:
	return TranslationServer.translate(base + (".one" if count == 1 else ".other")).format({"count": format_int(count)})


static func format_int(v: float) -> String:
	var n := int(round(absf(v)))
	var digits := str(n)
	var out := ""
	var group := TranslationServer.translate(&"economy.group")
	while digits.length() > GROUP:
		out = group + digits.substr(digits.length() - GROUP) + out
		digits = digits.substr(0, digits.length() - GROUP)
	return ("-" if v < 0.0 and n != 0 else "") + digits + out


## One decimal, with the language's decimal mark (economy.decimal, as economy.group its thousands).
static func format_tenths(v: float) -> String:
	return ("%.1f" % v).replace(".", TranslationServer.translate(&"economy.decimal"))


## As short as the number can be written and still read back the same, with the language's decimal mark: a tuning value
## as it stands (4096, 0.25, 1.5).
static func format_number(v: float) -> String:
	return String.num(v).replace(".", TranslationServer.translate(&"economy.decimal"))


## Two decimals, with the language's decimal mark (a norm's measure as it is written: "1,20 m").
static func format_hundredths(v: float) -> String:
	return ("%.2f" % v).replace(".", TranslationServer.translate(&"economy.decimal"))


## A measure as the catalogue writes it (Roadcraft crossSection.ts metresText): whole numbers bare, else one decimal.
static func format_measure(v: float) -> String:
	return str(int(round(v))) if absf(v - round(v)) < WHOLE_EPS else format_tenths(v)


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


## What a road in hand costs: its price, or what an edit that lowers the value gives back.
static func format_cost(cost: float) -> String:
	if cost < 0.0:
		return TranslationServer.translate(&"economy.refund").format({"value": format_money(-cost)})
	return format_money(cost)


## A slider's value, already in percentage points.
static func format_points(v: float) -> String:
	return "%d %%" % int(v)


static func format_percent(v: float) -> String:
	return "%d%%" % int(round(v * Units.PERCENT))


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
