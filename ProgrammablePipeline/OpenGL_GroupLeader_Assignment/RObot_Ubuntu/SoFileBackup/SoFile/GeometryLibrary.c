
#define IDBITMAP_H 100
#define IDBITMAP_C 101
#define IDBITMAP_O 102

//#define M_PI 3.1415
#include <stdio.h> // for FILE I/O
#include<stdlib.h>
#include<math.h>
#include<GL/glew.h>
#include<GL/gl.h>
#include<GL/glx.h>
#include<stdbool.h>
#include"GeometryLibrary.h"


FILE *gpFile = NULL;

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0
};

/******************************************
*                CYLINDER                *
******************************************/
GLuint vao_cylinder = 0;
GLuint vbo_position_cylinder = 0;
GLuint vbo_normal_cylinder = 0;
GLuint vbo_texture_cylinder = 0;
GLuint vbo_index_cylinder = 0;

float *vertices_cylinder, *normals_cylinder, *textures_cylinder;
unsigned short  *elements_cylinder;

unsigned int num_elements_cylinder = 0;
unsigned int num_vertices_cylinder = 0;

GLsizei num_parts_cylinder;
GLsizei num_vertices_per_part_cylinder;


/* CYLINDER */
void draw_cylinder();
void make_cylinder(double radius, double height, int slices, int stacks);
int get_index_count_cylinder();
int get_vertex_count_cylinder();

/******************************************
*                CYLINDER                *
******************************************/
int get_index_count_cylinder()
{
	/* code */
	return(num_elements_cylinder);
}

int get_vertex_count_cylinder()
{
	/* code */
	return(num_vertices_cylinder);
}

void generate_cylinder(float radius, float height, int slices, int stacks, float **vertices, float **normals, int *n_vert)
{
	/* function prototype */
	void circle_table(float **sint, float **cost, const int n, const bool half_circle);

	int i, j;
	int idx = 0; /* idx into vertex/normal buffer */
	float x, y, z;
	float ds = 1.0f / (float)(slices);
	float dt = 1.0f / (float)(stacks);
	float t = 1.0f;
	float s = 0.0f;
	const float z_step = height / ((stacks > 0) ? stacks : 1);

	/* Pre-computed circle */
	float *sint, *cost;

	/* code */
	/* number of unique vertices */
	if (slices == 0 || stacks < 1)
	{
		/* nothing to generate */
		*n_vert = 0;
		num_vertices_cylinder = *n_vert;
		return;
	}
	*n_vert = slices * (stacks + 3) + 2; /* need two extra stacks for closing off top and bottom with correct normals */
	num_vertices_cylinder = *n_vert;
	if ((*n_vert) > 65535)
	{
		printf("%d Too many slices or stacks requested, indices will wrap", __LINE__);
		return;
	}

	/* precompute values on unit circle */
	circle_table(&sint, &cost, -slices, false);

	/* Allocate vertex and normal buffers, bail out if memory allocation fails */
	*vertices = (float *)malloc((*n_vert) * 3 * sizeof(float));
	if (!(*vertices))
	{
		printf("%d Failed to allocate memory for vertices\n", __LINE__);
		return;
	}
	*normals = (float *)malloc((*n_vert) * 3 * sizeof(float));
	if (!(*normals))
	{
		printf("%d Failed to allocate memory for normals\n", __LINE__);
		free(*vertices);
		*vertices = NULL;
		return;
	}

	z = 0.0f;
	/* top on Z-axis */
	(*vertices)[0] = 0.0f;
	(*vertices)[1] = 0.0f;
	(*vertices)[2] = 0.0f;
	(*normals)[0] = 0.0f;
	(*normals)[1] = 0.0f;
	(*normals)[2] = -1.0f;
	idx = 3;

	/* other on top (get normals right) */
	for (j = 0; j < slices; j++, idx += 3)
	{
		x = cost[j];
		y = sint[j];

		(*vertices)[idx] = x * radius;
		(*vertices)[idx + 1] = y * radius;
		(*vertices)[idx + 2] = z;
		(*normals)[idx] = 0.0;
		(*normals)[idx + 1] = 0.0f;
		(*normals)[idx + 2] = -1.0f;
		s += ds;
	}

	/* each stack */
	for (i = 0; i < stacks + 1; i++)
	{
		s = 0.0f;
		t -= dt;

		for (j = 0; j < slices; j++, idx += 3)
		{
			x = cost[j];
			y = sint[j];

			(*vertices)[idx] = x * radius;
			(*vertices)[idx + 1] = y * radius;
			(*vertices)[idx + 2] = z;
			(*normals)[idx] = x;
			(*normals)[idx + 1] = y;
			(*normals)[idx + 2] = 0.0f;
			s += ds;
		}

		z += z_step;
	}

	/* other on bottom (get normals right) */
	z -= z_step;
	for (j = 0; j < slices; j++, idx += 3)
	{
		x = cost[j];
		y = sint[j];

		(*vertices)[idx] = x * radius;
		(*vertices)[idx + 1] = y * radius;
		(*vertices)[idx + 2] = z;
		(*normals)[idx] = 0.0;
		(*normals)[idx + 1] = 0.0f;
		(*normals)[idx + 2] = 1.0f;
		s += ds;
	}

	/* bottom */
	(*vertices)[idx] = 0.0f;
	(*vertices)[idx + 1] = 0.0f;
	(*vertices)[idx + 2] = height;
	(*normals)[idx] = 0.0f;
	(*normals)[idx + 1] = 0.0f;
	(*normals)[idx + 2] = 1.0f;

	/* Done creating vertices, release sin and cos tables */
	if (sint)
	{
		free(sint);
		sint = NULL;
	}
	if (cost)
	{
		free(cost);
		cost = NULL;
	}
}

