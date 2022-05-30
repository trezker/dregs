typedef struct {
	float rotation[3];
} Camera;

void init_camera(Camera* c);
void apply_camera(Camera* c);