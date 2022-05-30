#include "camera.h"
#include <GL/glu.h>

void init_camera(Camera* c) {
	 *c = (Camera) {0, 0, 0};
}

void apply_camera(Camera* c) {
	glRotatef(c->rotation[0], 1, 0, 0);
	glRotatef(c->rotation[1], 0, 1, 0);
	glRotatef(c->rotation[2], 0, 0, 1);
}