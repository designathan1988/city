// A building read off a 3D model (the 4.0's editor/fromReference.ts): its blocks from the faces that look up, its walls and bays from what the model shows, and what rises above
// its roof. Plan axes: x and y on the plan, z up; metres. The pictures are the model drawn flat, unlit, from each side and from above.
#pragma once

#include "sim/buildings/b40/data.h"

#include <array>
#include <cstdint>
#include <vector>

namespace city {

struct SurveyPoint {
	float x = 0, y = 0, z = 0;
};
using SurveyTriangle = std::array<SurveyPoint, 3>;

// One picture: looked at along -`out`, `right` and `up` the picture's axes, `half_w` and `half_h` how far it reaches from `centre`. RGBA8, the top row first.
struct SurveyView {
	std::vector<uint8_t> rgba;
	int w = 0, h = 0;
	SurveyPoint centre, right, up, out;
	float half_w = 0, half_h = 0;
	std::vector<float> nearest; // how far towards the eye the model comes at each pixel: filled by `survey_body`
};

using SurveyColour = std::array<float, 3>;
// How near the model comes at each pixel of a picture, the model laid over it triangle by triangle: what stands in front of a face hides it.
void survey_depth(SurveyView &v, const std::vector<SurveyTriangle> &tris);
// What the model shows at a point of its surface, seen along the normal; false for nothing there.
bool survey_sample(const std::vector<SurveyView> &views, const SurveyPoint &p, float nx, float ny, SurveyColour &out);

// Fills `b`; false when the model has no block big enough to be a room. Pure: it reads only the model and the generator's
// tables (b40::Data, read-only after boot), so a command runs it before it locks the map.
bool survey_body(const b40::Data &data, const std::vector<SurveyTriangle> &tris, std::vector<SurveyView> &views, b40::Body &b);

} // namespace city
