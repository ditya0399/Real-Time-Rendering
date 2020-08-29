#include<Windows.h>
#include<glew.h>               // This SHOULD be above GL.h header.
#include<gl/GL.h>


#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <math.h>
#include<stdio.h>
#include"vmath.h"

#pragma comment(lib,"opengl32.lib")//for linking
#pragma comment(lib,"glew32.lib")//for linking

#include "include/cuda_runtime.h"
#include "include/cuda_gl_interop.h"

#include "inc/helper_functions.h"
#include "inc/helper_cuda.h"
//
#include "ParticleSystem.h"
#include "ParticleSystem.cuh"


#include "SmokeRenderer.h"
#include "paramgl.h"
#include "GLSLProgram.h"
#include "SmokeShaders.h"



#pragma comment(lib,"cudart.lib")//for linking
#pragma comment(lib,"glu32.lib")//for linking

int flag = 0;

int Win_Width = 1280;
int Win_Height = 1024;
using namespace vmath;

static GLfloat angle = 0.0f; 
GLuint gShaderProgramObject;

//CUDA
uint numParticles = 1 << 16;

ParticleSystem *psystem = 0;
SmokeRenderer *renderer = 0;
GLSLProgram *floorProg = 0;

int g_TotalErrors = 0;



// view params
int ox, oy;
int buttonState = 0;
bool keyDown[256];

vec3f cameraPos(0, -1, 4);
vec3f cameraRot(0, 0, 0);
vec3f cameraPosLag(cameraPos);
vec3f cameraRotLag(cameraRot);
vec3f cursorPos(0, 1, 0);
vec3f cursorPosLag(cursorPos);

vec3f lightPos(5.0, 5.0, 5.0);

const float inertia = 0.1f;
const float translateSpeed = 0.002f;
const float cursorSpeed = 0.01f;
const float rotateSpeed = 0.2f;
const float walkSpeed = 0.05f;

enum { M_VIEW = 0, M_MOVE_CURSOR, M_MOVE_LIGHT };
int mode = 0;
int displayMode = (int)SmokeRenderer::POINTS;

// QA AutoTest
bool g_bQAReadback = false;

// toggles
bool displayEnabled = true;
bool paused = false;
bool displaySliders = false;
bool wireframe = false;
bool animateEmitter = true;
bool emitterOn = true;
bool sort = true;
bool displayLightBuffer = false;
bool drawVectors = false;
bool doBlur = false;

float emitterVel = 0.0f;
uint emitterRate = 1000;
float emitterRadius = 0.25;
float emitterSpread = 0.0;
uint emitterIndex = 0;

// simulation parameters
float timestep = 0.5f;
float currentTime = 0.0f;
float spriteSize = 0.05f;
float alpha = 0.1f;
float shadowAlpha = 0.02f;
float particleLifetime = (float)numParticles / (float)emitterRate;
vec3f lightColor(1.0f, 1.0f, 0.8f);
vec3f colorAttenuation(0.5f, 0.75f, 1.0f);
float blurRadius = 2.0f;

int numSlices = 12;
int numDisplayedSlices = numSlices;

// fps
static int fpsCount = 0;
static int fpsLimit = 1;
StopWatchInterface *timer = NULL;

float modelView[16];

//

ParamListGL *params;

GLuint floorTex = 0;

// CheckRender object for verification
#define MAX_EPSILON_ERROR 10.0f
#define THRESHOLD          0.40f

// Define the files that are to be saved and the reference images for validation
const char *sSDKsample = "CUDA Smoke Particles";

const char *sRefBin[] =
{
	"ref_smokePart_pos.bin",
	"ref_smokePart_vel.bin",
	NULL
};

void runEmitter();





enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0
};

