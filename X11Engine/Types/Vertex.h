#pragma once

#include "Vector3.h"
#include "Vector2.h"

struct vertex {
	vertex(const vector3& pos = {}, const vector2& uv = {}): pos(pos), uv(uv) {};

	vector3 pos;
	vector2 uv;
};