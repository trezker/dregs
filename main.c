#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <GL/glu.h>

#include "view.h"
#include "static_model.h"

#define MAX_STEPS 100
#define MAX_DIST 100.
#define SURF_DIST .01

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

void Quad() {
	glDisable(GL_LIGHTING);
	glBegin(GL_QUADS);
	float color[4] = {1, 1, 1, 1};
	glColor4fv(color);
	glNormal3f( 0,  0, 1);
	glTexCoord2f(1, 1); glVertex3f( 1,  1, -4);
	glTexCoord2f(1, 0); glVertex3f( 1, -1, -4);
	glTexCoord2f(0, 0); glVertex3f(-1, -1, -4);
	glTexCoord2f(0, 1); glVertex3f(-1,  1, -4);
	glEnd();
	glEnable(GL_LIGHTING);
}

void Render(Static_model* m)
{
	float fov = 45.f;
	float near = 1.f;
	float far = 1000.f;
	float width = 640;
	float height = 480;
	Init_perspective_view(fov, width/height, near, far);

	glEnable(GL_DEPTH_TEST);
 	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_LIGHTING);

	//Quad();
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glTranslatef(0, 0, -10);
	float color[4] = {1, 1, 1, 1};
	glColor4fv(color);
	render_static_model(m);
	glPopMatrix();

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

	al_set_new_display_flags(ALLEGRO_OPENGL);
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
	Static_model* m = load_static_model("data/cube.tmf");

	int done = 0;
	while(!done) {
		while(al_get_next_event(queue, &event)) {
			switch(event.type) {
				case ALLEGRO_EVENT_KEY_DOWN:
					if(event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
						done = 1;
					}
					break;
				case ALLEGRO_EVENT_DISPLAY_CLOSE:
					done = 1;
					break;
			}
		}

		al_clear_to_color(black);
		Render(m);
		al_flip_display();
	}

	destroy_static_model(m);
	al_destroy_event_queue(queue);

	return 0;
}
