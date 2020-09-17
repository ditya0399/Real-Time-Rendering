#include<Windows.h>
#include"glew.h"               // This SHOULD be above GL.h header.
#include<gl/GL.h>
#include<stdio.h>
#include<time.h>
#include<windowsx.h>
#include <iostream>
#include <map>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#include"vmath.h"
#include"3DTexture.h"
#pragma comment(lib,"opengl32.lib")//for linking
#pragma comment(lib,"glew32.lib")//for linking
#pragma comment(lib,"FreeType.lib")//for linking



// CUDA HEADERS
#include "include/cuda_gl_interop.h"
#include"include/cuda_runtime.h"
#include"inc/helper_cuda.h"
#include"inc/helper_timer.h"
#pragma comment(lib,"cudart.lib")

//timer
StopWatchInterface *timerGPU = NULL;
StopWatchInterface *timerCPU = NULL;

//Variables for font rendering 

float Red, RedTitle,RedTitleEnd,RedEnd;
float Green, GreenTitle,GreenTitleEnd,GreenEnd;
float Blue, BlueTitle,BlueEnd,BlueTitleEnd;

float timeGPU;
float timeCPU;

bool PresentsToggle = true;
bool EndCreditsToggle = true;
bool SirEndCreditsToggle = true;
bool TitleToggle = true;

