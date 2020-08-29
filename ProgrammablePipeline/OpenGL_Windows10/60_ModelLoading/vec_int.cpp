#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vec_int.h"

//int main()
//{
//    vec_int *p_vec_int = NULL;
//    p_vec_int = create_vec_int();
//
//    for(int i = 0; i < 10; i++)
//    {
//        push_back_vec_int(p_vec_int, (i+1)*10);
//    }
//
//    show_vec_int(p_vec_int);
//    destroy_vec_int(p_vec_int);
//    p_vec_int = NULL;
//
//    return(0);
//}

vec_int *create_vec_int(void)
{
	vec_int *p_new = NULL;
	p_new = (vec_int *)malloc(sizeof(vec_int));

	if (p_new == NULL)
	{
		puts("alloc failed..");
		exit(-1);
	}

	memset(p_new, 0, sizeof(vec_int));
	return p_new;
}

int push_back_vec_int(vec_int *p_vec, int new_data)
{
	p_vec->p = (int *)realloc(p_vec->p, (p_vec->size + 1) * sizeof(int));
	if (p_vec->p == NULL)
	{
		puts("realloc failed..");
		exit(-1);
	}

	p_vec->size++;
	p_vec->p[p_vec->size - 1] = new_data;
	return SUCCESS;
}

void show_vec_int(vec_int *p_vec)
{
	for (int i = 0; i < p_vec->size; i++)
	{
		printf("p_vec->p[%d]: %d\n", i, p_vec->p[i]);
	}
}

int destroy_vec_int(vec_int *p_vec)
{
	free(p_vec->p);
	free(p_vec);
	p_vec = NULL;

	return SUCCESS;
}
