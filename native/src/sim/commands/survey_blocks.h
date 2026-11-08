// The blocks a building read off a 3D model stands on (body_survey.cpp reads their looks): the faces that look up, each joining
// the first-made top it meets, tops that meet merging, each top a block from the ground to its height with its hull in plan.
#pragma once

#include "sim/commands/body_survey.h"

#include <array>
#include <vector>

namespace city {

using SurveyHull = std::vector<std::array<float, 2>>;

struct SurveyBlock {
	float x0 = 0, x1 = 0, y0 = 0, y1 = 0, z0 = 0, z1 = 0;
	SurveyHull hull;
	int base = 0, top = 0;
	bool has_look = false;
	SurveyColour look{};
};

// The model's blocks, lowest top first (K: buildings.json shapes.survey).
std::vector<SurveyBlock> survey_blocks(const b40::SurveyShape &K, const std::vector<SurveyTriangle> &tris);

} // namespace city