bool startDisplay = false;
bool mainCUDAContent = false;
GLuint vao_Text;
GLuint vbo_position_Text;
GLuint gShaderProgramObjectFont;
struct Character{
	unsigned int TextureID; // ID handle of the glyph texture
	vmath::ivec2   Size;      // Size of glyph
	vmath::ivec2   Bearing;   // Offset from baseline to left/top of glyph
	unsigned int Advance;   // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;

bool encreditsCall = false;
//


#define Z_PLANE 50.0f

float xoffset, yoffset, sensitivity;
//for mouse Position
LONG xPos;
LONG  yPos;

bool firstMouse = true;

float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

clock_t start_t, end_t, total_t;

clock_t deltaTime = 0.0f;	// time between current frame and last frame
clock_t lastFrame = 0.0f;
vmath::vec3 cameraPos = vmath::vec3(0.0f, 0.0f, 0.0f);
vmath::vec3 cameraFront = vmath::vec3(0.0f, 0.0f, -1.0f);
vmath::vec3 cameraUp = vmath::vec3(0.0f, 1.0f, 0.0f);
float CameraSpeed = 0.08;

bool CameraToggle = false;

int k = 0;
float gain = 0.75f, xStart = 2.0f, yStart = 1.0f;
float zOffset = 0.0f, octaves = 2.0f, lacunarity = 2.0f;

float start[2];
float delta[2];

bool Toggle = true;
bool ToggleCPU = true;


char CPUTIME[20];
char GPUTIME[20];

//CUDA VARIABLES

GLuint vbo_texture;
GLuint gTextureSamplerUniform;
GLuint gTextureStone;


float zpos = -3.0f;
float ypos = 0.0f;
float xpos = 0.0f;

const int gMesh_Width = 512;
const int gMesh_Height = 512;
#define MYARRAYSIZE gMesh_Width * gMesh_Height * 4
float pos[gMesh_Width * gMesh_Height][4];
struct cudaGraphicsResource *graphicsResource = NULL;
GLuint vbo_GPU;
float animationTime = 0.0f;
bool bOnGPU = false;
cudaError_t error;
GLuint vbo_GPU_Colors;
struct cudaGraphicsResource *graphicsResourceColor = NULL;


float positions[gMesh_Height *gMesh_Height *4];
float Colors[gMesh_Width *gMesh_Height][4];
float Textures[256 *256] [2];

bool MainSceneFadeOut;

const static unsigned char h_perm[] = { 151,160,137,91,90,15,
   131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
   190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
   88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
   77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
   102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
   135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
   5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
   223,183,170,213,119,248,152,2,44,154,163, 70,221,153,101,155,167, 43,172,9,
   129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
   251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
   49,192,214, 31,181,199,106,157,184,84,204,176,115,121,50,45,127, 4,150,254,
   138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};
static unsigned char s_perms[256];



inline int permCPU(int);
inline float fadeCPU(float);
inline float lerpPCPU(float, float, float);
inline float gradCPU(int, float, float, float);
float inoiseCPU(float x, float y, float z) {


	int X = ((int)floorf(x)) & 255, // FIND UNIT CUBE THAT
		Y = ((int)floorf(y)) & 255,   // CONTAINS POINT.
		Z = ((int)floorf(z)) & 255;
	x -= floorf(x);               // FIND RELATIVE X,Y,Z
	y -= floorf(y);               // OF POINT IN CUBE.
	z -= floorf(z);
	float u = fadeCPU(x),            // COMPUTE FADE CURVES
		v = fadeCPU(y),                // FOR EACH OF X,Y,Z.
		w = fadeCPU(z);
	int A = permCPU(X) + Y, AA = permCPU(A) + Z, AB = permCPU(A + 1) + Z, // HASH COORDINATES OF
		B = permCPU(X + 1) + Y, BA = permCPU(B) + Z, BB = permCPU(B + 1) + Z; // THE 8 CUBE CORNERS,

	return lerpPCPU(w, lerpPCPU(v, lerpPCPU(u, gradCPU(permCPU(AA), x, y, z), // AND ADD
		gradCPU(permCPU(BA), x - 1.0f, y, z)),   // BLENDED
		lerpPCPU(u, gradCPU(permCPU(AB), x, y - 1.0f, z),    // RESULTS
			gradCPU(permCPU(BB), x - 1.0f, y - 1.0f, z))),     // FROM  8
		lerpPCPU(v, lerpPCPU(u, gradCPU(permCPU(AA + 1), x, y, z - 1.f),  // CORNERS
			gradCPU(permCPU(BA + 1), x - 1.0f, y, z - 1.0f)),    // OF CUBE
			lerpPCPU(u, gradCPU(permCPU(AB + 1), x, y - 1.0f, z - 1.0f),
				gradCPU(permCPU(BB + 1), x - 1.0f, y - 1.0f, z - 1.0f))));

}
float fBmCPU(float x, float y, int octaves, float lacunarity = 2.0f, float gain = 0.5f)
{
	float freq = 1.0f, amp = 0.5f;
	float sum = 0.0f;
	for (int i = 0; i < octaves; i++) {
		sum += inoiseCPU(x*freq, y*freq, Z_PLANE)*amp;
		freq *= lacunarity;
		amp *= gain;
	}
	return sum;
}



extern "C"
void launch_kernel(float4* pos, uchar4* posColor,
	unsigned int mesh_width, unsigned int mesh_height, float time);



int Win_Width = 800;
int Win_Height = 600;
using namespace vmath;

static GLfloat angle = 0.0f;
GLuint gShaderProgramObject;

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
GLuint vbo_Color_CPU;
//GLuint vbo_GPU;
GLuint mvpUniform;
GLuint mvpUniformFont;
mat4 perspectiveProjectionMatrix;

float uData[1], vData[1];
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
void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, vmath::vec3 color);
void FontColorAnimationUpdates();
void initFreeType();
void initializeFontShaders();
void ToggleFullScreen(void);
void update(void);
void uninitialize(void);
void launchCPUKernel(unsigned int MeshWidth, unsigned int MeshHeight, float Time);
void launchCudaKernel(float4 *,unsigned int  , unsigned int  ,float );
void EndCreditsFontAnimationUpdates();



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
		TEXT("CUDA - 3D Perlin Noise Terrain - Aditya Boob"),
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
	case WM_MOUSEMOVE:

		xPos = GET_X_LPARAM(lParam);
		//fprintf(gpLogFile, " X POSITION : %ld", xPos);
		yPos = GET_Y_LPARAM(lParam);
	//	fprintf(gpLogFile, " Y POSITION : %ld", yPos);



		if (firstMouse == true)
		{
			lastX = xPos;
			lastY = yPos;
			firstMouse = false;
		}


		xoffset = xPos - lastX;
		yoffset = lastY - yPos; // reversed since y-coordinates go from bottom to top
		lastX = xPos;
		lastY = yPos;

		sensitivity = 0.1f; // change this value to your liking
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		/*if (yaw > 350.0f)
			yaw = 350.0f;
		if (yaw < -350.0)
			yaw = -350.0f;*/
		break;
	case WM_ERASEBKGND:
		return(0);

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	
	case WM_CHAR:
		switch (wParam)
		{
				
	/*	case 'w':
			zpos += 0.2f;
			break;
		case 's':
			zpos -= 0.2f;
			break;

		case 'a':
			xpos += 0.2f;
			break;
		case 'd':
			xpos -= 0.2f;
			break;*/

		case 'R':
		case 'r':
			cameraPos = vmath::vec3(0.0f, 0.0f, 0.0f);
			cameraFront = vmath::vec3(0.0f, 0.0f, -1.0f);
			cameraUp = vmath::vec3(0.0f, 1.0f, 0.0f);

			CameraToggle = !CameraToggle;
			break;

		case 'B':
		case 'b':
			//MainSceneFadeOut = !MainSceneFadeOut;
		//	EndCreditsFontAnimationUpdates();
			encreditsCall = !encreditsCall;
			break;
		
		case 'W':
		case 'w':
			cameraPos = cameraPos + (CameraSpeed * cameraFront);
			break;
		case 's':
		case 'S':
			cameraPos -= CameraSpeed * cameraFront;
			break;
		case 'A':
		case 'a':
			cameraPos -= vmath::normalize(vmath::cross(cameraFront, cameraUp)) * CameraSpeed;
			break;
		case 'D':
		case 'd':
			cameraPos += vmath::normalize(vmath::cross(cameraFront, cameraUp))* CameraSpeed;
			break;

		case 'q':
			ypos += 0.2f;
			break;
		case 'z':
			ypos -= 0.2f;
			break;


		case '+': // Perlin: lower the ocean level
			zOffset += 0.01;
			zOffset = (zOffset > 1.0) ? 1.0 : zOffset; // guard input
			break;
		case '-': // Perlin: raise the ocean level
			zOffset -= 0.01;
			zOffset = (zOffset < -1.0) ? -1.0 : zOffset; // guard input
			break;
		case 'k': // move within the Perlin function
			yStart -= 0.1;
			break;
		case 'j': // move within the Perlin function
			yStart += 0.1;
			break;
		case 'l': // move within the Perlin function
			xStart += 0.1;
			break;
		case 'h': // move within the Perlin function
			xStart -= 0.1;
			break;
		case 'I': // Perlin: change gain
			gain += 0.25;
			break;
		case 'i': // Perlin: change gain
			gain -= 0.25;
			gain = (gain < 0.25) ? 0.25 : gain; // guard input
			break;
		case 'O': // Perlin: change octaves
			octaves += 1.0f;
			octaves = (octaves > 8) ? 8 : octaves; // guard input
			break;
		case 'o': // Perlin: change octaves
			octaves -= 1.0f;
			octaves = (octaves < 2) ? 2 : octaves; // guard input
			break;
		case 'P': // Perlin: change lacunarity
			lacunarity += 0.25;
			break;
		case 'p': // Perlin: change lacunarity
			lacunarity -= 0.25;
			lacunarity = (lacunarity < 0.2) ? 0.2 : lacunarity; // guard input
			break;
		}
		break;
	case WM_KEYDOWN:

		switch (wParam)
		{
		case VK_SPACE:
			startDisplay = true;
			break;
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;
		case VK_NUMPAD1:
			
			break;
		case 'F':
		case 'f':
			ToggleFullScreen();
			break;
		case 'G':
		case 'g':
			bOnGPU = true;
			break;
		case 'C':
		case 'c':
			bOnGPU = false;
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
int LoadGLTextures(GLuint *texture, TCHAR imageResourceId[])
{
	//variable declarations
	HBITMAP hBitmap = NULL;
	BITMAP bmp;
	int iStatus = FALSE;

	//code
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResourceId, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	glGenTextures(1, texture);

	if (hBitmap)
	{
		iStatus = TRUE;
		GetObject(hBitmap, sizeof(bmp), &bmp);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	// Set 1 rather than default 4 for better performance

		//Texture steps:
		glBindTexture(GL_TEXTURE_2D, *texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		//Generate mipmapped texture
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGB,
			bmp.bmWidth,
			bmp.bmHeight,
			0,
			GL_BGR,
			GL_UNSIGNED_BYTE,
			bmp.bmBits);

		// Create mipmaps for this texture for better image quality:
		glGenerateMipmap(GL_TEXTURE_2D);

		DeleteObject(hBitmap);
	}
	return(iStatus);

}
int initialize(void)
{
	

	int LoadGLTextures(GLuint *, TCHAR[]);

	int devCount;
	error = cudaGetDeviceCount(&devCount);
	if (error != cudaSuccess)
	{
		
		fprintf(gpLogFile, "ERROR : DevCount is 0\n");
		DestroyWindow(hwnd);
	}
	else if (devCount == 0)
	{
		fprintf(gpLogFile, "ERROR : DevCount is 0\n");
		DestroyWindow(hwnd);
	}
	else
	{
		cudaSetDevice(0);
	}


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
		"in vec2 vTexCoord;" \
		"out vec2 outTexCoord;" \
		"out vec4 out_Color;" \
		"uniform mat4 u_mvp_matrix;" \
		
		"void main(void)" \
		"{" \
		"gl_Position = u_mvp_matrix * vPosition;" \
		"out_Color = vColor;" \
		"outTexCoord = vTexCoord;" \
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
		"vec4 FragColor;" \
		"out vec4 FinalColor;" \
		"in vec4 out_Color;" \
		"in vec2 outTexCoord;" \
		"uniform sampler2D u_texture_sampler;" \
		"uniform float FadeOut;" 
		"void main(void)" \
		"{" \
		"FragColor =  out_Color;"	\
		"FinalColor =FragColor;" \
		"}";

	//FragColor = vec4(1,1,1,1) = White Color
	//this means here we are giving color to the Triangle.

	//"FinalColor = mix(vec4(FragColor),vec4(0.0),FadeOut);" \


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
	glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_TEXCOORD0, "vTexCoord");
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
	gTextureSamplerUniform = glGetUniformLocation(gShaderProgramObject, "u_texture_sampler");
	//Here we have done all the preparations of data transfer from CPU to GPU

	const GLfloat triangleVertices[] =
	{
		0.0f,1.0f,0.0f,
		-1.0f,-1.0f,0.0f,
		1.0f,-1.0f,0.0f
	};

	// Using Nested Loop 0Out the whole Pos Array
	/*for (int i = 0; i < gMesh_Width; i++)
	{
		for (int j = 0; j < gMesh_Height; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				pos[i][j][k] = 0.0f;
			}
		}
	}*/
	//Basically we are giving the vertices coordinates in the above array


	for (int i = 0; i < 256 ; i++)
	{
		for(int j = 0; j < 2; j++)
		{
			Textures[i][j] = 0.0f;
			Textures[i][j] = 0.0f;
			i += 4;
		}
	}
	for (int i = 1; i < 256 * 256; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			Textures[i][j] = 0.0f;
			Textures[i][j] = 0.0f;
			i += 4;
		}
	}
	for (int i = 2; i < 256 * 256; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			Textures[i][j] = 0.0f;
			Textures[i][j] = 0.0f;
			i += 4;
		}
	}
	for (int i = 3; i < 256 * 256; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			Textures[i][j] = 0.0f;
			Textures[i][j] = 0.0f;
			i += 4;
		}
	}
	const GLfloat cubeTexCoords[] =
	{							// Kundali textured Cube
		/*0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
*/
	/*	0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,*/

		//0.0f, 0.0f,
		//1.0f, 0.0f,
		//1.0f, 1.0f,
		//0.0f, 1.0f,

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
	};

	//Create vao - Vertex array objects
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, gMesh_Width * gMesh_Height *4 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	


	glGenBuffers(1, &vbo_Color_CPU);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Color_CPU);
	//in the above statement we have accesed the GL-ARRAY_BUFFER using vbo. Without it wouldn't be possible to get GL_ARRAY_BUFFER
	//For the sake of understanding . GL_ARRAY_BUFFER is in the GPU side ad=nd we have bind our CPU side vbo with it like a Pipe to get the access.
	glBufferData(GL_ARRAY_BUFFER, gMesh_Width * gMesh_Height * 4 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_texture);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Textures), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// GPU 
	glGenBuffers(1, &vbo_GPU);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_GPU);
	glBufferData(GL_ARRAY_BUFFER, MYARRAYSIZE * sizeof(float4), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Register our vbo with the CUDA GRAPHICS RESOURCE
	error = cudaGraphicsGLRegisterBuffer(&graphicsResource, vbo_GPU, cudaGraphicsMapFlagsWriteDiscard);


	glGenBuffers(1, &vbo_GPU_Colors);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_GPU_Colors);
	glBufferData(GL_ARRAY_BUFFER, MYARRAYSIZE * sizeof(uchar4), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Register our vbo with the CUDA GRAPHICS RESOURCE
	error = cudaGraphicsGLRegisterBuffer(&graphicsResourceColor, vbo_GPU_Colors, cudaGraphicsMapFlagsWriteDiscard);


	glBindVertexArray(0);



	LoadGLTextures(&gTextureStone, MAKEINTRESOURCE(IDBITMAP_STONE));

	initializeFontShaders();
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
void initializeFontShaders()
{
	GLuint gVertexShaderObject;
	GLuint gFragmentShaderObject;

	//Define Vertex Shader Object
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar **p;
	const GLchar *vertexShaderSourceCode =

		"#version 450 core"\
		"\n"\
		"in vec4 vPosition;"\
		"out vec2 TexCoords;"\
		"uniform mat4 u_mvp_matrix;"\
		"void main()"\
		"{"\
		"gl_Position = u_mvp_matrix * vec4(vPosition.xy, 0.0, 1.0);"\
		"TexCoords = vPosition.zw;"
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
		"#version 450 core"\
		"\n"\
		"in vec2 TexCoords;"\
		"uniform float iTime;" \
		"uniform vec3 iResolution;" \
		"out vec4 color;"\
		"uniform sampler2D text;"\
		"uniform vec3 textColor;"\
		"void main()"\
		"{"\


		"vec4 sampled = vec4(1.0,1.0,1.0,texture(text,TexCoords ).r);"\
		"color = vec4(textColor, 1.0) *sampled;"\
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
	gShaderProgramObjectFont = glCreateProgram();
	//attach vertex shader to the gShaderProgramObjectFont
	glAttachShader(gShaderProgramObjectFont, gVertexShaderObject);

	//attach fragment shader to the gShaderProgramObjectFont
	glAttachShader(gShaderProgramObjectFont, gFragmentShaderObject);


	//Pre-Linking  binding to vertexAttributes
	glBindAttribLocation(gShaderProgramObjectFont, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObjectFont, AMC_ATTRIBUTE_COLOR, "vColor");
	//Here the above line means that we are linking the GPU's variable vPosition with the CPU's  enum member  i.e AMC_ATTRIBUTE_POSITION .
	//So whatever changes will be done in AMC_ATTRIBUTE_POSITION , those will also reflect in vPosition

	//RULE : ALWAYS BIND THE ATTRIBUTES BEFORE LINKING AND BIND THE UNIFORM AFTER LINKING.

	//Link the shader program 
	glLinkProgram(gShaderProgramObjectFont);

	//Code for catching the errors 
	GLint iProgramLinkStatus = 0;



	glGetProgramiv(gShaderProgramObjectFont, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObjectFont, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written3;
				glGetProgramInfoLog(gShaderProgramObjectFont, iInfoLogLength, &written3, szInfoLog);
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
	mvpUniformFont = glGetUniformLocation(gShaderProgramObjectFont, "u_mvp_matrix");
	//Here we have done all the preparations of data transfer from CPU to GPU

	
	initFreeType();

	glGenVertexArrays(1, &vao_Text);
	glBindVertexArray(vao_Text);
	glGenBuffers(1, &vbo_position_Text);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_Text);
	//in the above statement we have accesed the GL-ARRAY_BUFFER using vbo. Without it wouldn't be possible to get GL_ARRAY_BUFFER
	//For the sake of understanding . GL_ARRAY_BUFFER is in the GPU side ad=nd we have bind our CPU side vbo with it like a Pipe to get the access.
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);
	//GL_FALSE = We are not giving normalized coordinates as our coordinates are not converted in 0 - 1 range.
	//3 = This is the thing I was talking about in initialize. Here, we are telling GPU to break our array in 3 parts . 
	//0 and Null are for the Interleaved. 
	//GL_FLOAT- What is the type? .
	//AMC_ATTRIBUTE_POSITION. here we are passing data to vPosition. 

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	

	glBindVertexArray(0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
void initFreeType()
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		MessageBox(NULL, TEXT("ERROR::FREETYPE: Could Not init FreeType Library"), TEXT("ERROR"), MB_OK);
	FT_Face face;
	if (FT_New_Face(ft, "C:\\Windows\\Fonts\\Arial.ttf", 0, &face))
		MessageBox(NULL, TEXT("ERROR::FREETYPE: Failed to load font"), TEXT("ERROR"), MB_OK);
	FT_Set_Pixel_Sizes(face, 0, 48);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (GLubyte c = 0; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			MessageBox(NULL, TEXT("ERROR::FREETYPE: Failed to load Glyph"), TEXT("ERROR"), MB_OK);
			continue;
		}
		GLuint texturePresents;
		glGenTextures(1, &texturePresents);
		glBindTexture(GL_TEXTURE_2D, texturePresents);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Character character = {
			texturePresents,
			vmath::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			vmath::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

}


void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, vmath::vec3 color)
{
	glUniform3fv(glGetUniformLocation(gShaderProgramObjectFont, "textColor"), 1, color);
	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(vao_Text);
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing[0] * scale;
		GLfloat ypos = y - (ch.Size[1] - ch.Bearing[1]) * scale;

		GLfloat w = ch.Size[0] * scale;
		GLfloat h = ch.Size[1] * scale;

		GLfloat vertices[6][4] = {
			{ xpos, ypos + h, 0.0, 0.0},
			{ xpos, ypos,     0.0, 1.0},
			{ xpos + w, ypos, 1.0, 1.0},
			{ xpos, ypos + h, 0.0, 0.0},
			{ xpos + w, ypos, 1.0, 1.0},
			{ xpos + w, ypos + h, 1.0,0.0}
		};

		glBindTexture(GL_TEXTURE_2D, ch.TextureID);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position_Text);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += (ch.Advance >> 6) * scale;

	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

}


