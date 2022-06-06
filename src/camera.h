typedef struct {
	float position[3];
	float rotation[3];
	float front[3];
	float right[3];
	float up[3];
	float quat_total[4];
} Camera;

void init_camera(Camera* c);
void apply_camera(Camera* c);
void rotate_local_axis(Camera* c, float v[3]);
void translate_camera(Camera* c, float v[3]);