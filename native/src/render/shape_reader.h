// How a table's "shapes" are read: every block of a shapes list (an X-macro of blocks, each an X-macro of typed fields:
// b40/shapes.h, render/buildings/look_shapes.h, render/effects/effect_shapes.h), each field under its own name and of its
// own type, through the strict reader (a missing key is reported). A loader writes
// `{ DataReader r = ...; const godot::Dictionary t = ...; auto &out = ...; LIST_OF_BLOCKS(SHAPE_READ_BLOCK) }`.
#pragma once

#include "render/data_reader.h"

#include <godot_cpp/variant/dictionary.hpp>

#define SHAPE_READ_FLOAT(n) o.n = float(rb.num(d, #n));
#define SHAPE_READ_COUNT(n) o.n = rb.integer(d, #n);
#define SHAPE_READ_DOUBLE(n) o.n = rb.num(d, #n);
#define SHAPE_READ_FLOATS(n) o.n = rb.floats(d, #n);
#define SHAPE_READ_COUNTS(n) o.n = rb.counts(d, #n);
#define SHAPE_READ_BLOCK(key, Type, LIST) \
	{ \
		city::DataReader rb = r.in(#key); \
		const godot::Dictionary d = r.table(t, #key); \
		Type &o = out.key; \
		LIST(SHAPE_READ_FLOAT, SHAPE_READ_COUNT, SHAPE_READ_DOUBLE, SHAPE_READ_FLOATS, SHAPE_READ_COUNTS) \
	}
