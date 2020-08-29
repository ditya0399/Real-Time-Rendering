

#define SUCCESS 1

typedef struct _vec_float
{
	float *p;
	int size;
} vec_float;

vec_float * create_vec_float(void);
int       push_back_vec_float(vec_float *p_vec, float new_data);
void      show_vec_float(vec_float *p_vec);
int       destroy_vec_float(vec_float *p_vec);