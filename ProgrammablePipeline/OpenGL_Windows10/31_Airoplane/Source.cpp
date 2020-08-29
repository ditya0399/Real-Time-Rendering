#include<Windows.h>
#include<glew.h>               // This SHOULD be above GL.h header.
#include<gl/GL.h>
#include<math.h>
#include<stdio.h>
#include"vmath.h"
#include"Sound.h"

#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"opengl32.lib")//for linking
#pragma comment(lib,"glew32.lib")//for linking
#define PI 3.14159

GLfloat Red = 0.0f;
GLfloat Orange = 0.0f;
GLfloat Green = 0.0f;





//airoplane variables
//for UpAiroplane
GLfloat x = -4.5f;
GLfloat y = 1.5f;
GLfloat n = 2.5f;
GLfloat i = -3.5f;
GLfloat r = 0.0f;
GLfloat g = -0.5f;
GLfloat b = 0.0f;
GLfloat midx = -4.0f;
GLfloat angleDown = 0.0f;
GLfloat angleup = 0.0f;
GLfloat upangle = 320.0f;
GLfloat dangle = 50.0f;
GLfloat angled1 = (2 * PI) / 1.3f;
GLfloat dangle1 = 0.0f;
GLfloat angleup1 = (PI / 2.0f);
GLfloat upangle1 = 360.0f;
GLfloat r1 = 1.0f;
GLfloat r12 = 0.5f;
GLfloat w = 1.0f;
GLfloat g1 = 1.0f;

int counter = 0;
float translateI1 = -3.4f;
float translateI2 = -3.3f;
float translateN = 3.5f;
float translateD;
float translateA = 3.5f;





int Win_Width = 800;
int Win_Height = 600;
using namespace vmath;

GLuint gShaderProgramObject;

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0
};

//
GLuint vao_triangle;
GLuint vbo_color_triangle;
GLuint vbo_position_triangle;

GLuint vao_N;
GLuint vbo_position_N;
GLuint vbo_color_N;

GLuint vao_D;
GLuint vbo_position_D;
GLuint vbo_color_D;

GLuint vao_I;
GLuint vbo_position_I;
GLuint vbo_color_I;

GLuint vao_A;
GLuint vbo_position_A;
GLuint vbo_color_A;


GLuint vao_Fire;
GLuint vbo_Fire;
GLuint vbo_FireColors;

GLuint vao_Fire1;
GLuint vbo_Fire1;
GLuint vbo_FireColors1;