void draw_cylinder()
{
	/* code */
	/* bind vao_cylinder */
	glBindVertexArray(vao_cylinder);

	/* draw */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index_cylinder);

	for (int i = 0; i < num_parts_cylinder; i++)
	{
		glDrawElements(GL_TRIANGLE_STRIP, num_vertices_per_part_cylinder, GL_UNSIGNED_SHORT, (GLvoid *)(sizeof(unsigned short) * i * num_vertices_per_part_cylinder));
	}

	/* unbind vao_cylinder */
	glBindVertexArray(0);
}

/*
* Draws a solid cylinder
*/
void make_cylinder(double radius, double height, int slices, int stacks)
{
	/* function prototype */
	void generate_cylinder(float radius, float height, int slices, int stacks, float **vertices, float **normals, int *n_vert);
	void prepare_to_draw_cylinder();
	void cleanup_cylinder_mesh();

	int i, j, idx;
	int n_vert;
	unsigned short offset;

	/* code */
	/* Generate vertices and normals */
	generate_cylinder(radius, height, slices, stacks, &vertices_cylinder, &normals_cylinder, &n_vert);

	if (n_vert == 0)
	{
		/* nothing to draw */
		return;
	}

	/* First, generate vertex index arrays for drawing with glDrawElement
	* All stacks, including top and bottom are covered with a triangle
	* strip.
	*/
	elements_cylinder = (unsigned short *)malloc((slices + 1) * 2 * (stacks + 2) * sizeof(unsigned short)); /* stacks +2 because of closing off bottom and top */
	if (!(elements_cylinder))
	{
		printf("%d Failed to allocate memory for elements_cylinder\n", __LINE__);
		cleanup_cylinder_mesh();
		return;
	}

	/* top stack */
	for (j = 0, idx = 0; j < slices; j++, idx += 2)
	{
		elements_cylinder[idx] = 0;
		elements_cylinder[idx + 1] = j + 1; /* 0 is top vertex, 1 is first for first stack */
	}
	elements_cylinder[idx] = 0; /* repeat first slice's idx for closing off shape */
	elements_cylinder[idx + 1] = 1;
	idx += 2;

	/* middle stacks: */
	/* Strip indices are relative to first index belonging to strip, NOT relative to first vertex/normal pair in array */
	for (i = 0; i < stacks; i++, idx += 2)
	{
		offset = 1 + (i + 1) * slices; /* triangle_strip indices start at 1 (0 is top vertex), and we advance one stack down as we go along */
		for (j = 0; j < slices; j++, idx += 2)
		{
			elements_cylinder[idx] = offset + j;
			elements_cylinder[idx + 1] = offset + j + slices;
		}
		elements_cylinder[idx] = offset; /* repeat first slice's idx for closing off shape */
		elements_cylinder[idx + 1] = offset + slices;
	}

	/* bottom stack */
	offset = 1 + (stacks + 2) * slices; /* triangle_strip indices start at 1 (0 is top vertex), and we advance one stack down as we go along */
	for (j = 0; j < slices; j++, idx += 2)
	{
		elements_cylinder[idx] = offset + j;
		elements_cylinder[idx + 1] = n_vert - 1; /* zero based index, last element in array (bottom vertex)... */
	}
	elements_cylinder[idx] = offset;
	elements_cylinder[idx + 1] = n_vert - 1; /* repeat first slice's idx for closing off shape */

	num_elements_cylinder = idx + 2;

	num_parts_cylinder = stacks + 2;
	num_vertices_per_part_cylinder = (slices + 1) * 2;

	prepare_to_draw_cylinder();
}

