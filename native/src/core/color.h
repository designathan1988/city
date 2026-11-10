// "#RRGGBB" / "#RRGGBBAA" -> packed RGBA8. The only colour parser (data tables store colours as hex).
#pragma once

#include <cstdint>
#include <cstdlib>
#include <string>

namespace city {

inline uint32_t parse_hex_color(const std::string &text) {
	std::string h = (!text.empty() && text[0] == '#') ? text.substr(1) : text;
	if (h.size() == 6) h += "ff";
	if (h.size() != 8) return 0xff00ffffu; // magenta: visible marker for a malformed colour
	return uint32_t(std::strtoul(h.c_str(), nullptr, 16));
}

} // namespace city
