#include "static_model.h"
#include <stdlib.h>

typedef struct {
	float rotpos[6];
	float momentum[6];
} Crate;

Crate* crates;
int num_crates;

void init_crates() {
	crates = malloc(sizeof(Crate)*10);
	crates[0].rotpos[0] = 0;
	crates[0].rotpos[1] = 0;
	crates[0].rotpos[2] = 0;
	crates[0].rotpos[3] = (rand()%1000)-500;
	crates[0].rotpos[4] = (rand()%1000)-500;
	crates[0].rotpos[5] = (rand()%1000)-500;
	crates[0].momentum[0] = 0;
	crates[0].momentum[1] = 0;
	crates[0].momentum[2] = 0;
	crates[0].momentum[3] = 0;
	crates[0].momentum[4] = 0;
	crates[0].momentum[5] = 0;
	++num_crates;
}

void update_crates(float dt) {
	for(int i = 0; i<6; ++i) {
		crates[0].rotpos[i] += crates[0].momentum[i];
	}
}

void render_crates(Static_model* model) {
	render_static_model(model);
}