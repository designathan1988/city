// "#RRGGBB" / "#RRGGBBAA" -> packed RGBA8. The only colour parser (data tables store colours as hex).
#pragma once

#include <cstdint>
#include <cstdlib>
#include <string>

namespace city {

// The digits of "#RRGGBB" and "#RRGGBBAA", read in base 16.
constexpr size_t HEX_RGB_DIGITS = 6, HEX_RGBA_DIGITS = 8;
constexpr int HEX_BASE = 16;
constexpr uint32_t MALFORMED_COLOUR = 0xff00ffffu; // magenta: a visible marker for a malformed colour

inline uint32_t parse_hex_color(const std::string &text) {
	std::string h = (!text.empty() && text[0] == '#') ? text.substr(1) : text;
	if (h.size() == HEX_RGB_DIGITS) h += "ff";
	if (h.size() != HEX_RGBA_DIGITS) return MALFORMED_COLOUR;
	return uint32_t(std::strtoul(h.c_str(), nullptr, HEX_BASE));
}

} // namespace city
