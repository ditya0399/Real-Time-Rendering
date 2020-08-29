#include<Windows.h>
#include<glew.h>               // This SHOULD be above GL.h header.
#include<gl/GL.h>
#include"Sphere.h"
#include<stdio.h>
#include"vmath.h"

#pragma comment(lib,"opengl32.lib")//for linking
#pragma comment(lib,"glew32.lib")//for linking
#pragma comment(lib,"Sphere.lib")


//Global variables for FrameBuffer

// Sphere

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

int gNumVertices;
int gNumElements;

//
GLuint vao_Sphere;
GLuint vbo_position_Sphere;
GLuint vbo_Normals_Sphere;
GLuint vbo_Element_Sphere;



GLuint gVertexShaderObject_FBO;
GLuint gFragmentShaderObject_FBO;
GLuint gShaderProgramObject_FBO;
GLuint gMVPUniform_FBO;
GLuint SamplerUniform_FBO;

GLuint FBO; //our framebuffer object
GLuint Texture_FBO; //its texture

GLuint gVbo_Texture; //texture vbo for cube


int windowWidth;
int windowHeight;


int Win_Width = 800;
int Win_Height = 600;
using namespace vmath;

static GLfloat angle = 0.0f;
GLuint gShaderProgramObject;
GLfloat angleTriangle = 0.0f;
GLfloat angleRectangle = 0.0f;
enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0
};

//
GLuint vao_triangle;
GLuint vao_rectangle;
GLuint vbo_position_triangle;
GLuint vbo_position_rectangle;
GLuint vbo_color_rectangle;
GLuint vbo_color_triangle;
GLuint mvpUniform;
mat4 perspectiveProjectionMatrix;


//

