#include "camera.h"
#include "quat.h"
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>

void init_camera(Camera* c) {
	 *c = (Camera) {
	 	0, 0, 0,
	 	0, 0, -1,
	 	1, 0, 0,
	 	0, 1, 0,
	 };
 	Quat_init(c->quat_total);
}

void apply_camera(Camera* c) {
	/*
	glRotatef(c->rotation[0], 1, 0, 0);
	glRotatef(c->rotation[1], 0, 1, 0);
	glRotatef(c->rotation[2], 0, 0, 1);
*/
	matrix4_t matrix;
	Quat_to_matrix4 (c->quat_total, matrix);
	glMultMatrixf(matrix);
}

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

void rotate_local_axis(Camera* c, float rotrad[3]) {
	quat4_t quat_temp;

	quat4_t quat_local;
	Quat_from_axisangle(quat_local, c->right[0], c->right[1], c->right[2], rotrad[0]);
	Quat_multQuat (quat_local, c->quat_total, quat_temp);
	Quat_copy(quat_temp, c->quat_total);

	Quat_from_axisangle(quat_local, c->up[0], c->up[1], c->up[2], rotrad[1]);
	Quat_multQuat (quat_local, c->quat_total, quat_temp);
	Quat_copy(quat_temp, c->quat_total);

	Quat_from_axisangle(quat_local, c->front[0], c->front[1], c->front[2], rotrad[2]);
	Quat_multQuat (quat_local, c->quat_total, quat_temp);
	Quat_copy(quat_temp, c->quat_total);

	Quat_to_euler(c->quat_total, c->rotation);
	c->rotation[0] *= (180. / M_PI);
	c->rotation[1] *= (180. / M_PI);
	c->rotation[2] *= (180. / M_PI);

	vec3_t in;
	in[0] = 0;
	in[1] = 0;
	in[2] = -1;
	Quat_rotatePoint (c->quat_total, in, c->front);

	in[0] = 0;
	in[1] = 1;
	in[2] = 0;
	Quat_rotatePoint (c->quat_total, in, c->up);

	normalize_vec3(c->front);
	normalize_vec3(c->up);
	vec3_cross(c->front, c->up, c->right);
	normalize_vec3(c->right);
}