void prepare_to_draw_cylinder()
{
	/* vao_cylinder */
	glGenVertexArrays(1, &vao_cylinder);
	glBindVertexArray(vao_cylinder);

	/* vbo for position */
	glGenBuffers(1, &vbo_position_cylinder);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_cylinder);
	glBufferData(GL_ARRAY_BUFFER, (num_vertices_cylinder * 3 * sizeof(float)), vertices_cylinder, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0); /* Unbind with vbo_position */

									  /* vbo for normals */
	glGenBuffers(1, &vbo_normal_cylinder);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal_cylinder);
	glBufferData(GL_ARRAY_BUFFER, (num_vertices_cylinder * 3 * sizeof(float)), normals_cylinder, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0); /* Unbind with vbo_normal */

									  /* vbo for texture */
	glGenBuffers(1, &vbo_texture_cylinder);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_cylinder);
	glBufferData(GL_ARRAY_BUFFER, (num_vertices_cylinder * 2 * sizeof(float)), textures_cylinder, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); /* Unbind with vbo_texture */

									  /* vbo for index */
	glGenBuffers(1, &vbo_index_cylinder);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index_cylinder);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (num_elements_cylinder * sizeof(unsigned short)), elements_cylinder, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); /* Unbind with vbo_index */

	glBindVertexArray(0); /* Unbind with vao_sphere */
}


void cleanup_cylinder_mesh()
{
	/* cleanup allocated memory */
	if (elements_cylinder)
	{
		free(elements_cylinder);
		elements_cylinder = NULL;
	}
	if (vertices_cylinder)
	{
		free(vertices_cylinder);
		vertices_cylinder = NULL;
	}
	if (normals_cylinder)
	{
		free(normals_cylinder);
		normals_cylinder = NULL;
	}
	if (textures_cylinder)
	{
		free(textures_cylinder);
		textures_cylinder = NULL;
	}
}

