float vec3_length(float v[3]);
void normalize_vec3(float* v);
void vec3_cross(float* in1, float* in2, float* out);
float vec3_dot(float* a, float* b);
void vec3_subv(float* a, float* b, float* out);
void vec3_multf(float* in, float m, float* out);
void vec3_addv(float* a, float* b, float* out);
void vec3_hadamard(float* a, float* b, float* out);