void displayFont()
{
	

	glUseProgram(gShaderProgramObjectFont);

	//declaration of matrices
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 RotationMatrix;
	mat4 TranslateMatrix;
	// intialize above matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	TranslateMatrix = translate(-2.5f, 0.0f, -55.0f);

	// perform necessary transformations
	modelViewMatrix = TranslateMatrix;
	// do necessary matrix multiplication
	// this was internally done by gluPerspective() in FFP.	

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	// send necessary matrices to shader in respective uniforms
	glUniformMatrix4fv(mvpUniformFont, 1, GL_FALSE, modelViewProjectionMatrix);
	//	RenderText("ASTROMEDICOMP'S", -23.0f, 6.0f, 0.1f, vmath::vec3(1.0f, Green, Blue));

	FontColorAnimationUpdates();



	// unuse program
	glUseProgram(0);
}
void displayCPUAndGPUFont()
{

	glUseProgram(gShaderProgramObjectFont);

	//declaration of matrices
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 RotationMatrix;
	mat4 TranslateMatrix;
	// intialize above matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	TranslateMatrix = translate(-2.5f, 0.0f, -55.0f);

	// perform necessary transformations
	modelViewMatrix = TranslateMatrix * scale(0.3f,0.3f,0.3f);
	// do necessary matrix multiplication
	// this was internally done by gluPerspective() in FFP.	

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	// send necessary matrices to shader in respective uniforms
	glUniformMatrix4fv(mvpUniformFont, 1, GL_FALSE, modelViewProjectionMatrix);
	//	RenderText("ASTROMEDICOMP'S", -23.0f, 6.0f, 0.1f, vmath::vec3(1.0f, Green, Blue));

	if (bOnGPU == false)
	{
		sprintf_s(CPUTIME, "%f", timeCPU);
		RenderText("On    : CPU", 90.0f, 68.0f, 0.1f, vmath::vec3(1.0f, 0.0f, 0.0f));		
		
		RenderText(CPUTIME, 105.0f, 62.0f, 0.1f, vmath::vec3(1.0f, 0.0f, 0.0f));
		RenderText("Time : ", 90.0f, 62.0f, 0.1f, vmath::vec3(1.0f, 0.0f, 0.0f));
		RenderText("ms", 132.0f, 62.0f, 0.1f, vmath::vec3(1.0f, 0.0f, 0.0f));
		
		RenderText("Vertices : 512 * 512 * 4 ", -120.0f, 68.0f, 0.1f, vmath::vec3(1.0f, 1.0f, 0.0f));
	
	    //RenderText("MESH : 256 * 256 * 4", -33.0f, 18.0f, 0.1f, vmath::vec3(1.0f, 1.0f, 0.0f));
	}
	else
	{
		sprintf_s(GPUTIME, "%f", timeGPU);
		RenderText("On    : GPU", 90.0f, 68.0f, 0.1f, vmath::vec3(0.0f, 1.0f, 0.0f));

		RenderText(GPUTIME, 105.0f, 62.0f, 0.1f, vmath::vec3(0.0f, 1.0f, 0.0f));
		RenderText("ms", 126.5f, 62.0f, 0.1f, vmath::vec3(0.0f, 1.0f, 0.0f));
		RenderText("Time : ", 90.0f, 62.0f, 0.1f, vmath::vec3(0.0f, 1.0f, 0.0f));

		RenderText("Vertices : 512 * 512 * 4 ", -120.0f, 68.0f, 0.1f, vmath::vec3(1.0f, 1.0f, 0.0f));

	}



	// unuse program
	glUseProgram(0);
}
void displayCudaAndCpu()
{	
	void displayCPUAndGPUFont();

	static float FadeOut =0.0f;

	displayCPUAndGPUFont();
	glUseProgram(gShaderProgramObject);

	float4 *pPos = NULL;
	uchar4 *pColor = NULL;

	size_t byteCount;
	glBindVertexArray(vao);
	if (bOnGPU == true)
	{
		// 4 Steps Total : Do error checking on each except 3rd step
		cudaGraphicsMapResources(1, &graphicsResource, 0);
		if (error != cudaSuccess)
		{
			fprintf(gpLogFile, "STEP 1  FAILED\n");
		}
		error = cudaGraphicsResourceGetMappedPointer((void **)&pPos, &byteCount, graphicsResource);
		if (error != cudaSuccess)
		{
			fprintf(gpLogFile, "STEP 2 FAILED\n");
		}

		cudaGraphicsMapResources(1, &graphicsResourceColor, 0);
		if (error != cudaSuccess)
		{
			fprintf(gpLogFile, "STEP 1  FAILED\n");
		}
		error = cudaGraphicsResourceGetMappedPointer((void **)&pColor, &byteCount, graphicsResourceColor);
		if (error != cudaSuccess)
		{
			fprintf(gpLogFile, "STEP 2 FAILED\n");
		}

		sdkCreateTimer(&timerGPU);
		sdkStartTimer(&timerGPU);

		launch_kernel(pPos, pColor, gMesh_Width, gMesh_Height, animationTime);
		cudaMemcpy(positions, pPos, 4 * gMesh_Width * gMesh_Height * sizeof(float), cudaMemcpyDeviceToHost);
		error = cudaGraphicsUnmapResources(1, &graphicsResource, 0);
		error = cudaGraphicsUnmapResources(1, &graphicsResourceColor, 0);


		
		timeGPU = sdkGetTimerValue(&timerGPU);
		
		sdkDeleteTimer(&timerGPU);


		if (error != cudaSuccess)
		{
			fprintf(gpLogFile, "STEP 3 FAILED\n");
		}

		
	}
	else
	{
		sdkCreateTimer(&timerCPU);
		sdkStartTimer(&timerCPU);
		

		launchCPUKernel(gMesh_Width, gMesh_Height, animationTime);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, gMesh_Width * gMesh_Height * 4 * sizeof(float) , pos, GL_DYNAMIC_DRAW);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_Color_CPU);
		glBufferData(GL_ARRAY_BUFFER, gMesh_Width * gMesh_Height * 4 * sizeof(float), Colors, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		
		timeCPU = sdkGetTimerValue(&timerCPU);
		sdkDeleteTimer(&timerCPU);
		

	}
	if (bOnGPU == true)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo_GPU);
	}
	else
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
	}
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	if (bOnGPU == true)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo_GPU_Colors);
		glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}
	else
	{
		//Here the new Color array will be given by the CPU to the GPU. 
		//Calculated on CPU.
		glBindBuffer(GL_ARRAY_BUFFER, vbo_Color_CPU);
		glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 4, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}


	/*glBindBuffer(GL_ARRAY_BUFFER, vbo_texture);
	glBufferData(GL_ARRAY_BUFFER, gMesh_Width * gMesh_Height * 4 * sizeof(float), pos, GL_DYNAMIC_DRAW);
*/
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//declaration of matrices
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 viewMatrix;

	viewMatrix = lookat(cameraPos,cameraPos + cameraFront,cameraUp);

	// intialize above matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	// perform necessary transformations

	if (!CameraToggle)
	{
		modelViewMatrix = translate(0.0f, -0.0f, -10.0f);
		modelViewMatrix *= scale(50.0f, 25.0f, 50.0f);
	}
	else
	{
		modelViewMatrix = translate(0.0f, -0.0f, -3.0f);
		modelViewMatrix *= rotate(90.0f, 1.0f, 0.0f, 0.0f);
	}
	modelViewMatrix = viewMatrix * modelViewMatrix;
	//modelViewMatrix *= rotate(90.0f, 1.0f, 0.0f, 0.0f);
	// do necessary matrix multiplication
	// this was internally done by glOrtho() in FFP.	

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	// send necessary matrices to shader in respective uniforms
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform1f(glGetUniformLocation(gShaderProgramObject, "FadeOut"), FadeOut);

	if (MainSceneFadeOut)
	{
		FadeOut += 0.0003f;
	}
	//glActiveTexture(GL_TEXTURE0);					
	//glBindTexture(GL_TEXTURE_2D, gTextureStone);	
	//glUniform1i(gTextureSamplerUniform, 0);

	// bind with textures

	// draw necessary scene
	glDrawArrays(GL_LINES, 0, gMesh_Width * gMesh_Height*4);
	
	glBindVertexArray(0);

	vec3 front(cos(vmath::radians(pitch)) * cos(vmath::radians(yaw)),
		sin(vmath::radians(pitch)),
		cos(vmath::radians(pitch))*sin(vmath::radians(yaw)));

	cameraFront = vmath::normalize(front);

	glUseProgram(0);
}

