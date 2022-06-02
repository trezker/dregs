#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_opengl.h>
#include <GL/glu.h>

#include "view.h"
#include "static_model.h"
#include "camera.h"

#define MAX_STEPS 100
#define MAX_DIST 100.
#define SURF_DIST .01

Camera camera;

float clamp(float v, float min, float max) {
	return v<min?min:v>max?max:v;
}

void abort_example(char const *format, ...)
{
	char str[1024];
	va_list args;
	ALLEGRO_DISPLAY *display;

	va_start(args, format);
	vsnprintf(str, sizeof str, format, args);
	va_end(args);

	if (al_init_native_dialog_addon()) {
		display = al_is_system_installed() ? al_get_current_display() : NULL;
		al_show_native_message_box(display, "Error", "Cannot run example", str, NULL, 0);
	}
	else {
		fprintf(stderr, "%s", str);
	}
	exit(1);
}

ALLEGRO_BITMAP* texture;
void Quad() {
	glBindTexture(GL_TEXTURE_2D, al_get_opengl_texture(texture));
	glEnable(GL_TEXTURE_2D);

	glDisable(GL_LIGHTING);
	glBegin(GL_TRIANGLES);
	float color[4] = {1, 1, 1, 1};
	glColor4fv(color);
	glNormal3f( 0,  0, 1);
	glTexCoord2f(1, 1); glVertex3f( 1,  1, -4);
	glTexCoord2f(1, 0); glVertex3f( 1, -1, -4);
	glTexCoord2f(0, 0); glVertex3f(-1, -1, -4);
//	glTexCoord2f(0, 1); glVertex3f(-1,  1, -4);
	glEnd();
	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
}

void RenderModel(float* p, float* c, Static_model* m) {
	glPushMatrix();
	glTranslatef(p[0], p[1], p[2]);
	//apply_camera(&camera);
	glColor4fv(c);
	render_static_model(m);
	glPopMatrix();
}

int face = 0;
void Render(Static_model* m)
{
	float fov = 45.f;
	float near = 1.f;
	float far = 100.f;
	float width = 640;
	float height = 480;
	Init_perspective_view(fov, width/height, near, far);
	glEnable(GL_DEPTH_TEST);
 	glClear(GL_DEPTH_BUFFER_BIT);
 	glDepthFunc(GL_LESS);
	apply_camera(&camera);

	//Quad();
/*
	glTranslatef(0, 0, -5);
	apply_camera(&camera);

	glBindTexture(GL_TEXTURE_2D, al_get_opengl_texture(texture));
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glBegin(GL_TRIANGLES);
	float color[4] = {1, 1, 1, 1};
	glColor4fv(color);
	glNormal3f( 0,  0, 1);

	for(face = 0; face<12; ++face) {
		glTexCoord2f(m->uvs[face*6+0], m->uvs[face*6+1]);
		int v = m->faces[face*3+0];
		glVertex3f( m->vertices[v*3+0],  m->vertices[v*3+1], m->vertices[v*3+2]);
		glTexCoord2f(m->uvs[face*6+2], m->uvs[face*6+3]);
		v = m->faces[face*3+1];
		glVertex3f( m->vertices[v*3+0],  m->vertices[v*3+1], m->vertices[v*3+2]);
		glTexCoord2f(m->uvs[face*6+4], m->uvs[face*6+5]);
		v = m->faces[face*3+2];
		glVertex3f( m->vertices[v*3+0],  m->vertices[v*3+1], m->vertices[v*3+2]);
	}
	glEnd();
	glEnable(GL_LIGHTING);
*/
	glEnable(GL_DEPTH_TEST);
 	glClear(GL_DEPTH_BUFFER_BIT);
 	glDepthFunc(GL_LESS);
	glEnable(GL_LIGHTING);

	glBindTexture(GL_TEXTURE_2D, al_get_opengl_texture(texture));
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);

	glAlphaFunc(GL_GREATER, 0.01f);
	glEnable(GL_ALPHA_TEST);

	glDisable(GL_LIGHTING);
	RenderModel((float[3]){0, 0, -10}, (float[4]){1,1,1,1}, m);
	/*
	RenderModel((float[3]){0, 0, 10}, (float[4]){1,1,1,1}, m);
	RenderModel((float[3]){0, 10, 0}, (float[4]){1,1,1,1}, m);
	RenderModel((float[3]){0, -10, 0}, (float[4]){1,1,1,1}, m);
	RenderModel((float[3]){10, 0, 0}, (float[4]){1,1,1,1}, m);
	RenderModel((float[3]){-10, 0, 0}, (float[4]){1,1,1,1}, m);
*/
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_TEXTURE_2D);
	glShadeModel(GL_FLAT);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	Pop_view();
}

