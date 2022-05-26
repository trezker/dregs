typedef struct {
	int num_vertices;
	int num_indices;
	int smooth;
	float* vertices;
	float* normals;
	float* uvs;
	int* faces;
} Static_model;

Static_model* load_static_model(char* path);
void destroy_static_model(Static_model* model);