void display(void)
{
	void displayFont();
	
	
		end_t = clock();
		deltaTime = end_t - lastFrame;
		lastFrame = end_t;


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (startDisplay)
		{
			if (encreditsCall)
				EndCreditsFontAnimationUpdates();
			else
				displayFont();
		}

			displayCudaAndCpu();


		SwapBuffers(ghdc);
		animationTime = animationTime + 0.01f;
	
}

void EndCreditsFontAnimationUpdates()
{
	glUseProgram(gShaderProgramObjectFont);

	//declaration of matrices
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 RotationMatrix;
	mat4 TranslateMatrix;
	// intialize above matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	TranslateMatrix = translate(-2.5f, 0.0f, -55.0f);

	// perform necessary transformations
	modelViewMatrix = TranslateMatrix;
	// do necessary matrix multiplication
	// this was internally done by gluPerspective() in FFP.	

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	// send necessary matrices to shader in respective uniforms
	glUniformMatrix4fv(mvpUniformFont, 1, GL_FALSE, modelViewProjectionMatrix);
	if (RedEnd <= 1.0f && EndCreditsToggle)
	{
		/*RenderText("ASTROMEDICOMP'S", -23.0f, 6.0f, 0.1f, vmath::vec3(1.0f, Green, Blue));
		RenderText("BLEND", -18.0f, -2.0f, 0.1f, vmath::vec3(Red, Green, 0.0f));
		RenderText("GROUP", 1.5f, -2.0f, 0.1f, vmath::vec3(Red, Green, 0.0f));
		RenderText("PRESENTS", -14.0f, -10.0f, 0.1f, vmath::vec3(Red, Green, 0.0f));*/

		RenderText("TECHNOLOGY : CUDA", -33.0f, 6.0f, 0.1f, vmath::vec3(RedEnd,GreenEnd,BlueEnd));
		RenderText("RENDERER      : OPENGL", -33.0f, -2.0f, 0.1f, vmath::vec3(RedEnd, GreenEnd, BlueEnd));
		RenderText("LANGUAGE      : C", -33.0f, -10.0f, 0.1f, vmath::vec3(RedEnd, GreenEnd, BlueEnd));
	

		RedEnd += 0.00009f;
		if (GreenEnd <= 0.5f)
			GreenEnd += 0.000045f;
	}
	if (RedEnd >= 1.0f)
		EndCreditsToggle = false;

	if (!EndCreditsToggle)
	{

		RenderText("TECHNOLOGY : CUDA", -33.0f, 6.0f, 0.1f, vmath::vec3(RedEnd, GreenEnd, BlueEnd));
		RenderText("RENDERER      : OPENGL", -33.0f, -2.0f, 0.1f, vmath::vec3(RedEnd, GreenEnd, BlueEnd));
		RenderText("LANGUAGE      : C", -33.0f, -10.0f, 0.1f, vmath::vec3(RedEnd, GreenEnd, BlueEnd));


		if (RedEnd >= 0.0f)
			RedEnd -= 0.00009f;

		if (GreenEnd >= 0.0f)
			GreenEnd -= 0.000045f;

		if (RedEnd <= 0.0f)
		{
			if (RedTitleEnd <= 1.0f && SirEndCreditsToggle)
			{
				RenderText("IGNITED BY", -16.0f, 6.0f, 0.1f, vmath::vec3(RedTitleEnd, GreenTitleEnd, 0.0f));
				RenderText("MY GURU", -14.0f, -1.0f, 0.1f, vmath::vec3(RedTitleEnd, GreenTitleEnd, 0.0f));
				RenderText("DR.VIJAY GOKHALE SIR", -27.8f, -8.0f, 0.1f, vmath::vec3(RedTitleEnd, GreenTitleEnd, 0.0f));

				RedTitleEnd += 0.00009f;
				if (GreenTitleEnd <= 0.5f)
					GreenTitleEnd += 0.000045f;
			}

			if (RedTitleEnd >= 1.0f)
				SirEndCreditsToggle = false;

			if (!SirEndCreditsToggle)
			{
				RenderText("IGNITED BY", -16.0f, 6.0f, 0.1f, vmath::vec3(RedTitleEnd, GreenTitleEnd, 0.0f));
				RenderText("MY GURU", -14.0f, -1.0f, 0.1f, vmath::vec3(RedTitleEnd, GreenTitleEnd, 0.0f));
				RenderText("DR.VIJAY GOKHALE SIR", -27.8f, -8.0f, 0.1f, vmath::vec3(RedTitleEnd, GreenTitleEnd, 0.0f));

				if (RedTitleEnd >= 0.0f)
					RedTitleEnd -= 0.00009f;

				if (GreenTitleEnd >= 0.0f)
					GreenTitleEnd -= 0.000045f;

			}
			
		}
	}
}