HDC ghdc = NULL;
HGLRC ghglrc = NULL;
HWND ghwnd;
HWND hwnd;
FILE *gpLogFile = NULL;
WINDOWPLACEMENT gwpprev = { sizeof(WINDOWPLACEMENT) };
DWORD gdwStyle = 0;
bool gbIsActiveWindow = false;
bool gbIsFullScreen = false;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void ToggleFullScreen(void);
void update(void);
void uninitialize(void);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{
	int initialize(void);

	//Variable Declarations
	bool bIsDone = false;
	int iRet = 0;
	TCHAR szAppName[] = TEXT("perspective");
	WNDCLASSEX wndclass;
	MSG msg;
	void display(void);

	if (fopen_s(&gpLogFile, "Log.TXT", "w") != 0)
	{
		MessageBox(NULL, TEXT("LOG FILE WAS NOT CREATED"), TEXT("ERROR"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf_s(gpLogFile, "Log File Created\n");
	}

	//andclassinitialization
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;// | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (RegisterClassEx(&wndclass) == 0)
	{
		//Failed to register wndclassex
	}

	//CreateWindow
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("2DShapes!"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		100,
		100,
		Win_Width, Win_Height,
		NULL,
		NULL,
		hInstance,
		0);

	ghwnd = hwnd;

	iRet = initialize();
	if (iRet == -1)
	{
		fprintf_s(gpLogFile, "choosePixekFormat() Failed\n");
		DestroyWindow(hwnd);
	}
	else if (iRet == -2)
	{
		fprintf_s(gpLogFile, "SetPixelFormat() Failed\n");
		DestroyWindow(hwnd);
	}
	else if (iRet == -3)
	{
		fprintf_s(gpLogFile, "wglCreateContext() Failed\n");
		DestroyWindow(hwnd);
	}
	else if (iRet == -4)
	{
		fprintf_s(gpLogFile, "wglMakeCurrent() Failed\n");
		DestroyWindow(hwnd);
	}
	else
	{
		fprintf_s(gpLogFile, "Initialization successfull\n");
	}

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	SetFocus(hwnd);

	//gameloop
	while (bIsDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bIsDone = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbIsActiveWindow == true)
			{
				//Here actually play the game. 

			}
			/*Here actually call Display,As this is Double Buffer Program. No need of WM_PAINT*/

			display();
		}
	}
	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	void resize(int, int);


	switch (iMsg)
	{
	case WM_CREATE:

		break;

	case WM_SETFOCUS:
		gbIsActiveWindow = true;
		break;

	case WM_KILLFOCUS:
		gbIsActiveWindow = false;
		break;

	case WM_SIZE:
		windowWidth = LOWORD(lParam);
		windowHeight = HIWORD(lParam);
		resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_ERASEBKGND:
		return(0);

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_KEYDOWN:

		switch (wParam)
		{
		case VK_ESCAPE:
			ToggleFullScreen();
			DestroyWindow(hwnd);
			break;

		case 'F':
		case 'f':
			ToggleFullScreen();
			break;
		}
		break;

	case WM_DESTROY:
		uninitialize();
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

int initialize(void)
{
	GLuint gVertexShaderObject;
	GLuint gFragmentShaderObject;


	GLenum result;
	void resize(int, int);

	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	memset((void*)&pfd, NULL, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;
	ghdc = GetDC(ghwnd);

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		return(-1);
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		return(-2);
	}

	ghglrc = wglCreateContext(ghdc);

	if (ghglrc == NULL)
	{
		return(-3);
	}

	if (wglMakeCurrent(ghdc, ghglrc) == FALSE)
	{
		return(-4);
	}
	result = glewInit();
	if (result != GLEW_OK)
	{
		fprintf(gpLogFile, "ERROR : glewInit FAILED!!!\n");
		uninitialize();
		DestroyWindow(hwnd);
	}



	//////////////////////////////// V E R T E X - S H A D E R //////////////////////////


	//Define Vertex Shader Object

	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar **p;
	const GLchar *vertexShaderSourceCode =

		"#version 430 core" \
		"\n"
		"in vec4 vPosition;" \
		"in vec4 vColor;" \
		"out vec4 out_color;" \
		"uniform mat4 u_mvp_matrix;" \

		"void main(void)" \
		"{" \
		"gl_Position = u_mvp_matrix * vPosition;" \
		"out_color = vColor;" \
		"}";
	// GPU will run the above code. And GPU WILL RUN FOR PER VERTEX. If there are 1000 vertex. Then GPU will run this shader for
	//1000 times. We are Multiplying each vertex with the Model View Matrix.
	//And how does the GPU gets to know about at what offset the array has to be taken . Go to glVertexAttribPointer() in Display.
	// in = Input. 

	//p = &vertexShaderSourceCode;
		//Specify above source code to the vertex shader object
	glShaderSource(gVertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);

	//Compile the vertex shader 
	glCompileShader(gVertexShaderObject);

	//////////////// Error Checking//////////////////
	//Code for catching the errors 
	GLint iShaderCompileStatus = 0;
	GLint iInfoLogLength = 0;
	GLchar *szInfoLog = NULL;


	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpLogFile, "%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}
	/////////////////    F R A G M E N T S H A D E R            //////////////////////////
	//Define Vertex Shader Object
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar *fragmentShaderSourceCode =
		"#version 430 core" \
		"\n" \
		"in vec4 out_color;" \
		"out vec4 FragColor;" \

		"void main(void)" \
		"{" \
		"FragColor = vec4(1.0,1.0,1.0,1.0);" \
		"}";

	//FragColor = vec4(1,1,1,1) = White Color
	//this means here we are giving color to the Triangle.


	//Specify above source code to the vertex shader object
	glShaderSource(gFragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);

	//Compile the vertex shader 
	glCompileShader(gFragmentShaderObject);
	//Code for catching the errors 
		   /*iShaderCompileStatus = 0;
		   iInfoLogLength = 0;*/
	szInfoLog = NULL;


	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written1;
				glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written1, szInfoLog);
				fprintf(gpLogFile, "%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}
	// CREATE SHADER PROGRAM OBJECT
	gShaderProgramObject = glCreateProgram();
	//attach vertex shader to the gShaderProgramObject
	glAttachShader(gShaderProgramObject, gVertexShaderObject);

	//attach fragment shader to the gShaderProgramObject
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);


	//Pre-Linking  binding to vertexAttributes
	glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_COLOR, "vColor");
	//Here the above line means that we are linking the GPU's variable vPosition with the CPU's  enum member  i.e AMC_ATTRIBUTE_POSITION .
	//So whatever changes will be done in AMC_ATTRIBUTE_POSITION , those will also reflect in vPosition

	//RULE : ALWAYS BIND THE ATTRIBUTES BEFORE LINKING AND BIND THE UNIFORM AFTER LINKING.

	//Link the shader program 
	glLinkProgram(gShaderProgramObject);

	//Code for catching the errors 
	GLint iProgramLinkStatus = 0;



	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written3;
				glGetProgramInfoLog(gShaderProgramObject, iInfoLogLength, &written3, szInfoLog);
				fprintf(gpLogFile, "%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}

	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();
	//POST Linking
	//Retrieving uniform locations 
	mvpUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
	//Here we have done all the preparations of data transfer from CPU to GPU
	const GLfloat triangleVertices[] =
	{
		// Perspective triangle (Front face)
		0.0f, 1.0f, 0.0f,		// Apex
		-1.0f, -1.0f, 1.0f,		// Left bottom
		1.0f, -1.0f, 1.0f,		// Right bottom
								// Perspective triangle (Right face)
		0.0f, 1.0f, 0.0f,		// Apex
		1.0f, -1.0f, 1.0f,		// Left bottom
		1.0f, -1.0f, -1.0f,		// Right bottom
								// Perspective triangle (Back face)
		0.0f, 1.0f, 0.0f,		// Apex
		1.0f, -1.0f, -1.0f,		// Left bottom
		-1.0f, -1.0f, -1.0f,	// Right bottom
								// Perspective triangle (Left face)
		0.0f, 1.0f, 0.0f,		// Apex
		-1.0f, -1.0f, -1.0f,	// Left bottom
		-1.0f, -1.0f, 1.0f
	};
	const GLfloat triangleColors[] =
	{
		1.0f, 0.0f, 0.0f,		// Red apex
		0.0f, 1.0f, 0.0f,		// Green left bottom
		0.0f, 0.0f, 1.0f,		// Blue right bottom
		1.0f, 0.0f, 0.0f,		// Red apex
		0.0f, 0.0f, 1.0f,		// Blue right bottom
		0.0f, 1.0f, 0.0f,		// Green left bottom
		1.0f, 0.0f, 0.0f,		// Red apex
		0.0f, 1.0f, 0.0f,		// Green left bottom
		0.0f, 0.0f, 1.0f,		// Blue right bottom
		1.0f, 0.0f, 0.0f,		// Red apex
		0.0f, 0.0f, 1.0f,		// Blue right bottom
		0.0f, 1.0f, 0.0f,
	};

	const GLfloat rectangleVertices[] =
	{
		// Perspective square (Top face)
		1.0f, 1.0f, -1.0f,		// Right top
		-1.0f, 1.0f, -1.0f, 	// Left top
		-1.0f, 1.0f, 1.0f,		// Left bottom
		1.0f, 1.0f, 1.0f,		// Right bottom
								// Perspective square (Bottom face)
		1.0f, -1.0f, -1.0f,		// Right top
		-1.0f, -1.0f, -1.0f, 	// Left top
		-1.0f, -1.0f, 1.0f,		// Left bottom
		1.0f, -1.0f, 1.0f,		// Right bottom
								// Perspective square (Front face)
		1.0f, 1.0f, 1.0f,		// Right top
		-1.0f, 1.0f, 1.0f,		// Left top
		-1.0f, -1.0f, 1.0f, 	// Left bottom
		1.0f, -1.0f, 1.0f,		// Right bottom
								// Perspective square (Back face)
		1.0f, 1.0f, -1.0f,		// Right top											
		-1.0f, 1.0f, -1.0f,		// Left top
		-1.0f, -1.0f, -1.0f, 	// Left bottom
		1.0f, -1.0f, -1.0f,		// Right bottom
								// Perspective square (Right face)
		1.0f, 1.0f, -1.0f,		// Right top											
		1.0f, 1.0f, 1.0f,		// Left top
		1.0f, -1.0f, 1.0f, 		// Left bottom
		1.0f, -1.0f, -1.0f,		// Right bottom
								// Perspective square (Left face)
		-1.0f, 1.0f, 1.0f,		// Right top																						
		-1.0f, 1.0f, -1.0f,		// Left top
		-1.0f, -1.0f, -1.0f, 	// Left bottom
		-1.0f, -1.0f, 1.0f		// Right bottom


	};



	const GLfloat cubeTexcoords[] =
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};
	glGenVertexArrays(1, &vao_Sphere);
	glBindVertexArray(vao_Sphere);
	glGenBuffers(1, &vbo_position_Sphere);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_Sphere);
	//in the above statement we have accesed the GL-ARRAY_BUFFER using vbo. Without it wouldn't be possible to get GL_ARRAY_BUFFER
	//For the sake of understanding . GL_ARRAY_BUFFER is in the GPU side ad=nd we have bind our CPU side vbo with it like a Pipe to get the access.
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	//The above statement states that , we are passing our vertices array to the GPU and GL_STATIC_DRAW means draw it now only. Don't draw it in Runtime. 
	//The below statement states that after storing the data in the GPU'S buffer . We are passing it to the vPosition now . 

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//GL_FALSE = We are not giving normalized coordinates as our coordinates are not converted in 0 - 1 range.
	//3 = This is the thing I was talking about in initialize. Here, we are telling GPU to break our array in 3 parts . 
	//0 and Null are for the Interleaved. 
	//GL_FLOAT- What is the type? .
	//AMC_ATTRIBUTE_POSITION. here we are passing data to vPosition. 

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Elements
	glGenBuffers(1, &vbo_Element_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	////Color 
	//glGenBuffers(1, &vbo_Normals_Sphere);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo_Normals_Sphere);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	//glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	///////////////// FRAME BUFFER ////////////////////


	// *** Cube Block ***

		// *** VERTEX SHADER ***
		// create shader
	gVertexShaderObject_FBO = glCreateShader(GL_VERTEX_SHADER);

	// provide source code to shader
	const GLchar *vertexShaderSourceCodeFBO =
		"#version 430 core" \
		"\n" \
		"in vec2 vTexture0_Coord;"\
		"in vec4 vPosition;" \
		"uniform mat4 u_mvp_matrix;" \
		"out vec2 out_texture0;"\
		"void main(void)" \
		"{" \
		"gl_Position = u_mvp_matrix * vPosition;" \
		"out_texture0 = vTexture0_Coord;"\
		"}";
	glShaderSource(gVertexShaderObject_FBO, 1, (const GLchar **)&vertexShaderSourceCodeFBO, NULL);

	// compile shader
	glCompileShader(gVertexShaderObject_FBO);
	// compile error checking
	iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	szInfoLog = NULL;
	glGetShaderiv(gVertexShaderObject_FBO, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_FBO, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject_FBO, iInfoLogLength, &written, szInfoLog);
				fprintf(gpLogFile, "Vertex Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
	// *** FRAGMENT SHADER ***
	   // create shader
	gFragmentShaderObject_FBO = glCreateShader(GL_FRAGMENT_SHADER);

	// provide source code to shader
	const GLchar *fragmentShaderSourceCodeFBO =
		"#version 430 core" \
		"\n" \
		"in vec2 out_texture0;" \
		"out vec4 FragColor;" \
		"uniform sampler2D fboTextureSampler;" \
		"void main(void)"\
		"{"\
		"FragColor = texture(fboTextureSampler, out_texture0);" \
		"}";

	//"FragColor = texture(fboTextureSampler, out_texture0);" \
	
	glShaderSource(gFragmentShaderObject_FBO, 1, (const GLchar **)&fragmentShaderSourceCodeFBO, NULL);

	// compile shader
	glCompileShader(gFragmentShaderObject_FBO);
	// compile error checking
	glGetShaderiv(gFragmentShaderObject_FBO, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_FBO, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject_FBO, iInfoLogLength, &written, szInfoLog);
				fprintf(gpLogFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
	// *** SHADER PROGRAM ***
	  // create
	gShaderProgramObject_FBO = glCreateProgram();

	// attach vertex shader to shader program
	glAttachShader(gShaderProgramObject_FBO, gVertexShaderObject_FBO);

	// attach fragment shader to shader program
	glAttachShader(gShaderProgramObject_FBO, gFragmentShaderObject_FBO);

	// pre-link binding of shader program object with vertex shader position attribute
	glBindAttribLocation(gShaderProgramObject_FBO, AMC_ATTRIBUTE_POSITION, "vPosition");
	// pre-link binding of texture
	glBindAttribLocation(gShaderProgramObject_FBO, AMC_ATTRIBUTE_TEXCOORD0, "vTexture0_Coord");

	// link shader
	glLinkProgram(gShaderProgramObject_FBO);

	// link error check
	iInfoLogLength = 0;
	iShaderCompiledStatus = 0;
	szInfoLog = NULL;

	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(gShaderProgramObject_FBO, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
	
		glGetProgramiv(gShaderProgramObject_FBO, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject_FBO, iInfoLogLength, &written, szInfoLog);
				fprintf(gpLogFile, "Shader Program Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	// get MVP uniform location
	gMVPUniform_FBO = glGetUniformLocation(gShaderProgramObject_FBO, "u_mvp_matrix");
	// send the sampler
	SamplerUniform_FBO = glGetUniformLocation(gShaderProgramObject_FBO, "fboTextureSampler");




	//Basically we are giving the vertices coordinates in the above array
	////////////////////FOR TRIANGLE ///////////////// 
	//Create vao - Vertex array objects


	//////////////////////// FOR RECTANGLE //////////////// 

	glGenVertexArrays(1, &vao_rectangle);
	glBindVertexArray(vao_rectangle);
	glGenBuffers(1, &vbo_position_rectangle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_rectangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Texture 
	glGenBuffers(1, &gVbo_Texture);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexcoords), cubeTexcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	// *** Framebuffer Object ***
	   // Create a Framebuffer Object
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// create texture for our color buffer
	glGenTextures(1, &Texture_FBO);
	glBindTexture(GL_TEXTURE_2D, Texture_FBO);
	glTexStorage2D(GL_TEXTURE_2D, 9, GL_RGBA8, 512, 512);

	//turn off mipmaps
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// now attach color textures to the FBO
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, Texture_FBO, 0);

	GLenum fboStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (fboStatus == GL_FRAMEBUFFER_COMPLETE)
		fprintf(gpLogFile, "fbo status %d\n", fboStatus);


	// Tell OpenGL we want to draw into framebuffer's first color attachment
	static const GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, draw_buffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);





	//Depth Lines
	glClearDepth(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//initializing Perspective ProjectionMatrix to identity.
	perspectiveProjectionMatrix = mat4::identity();

	resize(windowWidth, windowHeight);

	return(0);
}

void resize(int width, int height)
{


	if (height == 0)
	{
		height = 1;
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	/*if (width < height)
	{
		orthographicProjectionMatrix = ortho(
			-100.0f, 100.0f,
			-100.0f * ((float)height / (float)width), 100.0f * ((float)height / (float)width),
			-100.0f, 100.0f);
	}
	else
	{
		orthographicProjectionMatrix = ortho(
			-100.0f * ((float)width / (float)height), 100.0f * ((float)width / (float)height),
			-100.0f, 100.0f,
			-100.0f, 100.0f);
	}*/
	perspectiveProjectionMatrix = perspective(45.0f,
		(GLfloat)width / (GLfloat)height,
		0.1f,
		100.0f);

}



void display(void)
{
	// now render the cube into our application framebuffer
	   // set the viewport of same size as of texture of framebuffer which we have mentioned in glTexStorage2D()
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glViewport(0, 0, 512, 512);
	glClearColor(0.50f, 0.50f, 0.50f, 0.0f);	//black

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use shader program
	glUseProgram(gShaderProgramObject);

	////declaration of matrices
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 RotationMatrix;
	mat4 TranslateMatrix;
	mat4 ScaleMatrix;

	// intialize above matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	ScaleMatrix = mat4::identity();


	modelViewMatrix = translate(0.0f, 0.15f, -4.0f);
	//modelViewMatrix = modelViewMatrix * TranslateMatrix;
	RotationMatrix = rotate(angleTriangle, 0.0f, 1.0f, 0.0f);
	// perform necessary transformations
	modelViewMatrix = modelViewMatrix;
	// do necessary matrix multiplication
	// this was internally done by gluPerspective() in FFP.	

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	// send necessary matrices to shader in respective uniforms
	glUniformMatrix4fv(gMVPUniform_FBO, 1, GL_FALSE, modelViewProjectionMatrix);
	//GL_FALSE = Should we transpose the matrix?
	//DirectX is roj major so there we will have to transpose but OpenGL is Colomn major so no need to transpose. 



	// bind with vao

	////////////////// TRIANGLE ////////////////////

	//this will avoid many binding to vbo
	//glBindVertexArray(vao_triangle);

	//// bind with textures

	//// draw necessary scene
	//glDrawArrays(GL_TRIANGLES, 0, 12);
	//  0 =  From where to start in the array. 
	// We have to start from 0th element i.e 0. if the 0th element would be 50.0f then we would have given the 2nd parameter as 50.
	//3 = How many Vertices? 
	//GL_TRIANGLES is the thing between glBegin() and glEnd()

	glBindVertexArray(vao_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	// unbind vao
	//glBindVertexArray(0);
	glUseProgram(0);


	//// now bind with default framebuffer and render cube in it
	//	// Cube Block ///////////////////////////
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	//black
	glViewport(0, 0, windowWidth, windowHeight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(gShaderProgramObject_FBO);

	////////  RECTANGLE //////////////
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	ScaleMatrix = mat4::identity();
	modelViewMatrix = translate(0.0f, 0.0f, -5.0f);
	ScaleMatrix = scale(0.75f, 0.75f, 0.75f);
	modelViewMatrix = modelViewMatrix;

	RotationMatrix = rotate(angleRectangle, angleRectangle, angleRectangle);
	modelViewMatrix = modelViewMatrix * RotationMatrix;

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture_FBO);
	// pass texture
	glUniform1i(SamplerUniform_FBO, 0);

	glBindVertexArray(vao_rectangle);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	glBindVertexArray(0);
	// unuse program
	glUseProgram(0);
	SwapBuffers(ghdc);
	angleTriangle = angleTriangle + 0.2f;
	angleRectangle = angleRectangle + 0.2f;
}

void uninitialize(void)
{
	if (vbo_position_triangle)
	{
		glDeleteBuffers(1, &vbo_position_triangle);
		vbo_position_triangle = 0;
	}

	if (vao_triangle)
	{
		glDeleteBuffers(1, &vao_triangle);
		vao_triangle = 0;
	}
	if (vbo_position_rectangle)
	{
		glDeleteBuffers(1, &vbo_position_rectangle);
		vbo_position_rectangle = 0;
	}

	if (vao_rectangle)
	{
		glDeleteBuffers(1, &vao_rectangle);
		vao_rectangle = 0;
	}


	if (gShaderProgramObject)
	{
		GLsizei shaderCount;
		GLsizei shaderNumber;

		glUseProgram(gShaderProgramObject);
		glGetProgramiv(gShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);

		GLuint *pShaders = (GLuint *)malloc(sizeof(GLuint) * shaderCount);
		if (pShaders)
		{
			glGetAttachedShaders(gShaderProgramObject, shaderCount, &shaderCount, pShaders);

			for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++)
			{
				// detach shader
				glDetachShader(gShaderProgramObject, pShaders[shaderNumber]);

				// delete shader
				glDeleteShader(pShaders[shaderNumber]);
				pShaders[shaderNumber] = 0;
			}
			free(pShaders);
		}

		glDeleteProgram(gShaderProgramObject);
		gShaderProgramObject = 0;
		glUseProgram(0);

	}
	if (gbIsFullScreen == true)
	{
		SetWindowLong(ghwnd, GWL_STYLE, gdwStyle | WS_OVERLAPPEDWINDOW);

		SetWindowPlacement(ghwnd, &gwpprev);

		SetWindowPos(ghwnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
		ShowCursor(TRUE);
	}

	if (wglGetCurrentContext() == ghglrc)
	{
		wglMakeCurrent(NULL, NULL);
		if (ghglrc)
		{
			wglDeleteContext(ghglrc);
			ghglrc = NULL;
		}
	}

	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (gpLogFile)
	{
		fprintf_s(gpLogFile, "Log File Closed");
		fclose(gpLogFile);
	}
}


void ToggleFullScreen()
{
	MONITORINFO MI;

	if (gbIsFullScreen == false)
	{
		gdwStyle = GetWindowLong(ghwnd, GWL_STYLE);

		if (gdwStyle & WS_OVERLAPPEDWINDOW)
		{
			MI = { sizeof(MONITORINFO) };

			if (
				GetWindowPlacement(ghwnd, &gwpprev)
				&&
				GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &MI)
				)
			{
				SetWindowLong(ghwnd, GWL_STYLE, gdwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP,
					MI.rcMonitor.left,
					MI.rcMonitor.top,
					MI.rcMonitor.right - MI.rcMonitor.left,
					MI.rcMonitor.bottom - MI.rcMonitor.top,
					SWP_NOZORDER | WS_OVERLAPPED);
			}
		}
		ShowCursor(FALSE);
		gbIsFullScreen = true;
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, gdwStyle | WS_OVERLAPPEDWINDOW);

		SetWindowPlacement(ghwnd, &gwpprev);


		SetWindowPos(ghwnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
		ShowCursor(TRUE);
		gbIsFullScreen = false;
	}

}

void update(void)
{


}