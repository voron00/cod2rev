#include "i_math.h"

int Vector2Compare(const vec2_t v1, const vec2_t v2)
{
	if (v1[0] != v2[0] || v1[1] != v2[1])
		return 0;

	return 1;
}

int VectorCompare(const vec3_t v1, const vec3_t v2)
{
	if (v1[0] != v2[0] || v1[1] != v2[1] || v1[2] != v2[2])
		return 0;

	return 1;
}

int Vector4Compare(const vec4_t v1, const vec4_t v2)
{
	if (v1[0] != v2[0] || v1[1] != v2[1] || v1[2] != v2[2] || v1[3] != v2[3])
		return 0;

	return 1;
}