void FontColorAnimationUpdates()
{
	void displayCudaAndCpu();


	if (Red <= 1.0f && PresentsToggle && !encreditsCall)
	{
		if (!mainCUDAContent)
		{
			RenderText("ASTROMEDICOMP'S", -23.0f, 6.0f, 0.1f, vmath::vec3(Red, Green, Blue));
			RenderText("BLEND", -18.0f, -2.0f, 0.1f, vmath::vec3(Red, Green, 0.0f));
			RenderText("GROUP", 1.5f, -2.0f, 0.1f, vmath::vec3(Red, Green, 0.0f));
			RenderText("PRESENTS", -14.0f, -10.0f, 0.1f, vmath::vec3(Red, Green, 0.0f));
		}

		//RenderText("PRESENTS", -14.0f, -10.0f, 0.1f, vmath::vec3(Red, Green, 0.0f));


		Red += 0.00009f;
		if (Green <= 0.5f)
			Green += 0.000045f;
	}
	if (Red >= 1.0f)
		PresentsToggle = false;

	if (!PresentsToggle && !encreditsCall)
	{

		if (!mainCUDAContent)
		{
			RenderText("ASTROMEDICOMP'S", -23.0f, 6.0f, 0.1f, vmath::vec3(Red, Green, Blue));
			RenderText("BLEND", -18.0f, -2.0f, 0.1f, vmath::vec3(Red, Green, 0.0f));
			RenderText("GROUP", 1.5f, -2.0f, 0.1f, vmath::vec3(Red, Green, 0.0f));
			RenderText("PRESENTS", -14.0f, -10.0f, 0.1f, vmath::vec3(Red, Green, 0.0f));
		}

		if (Red >= 0.0f)
			Red -= 0.00009f;

		if (Green >= 0.0f)
			Green -= 0.000045f;

		if (Red <= 0.0f)
		{
			if (RedTitle <= 1.0f && TitleToggle && !encreditsCall)
			{
				if (!mainCUDAContent)
				{
					RenderText("Virtual Terrain", -16.0f, 6.0f, 0.1f, vmath::vec3(RedTitle, GreenTitle, 0.0f));
					RenderText("using", -8.0f, -1.0f, 0.1f, vmath::vec3(RedTitle, GreenTitle, 0.0f));
					RenderText("3D Perlin Noise", -16.8f, -8.0f, 0.1f, vmath::vec3(RedTitle, GreenTitle, 0.0f));
				}
				RedTitle += 0.00009f;
				if (GreenTitle <= 0.5f)
					GreenTitle += 0.000045f;
			}

			if (RedTitle >= 1.0f)
				TitleToggle = false;

			if (!TitleToggle && !encreditsCall)
			{
				if (!mainCUDAContent)
				{
					RenderText("Virtual Terrain", -16.0f, 6.0f, 0.1f, vmath::vec3(RedTitle, GreenTitle, 0.0f));
					RenderText("using", -8.0f, -1.0f, 0.1f, vmath::vec3(RedTitle, GreenTitle, 0.0f));
					RenderText("3D Perlin Noise", -16.8f, -8.0f, 0.1f, vmath::vec3(RedTitle, GreenTitle, 0.0f));
				}
				if (RedTitle >= 0.0f)
					RedTitle -= 0.00009f;

				if (GreenTitle >= 0.0f)
					GreenTitle -= 0.000045f;

				if (RedTitle <= 0.0f && !encreditsCall)
				{
					mainCUDAContent = true;
					displayCudaAndCpu();
				}
			}
			
		}
	}
}

