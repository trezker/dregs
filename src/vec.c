#include <math.h>

float vec3_length(float v[3]) {
	return sqrt( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] );
}

void normalize_vec3(float* v) {
	float len = vec3_length(v);
	len = (len != 0 ? len : 1.0f);
	float lengthMul = 1.0f / len;
	v[0] *= lengthMul;
	v[1] *= lengthMul;
	v[2] *= lengthMul;
}

void vec3_cross(float* in1, float* in2, float* out) {
	out[0] = in1[1] * in2[2] - in1[2] * in2[1];
	out[1] = in1[2] * in2[0] - in1[0] * in2[2],
	out[2] = in1[0] * in2[1] - in1[1] * in2[0];
}
