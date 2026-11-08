// The fields of a block of the building look's shapes (look_shapes.h): X a float, XI a count, XD a double, XV and XVI lists.
#pragma once

#include <vector>

#define LOOK_SHAPE_FLOAT(n) float n = 0;
#define LOOK_SHAPE_COUNT(n) int n = 0;
#define LOOK_SHAPE_DOUBLE(n) double n = 0;
#define LOOK_SHAPE_FLOATS(n) std::vector<float> n;
#define LOOK_SHAPE_COUNTS(n) std::vector<int> n;