void launchCPUKernel(unsigned int MeshWidth, unsigned int MeshHeight, float Time)
{
	

	float xExtent = 10.0f;
	float yExtent = 10.0f;
	float xDelta = xExtent / (float)MeshWidth;
	float yDelta = yExtent / (float)MeshHeight;


	start[0] = xStart;
	start[1] = yStart;
	
	delta[0] = xDelta;
	delta[1] = yDelta;



	int iLL = 0;
	int idx = 255;
	float localw = 0;
	for (int i = 0; i < MeshWidth*MeshHeight; i++)
	{
		for (int j = 0; j < 4; j++)
		{

			
	//	fprintf(gpLogFile, "divx.x : %f\t  div.y  : %f\n  ", divx, divy);

			float u = (float)((i % MeshWidth)) / (float)MeshWidth;
			float v = (float)((i / MeshWidth)) / (float)MeshHeight;

			
			
			u = u * 2.0f - 1.0f;
			v = v * 2.0f - 1.0f;

			if (j == 0)
			{
				pos[i][0] = u;
				//Colors[i][0] = 1.0f;
			}
			if (j == 1)
			{

				float xCur = start[0] + ((float)(i % MeshWidth)) * delta[0];
				float yCur = start[1] + ((float)(i / MeshWidth)) * delta[1];
				float w = fBmCPU(xCur, yCur, octaves, lacunarity, gain) + zOffset;
				localw = w;
					//fprintf(gpLogFile, "divx.x : %f\t  div.y  : %f\n  ", xCur, yCur);

				w = (w > 0.0f) ? w : 0.0f; // don't show region underwater

				pos[i][1] = w;
			//	Colors[i][1] = 1.0f;
			}
			if (j == 2)
			{
				pos[i][2] = v;
				//Colors[i][2] = 0.0f;
			}
			if (j == 3)
			{
				pos[i][3] = 1.0f;
				//Colors[i][3] = 1.0f;
			}
			//fprintf(gpLogFile, "Value of %d\t %d\t : %f\n",i,j,pos[i][j]);
		
				#pragma region ColorCode

			if (localw < -1.000f)
			{
				if (j == 0)
					Colors[i][0] = 000;
				else if (j == 1)
					Colors[i][1] = 000;
				else if (j == 2)
					Colors[i][2] = 128;
				else if (j == 3)
					Colors[i][3] = 255;
			}
			else if (localw < -0.2500f)
			{
				if (j == 0)
					Colors[i][0] = 000;
				else if (j == 1)
					Colors[i][1] = 000;
				else if (j == 2)
					Colors[i][2] = 255;
				else if (j == 3)
					Colors[i][3] = 255;
			}
			else if (localw < 0.000f)
			{
				if (j == 0)
					Colors[i][0] = 000;
				else if (j == 1)
					Colors[i][1] = 128;
				else if (j == 2)
					Colors[i][2] = 255;
				else if (j == 3)
					Colors[i][3] = 255;
			}
			else if (localw < 0.0125f)
			{
				if (j == 0)
					Colors[i][0] = 240;
				else if (j == 1)
					Colors[i][1] = 240;
				else if (j == 2)
					Colors[i][2] = 064;
				else if (j == 3)
					Colors[i][3] = 255;
			}
			else if (localw < 0.1250f)
			{
				if (j == 0)
					Colors[i][0] = 032;
				else if (j == 1)
					Colors[i][1] = 260;
				else if (j == 2)
					Colors[i][2] = 000;
				else if (j == 3)
					Colors[i][3] = 255;
			}
			else if (localw < 0.3750f)
			{
				if (j == 0)
					Colors[i][0] = 224;
				else if (j == 1)
					Colors[i][1] = 224;
				else if (j == 2)
					Colors[i][2] = 000;
				else if (j == 3)
					Colors[i][3] = 255;
			}
			else if (localw < 0.7500f)
			{
				if (j == 0)
					Colors[i][0] = 128;
				else if (j == 1)
					Colors[i][1] = 128;
				else if (j == 2)
					Colors[i][2] = 128;
				else if (j == 3)
					Colors[i][3] = 255;
			}
			else
			{
				if (j == 0)
					Colors[i][0] = 255;
				else if (j == 1)
					Colors[i][1] = 255;
				else if (j == 2)
					Colors[i][2] = 255;
				else if (j == 3)
					Colors[i][3] = 255;
			}

				#pragma endregion


			
		}
		idx += 1;
		

	}
	


	ToggleCPU = false;
}

