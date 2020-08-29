#pragma once

#define SUCCESS 1

typedef struct _vec_int
{
	int *p;
	int size;
} vec_int;

vec_int * create_vec_int(void);
int       push_back_vec_int(vec_int *p_vec, int new_data);
void      show_vec_int(vec_int *p_vec);
int       destroy_vec_int(vec_int *p_vec); 


