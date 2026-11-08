// The fields of a block of the building generator's shapes (shapes.h): X a float, XI a count, XD a double, XV and XVI
// lists of floats and of counts. A block's list expands with these to declare its struct.
#pragma once

#include <vector>

#define B40_SHAPE_FLOAT(n) float n = 0;
#define B40_SHAPE_COUNT(n) int n = 0;
#define B40_SHAPE_DOUBLE(n) double n = 0;
#define B40_SHAPE_FLOATS(n) std::vector<float> n;
#define B40_SHAPE_COUNTS(n) std::vector<int> n;
