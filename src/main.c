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
#include "vec.h"
#include "crates.h"

typedef struct {
	float controls[6];
	float throttle[6];
	float rates[6];
	float momentum[6];
} Ship;

Camera camera;
ALLEGRO_SHADER* shader;
Ship ship;

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

void ProjectPointOnPlane(float* planeNormal, float* planePoint, float* point, float* out) {
	float diff[3];
	vec3_subv(point, planePoint, diff);
	float distance = -vec3_dot(planeNormal, diff);
	vec3_multf(planeNormal, distance, diff);
	vec3_addv(point, diff, out);
}

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

void radar_plane() {
	al_use_shader(shader);
	glBegin(GL_QUADS);
	float color[4] = {1, 1, 1, .5};
	glColor4fv(color);
	glNormal3fv(camera.up);
	float r[3];
	vec3_multf(camera.right, 100, r);
	float f[3];
	vec3_multf(camera.front, 100, f);
	glTexCoord2f(0, 1); glVertex3f(-r[0]+f[0], -r[1]+f[1], -r[2]+f[2]);
	glTexCoord2f(1, 1); glVertex3f( r[0]+f[0],  r[1]+f[1],  r[2]+f[2]);
	glTexCoord2f(1, 0); glVertex3f( r[0]-f[0],  r[1]-f[1],  r[2]-f[2]);
	glTexCoord2f(0, 0); glVertex3f(-r[0]-f[0], -r[1]-f[1], -r[2]-f[2]);
	glEnd();
	al_use_shader(NULL);
}