GLuint vao_Airoplane;
GLuint vbo_position_Airoplane;

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
	ToggleFullScreen();
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
			counter = counter++;
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
		"FragColor =  out_color;" \
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


	//POST Linking
	//Retrieving uniform locations 
	mvpUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
	//Here we have done all the preparations of data transfer from CPU to GPU

	const GLfloat IVertices[] =
	{
		-1.0f, 1.0f,0.0f,
		-1.0f, -1.0f,0.0f

	};
	const GLfloat IColors[] =
	{
		1.0f, 0.5f, 0.0f,
		0.0f, 1.0f, 0.0f
	};
	const GLfloat NVertices[] =
	{
		-0.7f, 1.0f,0.0f,
		-0.7f, -1.0f,0.0f,
		-0.7f, 1.0f,0.0f,
		-0.2f, -1.0f,0.0f,
		-0.2f, 1.0f,0.0f,
		-0.2f, -1.0f,0.0f

	};
	const GLfloat NColor[] =
	{
		1.0f, 0.5f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 0.5f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 0.5f, 0.0f,
		0.0f, 1.0f, 0.0f

	};

	const GLfloat DVertices[] =
	{
		-0.3f, 1.0f,0.0f,
		0.25f, 1.0f,0.0f,
		0.25f, 1.0f,0.0f,
		0.25f, -1.0f,0.0f,
		-0.3f, -1.0f,0.0f,
		0.25f, -1.0f,0.0f
	};
	const GLfloat DColor[] =
	{
		1.0f, 0.5f, 0.0f,
		1.0f, 0.5f, 0.0f,
		1.0f, 0.5f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 0.5f, 0.0f,
		0.0f, 1.0f, 0.0f

	};
	const GLfloat AVertices[] =
	{
		0.7f, 1.0f,0.0f,
		0.3f,-1.0f,0.0f,
		0.7f,1.0f,0.0f,
		1.1f,-1.0f,0.0f,

		0.5f,0.0f,0.0f,
		0.9f,0.0f,0.0f,
		0.5f,-0.02f,0.0f,
		0.9f,-0.02f,0.0f,
		0.5f,-0.04f,0.0f,
		0.9f,-0.04f,0.0f

	};
	const GLfloat AColor[] =
	{
		1.0f,0.5f,0.0f,
		0.0f,1.0f,0.0f,
		1.0f,0.5f,0.0f,
		0.0f,1.0f,0.0f,

		1.0f,0.5f,0.0f,
		1.0f,0.5f,0.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,

	};
	const GLfloat Airoplane[] = 
	{
		1.0f, -0.1f, 0.0f,
		0.85f, 0.0f, 0.0f,
		0.85f, 0.0f, 0.0f,
		0.85f, -0.2f, 0.0f,
		0.85f, -0.2f, 0.0f,
		1.0f, -0.1f, 0.0f,
		0.85f, 0.0f, 0.0f,
		0.5f, 0.05f, 0.0f,
		0.5f, 0.05f, 0.0f,
		0.5f, -0.25f, 0.0f,
		0.5f, -0.25f, 0.0f,
		0.85f, -0.2f, 0.0f,
		0.5f, -0.35f, 0.0f,
		0.5f, 0.15f, 0.0f,
		0.5f, 0.15f, 0.0f,
		0.25f, 0.15f, 0.0f,
		0.25f, 0.15f, 0.0f,
		0.25f, -0.35f, 0.0f,
		0.25f, -0.35f, 0.0f,
		0.5f, -0.35f, 0.0f,
		0.25f, 0.15f, 0.0f,
		0.0f, 0.55f, 0.0f,
		0.0f, 0.55f, 0.0f,
		0.0f, 0.15f, 0.0f,
		0.0f, 0.55f, 0.0f,
		0.25f, 0.15f, 0.0f,
		0.0f, 0.55f, 0.0f,
		0.15f, 0.7f, 0.0f,
		0.15f, 0.7f, 0.0f,
		-0.1f, 0.7f, 0.0f,
		-0.1f, 0.7f, 0.0f,
		-0.2f, 0.15f, 0.0f,
		-0.2f, 0.15f, 0.0f,
		0.25f, 0.15f, 0.0f,
		0.25f, 0.15f, 0.0f,
		0.0f, 0.55f, 0.0f,
		0.25f, 0.15f, 0.0f,
		-0.2f, 0.15f, 0.0f,
		-0.2f, 0.15f, 0.0f,
		-0.2f, -0.35f, 0.0f,
		-0.2f, -0.35f, 0.0f,
		0.25f, -0.35f, 0.0f,
		0.25f, -0.35f, 0.0f,
		0.25f, 0.15f, 0.0f,
		0.25f, -0.35f, 0.0f,
		0.0f, -0.75f, 0.0f,
		0.0f, -0.75f, 0.0f,
		0.0f, -0.35f, 0.0f,
		0.0f, -0.35f, 0.0f,
		0.25f, -0.35f, 0.0f,
		0.0f, -0.75f, 0.0f,
		0.0f, -0.75f, 0.0f,
		0.15f, -0.92f, 0.0f,
		-0.1f, -0.9f, 0.0f,
		-0.1f, -0.9f, 0.0f,
		-0.2f, -0.35f, 0.0f,
		-0.2f, -0.35f, 0.0f,
		0.25f, -0.35f, 0.0f,
		0.25f, -0.35f, 0.0f,
		0.0f, -0.75f, 0.0f,
		-0.2f, 0.15f, 0.0f,
		-0.55f, 0.15f, 0.0f,
		-0.55f, 0.15f, 0.0f,
		-0.55f, -0.35f, 0.0f,
		-0.55f, -0.35f, 0.0f,
		-0.2f, -0.35f, 0.0f,
		-0.2f, -0.35f, 0.0f,
		-0.2f, 0.15f, 0.0f,
		-0.55f, 0.005f, 0.0f,
		-0.58f, 0.085f, 0.0f,
		-0.58f, 0.05f, 0.0f,
		-0.58f, -0.28f, 0.0f,
		-0.58f, -0.28f, 0.0f,
		-0.55f, -0.28f, 0.0f,
		-0.55f, -0.28f, 0.0f,
		-0.55f, 0.085f, 0.0f,
		-0.58f, 0.085f, 0.0f,
		-0.85f, 0.43f, 0.0f,
		-0.85f, 0.43f, 0.0f,
		-0.80f, 0.085f, 0.0f,
		-0.80f, 0.085f, 0.0f,
		-0.58f, 0.085f, 0.0f,
		-0.58f, -0.28f, 0.0f,
		-0.85f, -0.6f, 0.0f,
		-0.85f, -0.6f, 0.0f,
		-0.80f, -0.28f, 0.0f,
		-0.80f, -0.28f, 0.0f,
		-0.58f, -0.28f, 0.0f,
		-0.58f, 0.089f, 0.0f,
		-0.88f, 0.083f, 0.0f,
		-0.88f, 0.083f, 0.0f,
		-0.88f, -0.28f, 0.0f,
		-0.88f, -0.28f, 0.0f,
		-0.58f, -0.28f, 0.0f,
		-0.58f, -0.28f, 0.0f,
		-0.58f, 0.089f, 0.0f

	};



	const GLfloat Fire[] =
	{
		-0.89f, -0.04f, 0.0f,
		-70.0f, -0.04f, 0.0f,
		-0.89f, -0.12f, 0.0f,
		-70.0f, -0.12f, 0.0f,
		-0.89f, -0.2f, 0.0f,
		-70.0f, -0.2f, 0.0f
	};
	const GLfloat FireColors[] =
	{
		1.0f,0.5f,0.0f,
		1.0f,0.5f,0.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f
	};

	const GLfloat Fire1[] =
	{
		-0.89f, -0.04f, 0.0f,
		-2.0f, -0.04f, 0.0f,
		-0.89f, -0.12f, 0.0f,
			-2.0f, -0.12f, 0.0f,
		-0.89f, -0.2f, 0.0f,
		-2.0f, -0.2f, 0.0f,



	};
	const GLfloat FireColors1[] =
	{
		1.0f,0.5f,0.0f,
		1.0f,0.5f,0.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f
	};
	//Basically we are giving the vertices coordinates in the above array
	//////////////////// i  ///////////////// 
	//Create vao - Vertex array objects
	glGenVertexArrays(1, &vao_triangle);
	glBindVertexArray(vao_triangle);
	glGenBuffers(1, &vbo_position_triangle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_triangle);
	//in the above statement we have accesed the GL-ARRAY_BUFFER using vbo. Without it wouldn't be possible to get GL_ARRAY_BUFFER
	//For the sake of understanding . GL_ARRAY_BUFFER is in the GPU side ad=nd we have bind our CPU side vbo with it like a Pipe to get the access.
	glBufferData(GL_ARRAY_BUFFER, sizeof(IVertices), IVertices, GL_STATIC_DRAW);
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
	//Color 
	glGenBuffers(1, &vbo_color_triangle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_triangle);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(IColors), IColors, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, 3 * 2 * sizeof(GL_FLOAT), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//////////////////////// FOR RECTANGLE //////////////// 

	glGenVertexArrays(1, &vao_N);
	glBindVertexArray(vao_N);
	glGenBuffers(1, &vbo_position_N);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_N);
	glBufferData(GL_ARRAY_BUFFER, sizeof(NVertices), NVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//COLOR
		//Color 
	glGenBuffers(1, &vbo_color_N);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_N);
	glBufferData(GL_ARRAY_BUFFER, sizeof(NColor), NColor, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);


	glGenVertexArrays(1, &vao_D);
	glBindVertexArray(vao_D);
	glGenBuffers(1, &vbo_position_D);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_D);
	glBufferData(GL_ARRAY_BUFFER, sizeof(DVertices), DVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//COLOR
		//Color 
	glGenBuffers(1, &vbo_color_D);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_D);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(DColor), DColor, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, 3 * 8 * sizeof(GL_FLOAT), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glGenVertexArrays(1, &vao_A);
	glBindVertexArray(vao_A);
	glGenBuffers(1, &vbo_position_A);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_A);
	glBufferData(GL_ARRAY_BUFFER, sizeof(AVertices), AVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &vbo_color_A);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_A);
	glBufferData(GL_ARRAY_BUFFER, sizeof(AColor), AColor, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//////////////// A I R O P L A N E /////////////////////
	glBindVertexArray(0);
	glGenVertexArrays(1, &vao_Airoplane);
	glBindVertexArray(vao_Airoplane);
	glGenBuffers(1, &vbo_position_Airoplane);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_Airoplane);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Airoplane), Airoplane, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 0.7f, 0.8f, 0.9f);
	glBindVertexArray(0);




	//// FIRE  ////
	glGenVertexArrays(1, &vao_Fire);
	glBindVertexArray(vao_Fire);
	glGenBuffers(1, &vbo_Fire);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Fire);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Fire), Fire, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &vbo_FireColors);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_FireColors);
	glBufferData(GL_ARRAY_BUFFER,  18 * sizeof(GL_FLOAT),NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	//// FIRE1////////
	glGenVertexArrays(1, &vao_Fire1);
	glBindVertexArray(vao_Fire1);
	glGenBuffers(1, &vbo_Fire1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Fire1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Fire1), Fire1, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &vbo_FireColors1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_FireColors1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(FireColors1), FireColors1, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Depth Lines
	glClearDepth(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//initializing Perspective ProjectionMatrix to identity.
	perspectiveProjectionMatrix = mat4::identity();
	glEnable(GL_LINE_SMOOTH);
	resize(Win_Width, Win_Height);
	PlaySound(MAKEINTRESOURCE(MYWAV), NULL, SND_NODEFAULT | SND_RESOURCE | SND_ASYNC);
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
	GLfloat FireColor[18];
	GLfloat DColor[24];
	GLfloat IColor[6];



	FireColor[0] = r1;
	FireColor[1] = r12;
	FireColor[2] = 0.0f;
	FireColor[3] = r1;
	FireColor[4] = r12;
	FireColor[5] = 0;
	FireColor[6] = w;
	FireColor[7] = w;
	FireColor[8] = w;
	FireColor[9] = w;
	FireColor[10] = w;
	FireColor[11] = w;
	FireColor[12] = 0.0f;
	FireColor[13] = g1;
	FireColor[14] = 0.0f;
	FireColor[15] = 0.0f;
	FireColor[16] = g1;
	FireColor[17] = 0.0f;
	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use shader program
	glUseProgram(gShaderProgramObject);

	//declaration of matrices
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 ScaleMatrix;
	mat4 RotateMatrix;
	//// intialize above matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	// perform necessary transformations
	modelViewMatrix = translate(translateI1, 0.0f, -5.5f);
	// do necessary matrix multiplication
	// this was internally done by gluPerspective() in FFP.	

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	// send necessary matrices to shader in respective uniforms
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	//GL_FALSE = Should we transpose the matrix?
	//DirectX is roj major so there we will have to transpose but OpenGL is Colomn major so no need to transpose. 

	glLineWidth(3.0f);

	// bind with vao

	//////////////////            I               ////////////////////

	//this will avoid many binding to vbo
	glBindVertexArray(vao_triangle);
	IColor[0] = 1.0f;
	IColor[1] = 0.5f;
	IColor[2] = 0.0f;
	IColor[3] = 0.0f;
	IColor[4] = 1.0f;
	IColor[5] = 0.0f;
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_triangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(IColor), IColor, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// bind with textures

	// draw necessary scene
	glDrawArrays(GL_LINES, 0, 2);
	//  0 =  From where to start in the array. 
	// We have to start from 0th element i.e 0. if the 0th element would be 50.0f then we would have given the 2nd parameter as 50.
	//3 = How many Vertices? 
	//GL_TRIANGLES is the thing between glBegin() and glEnd()


	// unbind vao
	glBindVertexArray(0);
	
	////////////     N     //////////////
	if (counter > 2700)
	{
		modelViewMatrix = mat4::identity();
		ScaleMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();
		modelViewMatrix = translate(-0.8f, translateN, -5.5f);
		//ScaleMatrix = scale(2.0f, 2.0f, 2.0f);
		//modelViewMatrix = modelViewMatrix * ScaleMatrix;
		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
		glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
		glBindVertexArray(vao_N);
		glDrawArrays(GL_LINES, 0, 6);
		glBindVertexArray(0);
		////// unuse program

	}
	//////D
	if (counter > 6000)
	{

		modelViewMatrix = mat4::identity();
		ScaleMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();
		modelViewMatrix = translate(-0.2f, 0.0f, -5.5f);
		//ScaleMatrix = scale(2.0f, 2.0f, 2.0f);
		//modelViewMatrix = modelViewMatrix * ScaleMatrix;
		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
		glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
		glBindVertexArray(vao_D);
		DColor[0] = Red;
		DColor[1] = Orange;
		DColor[2] = 0.0f;

		DColor[3] = Red;
		DColor[4] = Orange;
		DColor[5] = 0.0f;

		DColor[6] = Red;
		DColor[7] = Orange;
		DColor[8] = 0.0f;

		DColor[9] = 0.0f;
		DColor[10] = Green;
		DColor[11] = 0.0f;

		DColor[12] = 0.0f;
		DColor[13] = Green;
		DColor[14] = 0.0f;

		DColor[15] = 0.0f;
		DColor[16] = Green;
		DColor[17] = 0.0f;


		DColor[18] = Red;
		DColor[19] = Orange;
		DColor[20] = 0.0f;

		DColor[21] = 0.0f;
		DColor[22] = Green;
		DColor[23] = 0.0f;
		glBindBuffer(GL_ARRAY_BUFFER, vbo_color_D);
		glBufferData(GL_ARRAY_BUFFER, sizeof(DColor), DColor, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_LINES, 0, 10);
		glBindVertexArray(0);


		//////D's Pillar
		modelViewMatrix = mat4::identity();
		ScaleMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();
		modelViewMatrix = translate(0.6f, 0.0f, -5.5f);
		//ScaleMatrix = scale(2.0f, 2.0f, 2.0f);
		//modelViewMatrix = modelViewMatrix * ScaleMatrix;
		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
		glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
		glBindVertexArray(vao_triangle);
		IColor[0] = Red;
		IColor[1] = Orange;
		IColor[2] = 0.0f;
		IColor[3] = 0.0f;
		IColor[4] = Green;
		IColor[5] = 0.0f;
		glBindBuffer(GL_ARRAY_BUFFER, vbo_color_triangle);
		glBufferData(GL_ARRAY_BUFFER, sizeof(IColor), IColor, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);
	}
	////I2
	if (counter > 4370)
	{
		modelViewMatrix = mat4::identity();
		ScaleMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();
		modelViewMatrix = translate(1.6f, translateI2, -5.5f);
		//ScaleMatrix = scale(2.0f, 2.0f, 2.0f);
		//modelViewMatrix = modelViewMatrix * ScaleMatrix;
		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
		glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
		glBindVertexArray(vao_triangle);
		IColor[0] = 1.0f;
		IColor[1] = 0.5f;
		IColor[2] = 0.0f;
		IColor[3] = 0.0f;
		IColor[4] = 1.0f;
		IColor[5] = 0.0f;
		glBindBuffer(GL_ARRAY_BUFFER, vbo_color_triangle);
		glBufferData(GL_ARRAY_BUFFER, sizeof(IColor), IColor, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);
	}
	////A
	if (counter > 1300)
	{
		modelViewMatrix = mat4::identity();
		ScaleMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();
		modelViewMatrix = translate(translateA, 0.0f, -5.5f);
		//ScaleMatrix = scale(2.0f, 2.0f, 2.0f);
		//modelViewMatrix = modelViewMatrix * ScaleMatrix;
		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
		glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
		glBindVertexArray(vao_A);
		glDrawArrays(GL_LINES, 0, 10);
		glBindVertexArray(0);
	}
//UpAiroplane
	if (counter > 6400)
	{
		if (counter > 6400 && counter < 6670)
		{
			modelViewMatrix = mat4::identity();
			ScaleMatrix = mat4::identity();
			modelViewProjectionMatrix = mat4::identity();
			modelViewMatrix = translate((float)(1.7f*(-(cos(angleup)) - 1.1f)), (float)((1.7*(-(sin(angleup))) + 1.5f)), -5.5f);
			ScaleMatrix = scale(0.3f, 0.3f, 0.3f);
			modelViewMatrix = modelViewMatrix * ScaleMatrix;
			RotateMatrix = rotate(upangle, 0.0f, 1.0f, 1.0f);
			modelViewMatrix = modelViewMatrix * RotateMatrix;
			//ScaleMatrix = scale(2.0f, 2.0f, 2.0f);
			//modelViewMatrix = modelViewMatrix * ScaleMatrix;
			modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
			glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
			glBindVertexArray(vao_Airoplane);
			glDrawArrays(GL_QUADS, 0, 96);
			glBindVertexArray(0);


			//Fire 
			modelViewMatrix = mat4::identity();
			ScaleMatrix = mat4::identity();
			modelViewProjectionMatrix = mat4::identity();
			modelViewMatrix = translate((float)(1.7f*(-(cos(angleup)) - 1.1f)), (float)((1.7*(-(sin(angleup))) + 1.5f)), -5.5f);
			ScaleMatrix = scale(0.3f, 0.3f, 0.3f);
			modelViewMatrix = modelViewMatrix * ScaleMatrix;
			RotateMatrix = rotate(upangle, 0.0f, 1.0f, 1.0f);
			modelViewMatrix = modelViewMatrix * RotateMatrix;
			//ScaleMatrix = scale(2.0f, 2.0f, 2.0f);
			//modelViewMatrix = modelViewMatrix * ScaleMatrix;
			modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
			glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
			glBindVertexArray(vao_Fire);
			glLineWidth(6.5f);
			glDrawArrays(GL_LINES, 0, 8);
			glBindVertexArray(0);





			//DownAiroplane
			modelViewMatrix = mat4::identity();
			ScaleMatrix = mat4::identity();
			modelViewProjectionMatrix = mat4::identity();
			modelViewMatrix = translate((float)(1.7f*(-(cos(angleDown)) - 1.1f)), (float)((1.7*((sin(angleDown))) - 1.5f)), -5.5f);
			ScaleMatrix = scale(0.3f, 0.3f, 0.3f);
			modelViewMatrix = modelViewMatrix * ScaleMatrix;
			RotateMatrix = rotate(dangle, 1.0f, 0.0f, 1.0f);
			modelViewMatrix = modelViewMatrix * RotateMatrix;
			//ScaleMatrix = scale(2.0f, 2.0f, 2.0f);
			//modelViewMatrix = modelViewMatrix * ScaleMatrix;
			modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
			glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
			glBindVertexArray(vao_Airoplane);
			glDrawArrays(GL_QUADS, 0, 96);
			glBindVertexArray(0);

			//Fire
			modelViewMatrix = mat4::identity();
			ScaleMatrix = mat4::identity();
			modelViewProjectionMatrix = mat4::identity();
			modelViewMatrix = translate((float)(1.7f*(-(cos(angleDown)) - 1.1f)), (float)((1.7*((sin(angleDown))) - 1.5f)), -5.5f);
			ScaleMatrix = scale(0.3f, 0.3f, 0.3f);
			modelViewMatrix = modelViewMatrix * ScaleMatrix;
			RotateMatrix = rotate(dangle, 1.0f, 0.0f, 0.0f);
			modelViewMatrix = modelViewMatrix * RotateMatrix;
			//ScaleMatrix = scale(2.0f, 2.0f, 2.0f);
			//modelViewMatrix = modelViewMatrix * ScaleMatrix;
			modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
			glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
			glBindVertexArray(vao_Fire);
			glLineWidth(6.5f);
			glDrawArrays(GL_LINES, 0, 8);
			glBindVertexArray(0);


		}
		//MidPlane
		modelViewMatrix = mat4::identity();
		ScaleMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();
		modelViewMatrix = translate(x, 0.0f, -5.5f);
		ScaleMatrix = scale(0.3f, 0.3f, 0.3f);
		modelViewMatrix = modelViewMatrix * ScaleMatrix;
		RotateMatrix = rotate(dangle, 0.0f, 1.0f, 0.0f);
		modelViewMatrix = modelViewMatrix;
		//ScaleMatrix = scale(2.0f, 2.0f, 2.0f);
		//modelViewMatrix = modelViewMatrix * ScaleMatrix;
		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
		glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
		glBindVertexArray(vao_Airoplane);
		glDrawArrays(GL_QUADS, 0, 96);
		glBindVertexArray(0);
		//Fire
		modelViewMatrix = mat4::identity();
		ScaleMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();
		modelViewMatrix = translate(x, 0.0f, -5.5f);
		ScaleMatrix = scale(0.3f, 0.3f, 0.3f);
		modelViewMatrix = modelViewMatrix * ScaleMatrix;
		RotateMatrix = rotate(dangle, 0.0f, 1.0f, 0.0f);
		modelViewMatrix = modelViewMatrix;
		//ScaleMatrix = scale(2.0f, 2.0f, 2.0f);
		//modelViewMatrix = modelViewMatrix * ScaleMatrix;
		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
		glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
		glBindVertexArray(vao_Fire);


		glBindBuffer(GL_ARRAY_BUFFER, vbo_FireColors);
		glBufferData(GL_ARRAY_BUFFER, sizeof(FireColor), FireColor, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		glLineWidth(6.5f);
		glDrawArrays(GL_LINES, 0, 8);
		glBindVertexArray(0);



	}

		if (x >= 2.5f)
		{
			if (angleup1 >= 0.0f)
			{
				modelViewMatrix = mat4::identity();
				ScaleMatrix = mat4::identity();
				modelViewProjectionMatrix = mat4::identity();
				modelViewMatrix = translate((float)(1.9f*((cos(angleup1)) + 1.3f)), (float)((1.9*((sin(angleup1))) - 1.9f)), -5.5f);
				ScaleMatrix = scale(0.3f, 0.3f, 0.3f);
				modelViewMatrix = modelViewMatrix * ScaleMatrix;
				RotateMatrix = rotate(upangle1, 1.0f, 0.0f, 1.0f);
				modelViewMatrix = modelViewMatrix * RotateMatrix;
				//ScaleMatrix = scale(2.0f, 2.0f, 2.0f);
				//modelViewMatrix = modelViewMatrix * ScaleMatrix;
				modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
				glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
				glBindVertexArray(vao_Airoplane);
				glDrawArrays(GL_QUADS, 0, 96);
				glBindVertexArray(0);

				//Fire
				modelViewMatrix = mat4::identity();
				ScaleMatrix = mat4::identity();
				modelViewProjectionMatrix = mat4::identity();
				modelViewMatrix = translate((float)(1.9f*((cos(angleup1)) + 1.3f)), (float)((1.9*((sin(angleup1))) - 1.9f)), -5.5f);
				ScaleMatrix = scale(0.3f, 0.3f, 0.3f);
				modelViewMatrix = modelViewMatrix * ScaleMatrix;
				RotateMatrix = rotate(upangle1, 1.0f, 0.0f, 1.0f);
				modelViewMatrix = modelViewMatrix * RotateMatrix;
				//ScaleMatrix = scale(2.0f, 2.0f, 2.0f);
				//modelViewMatrix = modelViewMatrix * ScaleMatrix;
				modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
				glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
				glBindVertexArray(vao_Fire1);
				glLineWidth(6.5f);
				glDrawArrays(GL_LINES, 0, 8);
				glBindVertexArray(0);


				if (upangle1 >= 290.0f)
				{
					upangle1 = upangle1 - 0.03f;
				}

				x = x + 0.002f;
				angleup1 = angleup1 - 0.00099f;
			}
			if (angled1 < (2 * PI))
			{
				modelViewMatrix = mat4::identity();
				ScaleMatrix = mat4::identity();
				modelViewProjectionMatrix = mat4::identity();
				modelViewMatrix = translate((float)(1.9f*((cos(angled1)) + 1.2f)), (float)((1.9*((sin(angled1))) + 1.9f)), -5.5f);
				ScaleMatrix = scale(0.3f, 0.3f, 0.3f);
				modelViewMatrix = modelViewMatrix * ScaleMatrix;
				RotateMatrix = rotate(dangle1, 0.0f, 1.0f, 1.0f);
				modelViewMatrix = modelViewMatrix * RotateMatrix;
				//ScaleMatrix = scale(2.0f, 2.0f, 2.0f);
				//modelViewMatrix = modelViewMatrix * ScaleMatrix;
				modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
				glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
				glBindVertexArray(vao_Airoplane);
				glDrawArrays(GL_QUADS, 0, 96);
				glBindVertexArray(0);


				//Fire
				modelViewMatrix = mat4::identity();
				ScaleMatrix = mat4::identity();
				modelViewProjectionMatrix = mat4::identity();
				modelViewMatrix = translate((float)(1.9f*((cos(angled1)) + 1.2f)), (float)((1.9*((sin(angled1))) + 1.9f)), -5.5f);
				ScaleMatrix = scale(0.3f, 0.3f, 0.3f);
				modelViewMatrix = modelViewMatrix * ScaleMatrix;
				RotateMatrix = rotate(dangle1, 0.0f, 1.0f, 1.0f);
				modelViewMatrix = modelViewMatrix * RotateMatrix;
				//ScaleMatrix = scale(2.0f, 2.0f, 2.0f);
				//modelViewMatrix = modelViewMatrix * ScaleMatrix;
				modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
				glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
				glBindVertexArray(vao_Fire1);
				glLineWidth(6.5f);
				glDrawArrays(GL_LINES, 0, 8);
				glBindVertexArray(0);



				angled1 = angled1 + 0.00099f;
				if (dangle1 <= 80)
				{
					dangle1 = dangle1 + 0.02f;
				}
			}
		}
	
	glUseProgram(0);
	if (translateI1 <= -1.0f)
	{

		translateI1 = translateI1 + 0.002f;

	}
	if (counter > 2700 &&translateN >= 0.0f)
	{
		translateN = translateN - 0.002f;
	}
	if (counter > 4370 && translateI2 <= 0.0f)
	{
		translateI2 = translateI2 + 0.002f;
	}
	if (counter > 1300 && translateA >= 0.8f)
	{
		translateA = translateA - 0.002f;
	}
	if(counter>6400)
	{

		if (x <= 2.5f)
		{
			x = x + 0.007f;
		}
	
		if (upangle <= 360.0f)
		{
			upangle = upangle + 0.15;
		}
		if (angleup <= (PI / 2.9f))
		{
			//angle = (angle + 0.001);
			angleup = (angleup + 0.004);
			fprintf(gpLogFile, "Value of angleup is %f\n", angleup);
		}
		if (angleDown <= (PI / 2.9f))
		{
			angleDown = (angleDown + 0.004);
		}
		if (dangle >= 0.0f)
		{
			dangle = dangle - 0.2;
		}
	
	}

	if (counter>6000 && Red <= 1.0f)
	{
		Red += 0.002f;
		Orange += 0.001f;
		Green += 0.002f;
	}
	fprintf(gpLogFile, "The value of X is %f\n", x);
		//UpAiroplane1();
		//DownAiroplane2();
	if (counter > 9300)
	{
		if (x > 5.5f)
		{
			if (r1 >= 0.0f)
			{

				g1 = g1 - 0.002f;
				r1 = r1 - 0.002f;
				w = w - 0.002f;

			}
			if (r12 >= 0.0f)
			{
				r12 = r12 - 0.002f;
			}

		}
	}
	if (counter > 9640)
	{
		if (x <= 125.0f)
		{
			x = x + 0.98f;
		}
	}
	SwapBuffers(ghdc);

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
	/*if (vbo_position_rectangle)
	{
		glDeleteBuffers(1, &vbo_position_rectangle);
		vbo_position_rectangle = 0;
	}

	if (vao_rectangle)
	{
		glDeleteBuffers(1, &vao_rectangle);
		vao_rectangle = 0;
	}*/


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
