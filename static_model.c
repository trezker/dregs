#include <allegro5/allegro.h>
#include <stdio.h>
#include "static_model.h"

#define DEBUG_PRINT

void i_static_model_load_normals(ALLEGRO_FILE* f, Static_model* m, int n) {
	m->normals = malloc(4*3*n);
	al_fread(f, m->normals, 4*3*n);
#ifdef DEBUG_PRINT
	for(int i=0; i<n; ++i) {
		printf("%f %f %f\n", m->normals[i*3], m->normals[i*3+1], m->normals[i*3+2]);
	}
#endif
}

Static_model* load_static_model(char* path){
	Static_model* m = malloc(sizeof(Static_model));
	ALLEGRO_FILE* f = al_fopen(path, "rb");
	m->num_vertices = al_fread16le(f);
	m->num_indices = al_fread16le(f);
	m->smooth = al_fread16le(f);
#ifdef DEBUG_PRINT
	printf("%i %i %i\n", m->num_vertices, m->num_indices, m->smooth);
#endif

	//Vertices
	m->vertices = malloc(4*3*m->num_vertices);
	al_fread(f, m->vertices, 4*3*m->num_vertices);
#ifdef DEBUG_PRINT
	for(int i=0; i<m->num_vertices; ++i) {
		printf("%f %f %f\n", m->vertices[i*3], m->vertices[i*3+1], m->vertices[i*3+2]);
	}
#endif

	//Normals
	if(m->smooth) {
		i_static_model_load_normals(f, m, m->num_vertices);
	}
	else {
		i_static_model_load_normals(f, m, m->num_indices/3);
	}

	m->uvs = malloc(4*2*m->num_indices);
	al_fread(f, m->uvs, 4*2*m->num_indices);
#ifdef DEBUG_PRINT
	for(int i=0; i<m->num_indices; ++i) {
		printf("%f %f\n", m->uvs[i*2], m->uvs[i*2+1]);
	}
#endif

	m->faces = malloc(2*m->num_indices);
	al_fread(f, m->faces, 2*m->num_indices);
#ifdef DEBUG_PRINT
	for(int i=0; i<m->num_indices; ++i) {
		printf("%i ", m->faces[i]);
	}
	printf("\n");
#endif

	al_fclose(f);
	return m;
}

void destroy_static_model(Static_model* model) {
	free(model->vertices);
	free(model->normals);
	free(model->uvs);
	free(model->faces);
	free(model);
}