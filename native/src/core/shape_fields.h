// The fields of a block of shapes, the values a data table's "shapes" gives a family of files (sim/buildings/b40/shapes.h,
// render/buildings/look_shapes.h, render/effects/effect_shapes.h, sim/citygen/gen_shapes.h): a block's list names each field
// with its type's macro (X a float, XI a count, XD a double, XV and XVI lists of floats and of counts), and SHAPE_DECLARE(LIST)
// makes those the members of the block's struct. render/shape_reader.h reads them the same way.
#pragma once

#include <vector>

#define SHAPE_FIELD_FLOAT(n) float n = 0;
#define SHAPE_FIELD_COUNT(n) int n = 0;
#define SHAPE_FIELD_DOUBLE(n) double n = 0;
#define SHAPE_FIELD_FLOATS(n) std::vector<float> n;
#define SHAPE_FIELD_COUNTS(n) std::vector<int> n;
#define SHAPE_DECLARE(LIST) LIST(SHAPE_FIELD_FLOAT, SHAPE_FIELD_COUNT, SHAPE_FIELD_DOUBLE, SHAPE_FIELD_FLOATS, SHAPE_FIELD_COUNTS)
