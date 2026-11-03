## The only number/date formatting used by the UI (owners.json: money_format -> format_money).
class_name Formatters
extends RefCounted

const CURRENCY := "R$"
const THOUSANDS := "."
const GROUP := 3
const PERCENT := 100.0


static func format_int(v: float) -> String:
	var n := int(round(absf(v)))
	var digits := str(n)
	var out := ""
	while digits.length() > GROUP:
		out = THOUSANDS + digits.substr(digits.length() - GROUP) + out
		digits = digits.substr(0, digits.length() - GROUP)
	return ("-" if v < 0.0 and n != 0 else "") + digits + out


static func format_money(v: float) -> String:
	return "%s %s" % [CURRENCY, format_int(v)]


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
