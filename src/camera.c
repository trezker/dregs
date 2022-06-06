#include <GL/glu.h>
#include <stdio.h>
#include <math.h>
#include "camera.h"
#include "quat.h"
#include "vec.h"

void init_camera(Camera* c) {
	 *c = (Camera) {
	 	0, 0, 0,
	 	0, 0, 0,
	 	0, 0, -1,
	 	1, 0, 0,
	 	0, 1, 0,
	 };
 	Quat_init(c->quat_total);
}

void apply_quat_rotation(quat4_t q) {
	matrix4_t matrix;
	Quat_to_matrix4 (q, matrix);
	glMultMatrixf(matrix);
}

void apply_camera(Camera* c) {
	/*
	glRotatef(c->rotation[0], 1, 0, 0);
	glRotatef(c->rotation[1], 0, 1, 0);
	glRotatef(c->rotation[2], 0, 0, 1);
*/
	apply_quat_rotation(c->quat_total);
	glTranslatef(c->position[0],c->position[1],c->position[2]);
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

void translate_camera(Camera* c, float v[3]) {
	vec3_addv(c->position, v, c->position);
}