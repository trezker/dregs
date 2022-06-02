#include <allegro5/allegro.h>
#include <stdio.h>
#include <GL/glu.h>
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
	i_static_model_load_normals(f, m, m->num_vertices);

	m->uvs = malloc(4*2*m->num_vertices);
	al_fread(f, m->uvs, 4*2*m->num_vertices);
#ifdef DEBUG_PRINT
	for(int i=0; i<m->num_vertices; ++i) {
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

void render_static_model(Static_model* m) {
	glEnableClientState (GL_VERTEX_ARRAY);
	glEnableClientState (GL_NORMAL_ARRAY);
	glEnableClientState (GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
	glVertexPointer (3, GL_FLOAT, 0, m->vertices);
	glTexCoordPointer(2, GL_FLOAT, 0, m->uvs);
	glNormalPointer(GL_FLOAT, 0, m->normals);

	glDrawElements (GL_TRIANGLES, m->num_indices,
					GL_UNSIGNED_SHORT, m->faces);

	glDisableClientState (GL_VERTEX_ARRAY);
	glDisableClientState (GL_NORMAL_ARRAY);
	glDisableClientState (GL_TEXTURE_COORD_ARRAY);
}