//
GLuint vao;
GLuint vbo;
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
void initParticles(int numParticles, bool bUseVBO, bool bUseGL)
{
	psystem = new ParticleSystem(numParticles, bUseVBO, bUseGL);
	psystem->reset(ParticleSystem::CONFIG_RANDOM);

	if (bUseVBO)
	{
		renderer = new SmokeRenderer(numParticles);
		renderer->setLightTarget(vec3f(0.0, 1.0, 0.0));

		sdkCreateTimer(&timer);
	}
}
void initParams()
{
	// create a new parameter list
	params = new ParamListGL("misc");

	params->AddParam(new Param<int>("displayed slices", numDisplayedSlices, 0, 256, 1, &numDisplayedSlices));

	params->AddParam(new Param<float>("time step", timestep, 0.0f, 1.0f, 0.001f, &timestep));

	SimParams &p = psystem->getParams();
	params->AddParam(new Param<float>("damping", 0.99f, 0.0f, 1.0f, 0.001f, &p.globalDamping));
	params->AddParam(new Param<float>("gravity", 0.0f, 0.01f, -0.01f, 0.0001f, &p.gravity.y));

	params->AddParam(new Param<float>("noise freq", 0.1f, 0.0f, 1.0f, 0.001f, &p.noiseFreq));
	params->AddParam(new Param<float>("noise strength", 0.001f, 0.0f, 0.01f, 0.001f, &p.noiseAmp));
	params->AddParam(new Param<float>("noise anim", 0.0f, -0.001f, 0.001f, 0.0001f, &p.noiseSpeed.y));

	params->AddParam(new Param<float>("sprite size", spriteSize, 0.0f, 0.1f, 0.001f, &spriteSize));
	params->AddParam(new Param<float>("alpha", alpha, 0.0f, 1.0f, 0.001f, &alpha));

	params->AddParam(new Param<float>("light color r", lightColor[0], 0.0f, 1.0f, 0.01f, &lightColor[0]));
	params->AddParam(new Param<float>("light color g", lightColor[1], 0.0f, 1.0f, 0.01f, &lightColor[1]));
	params->AddParam(new Param<float>("light color b", lightColor[2], 0.0f, 1.0f, 0.01f, &lightColor[2]));

	params->AddParam(new Param<float>("atten color r", colorAttenuation[0], 0.0f, 1.0f, 0.01f, &colorAttenuation[0]));
	params->AddParam(new Param<float>("atten color g", colorAttenuation[1], 0.0f, 1.0f, 0.01f, &colorAttenuation[1]));
	params->AddParam(new Param<float>("atten color b", colorAttenuation[2], 0.0f, 1.0f, 0.01f, &colorAttenuation[2]));
	params->AddParam(new Param<float>("shadow alpha", shadowAlpha, 0.0f, 0.1f, 0.001f, &shadowAlpha));

	params->AddParam(new Param<float>("blur radius", blurRadius, 0.0f, 10.0f, 0.1f, &blurRadius));

	params->AddParam(new Param<float>("emitter radius", emitterRadius, 0.0f, 2.0f, 0.01f, &emitterRadius));
	params->AddParam(new Param<uint>("emitter rate", emitterRate, 0, 10000, 1, &emitterRate));
	params->AddParam(new Param<float>("emitter velocity", emitterVel, 0.0f, 0.1f, 0.001f, &emitterVel));
	params->AddParam(new Param<float>("emitter spread", emitterSpread, 0.0f, 0.1f, 0.001f, &emitterSpread));

	params->AddParam(new Param<float>("particle lifetime", particleLifetime, 0.0f, 1000.0f, 1.0f, &particleLifetime));
}
void runEmitter()
{
	vec3f vel = vec3f(0, emitterVel, 0);
	vec3f vx(1, 0, 0);
	vec3f vy(0, 0, 1);
	vec3f spread(emitterSpread, 0.0f, emitterSpread);

	psystem->sphereEmitter(emitterIndex, cursorPosLag, vel, spread, emitterRadius, ftoi(emitterRate*timestep), particleLifetime, particleLifetime*0.1f);

	if (emitterIndex > numParticles - 1)
	{
		emitterIndex = 0;
	}
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{
	int initialize(void);
	void resize(int, int);

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
		TEXT("OrthoTriangle"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		100,
		100,
		Win_Width, Win_Height,
		NULL,
		NULL,
		hInstance,
		0);

	ghwnd = hwnd;

	
	//
	//
	


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
	
	flag = 1;
	initParticles(numParticles, true, true);
	initParams();
	resize(Win_Width, Win_Height);
	ToggleFullScreen();
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
	/*GLuint gVertexShaderObject;
	GLuint gFragmentShaderObject;
*/

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



	////////////////////////////////// V E R T E X - S H A D E R //////////////////////////


	////Define Vertex Shader Object
	//gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	////Now Write vertex shader code
	//const GLchar **p;
	//const GLchar *vertexShaderSourceCode =

	//	"#version 430 core" \
	//	"\n"
	//	"in vec4 vPosition;" \
	//	"uniform mat4 u_mvp_matrix;" \

	//	"void main(void)" \
	//	"{" \
	//	"gl_Position = u_mvp_matrix * vPosition;" \

	//	"}";
	//// GPU will run the above code. And GPU WILL RUN FOR PER VERTEX. If there are 1000 vertex. Then GPU will run this shader for
	////1000 times. We are Multiplying each vertex with the Model View Matrix.
	////And how does the GPU gets to know about at what offset the array has to be taken . Go to glVertexAttribPointer() in Display.
	//// in = Input. 

	////p = &vertexShaderSourceCode;
	//	//Specify above source code to the vertex shader object
	//glShaderSource(gVertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);

	////Compile the vertex shader 
	//glCompileShader(gVertexShaderObject);

	////////////////// Error Checking//////////////////
	////Code for catching the errors 
	//GLint iShaderCompileStatus = 0;
	//GLint iInfoLogLength = 0;
	//GLchar *szInfoLog = NULL;


	//glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	//if (iShaderCompileStatus == GL_FALSE)
	//{
	//	glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
	//	if (iInfoLogLength > 0)
	//	{
	//		szInfoLog = (GLchar *)malloc(iInfoLogLength);
	//		if (szInfoLog != NULL)
	//		{
	//			GLsizei written;
	//			glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
	//			fprintf(gpLogFile, "%s\n", szInfoLog);
	//			free(szInfoLog);
	//			uninitialize();
	//			DestroyWindow(hwnd);
	//			exit(0);


	//		}
	//	}
	//}
	///////////////////    F R A G M E N T S H A D E R            //////////////////////////
	////Define Vertex Shader Object
	//gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER); //This command will create the Shader Object
	////Now Write vertex shader code
	//const GLchar *fragmentShaderSourceCode =
	//	"#version 430 core" \
	//	"\n" \
	//	"out vec4 FragColor;" \
	//	"void main(void)" \
	//	"{" \
	//	"FragColor =  vec4(1.0,1.0,0.0,0.0);" \
	//	"}";

	////FragColor = vec4(1,1,1,1) = White Color
	////this means here we are giving color to the Triangle.




	////Specify above source code to the vertex shader object
	//glShaderSource(gFragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);

	////Compile the vertex shader 
	//glCompileShader(gFragmentShaderObject);
	////Code for catching the errors 
	//	   /*iShaderCompileStatus = 0;
	//	   iInfoLogLength = 0;*/
	//szInfoLog = NULL;


	//glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	//if (iShaderCompileStatus == GL_FALSE)
	//{
	//	glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
	//	if (iInfoLogLength > 0)
	//	{
	//		szInfoLog = (GLchar *)malloc(iInfoLogLength);
	//		if (szInfoLog != NULL)
	//		{
	//			GLsizei written1;
	//			glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written1, szInfoLog);
	//			fprintf(gpLogFile, "%s\n", szInfoLog);
	//			free(szInfoLog);
	//			uninitialize();
	//			DestroyWindow(hwnd);
	//			exit(0);


	//		}
	//	}
	//}
	//// CREATE SHADER PROGRAM OBJECT
	//gShaderProgramObject = glCreateProgram();
	////attach vertex shader to the gShaderProgramObject
	//glAttachShader(gShaderProgramObject, gVertexShaderObject);

	////attach fragment shader to the gShaderProgramObject
	//glAttachShader(gShaderProgramObject, gFragmentShaderObject);


	////Pre-Linking  binding to vertexAttributes
	//glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPosition");
	////Here the above line means that we are linking the GPU's variable vPosition with the CPU's  enum member  i.e AMC_ATTRIBUTE_POSITION .
	////So whatever changes will be done in AMC_ATTRIBUTE_POSITION , those will also reflect in vPosition

	////RULE : ALWAYS BIND THE ATTRIBUTES BEFORE LINKING AND BIND THE UNIFORM AFTER LINKING.

	////Link the shader program 
	//glLinkProgram(gShaderProgramObject);

	////Code for catching the errors 
	//GLint iProgramLinkStatus = 0;



	//glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iProgramLinkStatus);
	//if (iProgramLinkStatus == GL_FALSE)
	//{
	//	glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
	//	if (iInfoLogLength > 0)
	//	{
	//		szInfoLog = (GLchar *)malloc(iInfoLogLength);
	//		if (szInfoLog != NULL)
	//		{
	//			GLsizei written3;
	//			glGetProgramInfoLog(gShaderProgramObject, iInfoLogLength, &written3, szInfoLog);
	//			fprintf(gpLogFile, "%s\n", szInfoLog);
	//			free(szInfoLog);
	//			uninitialize();
	//			DestroyWindow(hwnd);
	//			exit(0);


	//		}
	//	}
	//}


	////POST Linking
	////Retrieving uniform locations 
	//mvpUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
	////Here we have done all the preparations of data transfer from CPU to GPU

	//const GLfloat triangleVertices[] =
	//{
	//	0.0f,1.0f,0.0f,
	//	-1.0f,-1.0f,0.0f,
	//	1.0f,-1.0f,0.0f
	//};
	////Basically we are giving the vertices coordinates in the above array

	////Create vao - Vertex array objects
	//glGenVertexArrays(1, &vao);
	//glBindVertexArray(vao);
	//glGenBuffers(1, &vbo);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
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
	//glBindVertexArray(0);


	//Depth Lines
	/*glClearDepth(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);*/
	//initializing Perspective ProjectionMatrix to identity.
	perspectiveProjectionMatrix = mat4::identity();

	

	return(0);
}
GLuint createTexture(GLenum target, GLint internalformat, GLenum format, int w, int h, void *data)
{
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(target, tex);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(target, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(target, 0, internalformat, w, h, 0, format, GL_UNSIGNED_BYTE, data);
	return tex;
}

GLuint loadTexture(char *filename)
{
	unsigned char *data = 0;
	unsigned int width, height;
	sdkLoadPPM4ub(filename, &data, &width, &height);

	if (!data)
	{
		printf("Error opening file '%s'\n", filename);
		return 0;
	}

	printf("Loaded '%s', %d x %d pixels\n", filename, width, height);

	return createTexture(GL_TEXTURE_2D, GL_RGBA8, GL_RGBA, width, height, data);
}
void resize(int width, int height)
{


	//if (height == 0)
	//{
	//	height = 1;
	//}

	//glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	///*if (width < height)
	//{
	//	orthographicProjectionMatrix = ortho(
	//		-100.0f, 100.0f,
	//		-100.0f * ((float)height / (float)width), 100.0f * ((float)height / (float)width),
	//		-100.0f, 100.0f);
	//}
	//else
	//{
	//	orthographicProjectionMatrix = ortho(
	//		-100.0f * ((float)width / (float)height), 100.0f * ((float)width / (float)height),
	//		-100.0f, 100.0f,
	//		-100.0f, 100.0f);
	//}*/
	//perspectiveProjectionMatrix = perspective(60.0f,
	//	(GLfloat)width / (GLfloat)height,
	//	0.01f,
	//	100.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (float)width / (float)height, 0.01, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, width, height);

	if (flag == 1)
	{
		fprintf(gpLogFile, "Resize being called. I am in flag  == 1\n");
		
		renderer->setFOV(60.0);
		renderer->setWindowSize(width, height);
		//flag = 0;
	}
}



void display(void)
{


	sdkStartTimer(&timer);
	glLoadIdentity();
	glTranslatef(0.0f, -1.0f, -5.0f);
	if (!paused)
	{

		if (emitterOn)
		{
			runEmitter();
		}

		SimParams &p = psystem->getParams();
		p.cursorPos = make_float3(cursorPosLag.x, cursorPosLag.y, cursorPosLag.z);

		psystem->step(timestep);
		currentTime += timestep;
	}


	renderer->calcVectors();
	vec3f sortVector = renderer->getSortVector();

	psystem->setSortVector(make_float3(sortVector.x, sortVector.y, sortVector.z));
	//psystem->setModelView(modelView);
	psystem->setSorting(sort);
	psystem->depthSort();

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//renderer->beginSceneRender(SmokeRenderer::LIGHT_BUFFER);
	////renderScene();
	//renderer->endSceneRender(SmokeRenderer::LIGHT_BUFFER);


	//This call will lead you to glBindFrameBufferExt();
	renderer->beginSceneRender(SmokeRenderer::SCENE_BUFFER);
	//renderScene();
	renderer->endSceneRender(SmokeRenderer::SCENE_BUFFER);

	renderer->setPositionBuffer(psystem->getPosBuffer());
	renderer->setVelocityBuffer(psystem->getVelBuffer());
	renderer->setIndexBuffer(psystem->getSortedIndexBuffer());

	renderer->setNumParticles(psystem->getNumParticles());
	renderer->setParticleRadius(spriteSize);
	renderer->setDisplayLightBuffer(displayLightBuffer);
	renderer->setAlpha(alpha);
	renderer->setShadowAlpha(shadowAlpha);
	renderer->setLightPosition(lightPos);
	renderer->setColorAttenuation(colorAttenuation);
	renderer->setLightColor(lightColor);
	renderer->setNumSlices(numSlices);
	renderer->setNumDisplayedSlices(numDisplayedSlices);
	renderer->setBlurRadius(blurRadius);

	renderer->render();

	if (drawVectors)
	{
		renderer->debugVectors();
	}
	// use shader program
	//glUseProgram(gShaderProgramObject);

	////declaration of matrices
	//mat4 modelViewMatrix;
	//mat4 modelViewProjectionMatrix;

	//// intialize above matrices to identity
	//modelViewMatrix = mat4::identity();
	//modelViewProjectionMatrix = mat4::identity();

	//// perform necessary transformations
	//modelViewMatrix = translate(0.0f, 0.0f, -3.0f);
	//// do necessary matrix multiplication
	//// this was internally done by glOrtho() in FFP.	

	//modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	//// send necessary matrices to shader in respective uniforms
	//glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	////GL_FALSE = Should we transpose the matrix?
	////DirectX is roj major so there we will have to transpose but OpenGL is Colomn major so no need to transpose. 



	//// bind with vao
	////this will avoid many binding to vbo
	//glBindVertexArray(vao);

	//// bind with textures

	//// draw necessary scene
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	////  0 =  From where to start in the array. 
	//// We have to start from 0th element i.e 0. if the 0th element would be 50.0f then we would have given the 2nd parameter as 50.
	////3 = How many Vertices? 
	////GL_TRIANGLES is the thing between glBegin() and glEnd()


	//// unbind vao
	//glBindVertexArray(0);

	//// unuse program
	//glUseProgram(0);
	SwapBuffers(ghdc);
	sdkStopTimer(&timer);

	fpsCount++;
}

void uninitialize(void)
{
	if (vbo)
	{
		glDeleteBuffers(1, &vbo);
		vbo = 0;
	}

	if (vao)
	{
		glDeleteBuffers(1, &vao);
		vao = 0;
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