void Radar(float* p, float* c) {
	float logp[3*6];

	for(int i=0;i<6; ++i) {
		vec3_subv(camera.position, p+i*3, logp);
		float l = vec3_length(logp+i*3);
		vec3_normalize(logp+i*3, logp+i*3);
		vec3_multf(logp+i*3, log(l), logp+i*3);
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
	glEnable(GL_DEPTH_TEST);
 	glClear(GL_DEPTH_BUFFER_BIT);
 	glDepthFunc(GL_LESS);

	glTranslatef(0, -2, -8);
	glScalef(0.01, 0.01, 0.01);
	glRotatef(30, 1, 0, 0);
	apply_quat_rotation(camera.quat_total);


	radar_plane();

	glTranslatef(camera.position[0],camera.position[1],camera.position[2]);
	glPointSize(5);
	glEnableClientState (GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	
	glVertexPointer (3, GL_FLOAT, 0, logp);
	glColorPointer  (4, GL_FLOAT, 0, c);

	glDrawArrays (GL_POINTS, 0, 6);

	glDisableClientState (GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	float o[3];
	float cp[3] = {0,0,0};
	glBegin(GL_LINES);
	for(int i=0;i<6; ++i) {
		ProjectPointOnPlane(camera.up, cp, logp+i*3, o);
		glColor4fv(c+i*4);
		glVertex3fv(logp+i*3);
		glVertex3fv(o);
	}
	glEnd();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void Models(float* p, Static_model* m) {
	for(int i=0;i<6;++i) {
		RenderModel(p+i*3, (float[4]){1,1,1,1}, m);
	}
}

void Render(Static_model* m)
{
	float fov = 45.f;
	float near = 1.f;
	float far = 1000.f;
	float width = 640;
	float height = 480;
	Init_perspective_view(fov, width/height, near, far);

	float positions[6*3] = {
		  0,  0,-1000,
		  0,  0, 1000,
		  0, 1000,  0,
		  0,-1000,  0,
		 1000,  0,  0,
		-1000,  0,  0,
	};
	float colors[6*4] = {
		0, 0, 1, 0.5,
		0, 0, 1, 0.5,
		0, 1, 0, 0.5,
		0, 1, 0, 0.5,
		1, 0, 0, 0.5,
		1, 0, 0, 0.5
	};

	glPushMatrix();
	apply_camera(&camera);
	Models(positions, m);
	render_crates(m);
	glPopMatrix();

	glPushMatrix();
	Radar(positions, colors);
	glPopMatrix();

	float n[3] = {ship.controls[2], ship.controls[0], 0};
	float l = fmin(1, vec3_length(n));
	vec3_normalize(n, n);
	float u[3] = {n[1], -n[0], 0};
	float v[3];
	vec3_multf(n, l, v);
	vec3_multf(u, 0.2, u);
	vec3_multf(n, 0.4, n);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
	glBegin(GL_TRIANGLES);
	glColor4f(1, .4, 0, l);
	glVertex3f(v[0], v[1], -10);
	glVertex3f(v[0]-n[0]+u[0], v[1]-n[1]+u[1], -10);
	glVertex3f(v[0]-n[0]-u[0], v[1]-n[1]-u[1], -10);
	glEnd();
	glDisable(GL_BLEND);


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


	shader = al_create_shader(ALLEGRO_SHADER_AUTO);
	if(!shader) {
		abort_example("Could not create shader.\n");
	}
	
	const char *vsource = "data/shaders/shader_vertex.glsl";
	const char *psource = "data/shaders/radar.glsl";
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

//	al_use_shader(shader);

	init_camera(&camera);
	Static_model* m = load_static_model("data/cube.tmf");
	m->texture = texture;

	al_set_mouse_xy(display, 320, 240);

	ship = (Ship) {
		0,0,0,0,0,0,
		0,0,0,0,0,0,
		.1,.1,.1,1,1,1,
		0,0,0,0,0,0
	};

	init_crates();

	double last_time = al_current_time();

	int done = 0;
	while(!done) {
		while(al_get_next_event(queue, &event)) {
			switch(event.type) {
				case ALLEGRO_EVENT_KEY_DOWN:
					if(event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
						done = 1;
					}
					if(event.keyboard.keycode == ALLEGRO_KEY_Q) {
						ship.throttle[1] = 1;
					}
					if(event.keyboard.keycode == ALLEGRO_KEY_E) {
						ship.throttle[1] = -1;
					}
					if(event.keyboard.keycode == ALLEGRO_KEY_W) {
						ship.controls[5] = 1;
					}
					if(event.keyboard.keycode == ALLEGRO_KEY_S) {
						ship.controls[5] = -1;
					}
					if(event.keyboard.keycode == ALLEGRO_KEY_R) {
						for(int i = 0; i < 6; ++i) {
							ship.momentum[i] = 0;
							ship.throttle[i] = 0;
						}
					}
					break;
				case ALLEGRO_EVENT_KEY_UP:
					if(event.keyboard.keycode == ALLEGRO_KEY_Q) {
						ship.throttle[1] = 0;
					}
					if(event.keyboard.keycode == ALLEGRO_KEY_E) {
						ship.throttle[1] = 0;
					}
					if(event.keyboard.keycode == ALLEGRO_KEY_W) {
						ship.controls[5] = 0;
					}
					if(event.keyboard.keycode == ALLEGRO_KEY_S) {
						ship.controls[5] = -0;
					}
					break;
				case ALLEGRO_EVENT_DISPLAY_CLOSE:
					done = 1;
					break;
				case ALLEGRO_EVENT_MOUSE_AXES: {
					ship.throttle[0] += -event.mouse.dy * 0.001;
					ship.throttle[0] = clamp(ship.throttle[0], -1, 1);
					ship.throttle[2] += event.mouse.dx * 0.001;
					ship.throttle[2] = clamp(ship.throttle[2], -1, 1);
					ship.controls[0] = ship.throttle[0];
					ship.controls[2] = ship.throttle[2];
					break;
				}
			}
		}
		al_set_mouse_xy(display, 320, 240);
		double current_time = al_current_time();
		double dt = current_time - last_time;
		last_time = current_time;
		al_use_shader(shader);
   		al_set_shader_float("iTime", current_time);
		al_use_shader(NULL);

		float dr[3];
		vec3_hadamard(ship.throttle, ship.rates, dr);
		vec3_multf(dr, dt, dr); //Impulse
		vec3_addv(ship.momentum, dr, ship.momentum);
		vec3_multf(ship.momentum, dt, dr); //Update values
		rotate_local_axis(&camera, dr);

		ship.throttle[5] += ship.controls[5]*dt;
		ship.throttle[5] = clamp(ship.throttle[5], -1, 1);
		vec3_hadamard(ship.throttle+3, ship.rates+3, dr);
		vec3_multf(dr, dt, dr); //Impulse
		//TODO: Use local axis to affect momentum
		vec3_addv(ship.momentum+3, dr, ship.momentum+3);
		vec3_multf(ship.momentum+3, dt, dr); //Update values
		translate_camera(&camera, ship.momentum+3);

		update_crates(dt);

		al_clear_to_color(black);
		Render(m);
		al_flip_display();
	}

	destroy_static_model(m);
	al_destroy_event_queue(queue);

	return 0;
}
