#include<Windows.h>
#include<glew.h>               // This SHOULD be above GL.h header.
#include<gl/GL.h>

#include<stdio.h>
#include"vmath.h"
#pragma comment(lib,"opengl32.lib")//for linking
#pragma comment(lib,"glew32.lib")//for linking

static const unsigned char palette[] =
{
	0xFF, 0x00, 0x00, 0xFF, 0x0E, 0x03, 0xFF, 0x1C,
	0x07, 0xFF, 0x2A, 0x0A, 0xFF, 0x38, 0x0E, 0xFF,
	0x46, 0x12, 0xFF, 0x54, 0x15, 0xFF, 0x62, 0x19,
	0xFF, 0x71, 0x1D, 0xFF, 0x7F, 0x20, 0xFF, 0x88,
	0x22, 0xFF, 0x92, 0x25, 0xFF, 0x9C, 0x27, 0xFF,
	0xA6, 0x2A, 0xFF, 0xB0, 0x2C, 0xFF, 0xBA, 0x2F,
	0xFF, 0xC4, 0x31, 0xFF, 0xCE, 0x34, 0xFF, 0xD8,
	0x36, 0xFF, 0xE2, 0x39, 0xFF, 0xEC, 0x3B, 0xFF,
	0xF6, 0x3E, 0xFF, 0xFF, 0x40, 0xF8, 0xFE, 0x40,
	0xF1, 0xFE, 0x40, 0xEA, 0xFE, 0x41, 0xE3, 0xFD,
	0x41, 0xDC, 0xFD, 0x41, 0xD6, 0xFD, 0x42, 0xCF,
	0xFC, 0x42, 0xC8, 0xFC, 0x42, 0xC1, 0xFC, 0x43,
	0xBA, 0xFB, 0x43, 0xB4, 0xFB, 0x43, 0xAD, 0xFB,
	0x44, 0xA6, 0xFA, 0x44, 0x9F, 0xFA, 0x45, 0x98,
	0xFA, 0x45, 0x92, 0xF9, 0x45, 0x8B, 0xF9, 0x46,
	0x84, 0xF9, 0x46, 0x7D, 0xF8, 0x46, 0x76, 0xF8,
	0x46, 0x6F, 0xF8, 0x47, 0x68, 0xF8, 0x47, 0x61,
	0xF7, 0x47, 0x5A, 0xF7, 0x48, 0x53, 0xF7, 0x48,
	0x4C, 0xF6, 0x48, 0x45, 0xF6, 0x49, 0x3E, 0xF6,
	0x49, 0x37, 0xF6, 0x4A, 0x30, 0xF5, 0x4A, 0x29,
	0xF5, 0x4A, 0x22, 0xF5, 0x4B, 0x1B, 0xF5, 0x4B,
	0x14, 0xF4, 0x4B, 0x0D, 0xF4, 0x4C, 0x06, 0xF4,
	0x4D, 0x04, 0xF1, 0x51, 0x0D, 0xE9, 0x55, 0x16,
	0xE1, 0x59, 0x1F, 0xD9, 0x5D, 0x28, 0xD1, 0x61,
	0x31, 0xC9, 0x65, 0x3A, 0xC1, 0x69, 0x42, 0xB9,
	0x6D, 0x4B, 0xB1, 0x71, 0x54, 0xA9, 0x75, 0x5D,
	0xA1, 0x79, 0x66, 0x99, 0x7D, 0x6F, 0x91, 0x81,
	0x78, 0x89, 0x86, 0x80, 0x81, 0x8A, 0x88, 0x7A,
	0x8E, 0x90, 0x72, 0x92, 0x98, 0x6A, 0x96, 0xA1,
	0x62, 0x9A, 0xA9, 0x5A, 0x9E, 0xB1, 0x52, 0xA2,
	0xBA, 0x4A, 0xA6, 0xC2, 0x42, 0xAA, 0xCA, 0x3A,
	0xAE, 0xD3, 0x32, 0xB2, 0xDB, 0x2A, 0xB6, 0xE3,
	0x22, 0xBA, 0xEB, 0x1A, 0xBE, 0xF4, 0x12, 0xC2,
	0xFC, 0x0A, 0xC6, 0xF5, 0x02, 0xCA, 0xE6, 0x09,
	0xCE, 0xD8, 0x18, 0xD1, 0xCA, 0x27, 0xD5, 0xBB,
	0x36, 0xD8, 0xAD, 0x45, 0xDC, 0x9E, 0x54, 0xE0,
	0x90, 0x62, 0xE3, 0x82, 0x6F, 0xE6, 0x71, 0x7C,
	0xEA, 0x61, 0x89, 0xEE, 0x51, 0x96, 0xF2, 0x40,
	0xA3, 0xF5, 0x30, 0xB0, 0xF9, 0x20, 0xBD, 0xFD,
	0x0F, 0xE3, 0xFF, 0x01, 0xE9, 0xFF, 0x01, 0xEE,
	0xFF, 0x01, 0xF4, 0xFF, 0x00, 0xFA, 0xFF, 0x00,
	0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x0A, 0xFF, 0xFF,
	0x15, 0xFF, 0xFF, 0x20, 0xFF, 0xFF, 0x2B, 0xFF,
	0xFF, 0x36, 0xFF, 0xFF, 0x41, 0xFF, 0xFF, 0x4C,
	0xFF, 0xFF, 0x57, 0xFF, 0xFF, 0x62, 0xFF, 0xFF,
	0x6D, 0xFF, 0xFF, 0x78, 0xFF, 0xFF, 0x81, 0xFF,
	0xFF, 0x8A, 0xFF, 0xFF, 0x92, 0xFF, 0xFF, 0x9A,
	0xFF, 0xFF, 0xA3, 0xFF, 0xFF, 0xAB, 0xFF, 0xFF,
	0xB4, 0xFF, 0xFF, 0xBC, 0xFF, 0xFF, 0xC4, 0xFF,
	0xFF, 0xCD, 0xFF, 0xFF, 0xD5, 0xFF, 0xFF, 0xDD,
	0xFF, 0xFF, 0xE6, 0xFF, 0xFF, 0xEE, 0xFF, 0xFF,
	0xF7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9,
	0xFF, 0xFF, 0xF3, 0xFF, 0xFF, 0xED, 0xFF, 0xFF,
	0xE7, 0xFF, 0xFF, 0xE1, 0xFF, 0xFF, 0xDB, 0xFF,
	0xFF, 0xD5, 0xFF, 0xFF, 0xCF, 0xFF, 0xFF, 0xCA,
	0xFF, 0xFF, 0xC4, 0xFF, 0xFF, 0xBE, 0xFF, 0xFF,
	0xB8, 0xFF, 0xFF, 0xB2, 0xFF, 0xFF, 0xAC, 0xFF,
	0xFF, 0xA6, 0xFF, 0xFF, 0xA0, 0xFF, 0xFF, 0x9B,
	0xFF, 0xFF, 0x96, 0xFF, 0xFF, 0x90, 0xFF, 0xFF,
	0x8B, 0xFF, 0xFF, 0x86, 0xFF, 0xFF, 0x81, 0xFF,
	0xFF, 0x7B, 0xFF, 0xFF, 0x76, 0xFF, 0xFF, 0x71,
	0xFF, 0xFF, 0x6B, 0xFF, 0xFF, 0x66, 0xFF, 0xFF,
	0x61, 0xFF, 0xFF, 0x5C, 0xFF, 0xFF, 0x56, 0xFF,
	0xFF, 0x51, 0xFF, 0xFF, 0x4C, 0xFF, 0xFF, 0x47,
	0xFF, 0xFF, 0x41, 0xF9, 0xFF, 0x40, 0xF0, 0xFF,
	0x40, 0xE8, 0xFF, 0x40, 0xDF, 0xFF, 0x40, 0xD7,
	0xFF, 0x40, 0xCF, 0xFF, 0x40, 0xC6, 0xFF, 0x40,
	0xBE, 0xFF, 0x40, 0xB5, 0xFF, 0x40, 0xAD, 0xFF,
	0x40, 0xA4, 0xFF, 0x40, 0x9C, 0xFF, 0x40, 0x95,
	0xFF, 0x40, 0x8D, 0xFF, 0x40, 0x86, 0xFF, 0x40,
	0x7E, 0xFF, 0x40, 0x77, 0xFF, 0x40, 0x6F, 0xFF,
	0x40, 0x68, 0xFF, 0x40, 0x60, 0xFF, 0x40, 0x59,
	0xFF, 0x40, 0x51, 0xFF, 0x40, 0x4A, 0xFA, 0x43,
	0x42, 0xF3, 0x48, 0x3E, 0xED, 0x4E, 0x3D, 0xE6,
	0x53, 0x3B, 0xDF, 0x58, 0x39, 0xD8, 0x5E, 0x37,
	0xD2, 0x63, 0x35, 0xCB, 0x68, 0x34, 0xC4, 0x6D,
	0x32, 0xBD, 0x73, 0x30, 0xB7, 0x78, 0x2E, 0xB0,
	0x7D, 0x2D, 0xA9, 0x83, 0x2B, 0xA2, 0x88, 0x29,
	0x9C, 0x8D, 0x27, 0x95, 0x92, 0x25, 0x8E, 0x98,
	0x24, 0x87, 0x9D, 0x22, 0x81, 0xA2, 0x20, 0x7A,
	0xA6, 0x1E, 0x74, 0xAB, 0x1D, 0x6E, 0xB0, 0x1B,
	0x68, 0xB5, 0x1A, 0x61, 0xB9, 0x18, 0x5B, 0xBE,
	0x17, 0x55, 0xC3, 0x15, 0x4F, 0xC8, 0x13, 0x48,
	0xCD, 0x12, 0x42, 0xD1, 0x10, 0x3C, 0xD6, 0x0F,
	0x36, 0xDB, 0x0D, 0x2F, 0xE0, 0x0C, 0x29, 0xE4,
	0x0A, 0x23, 0xE9, 0x08, 0x1D, 0xEE, 0x07, 0x16,
	0xF3, 0x05, 0x10, 0xF7, 0x04, 0x0A, 0xFC, 0x02,
	0x04, 0xFB, 0x01, 0x04, 0xEF, 0x00, 0x12, 0xE4,
	0x00, 0x1F, 0xD9, 0x00, 0x2D, 0xCE, 0x00, 0x3B,
	0xC2, 0x00, 0x48, 0xB7, 0x00, 0x56, 0xAC, 0x00,
	0x64, 0xA0, 0x00, 0x72, 0x95, 0x00, 0x7F, 0x8A,
	0x00, 0x88, 0x7F, 0x00, 0x92, 0x75, 0x00, 0x9C,
	0x6B, 0x00, 0xA6, 0x61, 0x00, 0xB0, 0x57, 0x00,
	0xBA, 0x4E, 0x00, 0xC4, 0x44, 0x00, 0xCE, 0x3A,
	0x00, 0xD8, 0x30, 0x00, 0xE2, 0x27, 0x00, 0xEC,
	0x1D, 0x00, 0xF6, 0x13, 0x00, 0xFF, 0x09, 0x00,
};
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
GLuint      palette_texture;
struct
{
	GLint   zoom;
	GLint   offset;
	GLint   C;
} uniforms;
bool paused = false;
float time_offset = 0.0f;
float zoom = 2.0f;
float x_offset = 0.0f;
float y_offset = 0.0f; 
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

	glGenVertexArrays(1, &vao_rectangle);
	glBindVertexArray(vao_rectangle);

	//Define Vertex Shader Object
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar **p;
	const GLchar *vertexShaderSourceCode =

		"#version 430 core" \
		"\n"
		"uniform float zoom;                                                                    \n"
		"                                                                                       \n"
		                                                                  
		"uniform vec2 offset;                                                                   \n"
		"                                                                                       \n"
		"out vec2 initial_z;                                                                    \n"
		"                                                                                       \n"
		"void main(void)                                                                        \n"
		"{                                                                                      \n"
		"    const vec4 vertices[4] = vec4[4](vec4(-1.0, -1.0, 0.5, 1.0),                       \n"
		"                                     vec4( 1.0, -1.0, 0.5, 1.0),                       \n"
		"                                     vec4( 1.0,  1.0, 0.5, 1.0),                       \n"
		"                                     vec4(-1.0,  1.0, 0.5, 1.0));                      \n"
		"    initial_z = (vertices[gl_VertexID].xy * zoom) + offset;                            \n"
		"    gl_Position = vertices[gl_VertexID];                                               \n"
		"}" ;       
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
		"in vec2 initial_z;                                                                     \n"
		"                                                                                       \n"
		"out vec4 color;                                                                        \n"
		"                                                                                       \n"
		"uniform sampler1D tex_gradient;                                                        \n"
		"uniform vec2 C;                                                                        \n"
		"                                                                                       \n"
		"void main(void)                                                                        \n"
		"{                                                                                      \n"
		"    vec2 Z = initial_z;                                                                \n"
		"    int iterations = 0;                                                                \n"
		"    const float threshold_squared = 16.0;                                              \n"
		"    const int max_iterations = 256;                                                    \n"
		"    while (iterations < max_iterations && dot(Z, Z) < threshold_squared) {             \n"
		"        vec2 Z_squared;                                                                \n"
		"        Z_squared.x = Z.x * Z.x - Z.y * Z.y;                                           \n"
		"        Z_squared.y = 2.0 * Z.x * Z.y;                                                 \n"
		"        Z = Z_squared + C;                                                             \n"
		"        iterations++;                                                                  \n"
		"    }                                                                                  \n"
		"    if (iterations == max_iterations)                                                  \n"
		"        color = vec4(0.0, 0.0, 0.0, 1.0);                                              \n"
		"    else                                                                               \n"
		"        color = texture(tex_gradient, float(iterations) / float(max_iterations));      \n"
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
	//mvpUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
	//Here we have done all the preparations of data transfer from CPU to GPU
	uniforms.zoom = glGetUniformLocation(gShaderProgramObject, "zoom");
	uniforms.offset = glGetUniformLocation(gShaderProgramObject, "offset");
	uniforms.C = glGetUniformLocation(gShaderProgramObject, "C");
	
	
	glGenTextures(1, &palette_texture);
	glBindTexture(GL_TEXTURE_1D, palette_texture);
	glTexStorage1D(GL_TEXTURE_1D, 8, GL_RGB8, 256);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 256, GL_RGB, GL_UNSIGNED_BYTE, palette);
	glGenerateMipmap(GL_TEXTURE_1D);
	
	//const GLfloat triangleVertices[] =
	//{
	//	// Perspective triangle (Front face)
	//	0.0f, 1.0f, 0.0f,		// Apex
	//	-1.0f, -1.0f, 1.0f,		// Left bottom
	//	1.0f, -1.0f, 1.0f,		// Right bottom
	//							// Perspective triangle (Right face)
	//	0.0f, 1.0f, 0.0f,		// Apex
	//	1.0f, -1.0f, 1.0f,		// Left bottom
	//	1.0f, -1.0f, -1.0f,		// Right bottom
	//							// Perspective triangle (Back face)
	//	0.0f, 1.0f, 0.0f,		// Apex
	//	1.0f, -1.0f, -1.0f,		// Left bottom
	//	-1.0f, -1.0f, -1.0f,	// Right bottom
	//							// Perspective triangle (Left face)
	//	0.0f, 1.0f, 0.0f,		// Apex
	//	-1.0f, -1.0f, -1.0f,	// Left bottom
	//	-1.0f, -1.0f, 1.0f
	//};
	//const GLfloat triangleColors[] =
	//{
	//	1.0f, 0.0f, 0.0f,		// Red apex
	//	0.0f, 1.0f, 0.0f,		// Green left bottom
	//	0.0f, 0.0f, 1.0f,		// Blue right bottom
	//	1.0f, 0.0f, 0.0f,		// Red apex
	//	0.0f, 0.0f, 1.0f,		// Blue right bottom
	//	0.0f, 1.0f, 0.0f,		// Green left bottom
	//	1.0f, 0.0f, 0.0f,		// Red apex
	//	0.0f, 1.0f, 0.0f,		// Green left bottom
	//	0.0f, 0.0f, 1.0f,		// Blue right bottom
	//	1.0f, 0.0f, 0.0f,		// Red apex
	//	0.0f, 0.0f, 1.0f,		// Blue right bottom
	//	0.0f, 1.0f, 0.0f,
	//};
	//const GLfloat rectangleVertices[] =
	//{
	//	// Perspective square (Top face)
	//	1.0f, 1.0f, -1.0f,		// Right top
	//	-1.0f, 1.0f, -1.0f, 	// Left top
	//	-1.0f, 1.0f, 1.0f,		// Left bottom
	//	1.0f, 1.0f, 1.0f,		// Right bottom
	//							// Perspective square (Bottom face)
	//	1.0f, -1.0f, -1.0f,		// Right top
	//	-1.0f, -1.0f, -1.0f, 	// Left top
	//	-1.0f, -1.0f, 1.0f,		// Left bottom
	//	1.0f, -1.0f, 1.0f,		// Right bottom
	//							// Perspective square (Front face)
	//	1.0f, 1.0f, 1.0f,		// Right top
	//	-1.0f, 1.0f, 1.0f,		// Left top
	//	-1.0f, -1.0f, 1.0f, 	// Left bottom
	//	1.0f, -1.0f, 1.0f,		// Right bottom
	//							// Perspective square (Back face)
	//	1.0f, 1.0f, -1.0f,		// Right top											
	//	-1.0f, 1.0f, -1.0f,		// Left top
	//	-1.0f, -1.0f, -1.0f, 	// Left bottom
	//	1.0f, -1.0f, -1.0f,		// Right bottom
	//							// Perspective square (Right face)
	//	1.0f, 1.0f, -1.0f,		// Right top											
	//	1.0f, 1.0f, 1.0f,		// Left top
	//	1.0f, -1.0f, 1.0f, 		// Left bottom
	//	1.0f, -1.0f, -1.0f,		// Right bottom
	//							// Perspective square (Left face)
	//	-1.0f, 1.0f, 1.0f,		// Right top																						
	//	-1.0f, 1.0f, -1.0f,		// Left top
	//	-1.0f, -1.0f, -1.0f, 	// Left bottom
	//	-1.0f, -1.0f, 1.0f		// Right bottom


	//};
	//const GLfloat rectangleColor[] =
	//{
	//	1.0f, 0.0f, 0.0f,		// Red 		- Top face
	//	1.0f, 0.0f, 0.0f,		// Red
	//	1.0f, 0.0f, 0.0f,		// Red
	//	1.0f, 0.0f, 0.0f,		// Red
	//	0.0f, 1.0f, 0.0f,		// Green 	- Bottom face
	//	0.0f, 1.0f, 0.0f,		// Green
	//	0.0f, 1.0f, 0.0f,		// Green											
	//	0.0f, 1.0f, 0.0f,		// Green
	//	0.0f, 0.0f, 1.0f,		// Blue		- Front face
	//	0.0f, 0.0f, 1.0f,		// Blue
	//	0.0f, 0.0f, 1.0f,		// Blue
	//	0.0f, 0.0f, 1.0f,		// Blue
	//	0.0f, 1.0f, 1.0f,		// Cyan		- Back face
	//	0.0f, 1.0f, 1.0f,		// Cyan
	//	0.0f, 1.0f, 1.0f,		// Cyan
	//	0.0f, 1.0f, 1.0f,		// Cyan
	//	1.0f, 0.0f, 1.0f,		// Magenta 	- Right face
	//	1.0f, 0.0f, 1.0f,		// Magenta
	//	1.0f, 0.0f, 1.0f,		// Magenta
	//	1.0f, 0.0f, 1.0f,		// Magenta
	//	1.0f, 1.0f, 0.0f,		// Yellow	- Left face
	//	1.0f, 1.0f, 0.0f,		// Yellow
	//	1.0f, 1.0f, 0.0f,		// Yellow											
	//	1.0f, 1.0f, 0.0f };
	////Basically we are giving the vertices coordinates in the above array
	//////////////////////FOR TRIANGLE ///////////////// 
	////Create vao - Vertex array objects
	//glGenVertexArrays(1, &vao_triangle);
	//glBindVertexArray(vao_triangle);
	//glGenBuffers(1, &vbo_position_triangle);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo_position_triangle);
	////in the above statement we have accesed the GL-ARRAY_BUFFER using vbo. Without it wouldn't be possible to get GL_ARRAY_BUFFER
	////For the sake of understanding . GL_ARRAY_BUFFER is in the GPU side ad=nd we have bind our CPU side vbo with it like a Pipe to get the access.
	//glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
	////The above statement states that , we are passing our vertices array to the GPU and GL_STATIC_DRAW means draw it now only. Don't draw it in Runtime. 
	////The below statement states that after storing the data in the GPU'S buffer . We are passing it to the vPosition now . 

	//glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	////GL_FALSE = We are not giving normalized coordinates as our coordinates are not converted in 0 - 1 range.
	////3 = This is the thing I was talking about in initialize. Here, we are telling GPU to break our array in 3 parts . 
	////0 and Null are for the Interleaved. 
	////GL_FLOAT- What is the type? .
	////AMC_ATTRIBUTE_POSITION. here we are passing data to vPosition. 

	//glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	////Color 
	//glGenBuffers(1, &vbo_color_triangle);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo_color_triangle);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(triangleColors), triangleColors, GL_STATIC_DRAW);
	//glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	////////////////////////// FOR RECTANGLE //////////////// 

	//glGenVertexArrays(1, &vao_rectangle);
	//glBindVertexArray(vao_rectangle);
	//glGenBuffers(1, &vbo_position_rectangle);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo_position_rectangle);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);
	//glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	////COLOR
	//	//Color 
	//glGenBuffers(1, &vbo_color_rectangle);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo_color_rectangle);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleColor), rectangleColor, GL_STATIC_DRAW);
	//glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glBindVertexArray(0);






	//Depth Lines
	glClearDepth(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//initializing Perspective ProjectionMatrix to identity.
	perspectiveProjectionMatrix = mat4::identity();

	resize(Win_Width, Win_Height);

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


double currentTime;
void display(void)
{


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use shader program
	glUseProgram(gShaderProgramObject);

	//declaration of matrices
	//mat4 modelViewMatrix;
	//mat4 modelViewProjectionMatrix;
	//mat4 RotationMatrix;
	//mat4 TranslateMatrix;
	//// intialize above matrices to identity
	//modelViewMatrix = mat4::identity();
	//modelViewProjectionMatrix = mat4::identity();
	//RotationMatrix = mat4::identity();
	//TranslateMatrix = mat4::identity();
	//TranslateMatrix = translate(-2.5f, 0.0f, -9.0f);
	//RotationMatrix = rotate(angleTriangle, 0.0f, 1.0f, 0.0f);
	//// perform necessary transformations
	//modelViewMatrix = TranslateMatrix * RotationMatrix;
	//// do necessary matrix multiplication
	//// this was internally done by gluPerspective() in FFP.	

	//modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	//// send necessary matrices to shader in respective uniforms
	//glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	////GL_FALSE = Should we transpose the matrix?
	////DirectX is roj major so there we will have to transpose but OpenGL is Colomn major so no need to transpose. 



	//// bind with vao

	//////////////////// TRIANGLE ////////////////////

	////this will avoid many binding to vbo
	//glBindVertexArray(vao_triangle);

	//// bind with textures

	//// draw necessary scene
	//glDrawArrays(GL_TRIANGLES, 0, 12);
	////  0 =  From where to start in the array. 
	//// We have to start from 0th element i.e 0. if the 0th element would be 50.0f then we would have given the 2nd parameter as 50.
	////3 = How many Vertices? 
	////GL_TRIANGLES is the thing between glBegin() and glEnd()


	//// unbind vao
	//glBindVertexArray(0);

	//////////  RECTANGLE //////////////
	//modelViewMatrix = mat4::identity();
	//modelViewProjectionMatrix = mat4::identity();
	//RotationMatrix = mat4::identity();
	//TranslateMatrix = mat4::identity();
	//TranslateMatrix = translate(1.0f, 0.0f, -9.0f);
	//RotationMatrix = rotate(angleRectangle, angleRectangle, angleRectangle);
	//// perform necessary transformations
	//modelViewMatrix = TranslateMatrix * RotationMatrix;
	//modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	//glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	//glBindVertexArray(vao_rectangle);
	//glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	//glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	//glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	//glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	//glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	//glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	//glBindVertexArray(0);
	//// unuse program
	//glUseProgram(0);


	static float t = 0.0f;
	float r = 0.0f;

	/*if (!paused)
		t = (float)currentTime;*/

	r =0.0f;

	float C[2] = { (sinf(r * 0.1f) + cosf(r * 0.23f)) * 0.5f, (cosf(r * 0.13f) + sinf(r * 0.21f)) * 0.5f };
	float offset[2] = { x_offset, y_offset };

	glUseProgram(gShaderProgramObject);

	glUniform2fv(uniforms.C, 1, C);
	glUniform2fv(uniforms.offset, 1, offset);
	glUniform1f(uniforms.zoom, 1.0f);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUseProgram(0);
	SwapBuffers(ghdc);

	currentTime += 0.01f;
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