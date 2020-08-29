#pragma once


/* Before using this header file. Call createStackFile in WM_CREATE: to open the Log file for stack.
  after that fClose in WM_DESTROY or uninitialize() function  */
#include<stdio.h>
#include"vmath.h"


FILE *stackFile;
using namespace vmath;

#define STACK_SIZE 10

mat4 stack[STACK_SIZE];

int start = -1;

void push(mat4 Matrix);
mat4 pop();
mat4 peek();
void resetStack();
void createStackFile()
{
	fopen_s(&stackFile, "StackFile.txt", "w");
}
void push(mat4 Matrix)
{
	

	if (start == (STACK_SIZE - 1))
	{
		fprintf_s(stackFile, "StackOverFlow\n");
	}
	else
	{
		start = start + 1;
		stack[start] = Matrix;
	}
}

mat4 pop()
{
	mat4 Matrix;
	if (start > -1)
	{
		Matrix = stack[start];
		start = start - 1;
	}
	else
	{
		fprintf(stackFile, "Stack Underflow\n");

	}
	return Matrix;
}

mat4 peek()
{
	if (start == -1)
	{
	//	return stack[start];

	}
	else
	{
		return stack[start];
	}
}
void resetStack()
{
	start = -1;
}


