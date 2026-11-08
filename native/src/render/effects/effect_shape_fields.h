// The fields of a block of the effects' shapes (effect_shapes.h): X a float, XI a count, XD a double, XV and XVI lists.
#pragma once

#include <vector>

#define EFFECT_SHAPE_FLOAT(n) float n = 0;
#define EFFECT_SHAPE_COUNT(n) int n = 0;
#define EFFECT_SHAPE_DOUBLE(n) double n = 0;
#define EFFECT_SHAPE_FLOATS(n) std::vector<float> n;
#define EFFECT_SHAPE_COUNTS(n) std::vector<int> n;
