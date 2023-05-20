#include <math.h>

float vec3_length(float v[3]) {
	return sqrt( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] );
}

void vec3_normalize(float* v, float* o) {
	float len = vec3_length(v);
	len = (len != 0 ? len : 1.0f);
	float lengthMul = 1.0f / len;
	o[0] = v[0] * lengthMul;
	o[1] = v[1] * lengthMul;
	o[2] = v[2] * lengthMul;
}

void vec3_cross(float* in1, float* in2, float* out) {
	out[0] = in1[1] * in2[2] - in1[2] * in2[1];
	out[1] = in1[2] * in2[0] - in1[0] * in2[2],
	out[2] = in1[0] * in2[1] - in1[1] * in2[0];
}

float vec3_dot(float* a, float* b) {
	return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

void vec3_subv(float* a, float* b, float* out) {
	out[0] = a[0] - b[0];
	out[1] = a[1] - b[1];
	out[2] = a[2] - b[2];
}

void vec3_multf(float* in, float m, float* out) {
	out[0] = in[0]*m;
	out[1] = in[1]*m;
	out[2] = in[2]*m;
}

void vec3_addv(float* a, float* b, float* out) {
	out[0] = a[0]+b[0];
	out[1] = a[1]+b[1];
	out[2] = a[2]+b[2];
}

void vec3_hadamard(float* a, float* b, float* out) {
	for(int i = 0; i<3; ++i) {
		out[i] = a[i]*b[i];
	}
}
