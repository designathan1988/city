// The operations the road heights are made of (road_grade.h), each on one line of stations a fixed step apart and each
// exact in a pass or two, with no count of passes deciding a result: the greatest line under a line and the least one
// over it whose slope never passes a grade (the envelope of the cones from every station: one sweep forward and one
// back, Felzenszwalb and Huttenlocher, "Distance Transforms of Sampled Functions", section 3: "the distance transform
// under the l1 norm can be computed using a simple two-pass algorithm"), their mean, and the mean of a line over a reach
// (a positive weight of bounded reach, by running sums: it never steepens a line and reaches no further than asked).
#pragma once

#include <vector>

namespace city {

// The greatest line under z whose slope never passes `grade` (z itself where it already holds).
void grade_lowered(std::vector<float> &z, float step, float grade);
// The least line over z whose slope never passes `grade`.
void grade_raised(std::vector<float> &z, float step, float grade);
// The mean of the two: within the grade, z where z already is, what is cut balancing what is filled elsewhere.
void within_grade(std::vector<float> &z, float step, float grade);

// Each station the mean of the line over `reach` metres either side; past an end the end's own value stands.
void mean_over(std::vector<float> &v, float step, float reach);
// The same for the stations strictly between `first` and `last`, the reach shrinking to nothing at each of the two (they
// and what lies past them stand as they are); `only_up`: a station is never lowered (the sags alone are rounded).
void rounded_between(std::vector<float> &v, float step, float reach, int first, int last, bool only_up);

// The line at `at` stations from its start: linear between stations, its ends held.
float line_at(const std::vector<float> &v, float at);
// 0 at t <= 0, 1 at t >= 1, level at both (the cubic between).
float eased(float t);
// The lower of two heights, the corner where they cross rounded over `band` of height (a quadratic).
float lower_rounded(float a, float b, float band);

} // namespace city
