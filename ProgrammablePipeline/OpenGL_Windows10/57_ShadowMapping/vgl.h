#ifndef __VGL_H__
#define __VGL_H__



#include<glew.h>               // This SHOULD be above GL.h header.
#include<gl/GL.h>

#pragma comment(lib,"opengl32.lib")//for linking
#pragma comment(lib,"glew32.lib")//for linking




//#  ifdef _DEBUG
//#     pragma comment (lib, "freeglut_staticd.lib")
//#  else
//#     pragma comment (lib, "freeglut_static.lib")
//#  endif

#define BUFFER_OFFSET(x)  ((const void*) (x))
#endif /* __VGL_H__ */