int permCPU(int i) { return(h_perm[i & 0xff]); }

float fadeCPU(float t) { return t * t * t * (t * (t * 6.f - 15.f) + 10.f); }

float lerpPCPU(float t, float a, float b) { return a + t * (b - a); }

float gradCPU(int hash, float x, float y, float z)
{
	//int h = hash & 15;                      // CONVERT LO 4 BITS OF HASH CODE
	//float u = h < 8 ? x : y,                 // INTO 12 GRADIENT DIRECTIONS.
	//	v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	//return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);

	//int h = hash & 15;                                    // Take the hashed value and take the first 4 bits of it (15 == 0b1111)
	//double u = h < 8 /* 0b1000 */ ? x : y;                // If the most significant bit (MSB) of the hash is 0 then set u = x.  Otherwise y.

	//double v;                                             // In Ken Perlin's original implementation this was another conditional operator (?:).  I
	//													  // expanded it for readability.

	//if (h < 4 /* 0b0100 */)                                // If the first and second significant bits are 0 set v = y
	//	v = y;
	//else if (h == 12 /* 0b1100 */ || h == 14 /* 0b1110*/)  // If the first and second significant bits are 1 set v = x
	//	v = x;
	//else                                                  // If the first and second significant bits are not equal (0/1, 1/0) set v = z
	//	v = z;

	//return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v); // Use the last 2 bits to decide if u and v are positive or negative.  Then return their addition.

//	goal of grad() is to calculate the dot product of a randomly selected gradient vector and the 8 location vectors.Ken Perlin used some fancy bit - flipping code to accomplish this:

	switch (hash & 0xF)
	{
	case 0x0: return  x + y;
	case 0x1: return -x + y;
	case 0x2: return  x - y;
	case 0x3: return -x - y;
	case 0x4: return  x + z;
	case 0x5: return -x + z;

	case 0x6: return  x - z;
	case 0x7: return -x - z;
	case 0x8: return  y + z;
	case 0x9: return -y + z;
	case 0xA: return  y - z;
	case 0xB: return -y - z;
	case 0xC: return  y + x;
	case 0xD: return -y + z;
	case 0xE: return  y - x;
	case 0xF: return -y - z;
	default: return 0; // never happens
	}
}



void uninitialize(void)
{
	cudaGraphicsUnregisterResource(graphicsResource);
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