int main() {
	ALLEGRO_DISPLAY *display;
	ALLEGRO_EVENT_QUEUE *queue;
	ALLEGRO_EVENT event;

	if (!al_init()) {
		abort_example("Could not init Allegro.\n");
	}

	al_init_primitives_addon();
	al_install_mouse();
	al_install_keyboard();
	al_init_image_addon();

	al_set_new_display_flags(ALLEGRO_OPENGL);
	al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 24, ALLEGRO_REQUIRE);
	display = al_create_display(640, 480);
	if (!display) {
		abort_example("Error creating display\n");
	}

	queue = al_create_event_queue();
	al_register_event_source(queue, al_get_mouse_event_source());
	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_display_event_source(display));

	ALLEGRO_COLOR black = al_map_rgb_f(0, 0, 0);
	ALLEGRO_COLOR white = al_map_rgb_f(1, 1, 1);

	texture = al_load_bitmap("data/tex.png");

/*
	ALLEGRO_SHADER* shader = al_create_shader(ALLEGRO_SHADER_AUTO);
	if(!shader) {
		abort_example("Could not create shader.\n");
	}
	
	const char *vsource = "shader_vertex.glsl";
	const char *psource = "shader_pixel.glsl";
	if (!al_attach_shader_source_file(shader, ALLEGRO_VERTEX_SHADER, vsource)) {
		abort_example("al_attach_shader_source_file failed: %s\n",
		al_get_shader_log(shader));
	}
	if (!al_attach_shader_source_file(shader, ALLEGRO_PIXEL_SHADER, psource)) {
		abort_example("al_attach_shader_source_file failed: %s\n",
		al_get_shader_log(shader));
	}

	if (!al_build_shader(shader)) {
		abort_example("al_build_shader failed: %s\n", al_get_shader_log(shader));
	}

	al_use_shader(shader);
*/
	init_camera(&camera);
	Static_model* m = load_static_model("data/cube.tmf");

	al_set_mouse_xy(display, 320, 240);

	float r[3] = {0,0,0};
	double last_time = al_current_time();
	float yaw = 0;

	int done = 0;
	while(!done) {
		while(al_get_next_event(queue, &event)) {
			switch(event.type) {
				case ALLEGRO_EVENT_KEY_DOWN:
					if(event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
						done = 1;
					}
					if(event.keyboard.keycode == ALLEGRO_KEY_Q) {
						yaw = 1;
					}
					if(event.keyboard.keycode == ALLEGRO_KEY_E) {
						yaw = -1;
					}
					if(event.keyboard.keycode == ALLEGRO_KEY_N) {
						face += 1;
						face %= 12;
					}
					break;
				case ALLEGRO_EVENT_KEY_UP:
					if(event.keyboard.keycode == ALLEGRO_KEY_Q) {
						yaw = 0;
					}
					if(event.keyboard.keycode == ALLEGRO_KEY_E) {
						yaw = 0;
					}
					break;
				case ALLEGRO_EVENT_DISPLAY_CLOSE:
					done = 1;
					break;
				case ALLEGRO_EVENT_MOUSE_AXES: {
					r[0] += -event.mouse.dy * 0.01;
					r[0] = clamp(r[0], -1, 1);
					r[2] += event.mouse.dx * 0.01;
					r[2] = clamp(r[2], -1, 1);
					break;
				}
			}
		}
		al_set_mouse_xy(display, 320, 240);
		double current_time = al_current_time();
		double dt = current_time - last_time;
		last_time = current_time;

		r[1] += dt*yaw;
		float dr[3] = {
			r[0]*dt,
			r[1]*dt,
			r[2]*dt
		};
		rotate_local_axis(&camera, dr);

		al_clear_to_color(black);
		Render(m);
		al_flip_display();
	}

	destroy_static_model(m);
	al_destroy_event_queue(queue);

	return 0;
}