void circle_table(float **sint, float **cost, const int n, const bool half_circle)
{
	int i;

	/* code */
	/* Table size, the sign of n flips the circle direction */
	const int size = abs(n);

	/* Determine the angle between samples */
	const float angle = (half_circle ? 1 : 2) * (float)M_PI / (float)((n == 0) ? 1 : n);

	/* Allocate memory for n samples, plus duplicate of first entry at the end */
	*sint = (float *)malloc(sizeof(float) * (size + 1));
	if (!(sint))
	{
		printf("%d Failed to allocate memory for sint\n", __LINE__);
		return;
	}
	*cost = (float *)malloc(sizeof(float) * (size + 1));
	if (!(cost))
	{
		printf("%d Failed to allocate memory for cost\n", __LINE__);
		free(*sint);
		*sint = NULL;
		return;
	}

	/* Compute cos and sin around the circle */
	(*sint)[0] = 0.0f;
	(*cost)[0] = 1.0f;

	for (i = 1; i < size; i++)
	{
		(*sint)[i] = (float)sin(angle * i);
		(*cost)[i] = (float)cos(angle * i);
	}

	if (half_circle)
	{
		(*sint)[size] = 0.0f;  /* sin PI */
		(*cost)[size] = -1.0f;  /* cos PI */
	}
	else
	{
		/* Last sample is duplicate of the first (sin or cos of 2 PI) */
		(*sint)[size] = (*sint)[0];
		(*cost)[size] = (*cost)[0];
	}
}
int Sphere(GLfloat r, int n, GLfloat **Vertices, GLfloat **Textures, GLfloat **Normals)
{
	int i, j;
	int NoOfVertices = 0;
	GLdouble phi1, phi2, theta, s, t;
	GLfloat ex, ey, ez, px, py, pz;
	
	*Vertices = (GLfloat *)malloc(3 * sizeof(GLfloat) * n * (n + 1) * 2);
	*Normals = (GLfloat *)malloc(3 * sizeof(GLfloat) * n * (n + 1) * 2);
	*Textures = (GLfloat *)malloc(2 * sizeof(GLfloat) * n * (n + 1) * 2);
	GLfloat *Vert = *Vertices;
	GLfloat *TexCoords = *Textures;
	GLfloat *NormalsSphere = *Normals;
	if (Vertices)
	{
		for (j = 0; j < n; j++)
		{

			phi1 = j * M_PI * 2 / n;
			phi2 = (j + 1) * M_PI * 2 / n;
			for (i = 0; i <= n; i++)
			{
				theta = i * M_PI / n;
				ex = sin(theta) * cos(phi2);
				ey = sin(theta) * sin(phi2);
				ez = cos(theta);
				px = r * ex;
				py = r * ey;
				pz = r * ez;

				NormalsSphere[(NoOfVertices * 3) + 0] = ex;
				NormalsSphere[(NoOfVertices * 3) + 1] = ey;
				NormalsSphere[(NoOfVertices * 3) + 2] = ez;

				s = phi2 / (M_PI * 2);
				t = 1 - (theta / M_PI);
				TexCoords[(NoOfVertices * 2) + 0] = s;
				TexCoords[(NoOfVertices * 2) + 1] = t;

				Vert[(NoOfVertices * 3) + 0] = px;
				Vert[(NoOfVertices * 3) + 1] = py;
				Vert[(NoOfVertices * 3) + 2] = pz;

				///////////////////////





				ex = sin(theta) * cos(phi1);
				ey = sin(theta) * sin(phi1);
				ez = cos(theta);
				px = r * ex;
				py = r * ey;
				pz = r * ez;
				NormalsSphere[(NoOfVertices * 3) + 3] = ex;
				NormalsSphere[(NoOfVertices * 3) + 4] = ey;
				NormalsSphere[(NoOfVertices * 3) + 5] = ez;


				//
				s = phi1 / (M_PI * 2);
				t = 1 - (theta / M_PI);
				TexCoords[(NoOfVertices * 2) + 2] = s;
				TexCoords[(NoOfVertices * 2) + 3] = t;

				//
				Vert[(NoOfVertices * 3) + 3] = px;
				Vert[(NoOfVertices * 3) + 4] = py;
				Vert[(NoOfVertices * 3) + 5] = pz;
				NoOfVertices = NoOfVertices + 2;
			}
		}
	}
	//fprintf(gpLogFile, "No of vertices %d\n", NoOfVertices);
	return NoOfVertices;

}

