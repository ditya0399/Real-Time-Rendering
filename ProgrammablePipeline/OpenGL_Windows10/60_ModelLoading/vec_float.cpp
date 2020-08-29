#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vec_float.h"

//int main()
//{
//    vec_float *p_vec_float = NULL;
//    p_vec_float = create_vec_float();
//
//    for(int i = 0; i < 1000; i++)
//    {
//        push_back_vec_float(p_vec_float, i*10.24);
//    }
//
//    show_vec_float(p_vec_float);
//    destroy_vec_float(p_vec_float);
//    p_vec_float = NULL;
//
//    return(0);
//}

vec_float *create_vec_float(void)
{
	vec_float *p_new = NULL;
	p_new = (vec_float *)malloc(sizeof(vec_float));

	if (p_new == NULL)
	{
		puts("alloc failed..");
		exit(-1);
	}

	memset(p_new, 0, sizeof(vec_float));
	return p_new;
}

int push_back_vec_float(vec_float *p_vec, float new_data)
{
	p_vec->p = (float *)realloc(p_vec->p, (p_vec->size + 1) * sizeof(float));
	if (p_vec->p == NULL)
	{
		puts("realloc failed..");
		exit(-1);
	}

	p_vec->size++;
	p_vec->p[p_vec->size - 1] = new_data;
	return SUCCESS;
}

void show_vec_float(vec_float *p_vec)
{
	for (int i = 0; i < p_vec->size; i++)
	{
		printf("p_vec->p[%d]: %g\n", i, p_vec->p[i]);
	}
}

int destroy_vec_float(vec_float *p_vec)
{
	free(p_vec->p);
	free(p_vec);
	p_vec = NULL;

	return SUCCESS;
}
