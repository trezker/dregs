typedef struct {
	int16_t num_vertices;
	int16_t num_indices;
	int16_t smooth;
	float* vertices;
	float* normals;
	float* uvs;
	int16_t* faces;
} Static_model;

Static_model* load_static_model(char* path);
void destroy_static_model(Static_model* model);
void render_static_model(Static_model* m);