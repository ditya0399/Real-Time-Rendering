#include<Windows.h>

#include<glew.h>           // This SHOULD be above GL.h header.
#include<gl/GL.h>
#include<stdio.h>
#include<string>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <vector>
#include <sstream>
#include <queue>
#include <map>
#include <set>


#include"HeightMap.h"
#include "FreeImage.h"


#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include"TexturedSmiley.h"
#include"ktx_loading_header.h"
#include"vmath.h"
#include"Model.h"
#include"Header.h"


#pragma comment(lib,"opengl32.lib")//for linking
#pragma comment(lib,"glew32.lib")//for linking
#pragma comment(lib,"assimp-vc141-mtd.lib")//to link the assimp library
#pragma comment(lib, "FreeImage.lib")

#define FOR(q,n) for(int q=0;q<n;q++)

#define noiseWidth 128
#define noiseHeight 128
#define SIZE 64
#define CheckImageWidth 64
#define CheckImageHeight 64

//int iBufferType;
int Win_Width = 800; 
int Win_Height = 600;
using namespace vmath;

float t = 0.0f;
float z = -10.0f;
Model *myModel = NULL;
Model *myModelFlowers = NULL;
Model *myModelLavender = NULL;

GLfloat angle1 = 0.0f;
GLuint gShaderProgramObject;

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0
};
enum
{
	MAX_SCENE_WIDTH = 2000,
	MAX_SCENE_HEIGHT = 2000,
	SPHERE_COUNT = 1,
};

//
GLuint vao;
GLuint vbo;
GLuint vboColor;
GLuint mvpUniform;
mat4 perspectiveProjectionMatrix;
GLuint samplerUniform;

//
GLuint gbLighting = 1;
//Lighting Variables
GLuint projectionUniform;
GLuint modelUniform;
GLuint ViewUniform;
GLuint LDUniform;
GLuint LAUniform;
GLuint LSUniform;
GLuint gTextureSamplerUniform;

GLuint KDUniform;
GLuint KAUniform;
GLuint KSUniform;
float MaterialShininessUniform;
GLuint LightPositionUniform;
GLuint LKeyPressed = 1;


float xView = 0.0f;
float yView = 35.0f;
float zView = 35.0f;

GLfloat lightAmbient[] = { 0.25f,0.25f,0.25f,1.0f };
GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightPosition[] = { 100.0f,100.0f,100.0f,1.0f };

GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialShininess = 2000.0f;


GLuint grassHeight_Uniform;
GLfloat grassHeight = 3.5f;

HDC ghdc = NULL;
HGLRC ghglrc = NULL;
HWND ghwnd;
HWND hwnd;
FILE *gpLogFile = NULL;
WINDOWPLACEMENT gwpprev = { sizeof(WINDOWPLACEMENT) };
DWORD gdwStyle = 0;
bool gbIsActiveWindow = false;
bool gbIsFullScreen = false;









/////////////// VARIABLES FOR TESSELLATED TERRAIN //////////////////

GLuint T_smileyTexture;
GLuint T_heightmap;
GLuint moduloUniform;
GLuint T_gShaderProgramObject;
GLuint T_samplerUniform;
GLuint T_dmap_depthUniform;
GLuint T_tex_color;
GLuint T_current_Time;
GLfloat T_gWidth;
GLfloat T_gHeight;
double current_Time;
GLuint T_hmBMP;
GLuint T_vao;
GLuint T_vbo, T_mvpUniform, T_projectionUniform, T_modelViewUniform;
mat4 T_perspectiveProjectionMatrix;

int modulo = 256;

///////////////// VARIABLES FOR GRASS///////////////////
GLuint G_gShaderProgramObject;
vector< vector< glm::vec3> > vVertexData;
//
GLuint uiTexture;
int iWidth, iHeight, iBPP;
string sPath;
bool bMipMapsGenerated;
GLuint uiSampler;
int tfMinification, tfMagnification;
GLuint fTimePassedUniform;
float time;


glm::vec3 vRenderScale;
int iNumGrassTriangles;
bool bLoaded;
int iRows;
int iCols;
int i;

GLuint uiBuffer;
GLuint uiGrassVAO;


GLuint G_viewUniform;
GLuint G_vEyeUniform;
GLuint G_modelUniform;
GLuint G_vColorUniform;
GLuint G_fAlphaTest;
GLuint G_fAlphaMultiplier;
GLuint G_NormalMatrixUniform;
GLuint G_projectionUniform;
GLuint G_gSampler;


float fAlphaTestVAR = 0.25f;
float fAlphaMultiplierVAR = 1.5f;


vector<BYTE> data1;
int iCurrentSize;

float rainSpeed = 0.288f;
float rainSpeed1 = 0.355f;
float rainSpeed2 = 0.198f;
float rainSpeed3 = 0.53f;
float rainSpeed4 = 0.24f;
float rainSpeed5 = 0.332f;

enum ETextureFiltering
{
	TEXTURE_FILTER_MAG_NEAREST = 0, // Nearest criterion for magnification
	TEXTURE_FILTER_MAG_BILINEAR, // Bilinear criterion for magnification
	TEXTURE_FILTER_MIN_NEAREST, // Nearest criterion for minification
	TEXTURE_FILTER_MIN_BILINEAR, // Bilinear criterion for minification
	TEXTURE_FILTER_MIN_NEAREST_MIPMAP, // Nearest criterion for minification, but on closest mipmap
	TEXTURE_FILTER_MIN_BILINEAR_MIPMAP, // Bilinear criterion for minification, but on closest mipmap
	TEXTURE_FILTER_MIN_TRILINEAR, // Bilinear criterion for minification on two closest mipmaps, then averaged
};


//////////VARIABLES FOR NOISE /////////////////////////

GLuint N_vbo_texture;
GLuint N_vbo_normals;
GLuint N_smileyTexture;
GLuint N_samplerUniform;
GLuint N_LightPos;
GLuint N_Scale;
GLuint N_modelViewUniform;
GLuint N_SkyColor;
GLuint N_CloudColor;

GLuint N_animUniform;
GLfloat N_anim = 0.0f;
GLubyte N_checkImage[CheckImageHeight][CheckImageWidth][4];
GLuint N_texName;

GLuint N_gShaderProgramObject;
int noise3DTexSize = 128;
GLuint noise3DTexName = 0;
GLubyte *noise3DTexPtr;

GLuint N_vao;
GLuint N_vbo;
GLuint N_mvpUniform;
GLuint N_mvpUniformColor;

int Noise = 0;

///////////////////////////// RAIN ////////////////////////////////////////
int rainExtra = -1;

GLuint R_gVao;
GLuint R_gVbo;
GLuint R_gVao2;
GLuint R_gVbo2;


GLuint R_gVaoColor;
GLuint R_gVboColor;
GLuint R_gVboColor2;

GLuint R_gMVPUniform;
GLuint R_gMVPUniform2;


mat4 R_modelviewMatrix = mat4::identity();
mat4 R_modelviewProjectionMatrix = mat4::identity();

mat4 R_modelMatrix = mat4::identity();
mat4 R_viewMatrix = mat4::identity();

GLuint R_modelUniform, R_viewUniform, R_projectionUniform;

GLfloat R_gWidth, R_gHeight;

GLfloat R_yAnimateSlanting = 30.0f;			//-0.5

GLfloat R_yAnimateSlanting2 =30.1f;			//-1.0

GLfloat R_yAnimateSlanting3 = 30.2f;			//-1.5

GLfloat R_yAnimateSlanting4 = 30.3f;			//-2.0

GLfloat R_yAnimateSlanting5 = 30.4f;			//-2.5

GLfloat R_yAnimateSlanting6 = 30.5f;			//-3.0

GLfloat R_yAnimateSlanting7 = 30.6f;			//-3.5


GLuint R_gbLighting = 0;
GLuint R_vboNormals;
GLuint R_LDUniform;
GLuint R_LAUniform;
GLuint R_LSUniform;

GLuint R_KDUniform;
GLuint R_KAUniform;
GLuint R_KSUniform;
float R_MaterialShininessUniform;
GLuint R_LightPositionUniform;
GLuint R_LKeyPressed = 0;

GLuint R_gShaderProgramObject;

GLfloat R_lightAmbient[] = { 0.25f,0.25f,0.25f,1.0f };
GLfloat R_lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat R_lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat R_lightPosition[] = { 100.0f,100.0f,100.0f,1.0f };

GLfloat R_materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat R_materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat R_materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat R_materialShininess = 50.0f;

//////////////////////////////////////////////////////////////////////////

/////////// Variables for Flowers //////////////
GLuint Flowers_gShaderProgramObject;
GLuint Flowers_projectionUniform;
GLuint Flowers_modelUniform;
GLuint Flowers_ViewUniform;
GLuint Flowers_LDUniform;
GLuint Flowers_LAUniform;
GLuint Flowers_LSUniform;
GLuint Flowers_gTextureSamplerUniform;

GLuint Flowers_KDUniform;
GLuint Flowers_KAUniform;
GLuint Flowers_KSUniform;
float Flowers_MaterialShininessUniform;
GLuint Flowers_LightPositionUniform;
GLuint Flowers_LKeyPressed = 1;





//////////////////////////// VARIABLES FOR LIGHTNING //////////////////////

int light = 0;

float       L_exposure = 5.0f;
int         L_mode = 0;
bool        L_paused = false;
float       L_bloom_factor = 1.0f;
bool        L_show_bloom = true;
bool        L_show_scene = true;
bool        L_show_prefilter = false;
float       L_bloom_thresh_min = 0.8f;
float       L_bloom_thresh_max = 1.2f;

struct
{
	struct
	{
		int bloom_thresh_min;
		int bloom_thresh_max;
	} L_scene;
	struct
	{
		int exposure;
		int bloom_factor;
		int scene_factor;
	} L_resolve;
} L_uniforms;

GLuint      L_ubo_transform;
GLuint      L_ubo_material;


GLuint     L_tex_src;
GLuint      L_tex_lut;

GLuint      L_render_fbo;
GLuint      L_filter_fbo[2];

GLuint      L_tex_scene;
GLuint      L_tex_brightpass;
GLuint      L_tex_depth;
GLuint      L_tex_filter[2];


GLuint L_b0Uniform;
GLuint L_b1Uniform;
GLuint L_lengthUniform;
GLuint L_mvpUniform;

GLuint L_gNumberOfSegmentUniform; //MultipleLines -> Stips  | One Line divided into Multiple parts ->Segments
GLuint L_gNumberOfStripsUniform;




unsigned int L_gNumberOfLineSegments;
int L_gNumVertices;
int L_gNumElements;

GLuint L_vao;
GLuint L_gShaderProgramObject;
GLuint L_FB_gShaderProgramObject;
GLuint L_gShaderProgramObject_filter;
GLuint L_gShaderProgramObject_resolve;
GLfloat L_angleTriangle = 0.0f;
GLfloat L_angleRectangle = 0.0f;


GLuint L_vao_Sphere;
GLuint L_vbo_position_Sphere;
GLuint L_vbo_Normals_Sphere;
GLuint L_vbo_Element_Sphere;


GLuint L_projectionUniform;
GLuint L_modelUniform;
GLuint L_ViewUniform;
GLuint L_LDUniform;
GLuint L_LAUniform;
GLuint L_LSUniform;


GLuint L_KDUniform;
GLuint L_KAUniform;
GLuint L_KSUniform;
float L_MaterialShininessUniform;
GLuint L_LightPositionUniform;
GLuint L_LKeyPressed = 0;


GLfloat L_lightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat L_lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat L_lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat L_lightPosition[] = { 100.0f,100.0f,100.0f,1.0f };

GLfloat L_materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat L_materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat L_materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat L_materialShininess = 50.0f;




///////////// CUBE VARIABLES ////////////
GLuint L_FB_vao_triangle;
GLuint L_FB_vbo_position_triangle;
GLuint L_FB_mvpUniform1;
GLuint L_FB_modelViewUniform1;
GLuint L_fbo;
GLuint L_color_texture;
GLuint L_depth_texture;
GLuint L_vbo_texture_rectangle;
GLuint L_samplerForLightning;
GLuint L_samplerFilter;
GLuint L_samplerResolve0;
GLuint L_samplerResolve1;














LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void ToggleFullScreen(void);
void update(void);
void uninitialize(void);
void displayTerrain();
void initializeTerrain();
void initializeNoise();

//
bool LoadHeightMapFromImageFree(string);
void AddData(void* ptrData, UINT uiDataSize);
void LoadAllTextures();
bool LoadTexture2D(string a_sPath, bool bGenerateMipMaps);
void SetFiltering(int a_tfMagnification, int a_tfMinification);
void SetWrap();
void SetAnisotropyLevel(float fAnisotropyLevel);
void CreateFromData(BYTE* bData, int a_iWidth, int a_iHeight, int a_iBPP, GLenum format, bool bGenerateMipMaps);
float GetHeightFromRealVector(glm::vec3 vRealPosition);
void initializeGrass();
void initializeRain();
void displayRain();
void initializeFlowers();
void initializeLightning();
void initializeLightningFrameBuffer();
void displayLightning();
void displayLightningV2();
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
		TEXT("MultiColoredTriangle"),
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
			//displayLightning();
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
		case 'L':
		case 'l':

			if (gbLighting == 0)
			{

				fprintf(gpLogFile, "The bool value is %x\n", gbLighting);
				gbLighting = 1;
				R_gbLighting = 1;
				//  LKeyPressed = 3;
			}
			else
			{

				gbLighting = 0;
				R_gbLighting = 0;
			}
			break;
		case 'W':
		case 'w':

			zView -= 2.0f;
			break;
		case 'D':
		case 'd':
			xView += 2.0f;
			break;
		case 'A':
		case 'a':
			xView -=2.0f;
			break;
		case 'S':
		case 's':
			zView += 2.0f;
			break;
		case VK_SPACE:
			modulo = modulo - 1;
			fprintf(gpLogFile, "The value of modulo is %d\n", modulo);
			break;
		case 'c':
		case 'C':
			modulo = modulo + 1;
			fprintf(gpLogFile, "The value of modulo is %d\n", modulo);
			break;
		case 'G':
		case 'g':
			grassHeight += 0.1f;
			break;
		case 'H':
		case 'h':
			grassHeight -= 0.1f;
			break;
		case 'N':
		case 'n':
			if (Noise == 1)
				Noise = 0;
			else
				Noise = 1;
			break;
		case 'R':
		case 'r':

			 rainSpeed += 0.188f;
			 rainSpeed1 += 0.255f;
			 rainSpeed2 += 0.098f;
			 rainSpeed3 += 0.23f;
			 rainSpeed4 += 0.14f;
			 rainSpeed5 += 0.232f;
			break;
			
		case 'i':
		case 'I':
			
			if (rainExtra == 0)
				rainExtra = 1;
			else
				rainExtra = 0;
			break;
		case 'o':
		case 'O':
			rainExtra = 0;
			break;
		case 'z':
		case 'Z':
			L_exposure *= 1.1f;
			break;
		case 'B':
		case'b':
			if (light == 0)
				light = 1;
			else
				light = 0;
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
	GLuint gGeometryShaderObject;

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
//
//
	//Define Vertex Shader Object
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar **p;
	const GLchar *vertexShaderSourceCode =

		"#version 430 core" \
		"\n"
		"layout(location = 0)in vec4 vPosition;" \
		"in vec4 vColor;" \

		"out vec4 out_Color;" \
		"layout(location = 1)in vec3 vNormal;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform int u_LKeyPressed;" \
	
	
		"uniform vec4 u_LightPosition;" \
		"layout(location = 2)in vec2 vTextureCoord;" \
		
		"uniform float t;" \
		"out VertexData{" \
		"vec2 texCoord;" \
		"vec3 tNorm;" \
		"vec3 out_Normal;" \
		"vec3 light_direction;" \
		"vec3 viewVector; " \
		"}vs_out;" \

		"void main(void)" \
		"{" \
		"if(u_LKeyPressed == 1)" \
		"{" \
		"vec4 eye_coordinates = u_viewMatrix * u_modelMatrix *  vPosition;" \
		"vs_out.tNorm = mat3(u_viewMatrix * u_modelMatrix) * vNormal;" \
		"vs_out.light_direction = vec3(u_LightPosition - eye_coordinates);" \
		"vs_out.viewVector = vec3(-eye_coordinates.xyz);" \
		"}" \
		"gl_Position =   u_modelMatrix * vPosition;" \
		"if(gl_Position.y > 0.6)" \
		"gl_Position.x += (0.15 *sin(1.4 * t) * gl_Position.y);" \
		"gl_Position =   u_viewMatrix  * gl_Position;" \
		" vs_out.out_Normal = vNormal;" \
		"vs_out.texCoord = vTextureCoord;"	\
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
				fprintf(gpLogFile, "VS : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}


	/////////////////////////////////// G E O M E T R Y ----- S H A D E R /////////////////////////

	gGeometryShaderObject = glCreateShader(GL_GEOMETRY_SHADER);

	// code for the shader
	const GLchar *geometryShaderSource =
		"#version 430" \
		"\n" \

		"layout(triangles,invocations =32)in;" \
		"layout(triangle_strip,max_vertices = 3)out;" \
		"uniform mat4 u_projectionMatrix;" \

		"in VertexData{" \
		"vec2 texCoord;" \
		"vec3 tNorm;" \
		"vec3 out_Normal;" \
		"vec3 light_direction;" \
		"vec3 viewVector; " \
		"}gs_in[];" \
		
		"out vertex_data{" \
		"vec2 out_texCoord;" \
		"vec3 tNorm;" \
		"vec3 out_Normal;" \
		"vec3 light_direction;" \
		"vec3 viewVector; " \
		"}vertexOut;" \

		"out vec3 fcolor;" \

		"float rand(vec2 co)" \
		"{" \
		"return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);" \
		"}" \

		"float randX = 70 *rand(vec2(gl_InvocationID * 4.3,0.0)) -45.0;" \
		"float randY =  -1.0 * rand(vec2(gl_InvocationID,0.0));" \
		"float randZ =  100 *  rand(vec2(gl_InvocationID * 19.3,0.0)) - 50.0;" \

		"void main(void)" \
		"{" \
		

		"vertexOut.out_texCoord = gs_in[0].texCoord;" \
		"vertexOut.out_Normal = gs_in[0].out_Normal;" \
		"vertexOut.tNorm = gs_in[0].tNorm;" \
		"vertexOut.light_direction = gs_in[0].light_direction;" \
		"vertexOut.viewVector = gs_in[0].viewVector;" \
		"gl_Position = u_projectionMatrix * (gl_in[0].gl_Position + vec4(randX * 5.0,randY,randZ,0.0));" \
		"EmitVertex();" \
		
		"vertexOut.out_Normal = gs_in[1].out_Normal;" \
		"vertexOut.out_texCoord = gs_in[1].texCoord;" \
		"vertexOut.tNorm = gs_in[1].tNorm;" \
		"vertexOut.light_direction = gs_in[1].light_direction;" \
		"vertexOut.viewVector = gs_in[1].viewVector;" \
		"gl_Position = u_projectionMatrix * (gl_in[1].gl_Position +  vec4(randX * 5.0,randY,randZ,0.0));" \
		"EmitVertex();" \
	
		"vertexOut.out_Normal = gs_in[2].out_Normal;" \
		"vertexOut.out_texCoord = gs_in[2].texCoord;" \
		"vertexOut.tNorm = gs_in[2].tNorm;" \
		"vertexOut.light_direction = gs_in[2].light_direction;" \
		"vertexOut.viewVector = gs_in[2].viewVector;" \
		"gl_Position = u_projectionMatrix * (gl_in[2].gl_Position +  vec4(randX * 5.0,randY,randZ,0.0));" \
		"EmitVertex();" \

		"EndPrimitive();" \


		"}";


	glShaderSource(gGeometryShaderObject, 1, (const GLchar **)&geometryShaderSource, NULL);

	//Compile the vertex shader 
	glCompileShader(gGeometryShaderObject);

	//////////////// Error Checking//////////////////
	//Code for catching the errors 
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;


	glGetShaderiv(gGeometryShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gGeometryShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gGeometryShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpLogFile, "GS : %s\n", szInfoLog);
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
	
		"in vec4 out_Color;" \
		"in vec2 out_texture_coord;" \



		"in vertex_data{" \
		"vec2 out_texCoord;" \
		"vec3 tNorm;" \
		"vec3 out_Normal;" \
		"vec3 light_direction;" \
		"vec3 viewVector; " \
		"}fsIn;" \

		"uniform sampler2D u_texture_sampler;" \
		"vec3 phongADSLight;" \
		"uniform vec3 u_LDUniform;" \
		"uniform vec3 u_LAUniform;" \
		"uniform vec3 u_LSUniform;" \
		"uniform vec3 u_KDUniform;" \
		"uniform vec3 u_KAUniform;" \
		"uniform vec3 u_KSUniform;" \
		"uniform float u_MaterialShininess;" \
		"out vec4 FragColor;" \
		"uniform int u_LKeyPressed;" \
		"void main(void)" \
		"{" \

		"if(u_LKeyPressed == 1)" \
		"{" \
		"vec4 texColor= texture(u_texture_sampler,fsIn.out_texCoord);" \

		"vec3 normalizedTNorm = normalize(fsIn.tNorm);" \
		"vec3 normalizedLightDirection = normalize(fsIn.light_direction);" \
		"vec3 normalizedViewVector = normalize(fsIn.viewVector);" \
		"float tNormal_dot_lightDirection = max(dot(normalizedLightDirection,normalizedTNorm),0.0);" \
		"vec3 reflectionVector = reflect(-normalizedLightDirection,normalizedTNorm);" \

		"vec3 ambient = u_LAUniform * u_KAUniform * texColor.rgb ;" \
		"vec3 diffuse = u_LDUniform * u_KDUniform * tNormal_dot_lightDirection * texColor.rgb;" \
		"vec3 specular = u_LSUniform * u_KSUniform * pow(max(dot(reflectionVector,normalizedViewVector),0.0),u_MaterialShininess) * texColor.rgb;" \
		"phongADSLight = (ambient + diffuse + specular);" \

		"FragColor = vec4(phongADSLight,1.0);" \
	
		"}" \
		"else" \
		"{" \
		"FragColor =  texture(u_texture_sampler,fsIn.out_texCoord);" \
		"}" \
		
		
		"}";

	//FragColor = vec4(1,1,1,1) = White Color
	//this means here we are giving color to the Triangle.
	//"FragColor = vec4(( vec3(tex) * vec3(out_Color) * phongADSLight) , 1.0);" \
	//"vec4 tex = texture(u_texture_sampler, out_texture_coord);" \


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
	glAttachShader(gShaderProgramObject, gGeometryShaderObject);
	//attach fragment shader to the gShaderProgramObject
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);


	//Pre-Linking  binding to vertexAttributes
	//glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPosition");
	//glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_COLOR, "vColor");
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
	modelUniform = glGetUniformLocation(gShaderProgramObject, "u_modelMatrix");
	ViewUniform = glGetUniformLocation(gShaderProgramObject, "u_viewMatrix");
	projectionUniform = glGetUniformLocation(gShaderProgramObject, "u_projectionMatrix");

	LKeyPressed = glGetUniformLocation(gShaderProgramObject, "u_LKeyPressed");
	LDUniform = glGetUniformLocation(gShaderProgramObject, "u_LDUniform");
	LAUniform = glGetUniformLocation(gShaderProgramObject, "u_LAUniform");
	LSUniform = glGetUniformLocation(gShaderProgramObject, "u_LSUniform");

	KDUniform = glGetUniformLocation(gShaderProgramObject, "u_KDUniform");
	KAUniform = glGetUniformLocation(gShaderProgramObject, "u_KAUniform");
	KSUniform = glGetUniformLocation(gShaderProgramObject, "u_KSUniform");
	gTextureSamplerUniform = glGetUniformLocation(gShaderProgramObject, "u_texture_sampler");
	LightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_LightPosition");
	MaterialShininessUniform = glGetUniformLocation(gShaderProgramObject, "u_MaterialShininess");
	//Here we have done all the preparations of data transfer from CPU to GPU



//	myModel = new Model((char *)"nanosuit/firtree1.3ds");
	myModel = new Model((char *)"nanosuit/treeOak/Tree oak N231017.3ds");
	//myModel = new Model((char *)"nanosuit/nanosuit.obj");
	//myModel = new Model((char *)"nanosuit/tree4/Tree Chestnut N260213.3DS");


	//firtree 1.3ds

	//const GLfloat triangleVertices[] =
	//{
	//	0.0f,1.0f,0.0f,
	//	-1.0f,-1.0f,0.0f,
	//	1.0f,-1.0f,0.0f
	//};
	//const GLfloat triangleColors[] =
	//{							
	//	1.0f, 0.0f, 0.0f,		
	//	0.0f, 1.0f, 0.0f,		
	//	0.0f, 0.0f, 1.0f,		
	//};
	////Basically we are giving the vertices coordinates in the above array

	////Create vao - Vertex array objects
	//glGenVertexArrays(1, &vao);
	//glBindVertexArray(vao);

	//////// VERTEX BUFFER ////////

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

	//////// COLOR BUFFER //////////
	//glGenBuffers(1,&vboColor);
	//glBindBuffer(GL_ARRAY_BUFFER,vboColor);
	//glBufferData(GL_ARRAY_BUFFER,sizeof(triangleColors),triangleColors,GL_STATIC_DRAW);
	//glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0,NULL);
	//glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//
	////UNBINDING VAO
	//glBindVertexArray(0);
	
	
	initializeGrass();
	initializeTerrain();
	initializeNoise();
	initializeRain();
	initializeFlowers();
	initializeLightning();
//



	//Depth Lines
	glClearDepth(1.0f);
	////glClearColor(0.85f, 0.95f, 1.0f, 1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	////glClearColor(0.4f, 0.5f, 0.6f, 1.0f); //Rain Sky


	////glClearColor(0.4f, 0.5f, 0.5f, 1.0f);
	////glClearColor(0.0f, 0.0f, 0.2f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//initializing Perspective ProjectionMatrix to identity.
	perspectiveProjectionMatrix = mat4::identity();

	resize(Win_Width, Win_Height);

	return(0);
}
void initializeTerrain()
{
	BOOL loadTexture(GLuint *, TCHAR[]);
	GLuint gVertexShaderObject;
	GLuint gFragmentShaderObject;
	GLuint gTesselationControlShaderObject;
	GLuint gTesselationEvaluationShaderObject;



	//Define Vertex Shader Object
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	//const GLchar **p;
	const GLchar *vertexShaderSourceCode =

		"#version 430 core" \
		"\n"
		"out VS_OUT" \
		"{" \
		"vec2 tc;" \
		"}vs_out;" \
		"uniform int modulo;" \
		"void main(void)" \
		"{" \
		"const vec4 vertices[] = vec4[](vec4(-0.5,0.0,-0.5,1.0),vec4(0.5,0.0,-0.5,1.0),vec4(-0.5,0.0,0.5,1.0),vec4(0.5,0.0,0.5,1.0));" \
		"int x = gl_InstanceID & 255;" \
		"int y = gl_InstanceID >> 8;" \
		"vec2 offs = vec2(x,y);" \
		"vs_out.tc = ((vertices[gl_VertexID].xz + offs + vec2(0.5)) / 256.0);" \
		"gl_Position = vertices[gl_VertexID] + vec4(float(x - 104),0.0,float(y - 104),0.0);" \
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
				fprintf(gpLogFile, "  VERTEX SHADER :   %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}

	////////////////////// TESSELLATION CONTROL SHADER //////////////////////////
	//This shader will create patch of 4 vertices
	gTesselationControlShaderObject = glCreateShader(GL_TESS_CONTROL_SHADER);
	const GLchar *tessellationControlShaderSourceCode =
		"#version 430" \
		"\n" \
		"layout(vertices=4)out;" \


		"in VS_OUT" \
		"{" \
		"vec2 tc;" \
		"} tcs_in[];" \


		"out TCS_OUT" \
		"{" \
		"vec2 tc;" \
		"} tcs_out[];" \


		"uniform mat4 u_mvp_matrix;" \
		"void main(void)" \
		"{" \
		"if(gl_InvocationID == 0)" \
		"{" \
		"vec4 p0 =  u_mvp_matrix * gl_in[0].gl_Position;" \
		"vec4 p1 =  u_mvp_matrix * gl_in[1].gl_Position;" \
		"vec4 p2 =  u_mvp_matrix * gl_in[2].gl_Position;" \
		"vec4 p3 =  u_mvp_matrix * gl_in[3].gl_Position;" \
		"p0 /=p0.w;" \
		"p1 /=p1.w;" \
		"p2 /=p2.w;" \
		"p3 /=p3.w;" \

		"if(p0.z<=0.0||" \
		"p1.z<=0.0||" \
		"p2.z<=0.0||" \
		"p3.z<=0.0)" \
		"{" \
		"gl_TessLevelOuter[0] = 0.0;" \
		"gl_TessLevelOuter[1] = 0.0;" \
		"gl_TessLevelOuter[2] = 0.0;" \
		"gl_TessLevelOuter[3] = 0.0;" \
		"}" \
		"else" \
		"{" \
		"float l0 =length(p2.xy - p0.xy) * 100.0 + 1.0;" \
		"float l1 =length(p3.xy - p2.xy) * 100.0 + 1.0;" \
		"float l2 =length(p3.xy - p1.xy) * 100.0 + 1.0;" \
		"float l3 =length(p1.xy - p0.xy) * 100.0 + 1.0;" \
		"gl_TessLevelOuter[0] = l0;" \
		"gl_TessLevelOuter[1] = l1;" \
		"gl_TessLevelOuter[2] = l2;" \
		"gl_TessLevelOuter[3] = l3;" \
		"gl_TessLevelInner[0] = min(l1,l3);" \
		"gl_TessLevelInner[1] = min(l0,l2);" \

		"}" \
		"}" \

		"gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;" \
		"tcs_out[gl_InvocationID].tc = tcs_in[gl_InvocationID].tc;" \
		"}";



	glShaderSource(gTesselationControlShaderObject, 1, (const GLchar **)&tessellationControlShaderSourceCode, NULL);

	//compileShader
	glCompileShader(gTesselationControlShaderObject);

	//////////////// Error Checking//////////////////
	//Code for catching the errors 
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;


	glGetShaderiv(gTesselationControlShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gTesselationControlShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written9;
				glGetShaderInfoLog(gTesselationControlShaderObject, iInfoLogLength, &written9, szInfoLog);
				fprintf(gpLogFile, "TCS :  %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}

	////////////////// TESSELLATION EVLAUTATION SHADER /////////////

	gTesselationEvaluationShaderObject = glCreateShader(GL_TESS_EVALUATION_SHADER);

	const GLchar *tessellationEvalutationShaderSourceCode =
		"#version 430" \
		"\n" \
		"layout(quads,fractional_odd_spacing)in;" \

		"uniform mat4 u_mvp_matrix;" \
		"uniform mat4 u_p_matrix;" \
		"uniform mat4 u_modelView_matrix;" \
		"uniform sampler2D tex_displacement;" \
		"uniform float dmap_depth;" \

		"in TCS_OUT" \
		"{" \
		"vec2 tc;" \
		"}tes_in[];" \


		"out TES_OUT" \
		"{" \
		"vec2 tc;" \
		"vec3 world_coord;" \
		"vec3 eye_coord;" \
		"}tes_out;" \

		"void main(void)" \
		"{" \
		"vec2 tc1 = mix(tes_in[0].tc,tes_in[1].tc,gl_TessCoord.x);" \
		"vec2 tc2 = mix(tes_in[2].tc,tes_in[3].tc,gl_TessCoord.x);" \
		"vec2 tc = mix(tc2,tc1,gl_TessCoord.y);" \

		"vec4 p1 = mix(gl_in[0].gl_Position,gl_in[1].gl_Position,gl_TessCoord.x);" \
		"vec4 p2 = mix(gl_in[2].gl_Position,gl_in[3].gl_Position,gl_TessCoord.x);" \
		"vec4 p = mix(p2,p1,gl_TessCoord.y);" \
		"p.y += texture(tex_displacement,tc).r * dmap_depth;" \

		"vec4 P_eye = u_modelView_matrix * p;" \

		"tes_out.tc = tc;" \
		"tes_out.world_coord = p.xyz;" \
		"tes_out.eye_coord = P_eye.xyz;" \
		"gl_Position = u_p_matrix * P_eye;" \

		"}";

	glShaderSource(gTesselationEvaluationShaderObject, 1, (const GLchar **)&tessellationEvalutationShaderSourceCode, NULL);

	//compileShader
	glCompileShader(gTesselationEvaluationShaderObject);

	//////////////// Error Checking//////////////////
	//Code for catching the errors 
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;


	glGetShaderiv(gTesselationEvaluationShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gTesselationEvaluationShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written6;
				glGetShaderInfoLog(gTesselationEvaluationShaderObject, iInfoLogLength, &written6, szInfoLog);
				fprintf(gpLogFile, "TES :%s\n", szInfoLog);
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
		"layout(binding = 1) uniform sampler2D tex_color;" \
		"uniform bool enable_fog = true;" \
		"uniform vec4 fog_color = vec4(0.7,0.8,0.9,0.0);" \
		"in TES_OUT" \
		"{" \
		"vec2 tc;" \
		"vec3 world_coord;" \
		"vec3 eye_coord;" \
		"}fs_in;" \
		"out vec4 FragColor;" \

		"vec4 fog(vec4 c)" \
		"{" \
		"float z = length(fs_in.eye_coord);" \
		"float de = 0.015 * smoothstep(0.0,6.0,10.0 - fs_in.world_coord.y);" \
		"float di = 0.025 * (smoothstep(0.0,40.0,20.0 - fs_in.world_coord.y));" \
		"float extinction = exp(-z * de);" \
		"float inscattering = exp(-z * di);" \
		"return c * extinction + fog_color * (1.0 - inscattering); " \
		"}" \

		"void main(void)" \
		"{" \
		"vec4 landscape = texture(tex_color,fs_in.tc * 15.0);" \
		"landscape.rgb = landscape.rgb - 0.30;" \

		"if(enable_fog)" \
		"{" \

		"FragColor = landscape;" \

		"}" \
		"else" \
		"{" \
		"FragColor =  landscape;" \

		"}" \
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
				fprintf(gpLogFile, "FRAGMENT :%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}

	T_gShaderProgramObject = glCreateProgram();
	//attach vertex shader to the gShaderProgramObject
	glAttachShader(T_gShaderProgramObject, gVertexShaderObject);
	glAttachShader(T_gShaderProgramObject, gTesselationControlShaderObject);
	glAttachShader(T_gShaderProgramObject, gTesselationEvaluationShaderObject);
	glAttachShader(T_gShaderProgramObject, gFragmentShaderObject);


	//Pre-Linking  binding to vertexAttributes
	glBindAttribLocation(T_gShaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPosition");
	//glBindAttribLocation(gShaderProgramObject, AMC_, "vPosition");
	//Here the above line means that we are linking the GPU's variable vPosition with the CPU's  enum member  i.e AMC_ATTRIBUTE_POSITION .
	//So whatever changes will be done in AMC_ATTRIBUTE_POSITION , those will also reflect in vPosition

	//RULE : ALWAYS BIND THE ATTRIBUTES BEFORE LINKING AND BIND THE UNIFORM AFTER LINKING.

	//Link the shader program 
	glLinkProgram(T_gShaderProgramObject);

	//Code for catching the errors 
	GLint iProgramLinkStatus = 0;



	glGetProgramiv(T_gShaderProgramObject, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(T_gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written3;
				glGetProgramInfoLog(T_gShaderProgramObject, iInfoLogLength, &written3, szInfoLog);
				fprintf(gpLogFile, "%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}

	T_mvpUniform = glGetUniformLocation(T_gShaderProgramObject, "u_mvp_matrix");
	T_projectionUniform = glGetUniformLocation(T_gShaderProgramObject, "u_p_matrix");
	T_modelViewUniform = glGetUniformLocation(T_gShaderProgramObject, "u_modelView_matrix");
	T_dmap_depthUniform = glGetUniformLocation(T_gShaderProgramObject, "dmap_depth");
	T_samplerUniform = glGetUniformLocation(T_gShaderProgramObject, "tex_displacement");
	T_tex_color = glGetUniformLocation(T_gShaderProgramObject, "tex_color");
	moduloUniform = glGetUniformLocation(T_gShaderProgramObject, "modulo");
	//Here we have done all the preparations of data transfer from CPU to GPU

	const GLfloat Vertices[] =
	{
		-1.0f,-1.0f,-0.5f,1.0f,0.5f,-1.0f,1.0f,1.0f
	};
	//Basically we are giving the vertices coordinates in the above array

	//Create vao - Vertex array objects
	glGenVertexArrays(1, &T_vao);
	glBindVertexArray(T_vao);
	glGenBuffers(1, &T_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, T_vbo);
	//in the above statement we have accesed the GL-ARRAY_BUFFER using vbo. Without it wouldn't be possible to get GL_ARRAY_BUFFER
	//For the sake of understanding . GL_ARRAY_BUFFER is in the GPU side ad=nd we have bind our CPU side vbo with it like a Pipe to get the access.
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	//The above statement states that , we are passing our vertices array to the GPU and GL_STATIC_DRAW means draw it now only. Don't draw it in Runtime. 
	//The below statement states that after storing the data in the GPU'S buffer . We are passing it to the vPosition now . 

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	//GL_FALSE = We are not giving normalized coordinates as our coordinates are not converted in 0 - 1 range.
	//3 = This is the thing I was talking about in initialize. Here, we are telling GPU to break our array in 3 parts . 
	//0 and Null are for the Interleaved. 
	//GL_FLOAT- What is the type? .
	//AMC_ATTRIBUTE_POSITION. here we are passing data to vPosition. 

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//Depth Lines
	glClearDepth(1.0f);
	glClearColor(0.0f, 0.25f, 0.0f, 0.0f);
	glLineWidth(3.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE0);
	//loadTexture(&T_samplerUniform, MAKEINTRESOURCE(IDBITMAP_HEIGHTMAP));
	loadTexture(&T_hmBMP, MAKEINTRESOURCE(IDBITMAP_HEIGHTMAP));
//	T_samplerUniform = load("terragen1.ktx", 0);
	//loadTexture(&heightMap, MAKEINTRESOURCE(IDBITMAP_HEIGHTMAP));

	glActiveTexture(GL_TEXTURE1);
	//T_smileyTexture = load("terragen_color.ktx", 0);
	loadTexture(&T_smileyTexture, MAKEINTRESOURCE(IDBITMAP_SMILEY));
	//initializing Perspective ProjectionMatrix to identity.
//	perspectiveProjectionMatrix = mat4::identity();
	//gNumberOfLineSegments = 1;

}
void initializeGrass()
{

	GLuint gVertexShaderObject;
	GLuint gFragmentShaderObject;
	GLuint gGeometryShaderObject;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	LoadAllTextures();
	//////////////////////////////// V E R T E X - S H A D E R //////////////////////////


	//Define Vertex Shader Object
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar **p;
	const GLchar *vertexShaderSourceCode =

		"#version 430 core" \
		"\n"
		"layout(location = 0) in vec3 inPosition;" \
		"void main(void)" \
		"{" \
		"gl_Position = vec4(inPosition,1.0);" \

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


	////////// GEOMETRY SHADER ////////////////
	gGeometryShaderObject = glCreateShader(GL_GEOMETRY_SHADER);

	const GLchar *geometryShaderSourceCode =         //Source code of Vertex shader
		"#version 430 core" \
		"\n" \
		"layout(points) in;" \
		"layout(triangle_strip) out;" \
		"layout(max_vertices = 12) out;" \

		"uniform struct Matrices" \
		"{" \
		"mat4 projMatrix;" \
		"mat4 modelMatrix;" \
		"mat4 viewMatrix;" \
		"mat4 normalMatrix;" \
		"} matrices;" \

		"smooth out vec2 vTexCoord;" \
		"smooth out vec3 vWorldPos;" \
		"smooth out vec4 vEyeSpacePos;" \

		"uniform float fTimePassed;" \
		"uniform float grassHeight;" \
		"mat4 rotationMatrix(vec3 axis, float angle)" \
		"{" \
		"axis = normalize(axis);" \
		"float s = sin(angle);" \
		"float c = cos(angle);" \
		"float oc = 1.0 - c;" \

		"return mat4(oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s, 0.0," \
		"oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s, 0.0," \
		"oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c, 0.0," \
		"0.0, 0.0, 0.0, 1.0);" \
		"}" \

		"vec3 vLocalSeed;" \


		"float randZeroOne()" \
		"{" \
		"uint n = floatBitsToUint(vLocalSeed.y * 214013.0 + vLocalSeed.x * 2531011.0 + vLocalSeed.z * 141251.0);" \
		"n = n * (n * n * 15731u + 789221u);" \
		"n = (n >> 9u) | 0x3F800000u;" \

		"	float fRes = 2.0 - uintBitsToFloat(n);" \
		"vLocalSeed = vec3(vLocalSeed.x + 147158.0 * fRes, vLocalSeed.y*fRes + 415161.0 * fRes, vLocalSeed.z + 324154.0*fRes);" \
		"return fRes;" \
		"}" \

		"int randomInt(int min, int max)" \
		"{" \
		"float fRandomFloat = randZeroOne();" \
		"return int(float(min) + fRandomFloat * float(max - min));" \
		"}" \

		"void main()" \
		"{" \
		"mat4 mMV = matrices.viewMatrix*matrices.modelMatrix;" \
		"mat4 mMVP = matrices.projMatrix*matrices.viewMatrix*matrices.modelMatrix;" \

		"vec3 vGrassFieldPos = gl_in[0].gl_Position.xyz;" \

		"	float PIover180 = 3.1415 / 180.0;" \
		"vec3 vBaseDir[] =" \
		"{" \
		"vec3(1.0, 0.0, 0.0)," \
		"vec3(float(cos(45.0*PIover180)), 0.0f, float(sin(45.0*PIover180)))," \
		"vec3(float(cos(-45.0*PIover180)), 0.0f, float(sin(-45.0*PIover180)))" \
		"};" \

		"float fGrassPatchSize = 3.0;" \
		"float fWindStrength = 8.0;" \

		"vec3 vWindDirection = vec3(1.0, 0.0, 1.0);" \
		"vWindDirection = normalize(vWindDirection);" \

		"for (int i = 0; i < 3; i++)" \
		"{" \


		"vec3 vBaseDirRotated = (rotationMatrix(vec3(0, 1, 0), sin(fTimePassed*0.7f)*0.1f)*vec4(vBaseDir[i], 1.0)).xyz;" \

		"vLocalSeed = vGrassFieldPos * float(i);" \
		"int iGrassPatch = randomInt(0, 3);" \

		"float fGrassPatchHeight = grassHeight + randZeroOne()*2.0;" \

		"float fTCStartX = float(iGrassPatch)*0.25f;" \
		"float fTCEndX = fTCStartX + 0.25f;" \

		"float fWindPower = 0.5f + sin(vGrassFieldPos.x / 30 + vGrassFieldPos.z / 30 + fTimePassed * (1.2f + fWindStrength / 20.0f));" \
		"if (fWindPower < 0.0f)" \
		"fWindPower = fWindPower * 1.2f;" \
		"else fWindPower = fWindPower * 0.3f;" \

		"fWindPower *= fWindStrength;" \

		"vec3 vTL = vGrassFieldPos - vBaseDirRotated * fGrassPatchSize*0.5f + vWindDirection * fWindPower;" \
		"vTL.y += fGrassPatchHeight;" \
		"gl_Position = mMVP * vec4(vTL, 1.0);" \
		"vTexCoord = vec2(fTCStartX, 1.0);" \
		"vWorldPos = vTL;" \
		"vEyeSpacePos = mMV * vec4(vTL, 1.0);" \
		"EmitVertex();" \


		"vec3 vBL = vGrassFieldPos - vBaseDir[i] * fGrassPatchSize*0.5f;" \
		"gl_Position = mMVP * vec4(vBL, 1.0);" \
		"vTexCoord = vec2(fTCStartX, 0.0);" \
		"vWorldPos = vBL;" \
		"vEyeSpacePos = mMV * vec4(vBL, 1.0);" \
		"EmitVertex();" \


		"vec3 vTR = vGrassFieldPos + vBaseDirRotated * fGrassPatchSize*0.5f + vWindDirection * fWindPower;" \
		"vTR.y += fGrassPatchHeight;" \
		"gl_Position = mMVP * vec4(vTR, 1.0);" \
		"vTexCoord = vec2(fTCEndX, 1.0);" \
		"vWorldPos = vTR;" \
		"vEyeSpacePos = mMV * vec4(vTR, 1.0);" \
		"EmitVertex();" \


		"vec3 vBR = vGrassFieldPos + vBaseDir[i] * fGrassPatchSize*0.5f;" \
		"gl_Position = mMVP * vec4(vBR, 1.0);" \
		"vTexCoord = vec2(fTCEndX, 0.0);" \
		"vWorldPos = vBR;" \
		"vEyeSpacePos = mMV * vec4(vBR, 1.0);" \
		"EmitVertex();" \

		"EndPrimitive();" \
		"}" \

		"}";

	glShaderSource(gGeometryShaderObject, 1, (const GLchar**)&geometryShaderSourceCode, NULL); //NULL is for NULL terminated source code string

	//compile geometry shader
	glCompileShader(gGeometryShaderObject);
	//Shader compilation error checking goes here...
	iInfoLogLength = 0;
	iShaderCompileStatus = 0;
	szInfoLog = NULL;
	glGetShaderiv(gGeometryShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE) {
		glGetShaderiv(gGeometryShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0) {
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL) {
				GLsizei written;
				glGetShaderInfoLog(gGeometryShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpLogFile, "GS%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
	/////////////////    F R A G M E N T S H A D E R            //////////////////////////
	//Define Vertex Shader Object
	//vec4(0.2, 1.0, 0.6, 1.0); 
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar *fragmentShaderSourceCode =
		"#version 430 core" \
		"\n" \
		"	smooth in vec2 vTexCoord;" \


		"	smooth in vec3 vWorldPos;" \
		"smooth in vec4 vEyeSpacePos;" \

		"out vec4 outputColor;" \

		"uniform sampler2D gSampler;" \
		"uniform vec4 vColor = vec4(0.3, 1.0, 0.85,1.0);" \

		"uniform vec3 vEyePosition = vec3(0.0,9.0,85.0);" \



		"uniform float fAlphaTest = 0.05;" \
		"uniform float fAlphaMultiplier = 0.1;" \


		"void main(void)" \
		"{" \

		"vec4 vTexColor = texture2D(gSampler, vTexCoord);" \


		"float fNewAlpha = vTexColor.a*fAlphaMultiplier;" \
		"if (fNewAlpha < fAlphaTest)" \
		"discard;" \

		"if (fNewAlpha > 1.0f)" \
		"fNewAlpha = 0.5f;" \

		"vec4 vMixedColor = vTexColor * vColor;" \


		"outputColor = vec4(vMixedColor.zyx, fNewAlpha);" \

		"if(outputColor.r > 0.3)" \
		"discard;" \


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
	G_gShaderProgramObject = glCreateProgram();
	//attach vertex shader to the gShaderProgramObject
	glAttachShader(G_gShaderProgramObject, gVertexShaderObject);
	glAttachShader(G_gShaderProgramObject, gGeometryShaderObject);
	//attach fragment shader to the gShaderProgramObject
	glAttachShader(G_gShaderProgramObject, gFragmentShaderObject);


	//Pre-Linking  binding to vertexAttributes
	//glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPosition");
	//Here the above line means that we are linking the GPU's variable vPosition with the CPU's  enum member  i.e AMC_ATTRIBUTE_POSITION .
	//So whatever changes will be done in AMC_ATTRIBUTE_POSITION , those will also reflect in vPosition

	//RULE : ALWAYS BIND THE ATTRIBUTES BEFORE LINKING AND BIND THE UNIFORM AFTER LINKING.

	//Link the shader program 
	glLinkProgram(G_gShaderProgramObject);

	//Code for catching the errors 
	GLint iProgramLinkStatus = 0;



	glGetProgramiv(G_gShaderProgramObject, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(G_gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written3;
				glGetProgramInfoLog(G_gShaderProgramObject, iInfoLogLength, &written3, szInfoLog);
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
	G_modelUniform = glGetUniformLocation(G_gShaderProgramObject, "matrices.modelMatrix");
	G_viewUniform = glGetUniformLocation(G_gShaderProgramObject, "matrices.viewMatrix");
	G_vEyeUniform = glGetUniformLocation(G_gShaderProgramObject, "vEyePosition");
	G_projectionUniform = glGetUniformLocation(G_gShaderProgramObject, "matrices.projMatrix");
	G_NormalMatrixUniform = glGetUniformLocation(G_gShaderProgramObject, "matrices.normalMatrix");
	G_vColorUniform = glGetUniformLocation(G_gShaderProgramObject, "vColor");
	fTimePassedUniform = glGetUniformLocation(G_gShaderProgramObject, "fTimePassed");
	G_fAlphaTest = glGetUniformLocation(G_gShaderProgramObject, "fAlphaTest");
	G_fAlphaMultiplier = glGetUniformLocation(G_gShaderProgramObject, "fAlphaMultiplier");
	G_gSampler = glGetUniformLocation(G_gShaderProgramObject, "gSampler");
	grassHeight_Uniform = glGetUniformLocation(G_gShaderProgramObject, "grassHeight");

	//Here we have done all the preparations of data transfer from CPU to GPU

	const GLfloat triangleVertices[] =
	{
		0.0f,1.0f,0.0f,
		-1.0f,-1.0f,0.0f,
		1.0f,-1.0f,0.0f
	};
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0f);

	glActiveTexture(GL_TEXTURE5);
	LoadHeightMapFromImageFree("heightmap.bmp");
}
void initializeNoise()
{


	GLuint gVertexShaderObject;
	GLuint gFragmentShaderObject;


	//////////////////////////////// V E R T E X - S H A D E R //////////////////////////


	//Define Vertex Shader Object
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar **p;
	const GLchar *vertexShaderSourceCode =

		"#version 430 core" \
		"\n"
		"in vec3 vTexCoord;" \
		"out vec3 out_TexCoord;" \
		"in vec4 vPosition;" \

		"uniform mat4 u_mvp_matrix;" \
		"uniform mat4 u_mv_Matrix;" \
		"uniform vec3 LightPos;"  \
		"uniform float Scale;" \
		"in vec3 vNormal;" \
		"out vec3 MCPosition;" \
		"out float LightIntensity;" \
		"out vec3 MCposition;" \
		"void main(void)" \
		"{" \
		"vec3 ECposition = vec3(u_mv_Matrix * vPosition);" \
		"mat3 NormalMatrix = mat3(u_mv_Matrix);" \
		"MCposition = vec3(vPosition) * Scale;" \
		"vec3 tnorm = normalize(vec3(NormalMatrix * vNormal));" \
		"LightIntensity = dot(normalize(LightPos - ECposition),tnorm);" \
		"LightIntensity *= 1.5;" \
		"gl_Position = u_mvp_matrix * vPosition;" \
		"out_TexCoord =  vTexCoord;" \
		"MCposition = vPosition.xyz;" \
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
				fprintf(gpLogFile, " VERTEX SHADER :      %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}
	/////////////////    F R A G M E N T S H A D E R            //////////////////////////
	//Define Fragment Shader Object
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar *fragmentShaderSourceCode =
		"#version 430 core" \
		"\n" \

		"in vec3 out_TexCoord;" \
		"uniform vec4 SkyColor;" \
		"uniform vec4 CloudColor;" \
		"in float LightIntensity;" \
		"in vec3 MCposition;" \
		"out vec4 FragColor;" \
		"uniform sampler3D u_sampler;" \
		"uniform float anim;" \
		"void main(void)" \
		"{" \
		"vec4 noisevec = texture(u_sampler,MCposition + vec3(anim,0.0,anim));" \
		"float intensity = (noisevec[0] + noisevec[1] + noisevec[2] + noisevec[3] + 0.03125) * 1.5;" \
		"vec4 color = mix(SkyColor,CloudColor,intensity) * LightIntensity;" \

		"FragColor = color;" \
		"}";
	//texture(u_sampler,out_TexCoord)
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
				fprintf(gpLogFile, "FRAGMENT SHADER :       %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}
	// CREATE SHADER PROGRAM OBJECT
	N_gShaderProgramObject = glCreateProgram();
	//attach vertex shader to the gShaderProgramObject
	glAttachShader(N_gShaderProgramObject, gVertexShaderObject);

	//attach fragment shader to the gShaderProgramObject
	glAttachShader(N_gShaderProgramObject, gFragmentShaderObject);


	//Pre-Linking  binding to vertexAttributes
	glBindAttribLocation(N_gShaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(N_gShaderProgramObject, AMC_ATTRIBUTE_TEXCOORD0, "vTexCoord");
	glBindAttribLocation(N_gShaderProgramObject, AMC_ATTRIBUTE_NORMAL, "vNormal");
	//Here the above line means that we are linking the GPU's variable vPosition with the CPU's  enum member  i.e AMC_ATTRIBUTE_POSITION .
	//So whatever changes will be done in AMC_ATTRIBUTE_POSITION , those will also reflect in vPosition

	//RULE : ALWAYS BIND THE ATTRIBUTES BEFORE LINKING AND BIND THE UNIFORM AFTER LINKING.

	//Link the shader program 
	glLinkProgram(N_gShaderProgramObject);

	//Code for catching the errors 
	GLint iProgramLinkStatus = 0;



	glGetProgramiv(N_gShaderProgramObject, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(N_gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written3;
				glGetProgramInfoLog(N_gShaderProgramObject, iInfoLogLength, &written3, szInfoLog);
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
	/*mvpUniform = glGetUniformLocation(gShaderProgramObjectColor, "u_mvp_matrix");
	samplerUniform = glGetUniformLocation(gShaderProgramObject, "u_sampler");
	modelViewUniform = glGetUniformLocation(gShaderProgramObject, "u_mv_Matrix");
	SkyColor = glGetUniformLocation(gShaderProgramObject, "SkyColor");
	CloudColor = glGetUniformLocation(gShaderProgramObject, "CloudColor");
	Scale = glGetUniformLocation(gShaderProgramObject, "Scale");
	LightPos = glGetUniformLocation(gShaderProgramObject, "LightPos");
	animUniform = glGetUniformLocation(gShaderProgramObject, "anim");

*/

	N_mvpUniform = glGetUniformLocation(N_gShaderProgramObject, "u_mvp_matrix");
	N_samplerUniform = glGetUniformLocation(N_gShaderProgramObject, "u_sampler");
	N_modelViewUniform = glGetUniformLocation(N_gShaderProgramObject, "u_mv_Matrix");
	N_SkyColor = glGetUniformLocation(N_gShaderProgramObject, "SkyColor");
	N_CloudColor = glGetUniformLocation(N_gShaderProgramObject, "CloudColor");
	N_Scale = glGetUniformLocation(N_gShaderProgramObject, "Scale");
	N_LightPos = glGetUniformLocation(N_gShaderProgramObject, "LightPos");
	N_animUniform = glGetUniformLocation(N_gShaderProgramObject, "anim");





	//Here we have done all the preparations of data transfer from CPU to GPU

	const GLfloat CheckerboardVertices[] =
	{
		// Straight checkerboard
		-2.0f, -1.0f, 0.0f,		// left-bottom
		-2.0f, 1.0f, 0.0f,		// left-top
		0.0f, 1.0f, 0.0f,		// right-top
		0.0f, -1.0f, 0.0f,	// right-bottom
		// checkerboard at 45 degrees
			1.0f, -1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			2.41421f, 1.0f, -1.41421f,
			2.41421f, -1.0f, -1.41421f
	};

	const GLfloat checkerboardTexCoords[] =
	{										// Checkerboard texture
		0.0f, 0.0f,1.0f,
		0.0f, 1.0f,1.0f,
		1.0f, 1.0f,1.0f,
		1.0f, 0.0f,1.0f,
		//Rotated
		0.0f, 0.0f,1.0f,
		0.0f, 1.0f,1.0f,
		1.0f, 1.0f,1.0f,
		1.0f, 0.0f,1.0f
	};
	const GLfloat normals[] =
	{
		0.0f, 0.0f, 1.0f,
0.0f, 0.0f, 1.0f,
0.0f, 0.0f, 1.0f,
0.0f, 0.0f, 1.0f
	};

	//Basically we are giving the vertices coordinates in the above array

	//Create vao - Vertex array objects
	glGenVertexArrays(1, &N_vao);
	glBindVertexArray(N_vao);
	glGenBuffers(1, &N_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, N_vbo);
	//in the above statement we have accesed the GL-ARRAY_BUFFER using vbo. Without it wouldn't be possible to get GL_ARRAY_BUFFER
	//For the sake of understanding . GL_ARRAY_BUFFER is in the GPU side ad=nd we have bind our CPU side vbo with it like a Pipe to get the access.
	//glBufferData(GL_ARRAY_BUFFER, sizeof(CheckerboardVertices), CheckerboardVertices, GL_STATIC_DRAW);
	//The above statement states that , we are passing our vertices array to the GPU and GL_STATIC_DRAW means draw it now only. Don't draw it in Runtime. 
	//The below statement states that after storing the data in the GPU'S buffer . We are passing it to the vPosition now . 
	//Dynamic Draw of Checkerboard(Vertices)
	glBufferData(GL_ARRAY_BUFFER, 4 * 12 * sizeof(GL_FLOAT), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//GL_FALSE = We are not giving normalized coordinates as our coordinates are not converted in 0 - 1 range.
	//3 = This is the thing I was talking about in initialize. Here, we are telling GPU to break our array in 3 parts . 
	//0 and Null are for the Interleaved. 
	//GL_FLOAT- What is the type? .
	//AMC_ATTRIBUTE_POSITION. here we are passing data to vPosition.
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	///////////////////// T E X T U R E's VBO  /////////////////////////
	glGenBuffers(1, &N_vbo_texture);
	glBindBuffer(GL_ARRAY_BUFFER, N_vbo_texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(checkerboardTexCoords), checkerboardTexCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	/////////////// N O R M A L S /////////////////////////////////
	glGenBuffers(1, &N_vbo_normals);
	glBindBuffer(GL_ARRAY_BUFFER, N_vbo_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//unbind VAO
	glBindVertexArray(0);

	//Texture calls
	glEnable(GL_TEXTURE_3D);
	glActiveTexture(GL_TEXTURE7);
	make3DNoiseTexture();
	init3DNoiseTexture(Noise3DTexSize, Noise3DTexPtr);

}
void initializeRain()
{
	GLuint gVertexShaderObject;
	GLuint gGeometryShaderObject;

	GLuint gFragmentShaderObject;

	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to shader
	const GLchar *vertexShaderSourceCode =
		"#version 430 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec4 vColor;"\
		"out vec4 color;"\
		"in vec3 vNormal;" \
		"in vec2 vTextureCoord;" \


		"uniform int u_LKeyPressed;" \
		"uniform vec4 u_LightPosition;" \

		"out VertexData{" \
		"vec2 texCoord;" \
		"vec3 tNorm;" \
		"vec3 out_Normal;" \
		"vec3 light_direction;" \
		"vec3 viewVector; " \
		"}vs_out;" \



		"uniform mat4 u_projectionMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_modelMatrix;" \

		"uniform mat4 u_mvp_matrix;"\
		"void main(void)" \
		"{" \
		"if(u_LKeyPressed == 1)" \
		"{" \
		"vec4 eye_coordinates = u_viewMatrix * u_modelMatrix *  vPosition;" \
		"vs_out.tNorm = mat3(u_viewMatrix * u_modelMatrix) * vNormal;" \
		"vs_out.light_direction = vec3(u_LightPosition - eye_coordinates);" \
		"vs_out.viewVector = vec3(-eye_coordinates.xyz);" \
		"}" \
		"gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vPosition;" \
		"color = vColor;"\
		" vs_out.out_Normal = vNormal;" \
		"vs_out.texCoord = vTextureCoord;"	\
		"}";

	glShaderSource(gVertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);

	//compile shader
	glCompileShader(gVertexShaderObject);
	GLint iInfoLength = 0;
	GLint iShaderCompiledStatus = 0;
	char *szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfoLength, &written, szInfoLog);
				fprintf(gpLogFile, "Vertex Shader Compilation Log:%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}


	//*****************GEOMETRY SHADER************************
	gGeometryShaderObject = glCreateShader(GL_GEOMETRY_SHADER);

	//provide code to shader 
	const GLchar *geometryShaderSource =
		"#version 410" \
		"\n" \
		"layout(points,invocations = 32)in;"\
		"layout(line_strip,max_vertices = 4)out;" \
		"uniform mat4 u_mvp_matrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_modelMatrix;" \





		"in VertexData{" \
		"vec2 texCoord;" \
		"vec3 tNorm;" \
		"vec3 out_Normal;" \
		"vec3 light_direction;" \
		"vec3 viewVector; " \
		"}gs_in[];" \

		"out vertex_data{" \
		"vec2 out_texCoord;" \
		"vec3 tNorm;" \
		"vec3 out_Normal;" \
		"vec3 light_direction;" \
		"vec3 viewVector; " \
		"}vertexOut;" \



		"float rand(vec2 co)"\
		"{"\
		"	return fract(cos(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);"\
		"}"\
		"float randx = 60 *rand(vec2(gl_InvocationID * 1000,0.0)) - 55.0;" \
		"float randy =  5.0 * rand(vec2(gl_InvocationID,0.0));" \
		"float randz =  100 *  rand(vec2(gl_InvocationID * 119.3,0.0)) - 50.0;" \
		"in vec4 color[];"\
		"out vec4 fColor;"\


		"void main(void)" \
		"{"\


		
		"fColor = color[0];"\

		



		"vertexOut.out_texCoord = gs_in[0].texCoord;" \
		"vertexOut.out_Normal = gs_in[0].out_Normal;" \
		"vertexOut.tNorm = gs_in[0].tNorm;" \
		"vertexOut.light_direction = gs_in[0].light_direction;" \
		"vertexOut.viewVector = gs_in[0].viewVector;" \
		"gl_Position =  ((gl_in[0].gl_Position )+vec4(0.0, 15.0, -50.0, 1.0)+vec4( randx,randy,randz,0.0) );"\
		"fColor.a=0.19;"\
		"EmitVertex();"\

		"vertexOut.out_texCoord = gs_in[0].texCoord;" \
		"vertexOut.out_Normal = gs_in[0].out_Normal;" \
		"vertexOut.tNorm = gs_in[0].tNorm;"  \
		"vertexOut.light_direction = gs_in[0].light_direction;" \
		"vertexOut.viewVector = gs_in[0].viewVector;" \
		"gl_Position = ( (gl_in[0].gl_Position )+vec4(0.0, 16.0, -50.0, 1.0)+vec4(randx,randy,randz,0.0));"\
		"fColor.a = 0.007;" \
		"EmitVertex();" \











		"EndPrimitive();" \
		"}";

	glShaderSource(gGeometryShaderObject, 1, (const GLchar**)&geometryShaderSource, NULL);
	glCompileShader(gGeometryShaderObject);
	iInfoLength = 0;
	iShaderCompiledStatus = 0;
	szInfoLog = NULL;
	glGetShaderiv(gGeometryShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gGeometryShaderObject, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gGeometryShaderObject, iInfoLength, &written, szInfoLog);
				fprintf(gpLogFile, "Geometry Shader Compilation Log:%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}



	//////**** FRAGMENT SHADER ****
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar * fragmentShaderSourceCode =
		"#version 430" \
		"\n" \

		"in vec2 out_texture_coord;" \

		"in vertex_data{" \
		"vec2 out_texCoord;" \
		"vec3 tNorm;" \
		"vec3 out_Normal;" \
		"vec3 light_direction;" \
		"vec3 viewVector; " \
		"}fsIn;" \

		"vec3 phongADSLight;" \
		"uniform vec3 u_LDUniform;" \
		"uniform vec3 u_LAUniform;" \
		"uniform vec3 u_LSUniform;" \
		"uniform vec3 u_KDUniform;" \
		"uniform vec3 u_KAUniform;" \
		"uniform vec3 u_KSUniform;" \
		"uniform float u_MaterialShininess;" \

		"uniform int u_LKeyPressed;" \
		"uniform sampler2D u_texture_sampler;" \
		"out vec4 FragColor;" \
		"in vec4 fColor;" \
		"void main(void)" \
		"{" \
		"if(u_LKeyPressed == 1)" \
		"{" \

		"vec4 texColor= texture(u_texture_sampler,fsIn.out_texCoord);" \
		"vec3 normalizedTNorm = normalize(fsIn.tNorm);" \
		"vec3 normalizedLightDirection = normalize(fsIn.light_direction);" \
		"vec3 normalizedViewVector = normalize(fsIn.viewVector);" \
		"float tNormal_dot_lightDirection = max(dot(normalizedLightDirection,normalizedTNorm),0.0);" \
		"vec3 reflectionVector = reflect(-normalizedLightDirection,normalizedTNorm);" \

		"vec3 ambient = u_LAUniform * u_KAUniform * fColor.rgb;" \
		"vec3 diffuse = u_LDUniform * u_KDUniform * tNormal_dot_lightDirection * fColor.rgb;" \
		"vec3 specular = u_LSUniform * u_KSUniform * pow(max(dot(reflectionVector,normalizedViewVector),0.0),u_MaterialShininess) * fColor.rgb;" \
		"phongADSLight = (ambient + diffuse + specular);" \

		"FragColor = vec4(phongADSLight,fColor.a);" \

		"}" \
		"else" \
		"{" \
		"FragColor = vec4(fColor);" \
		"}" \
		"}";

	glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

	//compile shader
	glCompileShader(gFragmentShaderObject);
	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iShaderCompiledStatus);
		if (iInfoLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, iInfoLength, &written, szInfoLog);
				fprintf(gpLogFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}




	//********** SHADER PROGRAM *********
	//create
	R_gShaderProgramObject = glCreateProgram();

	// attach vertex shader to shader program
	glAttachShader(R_gShaderProgramObject, gVertexShaderObject);

	// attach tessellation control shader to shader program
	glAttachShader(R_gShaderProgramObject, gGeometryShaderObject);

	// attach fragment shader to shader program
	glAttachShader(R_gShaderProgramObject, gFragmentShaderObject);

	// pre-link binding of shader program object with vertex shader position attribute
	glBindAttribLocation(R_gShaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPosition");

	glBindAttribLocation(R_gShaderProgramObject, AMC_ATTRIBUTE_COLOR, "vColor");
	glBindAttribLocation(R_gShaderProgramObject, AMC_ATTRIBUTE_NORMAL, "vNormal");
	glBindAttribLocation(R_gShaderProgramObject, AMC_ATTRIBUTE_TEXCOORD0, "vTextureCoord");

	// link shader
	glLinkProgram(R_gShaderProgramObject);
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(R_gShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(R_gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(R_gShaderProgramObject, iInfoLength, &written, szInfoLog);
				fprintf(gpLogFile, "Shader Program Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}


	//get MVP uniform locaion
	R_gMVPUniform = glGetUniformLocation(R_gShaderProgramObject, "u_mvp_matrix");
	R_gMVPUniform2 = glGetUniformLocation(R_gShaderProgramObject, "u_mvp_matrix2");
	R_modelUniform = glGetUniformLocation(R_gShaderProgramObject, "u_modelMatrix");
	R_projectionUniform = glGetUniformLocation(R_gShaderProgramObject, "u_projectionMatrix");
	R_viewUniform = glGetUniformLocation(R_gShaderProgramObject, "u_viewMatrix");

	R_LKeyPressed = glGetUniformLocation(R_gShaderProgramObject, "u_LKeyPressed");
	R_LDUniform = glGetUniformLocation(R_gShaderProgramObject, "u_LDUniform");
	R_LAUniform = glGetUniformLocation(R_gShaderProgramObject, "u_LAUniform");
	R_LSUniform = glGetUniformLocation(R_gShaderProgramObject, "u_LSUniform");
	//R_gTextureSamplerUniform = glGetUniformLocation(gShaderProgramObject, "u_texture_sampler");



	R_KDUniform = glGetUniformLocation(R_gShaderProgramObject, "u_KDUniform");
	R_KAUniform = glGetUniformLocation(R_gShaderProgramObject, "u_KAUniform");
	R_KSUniform = glGetUniformLocation(R_gShaderProgramObject, "u_KSUniform");
	R_LightPositionUniform = glGetUniformLocation(R_gShaderProgramObject, "u_LightPosition");
	R_MaterialShininessUniform = glGetUniformLocation(R_gShaderProgramObject, "u_MaterialShininess");
	////**** vertices, colors, shader attribs, vbo, vao initializations ****//

	const GLfloat points[] =
	{

25.0f, -6.0f, 1.0f,
-10.0f, -6.0f, 1.0f,
55.0f, -10.0f, 1.0f,
-1.0f, -10.0f, 1.0f,

	};

	const GLfloat normal[] =
	{
		0.0f, 0.0f, 1.0f,
0.0f, 0.0f, 1.0f,
0.0f, 0.0f, 1.0f,
0.0f, 0.0f, 1.0f,
	};


	const GLfloat color[] =
	{
		1.0f, 1.0f, 1.0f,0.0f,
1.0f, 1.0f, 1.0f,0.0f,
1.0f, 1.0f, 1.0f,0.0f,
1.0f, 1.0f, 1.0f,0.0f
	};


	const GLfloat texture[] =
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

	};



	glGenVertexArrays(1, &R_gVao2);
	glBindVertexArray(R_gVao2);

	glGenBuffers(1, &R_gVbo2);
	glBindBuffer(GL_ARRAY_BUFFER, R_gVbo2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);			//fininshing/completion of above gVbo buffer state


	/////////////////////////

	glGenBuffers(1, &R_vboNormals);
	glBindBuffer(GL_ARRAY_BUFFER, R_vboNormals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normal), normal, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);			//fininshing/completion of above gVbo buffer state

	///////////////////////
	/*glGenBuffers(1, &R_vboTexture);
	glBindBuffer(GL_ARRAY_BUFFER, R_vboTexture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texture), texture, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);		*/	//fininshing/completion of above gVbo buffer state


	///////////////////////
	glGenBuffers(1, &R_gVboColor);
	glBindBuffer(GL_ARRAY_BUFFER, R_gVboColor);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);			//fininshing/completion of above gVbo buffer state


	glBindVertexArray(0);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}
void initializeFlowers()
{
	GLuint gVertexShaderObject;
	GLuint gFragmentShaderObject;
		GLuint gGeometryShaderObject;
	//////////////////////////////// V E R T E X - S H A D E R //////////////////////////


	//Define Vertex Shader Object
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar **p;
	const GLchar *vertexShaderSourceCode =


		"#version 430 core" \
		"\n"
		"layout(location = 0)in vec4 vPosition;" \
		"in vec4 vColor;" \

		"out vec4 out_Color;" \
		"layout(location = 1)in vec3 vNormal;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform int u_LKeyPressed;" \

		"uniform float t;" \

		"uniform vec4 u_LightPosition;" \
		"layout(location = 2)in vec2 vTextureCoord;" \


		"out VertexData{" \
		"vec2 texCoord;" \
		"vec3 tNorm;" \
		"vec3 out_Normal;" \
		"vec3 light_direction;" \
		"vec3 viewVector; " \
		"}vs_out;" \

		"void main(void)" \
		"{" \
		"if(u_LKeyPressed == 1)" \
		"{" \
		"vec4 eye_coordinates = u_viewMatrix * u_modelMatrix *  vPosition;" \
		"vs_out.tNorm = mat3(u_viewMatrix * u_modelMatrix) * vNormal;" \
		"vs_out.light_direction = vec3(u_LightPosition - eye_coordinates);" \
		"vs_out.viewVector = vec3(-eye_coordinates.xyz);" \
		"}" \
		"gl_Position = u_modelMatrix * vPosition;" \
		"gl_Position.x += (0.15 *sin(1.4 * t) * gl_Position.y);" \
		"gl_Position =   u_viewMatrix  * gl_Position;" \
		" vs_out.out_Normal = vNormal;" \
		"vs_out.texCoord = vTextureCoord;"	\
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

	/////////////////////////////////// G E O M E T R Y ----- S H A D E R /////////////////////////

	gGeometryShaderObject = glCreateShader(GL_GEOMETRY_SHADER);

	// code for the shader
	const GLchar *geometryShaderSource =
		"#version 430" \
		"\n" \

		"layout(triangles,invocations = 32)in;" \
		"layout(triangle_strip,max_vertices = 3)out;" \
		"uniform mat4 u_projectionMatrix;" \

		"in VertexData{" \
		"vec2 texCoord;" \
		"vec3 tNorm;" \
		"vec3 out_Normal;" \
		"vec3 light_direction;" \
		"vec3 viewVector; " \
		"}gs_in[];" \

		"out vertex_data{" \
		"vec2 out_texCoord;" \
		"vec3 tNorm;" \
		"vec3 out_Normal;" \
		"vec3 light_direction;" \
		"vec3 viewVector; " \
		"}vertexOut;" \

		"out vec3 fcolor;" \

		"float rand(vec2 co)" \
		"{" \
		"return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);" \
		"}" \

		"float randX = 50 *rand(vec2(gl_InvocationID * 4.3,0.0)) -23.0;" \
		"float randY =  1.0 * rand(vec2(gl_InvocationID,0.0));" \
		"float randZ =  100 *  rand(vec2(gl_InvocationID * 19.3,0.0)) - 50.0;" \

		"void main(void)" \
		"{" \


		"vertexOut.out_texCoord = gs_in[0].texCoord;" \
		"vertexOut.out_Normal = gs_in[0].out_Normal;" \
		"vertexOut.tNorm = gs_in[0].tNorm;" \
		"vertexOut.light_direction = gs_in[0].light_direction;" \
		"vertexOut.viewVector = gs_in[0].viewVector;" \
		"gl_Position = u_projectionMatrix * (gl_in[0].gl_Position + vec4(randX * 5.0,randY,randZ,0.0));" \
		"EmitVertex();" \

		"vertexOut.out_Normal = gs_in[1].out_Normal;" \
		"vertexOut.out_texCoord = gs_in[1].texCoord;" \
		"vertexOut.tNorm = gs_in[1].tNorm;" \
		"vertexOut.light_direction = gs_in[1].light_direction;" \
		"vertexOut.viewVector = gs_in[1].viewVector;" \
		"gl_Position = u_projectionMatrix * (gl_in[1].gl_Position +  vec4(randX * 5.0,randY,randZ,0.0));" \
		"EmitVertex();" \

		"vertexOut.out_Normal = gs_in[2].out_Normal;" \
		"vertexOut.out_texCoord = gs_in[2].texCoord;" \
		"vertexOut.tNorm = gs_in[1].tNorm;" \
		"vertexOut.light_direction = gs_in[1].light_direction;" \
		"vertexOut.viewVector = gs_in[1].viewVector;" \
		"gl_Position = u_projectionMatrix * (gl_in[2].gl_Position +  vec4(randX * 5.0,randY,randZ,0.0));" \
		"EmitVertex();" \

		"EndPrimitive();" \


		"}";


	glShaderSource(gGeometryShaderObject, 1, (const GLchar **)&geometryShaderSource, NULL);

	//Compile the vertex shader 
	glCompileShader(gGeometryShaderObject);

	//////////////// Error Checking//////////////////
	//Code for catching the errors 
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;


	glGetShaderiv(gGeometryShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gGeometryShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gGeometryShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpLogFile, "GS : %s\n", szInfoLog);
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

		"in vec4 out_Color;" \
		"in vec2 out_texture_coord;" \



		"in vertex_data{" \
		"vec2 out_texCoord;" \
		"vec3 tNorm;" \
		"vec3 out_Normal;" \
		"vec3 light_direction;" \
		"vec3 viewVector; " \
		"}fsIn;" \

		"uniform sampler2D u_texture_sampler;" \
		"vec3 phongADSLight;" \
		"uniform vec3 u_LDUniform;" \
		"uniform vec3 u_LAUniform;" \
		"uniform vec3 u_LSUniform;" \
		"uniform vec3 u_KDUniform;" \
		"uniform vec3 u_KAUniform;" \
		"uniform vec3 u_KSUniform;" \
		"uniform float u_MaterialShininess;" \
		"out vec4 FragColor;" \
		"uniform int u_LKeyPressed;" \
		"void main(void)" \
		"{" \

		"if(u_LKeyPressed == 1)" \
		"{" \
		"vec4 texColor= texture(u_texture_sampler,fsIn.out_texCoord);" \

		"vec3 normalizedTNorm = normalize(fsIn.tNorm);" \
		"vec3 normalizedLightDirection = normalize(fsIn.light_direction);" \
		"vec3 normalizedViewVector = normalize(fsIn.viewVector);" \
		"float tNormal_dot_lightDirection = max(dot(normalizedLightDirection,normalizedTNorm),0.0);" \
		"vec3 reflectionVector = reflect(-normalizedLightDirection,normalizedTNorm);" \

		"vec3 ambient = u_LAUniform * u_KAUniform ;" \
		"vec3 diffuse = u_LDUniform * u_KDUniform * tNormal_dot_lightDirection ;" \
		"vec3 specular = u_LSUniform * u_KSUniform * pow(max(dot(reflectionVector,normalizedViewVector),0.0),u_MaterialShininess) ;" \
		"phongADSLight = (ambient + diffuse + specular);" \

		"FragColor = vec4(phongADSLight,1.0);" \

		"}" \
		"else" \
		"{" \
		"FragColor =  vec4(1.0);" \
		"}" \


		"}";
	//"FragColor = vec4(( vec3(tex) * vec3(out_Color) * phongADSLight) , 1.0);" \
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
	Flowers_gShaderProgramObject = glCreateProgram();
	//attach vertex shader to the gShaderProgramObject
	glAttachShader(Flowers_gShaderProgramObject, gVertexShaderObject);
	glAttachShader(Flowers_gShaderProgramObject, gGeometryShaderObject);

	//attach fragment shader to the gShaderProgramObject
	glAttachShader(Flowers_gShaderProgramObject, gFragmentShaderObject);


	//Pre-Linking  binding to vertexAttributes
	//glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPosition");
	//glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_COLOR, "vColor");
	//Here the above line means that we are linking the GPU's variable vPosition with the CPU's  enum member  i.e AMC_ATTRIBUTE_POSITION .
	//So whatever changes will be done in AMC_ATTRIBUTE_POSITION , those will also reflect in vPosition

	//RULE : ALWAYS BIND THE ATTRIBUTES BEFORE LINKING AND BIND THE UNIFORM AFTER LINKING.

	//Link the shader program 
	glLinkProgram(Flowers_gShaderProgramObject);

	//Code for catching the errors 
	GLint iProgramLinkStatus = 0;



	glGetProgramiv(Flowers_gShaderProgramObject, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(Flowers_gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written3;
				glGetProgramInfoLog(Flowers_gShaderProgramObject, iInfoLogLength, &written3, szInfoLog);
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
	Flowers_modelUniform = glGetUniformLocation(Flowers_gShaderProgramObject, "u_modelMatrix");
	Flowers_ViewUniform = glGetUniformLocation(Flowers_gShaderProgramObject, "u_viewMatrix");
	Flowers_projectionUniform = glGetUniformLocation(Flowers_gShaderProgramObject, "u_projectionMatrix");

	Flowers_LKeyPressed = glGetUniformLocation(Flowers_gShaderProgramObject, "u_LKeyPressed");
	Flowers_LDUniform = glGetUniformLocation(Flowers_gShaderProgramObject, "u_LDUniform");
	Flowers_LAUniform = glGetUniformLocation(Flowers_gShaderProgramObject, "u_LAUniform");
	Flowers_LSUniform = glGetUniformLocation(Flowers_gShaderProgramObject, "u_LSUniform");

	Flowers_KDUniform = glGetUniformLocation(Flowers_gShaderProgramObject, "u_KDUniform");
	Flowers_KAUniform = glGetUniformLocation(Flowers_gShaderProgramObject, "u_KAUniform");
	Flowers_KSUniform = glGetUniformLocation(Flowers_gShaderProgramObject, "u_KSUniform");
	Flowers_gTextureSamplerUniform = glGetUniformLocation(Flowers_gShaderProgramObject, "u_texture_sampler");
	Flowers_LightPositionUniform = glGetUniformLocation(Flowers_gShaderProgramObject, "u_LightPosition");
	Flowers_MaterialShininessUniform = glGetUniformLocation(Flowers_gShaderProgramObject, "u_MaterialShininess");
	//Here we have done all the preparations of data transfer from CPU to GPU



	myModelFlowers = new Model((char *)"nanosuit/RutwikFlower/rutwikFlower.obj");
	myModelLavender = new Model((char *)"nanosuit/lavender1/Flower Hyacinth N100512.3DS");
}
void initializeLightning()
{
	GLuint gVertexShaderObject;
	GLuint gVertexShaderObject_filter;
	GLuint gVertexShaderObject_resolve;
	GLuint gFragmentShaderObject;
	GLuint gFragmentShaderObject_filter;
	GLuint gFragmentShaderObject_resolve;
	GLuint gTesselationControlShaderObject;
	GLuint gTesselationEvaluationShaderObject;



	//////////////////////////////// V E R T E X - S H A D E R //////////////////////////


	//Define Vertex Shader Object
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar **p;
	const GLchar *vertexShaderSourceCode =

		"#version 430 core" \
		"\n"
		"layout(location = 0)in vec2 position;" \
		"layout(location = 1)in vec3 normal;" \
		"out VS_OUT" \
		"{" \
		"vec3 N;" \
		"vec3 L;" \
		"vec3 V;" \
		"flat int material_index;" \
		"} vs_out;" \

		"uniform vec3 light_pos = vec3(100.0, 100.0, 100.0);" \


		"uniform mat4 mat_proj;" \
		"uniform mat4  mat_view;" \
		"uniform mat4 mat_model;" \

		"void main(void)" \
		"{" \
		"	mat4 mat_mv = mat_view * mat_model;" \

		// Calculate view-space coordinate
		"vec4 P = mat_mv * vec4(position,0.0,1.0);" \

		// Calculate normal in view-space
		"vs_out.N = mat3(mat_mv) * normal;" \

		//// Calculate light vector
		"vs_out.L = light_pos - P.xyz;" \

		//// Calculate view vector
		"vs_out.V = -P.xyz;" \

		// Pass material index
		//"vs_out.material_index = gl_InstanceID;" \

		// Calculate the clip-space position of each vertex
		"gl_Position = vec4(position,0.0,1.0);" \

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
				fprintf(gpLogFile, "VertexShader :\n");
				fprintf(gpLogFile, "%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}
	////////////////////// TESSELLATION CONTROL SHADER //////////////////////////
	//This shader will create patch of 4 vertices
	gTesselationControlShaderObject = glCreateShader(GL_TESS_CONTROL_SHADER);
	const GLchar *tessellationControlShaderSourceCode =
		"#version 430" \
		"\n" \
		"layout(vertices=4)out;" \
		"in VS_OUT" \
		"{" \
		"vec3 N;" \
		"vec3 L;" \
		"vec3 V;" \
		"flat int material_index;" \
		"} tcs_in[];" \

		"out TCS_OUT" \
		"{" \
		"vec3 N;" \
		"vec3 L;" \
		"vec3 V;" \
		"flat int material_index;" \
		"}tcs_out[];" \

		"uniform int numberOfSegments;" \
		"uniform int numberOfStrips;" \
		"void main(void)" \
		"{" \



		"gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;" \
		"gl_TessLevelOuter[0] = float(numberOfStrips);" \
		"gl_TessLevelOuter[1] = float(numberOfSegments);" \
		"if(gl_InvocationID == 0)" \
		"{" \
		"tcs_out[gl_InvocationID].N = tcs_in[gl_InvocationID].N;" \
		"tcs_out[gl_InvocationID].L = tcs_in[gl_InvocationID].L;" \
		"tcs_out[gl_InvocationID].V = tcs_in[gl_InvocationID].V;" \
		"tcs_out[gl_InvocationID].material_index = tcs_in[gl_InvocationID].material_index;" \
		"}" \
		"}";



	glShaderSource(gTesselationControlShaderObject, 1, (const GLchar **)&tessellationControlShaderSourceCode, NULL);

	//compileShader
	glCompileShader(gTesselationControlShaderObject);

	//////////////// Error Checking//////////////////
	//Code for catching the errors 
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;


	glGetShaderiv(gTesselationControlShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gTesselationControlShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written9;
				glGetShaderInfoLog(gTesselationControlShaderObject, iInfoLogLength, &written9, szInfoLog);
				fprintf(gpLogFile, "TCS :  %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}


	///////////////// TESSELLATION EVLAUTATION SHADER /////////////

	gTesselationEvaluationShaderObject = glCreateShader(GL_TESS_EVALUATION_SHADER);

	const GLchar *tessellationEvalutationShaderSourceCode =
		"#version 430" \
		"\n" \
		"layout(isolines)in;" \
		"uniform mat4 u_mvp_matrix;" \
		"uniform float length;" \

		"in TCS_OUT" \
		"{" \
		"vec3 N;" \
		"vec3 L;" \
		"vec3 V;" \
		"flat int material_index;" \
		"} tes_in[];" \

		"out TES_OUT" \
		"{" \
		"vec3 N;" \
		"vec3 L;" \
		"vec3 V;" \
		"flat int material_index;" \
		"}tes_out;" \

		"uniform float b2Constant;" \
		"uniform float b1Constant;" \

		"uniform mat4 mat_proj;" \
		"uniform mat4  mat_view;" \
		"uniform mat4 mat_model;" \
		"void main(void)" \
		"{" \


		"float u =  gl_TessCoord.x;" \
		"vec3 p0 = gl_in[0].gl_Position.xyz;" \
		"vec3 p1 = gl_in[1].gl_Position.xyz;" \
		"vec3 p2 = gl_in[2].gl_Position.xyz;" \
		"vec3 p3 = gl_in[3].gl_Position.xyz;" \
		"float u1 = (1.0 - u);" \
		"float u2 = u * u;" \
		"float b3 =  u2 * u;" \
		"float b2 = b2Constant * u2 * u1;" \
		"float b1 = b1Constant * u * u1 * u1;" \
		"float b0 =   u1* u1 * u1;" \
		"vec3 p =  length * p0 * b0 + p1 * b1 + p2 * b2 +p3 * b3 ;" \

		"gl_Position = mat_proj *mat_view *mat_model * vec4(p , 1.0);" \
		"tes_out.N = tes_in[0].N;" \
		"tes_out.L = tes_in[0].L;" \
		"tes_out.V = tes_in[0].V;" \
		"tes_out.material_index = tes_in[0].material_index;" \


		"}";

	glShaderSource(gTesselationEvaluationShaderObject, 1, (const GLchar **)&tessellationEvalutationShaderSourceCode, NULL);

	//compileShader
	glCompileShader(gTesselationEvaluationShaderObject);

	//////////////// Error Checking//////////////////
	//Code for catching the errors 
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;


	glGetShaderiv(gTesselationEvaluationShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gTesselationEvaluationShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written6;
				glGetShaderInfoLog(gTesselationEvaluationShaderObject, iInfoLogLength, &written6, szInfoLog);
				fprintf(gpLogFile, "TES :%s\n", szInfoLog);
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
		"layout(location = 0) out vec4 color0;" \
		"layout(location = 1) out vec4 color1;" \
		"out vec4 FragColor;" \
		"in TES_OUT" \
		"{" \
		"vec3 N;" \
		"vec3 L;" \
		"vec3 V;" \
		"flat int material_index;" \
		"} fs_in;" \

		// Material properties
		"uniform float bloom_thresh_min = 0.8;" \
		"uniform float bloom_thresh_max = 1.2;" \

		"struct material_t" \
		"{" \
		"vec3    diffuse_color;" \
		"vec3    specular_color;" \
		"float   specular_power;" \
		"vec3    ambient_color;" \
		"};" \

		"layout(binding = 1, std140) uniform MATERIAL_BLOCK" \
		"{" \
		"material_t  material[1];" \
		"} materials;" \

		"void main(void)" \
		"{" \
		// Normalize the incoming N, L and V vectors
		"	vec3 N = normalize(fs_in.N);" \
		"vec3 L = normalize(fs_in.L);" \
		"vec3 V = normalize(fs_in.V);" \

		// Calculate R locally
		"vec3 R = reflect(-L, N);" \

		"material_t m = materials.material[fs_in.material_index];" \

		// Compute the diffuse and specular components for each fragment
		"vec3 diffuse = max(dot(N, L), 0.0) * m.diffuse_color;" \
		"vec3 specular = pow(max(dot(R, V), 0.0), m.specular_power) * m.specular_color;" \
		"vec3 ambient = m.ambient_color;" \

		// Add ambient, diffuse and specular to find final color
		"vec3 color = ambient + diffuse + specular;" \

		// Write final color to the framebuffer
		//color0 = vec4(color, 1.0);" \

		// Calculate luminance
		"float Y = dot(color, vec3(0.299, 0.587, 0.144));" \

		// Threshold color based on its luminance and write it to
		// the second output
		"color = color * 4.0 * smoothstep(bloom_thresh_min, bloom_thresh_max, Y);"   \
		"FragColor = vec4(color,1.0);" \
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
				fprintf(gpLogFile, "%sFragment Shader : \n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}










	//////////////////////////// FILTER ////////////////////////////////////////////
	//////////////////////////////// V E R T E X - S H A D E R //////////////////////////


	//Define Vertex Shader Object
	gVertexShaderObject_filter = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
//	const GLchar **p;
	const GLchar *vertexShaderSourceCode_filter =

		"#version 430 core" \
		"\n"

		"void main(void)" \
		"{" \
		"const vec4 vertices[] = vec4[](vec4(-1.0, -1.0, 0.5, 1.0)," \
		"vec4(1.0, -1.0, 0.5, 1.0)," \
		"vec4(-1.0, 1.0, 0.5, 1.0)," \
		"vec4(1.0, 1.0, 0.5, 1.0));" \

		"gl_Position = vertices[gl_VertexID];" \

		"}";

	// GPU will run the above code. And GPU WILL RUN FOR PER VERTEX. If there are 1000 vertex. Then GPU will run this shader for
	//1000 times. We are Multiplying each vertex with the Model View Matrix.
	//And how does the GPU gets to know about at what offset the array has to be taken . Go to glVertexAttribPointer() in Display.
	// in = Input. 

	//p = &vertexShaderSourceCode;
		//Specify above source code to the vertex shader object
	glShaderSource(gVertexShaderObject_filter, 1, (const GLchar **)&vertexShaderSourceCode_filter, NULL);

	//Compile the vertex shader 
	glCompileShader(gVertexShaderObject_filter);

	//////////////// Error Checking//////////////////
	//Code for catching the errors 
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;


	glGetShaderiv(gVertexShaderObject_filter, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_filter, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject_filter, iInfoLogLength, &written, szInfoLog);
				fprintf(gpLogFile, "VertexShader :\n");
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
	gFragmentShaderObject_filter = glCreateShader(GL_FRAGMENT_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar *fragmentShaderSourceCode_filter =
		"#version 430 core" \
		"\n" \
		"uniform sampler2D hdr_image;" \

		"out vec4 color;" \

		"const float weights[] = float[](0.0024499299678342," \
		"0.0043538453346397," \
		"0.0073599963704157," \
		"0.0118349786570722," \
		"0.0181026699707781," \
		"0.0263392293891488," \
		"0.0364543006660986," \
		"0.0479932050577658," \
		"0.0601029809166942," \
		"0.0715974486241365," \
		"0.0811305381519717," \
		"0.0874493212267511," \
		"0.0896631113333857," \
		"0.0874493212267511," \
		"0.0811305381519717," \
		"0.0715974486241365," \
		"0.0601029809166942," \
		"0.0479932050577658," \
		"0.0364543006660986," \
		"0.0263392293891488," \
		"0.0181026699707781," \
		"0.0118349786570722," \
		"0.0073599963704157," \
		"0.0043538453346397," \
		"0.0024499299678342);" \

		"void main(void)" \
		"{" \
		"	vec4 c = vec4(0.0);" \
		"ivec2 P = ivec2(gl_FragCoord.yx) - ivec2(0, weights.length() >> 1);" \
		"int i;" \

		"for (i = 0; i < weights.length(); i++)" \
		"{" \
		"c += texelFetch(hdr_image, P + ivec2(0, i), 0) * weights[i];" \
		"}" \

		"color = c;" \
		"}";

	//FragColor = vec4(1,1,1,1) = White Color
	//this means here we are giving color to the Triangle.




	//Specify above source code to the vertex shader object
	glShaderSource(gFragmentShaderObject_filter, 1, (const GLchar **)&fragmentShaderSourceCode_filter, NULL);

	//Compile the vertex shader 
	glCompileShader(gFragmentShaderObject_filter);
	//Code for catching the errors 
		   /*iShaderCompileStatus = 0;
		   iInfoLogLength = 0;*/
	szInfoLog = NULL;


	glGetShaderiv(gFragmentShaderObject_filter, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_filter, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written1;
				glGetShaderInfoLog(gFragmentShaderObject_filter, iInfoLogLength, &written1, szInfoLog);
				fprintf(gpLogFile, "%sFragment Shader : \n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}











	////////////////////////////// RESOLVE //////////////////////////////////////////////
	//////////////////////////////// V E R T E X - S H A D E R //////////////////////////


	//Define Vertex Shader Object
	gVertexShaderObject_resolve = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
//	const GLchar **p;
	const GLchar *vertexShaderSourceCode_resolve =

		"#version 430 core" \
		"\n"

		"void main(void)" \
		"{" \
		"const vec4 vertices[] = vec4[](vec4(-1.0, -1.0, 0.5, 1.0)," \
		"vec4(1.0, -1.0, 0.5, 1.0)," \
		"vec4(-1.0, 1.0, 0.5, 1.0)," \
		"vec4(1.0, 1.0, 0.5, 1.0));" \

		"gl_Position = vertices[gl_VertexID];" \

		"}";

	// GPU will run the above code. And GPU WILL RUN FOR PER VERTEX. If there are 1000 vertex. Then GPU will run this shader for
	//1000 times. We are Multiplying each vertex with the Model View Matrix.
	//And how does the GPU gets to know about at what offset the array has to be taken . Go to glVertexAttribPointer() in Display.
	// in = Input. 

	//p = &vertexShaderSourceCode;
		//Specify above source code to the vertex shader object
	glShaderSource(gVertexShaderObject_resolve, 1, (const GLchar **)&vertexShaderSourceCode_resolve, NULL);

	//Compile the vertex shader 
	glCompileShader(gVertexShaderObject_resolve);

	//////////////// Error Checking//////////////////
	//Code for catching the errors 
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;


	glGetShaderiv(gVertexShaderObject_resolve, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_resolve, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject_resolve, iInfoLogLength, &written, szInfoLog);
				fprintf(gpLogFile, "VertexShader :\n");
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
	gFragmentShaderObject_resolve = glCreateShader(GL_FRAGMENT_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar *fragmentShaderSourceCode_resolve =
		"#version 430 core" \
		"\n" \
		" uniform sampler2D hdr_image;" \
		"uniform sampler2D bloom_image;" \

		"uniform float exposure = 0.9;" \
		"uniform float bloom_factor = 1.0;" \
		"uniform float scene_factor = 1.0;" \

		"out vec4 color;" \

		"void main(void)" \
		"{" \
		"vec4 c = vec4(0.0);" \

		"c += texelFetch(hdr_image, ivec2(gl_FragCoord.xy), 0) * scene_factor;" \
		"c += texelFetch(bloom_image, ivec2(gl_FragCoord.xy), 0) * bloom_factor;" \

		"c.rgb = vec3(1.0) - exp(-c.rgb * exposure);" \
		"	color = c;" \


		"}";

	//FragColor = vec4(1,1,1,1) = White Color
	//this means here we are giving color to the Triangle.




	//Specify above source code to the vertex shader object
	glShaderSource(gFragmentShaderObject_resolve, 1, (const GLchar **)&fragmentShaderSourceCode_resolve, NULL);

	//Compile the vertex shader 
	glCompileShader(gFragmentShaderObject_resolve);
	//Code for catching the errors 
		   /*iShaderCompileStatus = 0;
		   iInfoLogLength = 0;*/
	szInfoLog = NULL;


	glGetShaderiv(gFragmentShaderObject_resolve, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_resolve, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written1;
				glGetShaderInfoLog(gFragmentShaderObject_resolve, iInfoLogLength, &written1, szInfoLog);
				fprintf(gpLogFile, "%sFragment Shader : \n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}




	// CREATE SHADER PROGRAM OBJECT
	L_gShaderProgramObject = glCreateProgram();
	L_gShaderProgramObject_filter = glCreateProgram();
	L_gShaderProgramObject_resolve = glCreateProgram();
	//attach vertex shader to the gShaderProgramObject
	glAttachShader(L_gShaderProgramObject, gVertexShaderObject);
	glAttachShader(L_gShaderProgramObject, gTesselationControlShaderObject);
	glAttachShader(L_gShaderProgramObject, gTesselationEvaluationShaderObject);
	glAttachShader(L_gShaderProgramObject, gFragmentShaderObject);


	glAttachShader(L_gShaderProgramObject_filter, gVertexShaderObject_filter);
	glAttachShader(L_gShaderProgramObject_filter, gFragmentShaderObject_filter);


	glAttachShader(L_gShaderProgramObject_resolve, gVertexShaderObject_resolve);
	glAttachShader(L_gShaderProgramObject_resolve, gFragmentShaderObject_resolve);
	//Pre-Linking  binding to vertexAttributes
	//glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPosition");
	//glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_NORMAL, "vNormal");
	//Here the above line means that we are linking the GPU's variable vPosition with the CPU's  enum member  i.e AMC_ATTRIBUTE_POSITION .
	//So whatever changes will be done in AMC_ATTRIBUTE_POSITION , those will also reflect in vPosition

	//RULE : ALWAYS BIND THE ATTRIBUTES BEFORE LINKING AND BIND THE UNIFORM AFTER LINKING.

	//Link the shader program 
	glLinkProgram(L_gShaderProgramObject);
	glLinkProgram(L_gShaderProgramObject_filter);
	glLinkProgram(L_gShaderProgramObject_resolve);

	//Code for catching the errors 
	GLint iProgramLinkStatus = 0;



	glGetProgramiv(L_gShaderProgramObject, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(L_gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written3;
				glGetProgramInfoLog(L_gShaderProgramObject, iInfoLogLength, &written3, szInfoLog);
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



	L_uniforms.L_scene.bloom_thresh_min = glGetUniformLocation(L_gShaderProgramObject, "bloom_thresh_min");
	L_uniforms.L_scene.bloom_thresh_max = glGetUniformLocation(L_gShaderProgramObject, "bloom_thresh_max");
	L_samplerFilter = glGetUniformLocation(L_gShaderProgramObject_filter, "hdr_image");



	L_uniforms.L_resolve.exposure = glGetUniformLocation(L_gShaderProgramObject_resolve, "exposure");
	L_uniforms.L_resolve.bloom_factor = glGetUniformLocation(L_gShaderProgramObject_resolve, "bloom_factor");
	L_uniforms.L_resolve.scene_factor = glGetUniformLocation(L_gShaderProgramObject_resolve, "scene_factor");
	L_samplerResolve0 = glGetUniformLocation(L_gShaderProgramObject_resolve, "hdr_image");
	L_samplerResolve1= glGetUniformLocation(L_gShaderProgramObject_resolve, "bloom_image");

	L_projectionUniform = glGetUniformLocation(L_gShaderProgramObject, "mat_proj");
	L_ViewUniform = glGetUniformLocation(L_gShaderProgramObject, "mat_view");
	L_modelUniform = glGetUniformLocation(L_gShaderProgramObject, "mat_model");

	//
	L_mvpUniform = glGetUniformLocation(L_gShaderProgramObject, "u_mvp_matrix");
	L_b0Uniform = glGetUniformLocation(L_gShaderProgramObject, "b2Constant");
	L_b1Uniform = glGetUniformLocation(L_gShaderProgramObject, "b1Constant");
	//Here we have done all the preparations of data transfer from CPU to GPU

	L_gNumberOfSegmentUniform = glGetUniformLocation(L_gShaderProgramObject, "numberOfSegments");
	L_gNumberOfStripsUniform = glGetUniformLocation(L_gShaderProgramObject, "numberOfStrips");
	L_lengthUniform = glGetUniformLocation(L_gShaderProgramObject, "length");


	//modelUniform = glGetUniformLocation(gShaderProgramObject, "u_modelMatrix");
	//ViewUniform = glGetUniformLocation(gShaderProgramObject, "u_viewMatrix");
	//projectionUniform = glGetUniformLocation(gShaderProgramObject, "u_projectionMatrix");

	//LKeyPressed = glGetUniformLocation(gShaderProgramObject, "u_LKeyPressed");
	//LDUniform = glGetUniformLocation(gShaderProgramObject, "u_LDUniform");
	//LAUniform = glGetUniformLocation(gShaderProgramObject, "u_LAUniform");
	//LSUniform = glGetUniformLocation(gShaderProgramObject, "u_LSUniform");

	//KDUniform = glGetUniformLocation(gShaderProgramObject, "u_KDUniform");
	//KAUniform = glGetUniformLocation(gShaderProgramObject, "u_KAUniform");
	//KSUniform = glGetUniformLocation(gShaderProgramObject, "u_KSUniform");
	//LightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_LightPosition");
	//MaterialShininessUniform = glGetUniformLocation(gShaderProgramObject, "u_MaterialShininess");
	//Here we have done all the preparations of data transfer from CPU to GPU

	/*getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();
*/

	//Basically we are giving the vertices coordinates in the above array
	////////////////////FOR TRIANGLE ///////////////// 
	//Create vao - Vertex array objects

	//////////////////////// FOR RECTANGLE //////////////// 




	L_gNumberOfLineSegments = 1;





	int i;
	static const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	glGenVertexArrays(1, &L_vao);
	glBindVertexArray(L_vao);







	//load_shaders();

	static const GLfloat exposureLUT[20] = { 11.0f, 6.0f, 3.2f, 2.8f, 2.2f, 1.90f, 1.80f, 1.80f, 1.70f, 1.70f,  1.60f, 1.60f, 1.50f, 1.50f, 1.40f, 1.40f, 1.30f, 1.20f, 1.10f, 1.00f };
	//static const GLfloat exposureLUT[20] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.00f };

	glGenFramebuffers(1, &L_render_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, L_render_fbo);

	glGenTextures(1, &L_tex_scene);
	glBindTexture(GL_TEXTURE_2D, L_tex_scene);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, L_tex_scene, 0);
	glGenTextures(1, &L_tex_brightpass);
	glBindTexture(GL_TEXTURE_2D, L_tex_brightpass);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, L_tex_brightpass, 0);
	glGenTextures(1, &L_tex_depth);
	glBindTexture(GL_TEXTURE_2D, L_tex_depth);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT16, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, L_tex_depth, 0);
	glDrawBuffers(2, buffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &L_filter_fbo[0]);
	glGenTextures(1, &L_tex_filter[0]);

	glBindFramebuffer(GL_FRAMEBUFFER, L_filter_fbo[0]);
	glBindTexture(GL_TEXTURE_2D, L_tex_filter[0]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, L_tex_filter[0], 0);
	glDrawBuffers(1, buffers);


	glGenFramebuffers(1, &L_filter_fbo[1]);
	glGenTextures(1, &L_tex_filter[1]);
	glBindFramebuffer(GL_FRAMEBUFFER, L_filter_fbo[1]);
	glBindTexture(GL_TEXTURE_2D, L_tex_filter[1]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, L_tex_filter[1], 0);
	glDrawBuffers(1, buffers);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//glGenTextures(1, &tex_lut);
	//glBindTexture(GL_TEXTURE_1D, tex_lut);
	//glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, 20);
	//glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 20, GL_RED, GL_FLOAT, exposureLUT);
	//glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);




	const GLfloat rectangleVertices[] =
	{
		// Perspective square (Top face)
	/*	1.0f, 1.0f, -1.0f,
-1.0f, 1.0f, -1.0f,
-1.0f, 1.0f, 1.0f,
1.0f, 1.0f, 1.0f,

1.0f, -1.0f, -1.0f,
-1.0f, -1.0f, -1.0f,
-1.0f, -1.0f, 1.0f,
1.0f, -1.0f, 1.0f,*/

/////////////////////
//1.0f, 1.0f, 1.0f,
//-1.0f, 1.0f, 1.0f,
//-1.0f, -1.0f, 1.0f,
//1.0f, -1.0f, 1.0f,
//////////////////////////


-1.0f, 1.0f, 0.0f,
-1.0f, -1.0f, 0.0f,








//
//1.0f, 1.0f, -1.0f,
//-1.0f, 1.0f, -1.0f,
//-1.0f, -1.0f, -1.0f,
//1.0f, -1.0f, -1.0f,

//1.0f, 1.0f, -1.0f,
//1.0f, 1.0f, 1.0f,
//1.0f, -1.0f, 1.0f,
//1.0f, -1.0f, -1.0f,


//-1.0f, 1.0f, -1.0f,
//-1.0f, 1.0f, 1.0f,
//-1.0f, -1.0f, 1.0f,
//-1.0f, -1.0f, -1.0f

	};
	const GLfloat cubeNormals[] =
	{
		/*0.0f, 1.0f, 0.0f,
0.0f, 1.0f, 0.0f,
0.0f, 1.0f, 0.0f,
0.0f, 1.0f, 0.0f,

0.0f, -1.0f, 0.0f,
0.0f, -1.0f, 0.0f,
0.0f, -1.0f, 0.0f,
0.0f, -1.0f, 0.0f,*/

0.0f, 0.0f, 1.0f,
0.0f, 0.0f, 1.0f,
//0.0f, 0.0f, 1.0f,
//0.0f, 0.0f, 1.0f,
//
//0.0f, 0.0f, -1.0f,
//0.0f, 0.0f, -1.0f,
//0.0f, 0.0f, -1.0f,
//0.0f, 0.0f, -1.0f,
//
//1.0f, 0.0f, 0.0f,
//1.0f, 0.0f, 0.0f,
//1.0f, 0.0f, 0.0f,
//1.0f, 0.0f, 0.0f,
//
//-1.0f, 0.0f, 0.0f,
//-1.0f, 0.0f, 0.0f,
//-1.0f, 0.0f, 0.0f,
//-1.0f, 0.0f, 0.0f

	};

	const GLfloat lineVertices[] = {
		-0.5f,1.0f,0.5f,-1.0f,1.0f,1.0f,2.0f,-1.0f
	};
	const GLfloat lineNormals[] = {
		/*-1.0,0.5,1.0,
		1.0,-0.5,1.0,
		1.0,1.0,1.0,
		-1.0,-1.0,1.0,
		-1.0,0.5,1.0,
		1.0,-0.5,1.0*/

		0.0f, 0.0f, 1.0f,
0.0f, 0.0f, 1.0f,

	};

	glGenVertexArrays(1, &L_vao_Sphere);
	glBindVertexArray(L_vao_Sphere);



	glGenBuffers(1, &L_vbo_position_Sphere);
	glBindBuffer(GL_ARRAY_BUFFER, L_vbo_position_Sphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	//NORMALS 
	glGenBuffers(1, &L_vbo_Normals_Sphere);
	glBindBuffer(GL_ARRAY_BUFFER, L_vbo_Normals_Sphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineNormals), lineNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);



	////Elements
	/*glGenBuffers(1, &vbo_Element_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);*/
	//object.load("torus.sbm");



	/*glGenBuffers(1, &ubo_transform);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_transform);
	glBufferData(GL_UNIFORM_BUFFER, (2 + SPHERE_COUNT) * sizeof(vmath::mat4), NULL, GL_DYNAMIC_DRAW);*/

	struct material
	{
		vmath::vec3     diffuse_color;
		unsigned int : 1;           // pad
		vmath::vec3     specular_color;
		float           specular_power;
		vmath::vec3     ambient_color;
		unsigned int : 1;           // pad
	};

	glGenBuffers(1, &L_ubo_material);
	glBindBuffer(GL_UNIFORM_BUFFER, L_ubo_material);
	glBufferData(GL_UNIFORM_BUFFER, SPHERE_COUNT * sizeof(material), NULL, GL_STATIC_DRAW);

	material * m = (material *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, SPHERE_COUNT * sizeof(material), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	float ambient = 0.2f;
	for (i = 0; i < SPHERE_COUNT; i++)
	{
		float fi = 3.14159267f * (float)i / 8.0f;
		m[i].diffuse_color = vmath::vec3(sinf(fi) * 0.5f + 0.5f, sinf(fi + 1.345f) * 0.5f + 0.5f, sinf(fi + 2.567f) * 0.5f + 0.5f);
		m[i].specular_color = vmath::vec3(2.0f, 0.0f, 40.0f);
		m[i].specular_power = 1.0f;
		m[i].ambient_color = vmath::vec3(ambient * 0.025f);
		ambient *= 1.0f;
	}
	glUnmapBuffer(GL_UNIFORM_BUFFER);


initializeLightningFrameBuffer();


}
void initializeLightningFrameBuffer()
{
	GLuint gVertexShaderObject;
	GLuint gFragmentShaderObject1;

	//////////////////////////////// V E R T E X - S H A D E R //////////////////////////


	//Define Vertex Shader Object
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar **p;
	const GLchar *vertexShaderSourceCode =

		"#version 430 core" \
		"\n"
		"in vec4 vPosition;" \
		"in vec2 vTexCoord;" \

		"out VS_OUT" \
		"{" \
		"vec4 color;" \
		"vec2 texcoord;" \
		"} vs_out;" \

		"uniform mat4 u_modelView_matrix;"  \
		"uniform mat4 u_p_matrix;" \

		"void main(void)" \
		"{" \
		"gl_Position = u_p_matrix * u_modelView_matrix * vPosition;" \

		"vs_out.texcoord = vTexCoord;" \
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
				fprintf(gpLogFile, " Vertex Shader :%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}
	///////////////////  FRAGMENT SHADER 1 ////////////
	gFragmentShaderObject1 = glCreateShader(GL_FRAGMENT_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar *fragmentShaderSourceCode1 =
		"#version 430 core" \
		"\n" \

		"uniform sampler2D tex;" \

		"out vec4 FragColor;" \

		"in VS_OUT" \
		"{" \
		"vec4 color;" \
		"vec2 texcoord;" \
		"} fs_in;" \
		"void main(void)" \
		"{" \
		"FragColor = texture(tex,fs_in.texcoord);" \
		"}";

	//FragColor = vec4(1,1,1,1) = White Color
	//this means here we are giving color to the Triangle.




	//Specify above source code to the vertex shader object
	glShaderSource(gFragmentShaderObject1, 1, (const GLchar **)&fragmentShaderSourceCode1, NULL);

	//Compile the vertex shader 
	glCompileShader(gFragmentShaderObject1);
	//Code for catching the errors 
		   /*iShaderCompileStatus = 0;
		   iInfoLogLength = 0;*/
	szInfoLog = NULL;


	glGetShaderiv(gFragmentShaderObject1, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject1, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written1;
				glGetShaderInfoLog(gFragmentShaderObject1, iInfoLogLength, &written1, szInfoLog);
				fprintf(gpLogFile, "FRAGMENT  1   SHADER :%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}

	L_FB_gShaderProgramObject = glCreateProgram();
	glAttachShader(L_FB_gShaderProgramObject, gVertexShaderObject);
	glAttachShader(L_FB_gShaderProgramObject, gFragmentShaderObject1);

	glBindAttribLocation(L_FB_gShaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(L_FB_gShaderProgramObject, AMC_ATTRIBUTE_COLOR, "vColor");
	glBindAttribLocation(L_FB_gShaderProgramObject, AMC_ATTRIBUTE_TEXCOORD0, "vTexCoord");

	//	glBindAttribLocation(gShaderProgramObject1, AMC_ATTRIBUTE_TEXCOORD0, "vTexCoord");
		//Here the above line means that we are linking the GPU's variable vPosition with the CPU's  enum member  i.e AMC_ATTRIBUTE_POSITION .
		//So whatever changes will be done in AMC_ATTRIBUTE_POSITION , those will also reflect in vPosition

		//RULE : ALWAYS BIND THE ATTRIBUTES BEFORE LINKING AND BIND THE UNIFORM AFTER LINKING.

		//Link the shader program 

	glLinkProgram(L_FB_gShaderProgramObject);

	//Code for catching the errors 
	GLint iProgramLinkStatus = 0;



	glGetProgramiv(L_FB_gShaderProgramObject, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(L_FB_gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written3;
				glGetProgramInfoLog(L_FB_gShaderProgramObject, iInfoLogLength, &written3, szInfoLog);
				fprintf(gpLogFile, "%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}


	const GLfloat vertex_data[] =
	{
		

			1.0f, 1.0f,1.0f,	     0.0f, 0.0f,
			-1.0f, 1.0f, 1.0f,		 1.0f, 0.0f,
			-1.0f, -1.0f, 1.0f,      1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,	     0.0f, 1.0f,

		

	};
	L_FB_mvpUniform1 = glGetUniformLocation(L_FB_gShaderProgramObject, "u_p_matrix");
	L_FB_modelViewUniform1 = glGetUniformLocation(L_FB_gShaderProgramObject, "u_modelView_matrix");
	L_samplerForLightning = glGetUniformLocation(L_FB_gShaderProgramObject, "tex");

	//Basically we are giving the vertices coordinates in the above array
	////////////////////FOR TRIANGLE ///////////////// 
	//Create vao - Vertex array objects
	glGenVertexArrays(1, &L_FB_vao_triangle);
	glBindVertexArray(L_FB_vao_triangle);
	glGenBuffers(1, &L_FB_vbo_position_triangle);
	glBindBuffer(GL_ARRAY_BUFFER, L_FB_vbo_position_triangle);
	//in the above statement we have accesed the GL-ARRAY_BUFFER using vbo. Without it wouldn't be possible to get GL_ARRAY_BUFFER
	//For the sake of understanding . GL_ARRAY_BUFFER is in the GPU side ad=nd we have bind our CPU side vbo with it like a Pipe to get the access.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
	//The above statement states that , we are passing our vertices array to the GPU and GL_STATIC_DRAW means draw it now only. Don't draw it in Runtime. 
	//The below statement states that after storing the data in the GPU'S buffer . We are passing it to the vPosition now . 

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);
	//GL_FALSE = We are not giving normalized coordinates as our coordinates are not converted in 0 - 1 range.
	//3 = This is the thing I was talking about in initialize. Here, we are telling GPU to break our array in 3 parts . 
	//0 and Null are for the Interleaved. 
	//GL_FLOAT- What is the type? .
	//AMC_ATTRIBUTE_POSITION. here we are passing data to vPosition. 

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);

	//// FRAMEBUFFER /////////

	glGenFramebuffers(1, &L_fbo);   //create a frameBuffer Object 
	glBindFramebuffer(GL_FRAMEBUFFER, L_fbo); //Bind it 
	glGenTextures(1, &L_color_texture);//Create a texture for our color buffer.
	glBindTexture(GL_TEXTURE_2D, L_color_texture); //Bind it 
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, MAX_SCENE_WIDTH, MAX_SCENE_WIDTH);
	// we are going to read from this but it won't have MipMaps. TurnOff MipMaps for this texture.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &L_depth_texture);//Create a texture that will be ours FBO's depth Buffer
	glBindTexture(GL_TEXTURE_2D, L_depth_texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, MAX_SCENE_WIDTH, MAX_SCENE_WIDTH);


	//Attach BOTH COLOR AND TEXTURE with the FBO
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, L_color_texture, 0); //here 0 specifies the mipmap level 
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, L_depth_texture, 0);


	//Tell OpenGL what we want to draw into the framebuffer's memory first and Only Color Attachment
	static const GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, draw_buffers);

	GLenum fboStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (fboStatus == GL_FRAMEBUFFER_COMPLETE)
		fprintf(gpLogFile, "fbo status %d\n", fboStatus);
	glBindFramebuffer(GL_FRAMEBUFFER, 0); //UnBind it 
	glBindTexture(GL_TEXTURE_2D, 0); //UnBind it 
}
void init3DNoiseTexture(int texSize, GLubyte* texPtr)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &N_texName);
	glBindTexture(GL_TEXTURE_3D, N_texName);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, texSize, texSize, texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, texPtr);
	free(texPtr);
}


void resize(int width, int height)
{


	if (height == 0)
	{
		height = 1;
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	T_gWidth = width;
	T_gHeight = height;
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
	perspectiveProjectionMatrix = perspective(60.0f,
		(GLfloat)width / (GLfloat)height,
		0.1f,
		1000.0f);

}
void displayRain()
{
	void drawGeometry();
	//start using OpenGL program Object
	glUseProgram(R_gShaderProgramObject);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	if (R_gbLighting == 1)
	{
		//fprintf(gpLogFile, "gbLighting is TRUE!\n");
		glUniform1i(LKeyPressed, gbLighting);
		glUniform3fv(LDUniform, 1, lightDiffuse);
		glUniform3fv(LAUniform, 1, lightAmbient);
		glUniform3fv(LSUniform, 1, lightSpecular);

		glUniform3fv(KDUniform, 1, materialDiffuse);
		glUniform3fv(KAUniform, 1, materialAmbient);
		glUniform3fv(KSUniform, 1, materialSpecular);
		glUniform1f(MaterialShininessUniform, materialShininess);
		glUniform4fv(LightPositionUniform, 1, lightPosition);
	}
	else
	{
		//fprintf(gpLogFile, "gbLighting is FALSE!\n");
		glUniform1i(LKeyPressed, 0);
	}

	if(rainExtra == 0)
	{ 
	glLineWidth(1.0f);

	//gPerspectiveProjectionMatrix = perspective(45.0f, (GLfloat)gWidth / (GLfloat)gHeight, 0.1f, 100.0f);

	//OpenL Drawing 
	//set modelview & modelviewprojection matrices to identity 
	///////////////////////////////////////////// CENTER FRONT ///////////////////////////////////////////////////
	mat4 rotationMatrix = mat4::identity();

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.0f, R_yAnimateSlanting, 8.0f);

	drawGeometry();

	///////////////////////////////////////////////////CENTER BACK //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.5f, R_yAnimateSlanting,10.0f);
	drawGeometry();

	///////////////////////////////////////////////////CENTER UP BACK Y=2.5//////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.3f, R_yAnimateSlanting2, 10.0f);

	drawGeometry();


	/////////////////////////////////////////////////////CENTER UP BACK Y=2.0//////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.8f, R_yAnimateSlanting3, 10.0f);

	drawGeometry();
	///////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.0f, R_yAnimateSlanting4, 10.0f);
	drawGeometry();

	/////////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting5, 10.0f);
	drawGeometry();


	///////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.8f, R_yAnimateSlanting6, 5.0f);

	drawGeometry();
	///////////////////////////////////////////////////CENTER UP BACK Y=1.0 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-7.5f, R_yAnimateSlanting7, 10.0f);
	drawGeometry();


	////////////////////////////////////////////////////Z FRONT DOWN///////////////////////////////////////////////////////////


	///////////////////////////////////////////////  FRONT Down y=-0.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.8f, R_yAnimateSlanting, 8.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Down y=-1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.8f, R_yAnimateSlanting2, 8.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Down y=-1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.8f, R_yAnimateSlanting3, 8.0f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Down y=-2.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-2.8f, R_yAnimateSlanting4, 8.0f);

	drawGeometry();










	/////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////CENTER UP BACK Y=0.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.0f, R_yAnimateSlanting4, 10.0f);
	drawGeometry();

	////////////////////////////////////////////////CENTER DOWN BACK Y=-0.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.8f, R_yAnimateSlanting6, 10.0f);
	drawGeometry();

	////////////////////////////////////////////////CENTER DOWN BACK Y=-1.0 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-7.0f, R_yAnimateSlanting2, 10.0f);
	drawGeometry();

	////////////////////////////////////////////////CENTER DOWN BACK Y=-1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-7.5f, R_yAnimateSlanting5, 10.0f);
	drawGeometry();
	////////////////////////////////////////////////CENTER DOWN BACK Y=-2.0//////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.3f, R_yAnimateSlanting4, 10.0f);
	drawGeometry();

	////////////////////////////////////////////////CENTER DOWN BACK Y=-2.5//////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.8f, R_yAnimateSlanting2, 10.0f);
	drawGeometry();


	/////////////////////////////////////////////////////////////Z FRONT UP///////////////////////////////////////////////////////////


	///////////////////////////////////////////////  FRONT Top y=0.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.2f,R_yAnimateSlanting, 10.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.4f, R_yAnimateSlanting5, 10.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-7.2f, R_yAnimateSlanting2, 10.0f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-7.8f, R_yAnimateSlanting4, 10.0f);

	drawGeometry();








	///////////////////////////////////////////////////////////Z FRONT DOWN///////////////////////////////////////////////////////////


	/////////////////////////////////////////////  FRONT Down y=-0.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-2.0f, R_yAnimateSlanting, 8.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Down y=-1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-2.5f, R_yAnimateSlanting2, 8.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Down y=-1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.0f, R_yAnimateSlanting4, 8.0f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Down y=-2.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.5f, R_yAnimateSlanting6, 8.0f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Down y=-2.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-4.0f, R_yAnimateSlanting3, 8.0f);

	drawGeometry();



	///////////////////////////////////////////////   X NEGATIVE SIDE //////////////////////////////////////////////////



	///////////////////////////////////////////// CENTER FRONT ///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.8f, R_yAnimateSlanting5, 8.0f);

	drawGeometry();

	/////////////////////////////////////////////////CENTER BACK //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.0f, R_yAnimateSlanting3, 10.0f);
	drawGeometry();

	/////////////////////////////////////////////////CENTER UP BACK Y=2.5//////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-4.5f, R_yAnimateSlanting6, 10.0f);

	drawGeometry();


	/////////////////////////////////////////////////CENTER UP BACK Y=2.0//////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.0f, R_yAnimateSlanting, 10.0f);

	drawGeometry();
	/////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.2f, R_yAnimateSlanting2, 10.0f);
	drawGeometry();

	/////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.9f, R_yAnimateSlanting4, 10.0f);
	drawGeometry();


	/////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-4.1f, R_yAnimateSlanting5, 5.0f);

	drawGeometry();
	/////////////////////////////////////////////////CENTER UP BACK Y=1.0 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-4.1f, R_yAnimateSlanting6, 10.0f);
	drawGeometry();


	/////////////////////////////////////////////////CENTER UP BACK Y=0.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting, 10.0f);
	drawGeometry();

	////////////////////////////////////////////////CENTER DOWN BACK Y=-0.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.5f, R_yAnimateSlanting6, 10.0f);
	drawGeometry();

	////////////////////////////////////////////////CENTER DOWN BACK Y=-1.0 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-7.0f, R_yAnimateSlanting, 10.0f);
	drawGeometry();

	////////////////////////////////////////////////CENTER DOWN BACK Y=-1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-7.5f, R_yAnimateSlanting5, 10.0f);
	drawGeometry();
	////////////////////////////////////////////////CENTER DOWN BACK Y=-2.0//////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.0f, R_yAnimateSlanting3, 10.0f);
	drawGeometry();

	////////////////////////////////////////////////CENTER DOWN BACK Y=-2.5//////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.5f, R_yAnimateSlanting2, 10.0f);
	drawGeometry();


	///////////////////////////////////////////////////////////Z FRONT UP///////////////////////////////////////////////////////////


	/////////////////////////////////////////////  FRONT Top y=0.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.8f, R_yAnimateSlanting4, 8.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.0f, R_yAnimateSlanting2, 8.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-2.8f, R_yAnimateSlanting5, 8.0f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-1.8f, R_yAnimateSlanting, 8.0f);

	drawGeometry();


	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-4.8f, R_yAnimateSlanting3, 10.0f);

	drawGeometry();
	///////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.6f, R_yAnimateSlanting4, 13.0f);
	drawGeometry();

	/////////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-7.0f, R_yAnimateSlanting5, 13.0f);
	drawGeometry();


	///////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.5f, R_yAnimateSlanting6, 13.0f);

	drawGeometry();
	///////////////////////////////////////////////////CENTER UP BACK Y=1.0 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-7.2f, R_yAnimateSlanting7, 13.0f);
	drawGeometry();


	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.8f, R_yAnimateSlanting3, 13.0f);

	drawGeometry();
	///////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-9.6f, R_yAnimateSlanting4, 13.0f);
	drawGeometry();

	/////////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-11.0f, R_yAnimateSlanting5, 13.0f);
	drawGeometry();


	///////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting6, 13.0f);

	drawGeometry();
	///////////////////////////////////////////////////CENTER UP BACK Y=1.0 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.5f, R_yAnimateSlanting7, 13.0f);
	drawGeometry();



	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.8f, R_yAnimateSlanting3, 15.0f);

	drawGeometry();
	///////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-9.6f, R_yAnimateSlanting4, 15.0f);
	drawGeometry();

	/////////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-2.0f, R_yAnimateSlanting5, 15.0f);
	drawGeometry();


	///////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.0f, R_yAnimateSlanting6, 15.0f);

	drawGeometry();
	///////////////////////////////////////////////////CENTER UP BACK Y=1.0 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.5f, R_yAnimateSlanting7, 15.0f);
	drawGeometry();




	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-4.8f, R_yAnimateSlanting2, 6.0f);

	drawGeometry();
	///////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-1.6f, R_yAnimateSlanting6, 6.0f);
	drawGeometry();

	/////////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-7.0f, R_yAnimateSlanting4, 6.0f);
	drawGeometry();


	///////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting5, 6.0f);

	drawGeometry();
	///////////////////////////////////////////////////CENTER UP BACK Y=1.0 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.5f, R_yAnimateSlanting6, 6.0f);
	drawGeometry();

	
	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.0f,R_yAnimateSlanting, 10.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-9.0f, R_yAnimateSlanting5, 10.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.0f, R_yAnimateSlanting2, 10.0f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.0f, R_yAnimateSlanting4, 10.0f);

	drawGeometry();






	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.5f, R_yAnimateSlanting, 12.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-7.0f, R_yAnimateSlanting5, 12.2f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.0f, R_yAnimateSlanting2, 12.5f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-9.0f, R_yAnimateSlanting4, 12.7f);

	drawGeometry();



	/////////////////////////////////////




	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-2.0f, R_yAnimateSlanting5, 4.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-1.0f, R_yAnimateSlanting, 4.2f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.0f, R_yAnimateSlanting4, 4.5f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.0f, R_yAnimateSlanting2, 4.7f);

	drawGeometry();
	/////////////////////////////////////
	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-9.0f, R_yAnimateSlanting2, 4.7f);

	drawGeometry();
	/////////////////////////////////////
	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-2.0f, R_yAnimateSlanting2, 4.7f);

	drawGeometry();
	/////////////////////////////////////



	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.0f, R_yAnimateSlanting2, 4.7f);

	drawGeometry();
	/////////////////////////////////////
	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-4.0f, R_yAnimateSlanting2, 0.7f);

	drawGeometry();
	/////////////////////////////////////
	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.0f, R_yAnimateSlanting2, 1.7f);

	drawGeometry();
	/////////////////////////////////////

	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-1.0f, R_yAnimateSlanting5, 1.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.2f, R_yAnimateSlanting3, 1.2f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.5f, R_yAnimateSlanting, 1.5f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-7.8f, R_yAnimateSlanting2, 1.7f);

	drawGeometry();

	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.9f, R_yAnimateSlanting3, -0.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.0f, R_yAnimateSlanting4, -0.2f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-2.0f, R_yAnimateSlanting5, -0.5f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-1.0f, R_yAnimateSlanting6, -0.7f);

	drawGeometry();

	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.0f, R_yAnimateSlanting5, -1.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.5f, R_yAnimateSlanting4, -1.2f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.3f, R_yAnimateSlanting, -1.5f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-4.2f, R_yAnimateSlanting4, -1.7f);

	drawGeometry();








	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.0f, R_yAnimateSlanting, -2.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.0f, R_yAnimateSlanting5, -2.2f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-2.0f, R_yAnimateSlanting3, -2.5f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-9.0f, R_yAnimateSlanting5, -2.7f);

	drawGeometry();

	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.0f, R_yAnimateSlanting6, -3.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-1.0f, R_yAnimateSlanting, -3.2f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-9.0f, R_yAnimateSlanting5, -3.5f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.5f, R_yAnimateSlanting2, -3.7f);

	drawGeometry();

	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.5f, R_yAnimateSlanting, -4.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-4.0f, R_yAnimateSlanting2, -4.2f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-4.6f, R_yAnimateSlanting3, -4.5f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.2f, R_yAnimateSlanting4, -4.7f);

	drawGeometry();




	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.6f, R_yAnimateSlanting5, -6.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-7.0f, R_yAnimateSlanting6, -6.2f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.0f, R_yAnimateSlanting2, -5.5f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(2.0f, R_yAnimateSlanting4, -7.7f);

	drawGeometry();


	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(5.0f, R_yAnimateSlanting3, -8.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(3.0f, R_yAnimateSlanting4, -9.2f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(8.0f, R_yAnimateSlanting3, -8.5f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(2.0f, R_yAnimateSlanting6, -9.7f);

	drawGeometry();


	/////////////////
	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(0.0f, R_yAnimateSlanting6, -10.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.0f, R_yAnimateSlanting6, -10.2f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-2.0f, R_yAnimateSlanting, -10.5f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-9.0f, R_yAnimateSlanting3, -10.7f);

	drawGeometry();



	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(6.0f, R_yAnimateSlanting5, -12.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(2.0f, R_yAnimateSlanting6, -13.2f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-2.5f, R_yAnimateSlanting2, -15.5f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.0f, R_yAnimateSlanting4, -10.7f);

	drawGeometry();




	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting, -14.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting5, -14.2f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting2, -14.5f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting4, -14.7f);

	drawGeometry();




	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting2, -16.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting6, -16.2f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting3, -16.5f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting, -16.7f);

	drawGeometry();





	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.0f, R_yAnimateSlanting5, -18.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(8.0f, R_yAnimateSlanting2, -18.2f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.8f, R_yAnimateSlanting3, -18.5f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.0f, R_yAnimateSlanting4, -18.7f);

	drawGeometry();





	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.8f, R_yAnimateSlanting6, -19.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-1.5f, R_yAnimateSlanting, -19.2f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.5f, R_yAnimateSlanting4, -19.5f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-4.0f, R_yAnimateSlanting2, -19.7f);

	drawGeometry();




	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-4.5f, R_yAnimateSlanting6, -20.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.1f, R_yAnimateSlanting, -20.2f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.5f, R_yAnimateSlanting2, -20.5f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.0f, R_yAnimateSlanting5, -20.7f);

	drawGeometry();



	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-2.0f, R_yAnimateSlanting, -22.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-7.0f, R_yAnimateSlanting5, -22.2f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.5f, R_yAnimateSlanting2, -22.5f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.0f, R_yAnimateSlanting4, -22.7f);

	drawGeometry();



	///////////////


	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-4.0f, R_yAnimateSlanting5, -22.9f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.0f, R_yAnimateSlanting2, -23.1f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting3, -23.2f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.5f, R_yAnimateSlanting6, -23.4f);

	drawGeometry();
	}
	if(rainExtra == 1)
	{ 

	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	////mat4 rotationMatrix = mat4::identity();

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.0f, R_yAnimateSlanting, -24.4f);

	drawGeometry();

	///////////////////////////////////////////////////CENTER BACK //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.0f, R_yAnimateSlanting, -24.4f);
	drawGeometry();

	///////////////////////////////////////////////////CENTER UP BACK Y=2.5//////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.3f, R_yAnimateSlanting2, -24.4f);

	drawGeometry();


	/////////////////////////////////////////////////////CENTER UP BACK Y=2.0//////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.8f, R_yAnimateSlanting3, -24.4f);

	drawGeometry();
	///////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.6f, R_yAnimateSlanting4, -24.12f);
	drawGeometry();

	/////////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-7.0f, R_yAnimateSlanting5, -24.6f);
	drawGeometry();


	///////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting6, -24.4f);

	drawGeometry();
	///////////////////////////////////////////////////CENTER UP BACK Y=1.0 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.5f, R_yAnimateSlanting7, -24.8f);
	drawGeometry();


	////////////////////////////////////////////////////Z FRONT DOWN///////////////////////////////////////////////////////////


	///////////////////////////////////////////////  FRONT Down y=-0.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.8f, R_yAnimateSlanting, -26.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Down y=-1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.8f, R_yAnimateSlanting2, -26.8f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Down y=-1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.8f, R_yAnimateSlanting3, -26.2f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Down y=-2.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.8f, R_yAnimateSlanting4, -26.0f);

	drawGeometry();










	/////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////CENTER UP BACK Y=0.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting4, -28.4f);
	drawGeometry();

	////////////////////////////////////////////////CENTER DOWN BACK Y=-0.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting6, -28.4f);
	drawGeometry();

	////////////////////////////////////////////////CENTER DOWN BACK Y=-1.0 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting2, -28.4f);
	drawGeometry();

	////////////////////////////////////////////////CENTER DOWN BACK Y=-1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting5, -28.4f);
	drawGeometry();
	////////////////////////////////////////////////CENTER DOWN BACK Y=-2.0//////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting4, -28.4f);
	drawGeometry();

	////////////////////////////////////////////////CENTER DOWN BACK Y=-2.5//////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting2, -28.4f);
	drawGeometry();


	/////////////////////////////////////////////////////////////Z FRONT UP///////////////////////////////////////////////////////////


	///////////////////////////////////////////////  FRONT Top y=0.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting, -30.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting5, -30.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting2, -30.4f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting4, -30.4f);

	drawGeometry();








	///////////////////////////////////////////////////////////Z FRONT DOWN///////////////////////////////////////////////////////////


	/////////////////////////////////////////////  FRONT Down y=-0.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-2.0f, R_yAnimateSlanting, -32.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Down y=-1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-2.0f, R_yAnimateSlanting2, -32.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Down y=-1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-2.0f, R_yAnimateSlanting4, -32.4f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Down y=-2.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-2.0f, R_yAnimateSlanting6, -32.4f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Down y=-2.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-2.0f, R_yAnimateSlanting3, -32.4f);

	drawGeometry();



	///////////////////////////////////////////////   X NEGATIVE SIDE //////////////////////////////////////////////////



	///////////////////////////////////////////// CENTER FRONT ///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.8f, R_yAnimateSlanting5, -34.4f);

	drawGeometry();

	/////////////////////////////////////////////////CENTER BACK //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.0f, R_yAnimateSlanting3, -34.4f);
	drawGeometry();

	/////////////////////////////////////////////////CENTER UP BACK Y=2.5//////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.0f, R_yAnimateSlanting6, -34.4f);

	drawGeometry();


	/////////////////////////////////////////////////CENTER UP BACK Y=2.0//////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.0f, R_yAnimateSlanting, -34.4f);

	drawGeometry();
	/////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.0f, R_yAnimateSlanting2, -34.4f);
	drawGeometry();

	/////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.0f, R_yAnimateSlanting4, -35.4f);
	drawGeometry();


	/////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.0f, R_yAnimateSlanting5, -35.4f);

	drawGeometry();
	/////////////////////////////////////////////////CENTER UP BACK Y=1.0 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.0f, R_yAnimateSlanting6, -35.4f);
	drawGeometry();


	/////////////////////////////////////////////////CENTER UP BACK Y=0.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.0f, R_yAnimateSlanting, -36.4f);
	drawGeometry();

	////////////////////////////////////////////////CENTER DOWN BACK Y=-0.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.0f, R_yAnimateSlanting6, -36.4f);
	drawGeometry();

	////////////////////////////////////////////////CENTER DOWN BACK Y=-1.0 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.0f, R_yAnimateSlanting, -36.4f);
	drawGeometry();

	////////////////////////////////////////////////CENTER DOWN BACK Y=-1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.0f, R_yAnimateSlanting5, -36.4f);
	drawGeometry();
	////////////////////////////////////////////////CENTER DOWN BACK Y=-2.0//////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.0f, R_yAnimateSlanting3, -36.4f);
	drawGeometry();

	////////////////////////////////////////////////CENTER DOWN BACK Y=-2.5//////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.0f, R_yAnimateSlanting2, -36.4f);
	drawGeometry();


	///////////////////////////////////////////////////////////Z FRONT UP///////////////////////////////////////////////////////////


	/////////////////////////////////////////////  FRONT Top y=0.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.8f, R_yAnimateSlanting4, -38.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.8f, R_yAnimateSlanting2, -38.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.8f, R_yAnimateSlanting5, -38.4f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.8f, R_yAnimateSlanting, -38.4f);

	drawGeometry();


	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.8f, R_yAnimateSlanting3, -38.4f);

	drawGeometry();
	///////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.6f, R_yAnimateSlanting4, -38.4f);
	drawGeometry();

	/////////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-7.0f, R_yAnimateSlanting5, -38.4f);
	drawGeometry();


	///////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting6, -38.4f);

	drawGeometry();
	///////////////////////////////////////////////////CENTER UP BACK Y=1.0 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.5f, R_yAnimateSlanting7, -40.4f);
	drawGeometry();


	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.8f, R_yAnimateSlanting3, 13.0f);

	drawGeometry();
	///////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.6f, R_yAnimateSlanting4, -40.4f);
	drawGeometry();

	/////////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-7.0f, R_yAnimateSlanting5, -40.4f);
	drawGeometry();


	///////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting6, -40.4f);

	drawGeometry();
	///////////////////////////////////////////////////CENTER UP BACK Y=1.0 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.5f, R_yAnimateSlanting7, -40.4f);
	drawGeometry();



	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.8f, R_yAnimateSlanting3, -40.4f);

	drawGeometry();
	///////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.6f, R_yAnimateSlanting4, -42.4f);
	drawGeometry();

	/////////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-7.0f, R_yAnimateSlanting5, -42.4f);
	drawGeometry();


	///////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting6, -42.4f);

	drawGeometry();
	///////////////////////////////////////////////////CENTER UP BACK Y=1.0 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.5f, R_yAnimateSlanting7, -42.4f);
	drawGeometry();




	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-9.8f, R_yAnimateSlanting2, -42.4f);

	drawGeometry();
	///////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-7.6f, R_yAnimateSlanting6, -44.4f);
	drawGeometry();

	/////////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting4, -44.4f);
	drawGeometry();


	///////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting5, -44.4f);

	drawGeometry();
	///////////////////////////////////////////////////CENTER UP BACK Y=1.0 //////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-2.5f, R_yAnimateSlanting6, -44.4f);
	drawGeometry();


	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.0f, R_yAnimateSlanting, -44.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.0f, R_yAnimateSlanting5, -44.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.0f, R_yAnimateSlanting2, -44.4f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting4, -46.4f);

	drawGeometry();






	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(6.0f, R_yAnimateSlanting, -46.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-4.0f, R_yAnimateSlanting5, -46.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-9.0f, R_yAnimateSlanting2, -46.4f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-2.0f, R_yAnimateSlanting4, -46.4f);

	drawGeometry();



	/////////////////////////////////////




	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.0f, R_yAnimateSlanting5, -48.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting, -48.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-0.6f, R_yAnimateSlanting4, -48.4f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-2.2f, R_yAnimateSlanting2, -48.4f);

	drawGeometry();
	/////////////////////////////////////
	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.98f, R_yAnimateSlanting2, -50.4f);

	drawGeometry();
	/////////////////////////////////////
	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(12.0f, R_yAnimateSlanting2, -50.4f);

	drawGeometry();
	/////////////////////////////////////



	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(0.0f, R_yAnimateSlanting2, -50.4f);

	drawGeometry();
	/////////////////////////////////////
	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(3.0f, R_yAnimateSlanting2, -50.4f);

	drawGeometry();
	/////////////////////////////////////
	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(9.0f, R_yAnimateSlanting2, -50.4f);

	drawGeometry();
	/////////////////////////////////////

	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(0.9f, R_yAnimateSlanting5, -50.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(8.0f, R_yAnimateSlanting3, -52.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(0.0f, R_yAnimateSlanting, -52.4f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(3.0f, R_yAnimateSlanting2, -52.4f);

	drawGeometry();

	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-2.0f, R_yAnimateSlanting3, -52.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.0f, R_yAnimateSlanting4, -52.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.8f, R_yAnimateSlanting5, -52.4f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-0.2f, R_yAnimateSlanting6, -52.4f);

	drawGeometry();

	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.0f, R_yAnimateSlanting5, -54.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.0f, R_yAnimateSlanting4, -54.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-0.8f, R_yAnimateSlanting, -54.8f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(7.8f, R_yAnimateSlanting4, -54.4f);

	drawGeometry();








	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(6.0f, R_yAnimateSlanting, -54.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(0.0f, R_yAnimateSlanting5, -54.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.0f, R_yAnimateSlanting3, -56.4f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(3.8f, R_yAnimateSlanting5, -56.4f);

	drawGeometry();

	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(8.0f, R_yAnimateSlanting6, -56.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(6.0f, R_yAnimateSlanting, -56.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(4.0f, R_yAnimateSlanting5, -56.4f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(2.0f, R_yAnimateSlanting2, -58.4f);

	drawGeometry();

	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(0.0f, R_yAnimateSlanting, -58.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.0f, R_yAnimateSlanting2, -58.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-16.0f, R_yAnimateSlanting3, -58.4f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.8f, R_yAnimateSlanting4, -58.4f);

	drawGeometry();




	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(8.2f, R_yAnimateSlanting5, -60.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(3.6f, R_yAnimateSlanting6, -60.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-1.6f, R_yAnimateSlanting2, -60.4f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-2.8f, R_yAnimateSlanting4, -60.4f);

	drawGeometry();


	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(4.0f, R_yAnimateSlanting3, -60.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(8.0f, R_yAnimateSlanting4, -60.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(1.0f, R_yAnimateSlanting3, -60.4f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(3.0f, R_yAnimateSlanting6, -60.4f);

	drawGeometry();


	/////////////////
	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.0f, R_yAnimateSlanting6, -60.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(7.0f, R_yAnimateSlanting6, -62.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-2.0f, R_yAnimateSlanting, -62.4f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(4.0f, R_yAnimateSlanting3, -62.4f);

	drawGeometry();



	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-0.5f, R_yAnimateSlanting5, -62.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.0f, R_yAnimateSlanting6, -64.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(3.9f, R_yAnimateSlanting2, -64.4f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(10.0f, R_yAnimateSlanting4, -64.4f);

	drawGeometry();




	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.0f, R_yAnimateSlanting, -64.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-9.0f, R_yAnimateSlanting5, -64.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.0f, R_yAnimateSlanting2, -64.4f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-2.0f, R_yAnimateSlanting4, -66.4f);

	drawGeometry();




	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(3.0f, R_yAnimateSlanting2, -66.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(5.0f, R_yAnimateSlanting6, -66.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(6.0f, R_yAnimateSlanting3, -66.4f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-2.0f, R_yAnimateSlanting, -68.4f);

	drawGeometry();





	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-4.2f, R_yAnimateSlanting5, -68.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.0f, R_yAnimateSlanting2, -66.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.8f, R_yAnimateSlanting3, -68.4f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting4, -68.4f);

	drawGeometry();





	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(16.0f, R_yAnimateSlanting6, -70.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(6.0f, R_yAnimateSlanting, -70.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.0f, R_yAnimateSlanting4, -70.4f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-2.0f, R_yAnimateSlanting2, -72.4f);

	drawGeometry();




	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-6.0f, R_yAnimateSlanting6, -72.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.0f, R_yAnimateSlanting, -72.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-9.0f, R_yAnimateSlanting2, -72.4f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-1.0f, R_yAnimateSlanting5, -72.4f);

	drawGeometry();



	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.0f, R_yAnimateSlanting, -72.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-4.0f, R_yAnimateSlanting5, -72.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.0f, R_yAnimateSlanting2, -72.4f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-5.5f, R_yAnimateSlanting4, -72.4f);

	drawGeometry();



	///////////////


	/////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-8.0f, R_yAnimateSlanting5, -74.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(9.0f, R_yAnimateSlanting2, -72.4f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(-3.0f, R_yAnimateSlanting3, -72.4f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	R_modelMatrix = mat4::identity();
	R_modelMatrix = translate(3.0f, R_yAnimateSlanting6, -74.4f);

	drawGeometry();

	}

	glUseProgram(0);



	R_yAnimateSlanting = R_yAnimateSlanting - rainSpeed;
	if (R_yAnimateSlanting <= 5.0f)
	{
		R_yAnimateSlanting = 30.0f;
	}

	R_yAnimateSlanting2 = R_yAnimateSlanting2 - rainSpeed2;
	if (R_yAnimateSlanting2 <= 4.0f)
	{
		R_yAnimateSlanting2 = 30.1f;
	}


	R_yAnimateSlanting3 = R_yAnimateSlanting3 - rainSpeed5;
	if (R_yAnimateSlanting3 <= 3.5f)
	{
		R_yAnimateSlanting3 = 30.2f;
	}


	R_yAnimateSlanting4 = R_yAnimateSlanting4 - rainSpeed3;
	if (R_yAnimateSlanting4 <= 2.0f)
	{
		R_yAnimateSlanting4 = 30.3f;
	}

	//fprintf(gpLFile, "ySlanting5 %f\n", yAnimateSlanting5);
	R_yAnimateSlanting5 = R_yAnimateSlanting5 - rainSpeed1;
	if (R_yAnimateSlanting5 <= 1.5f)
	{

		R_yAnimateSlanting5 = 30.4f;

	}

	R_yAnimateSlanting6 = R_yAnimateSlanting6 - rainSpeed4;
	if (R_yAnimateSlanting6 <= 2.0f)
	{
		R_yAnimateSlanting6 = 30.5f;
	}

	R_yAnimateSlanting7 = R_yAnimateSlanting7 - rainSpeed;
	if (R_yAnimateSlanting7 <= 1.5f)
	{
		R_yAnimateSlanting7 = 31.6f;
	}




}
void drawGeometry()
{

	R_modelviewMatrix = R_viewMatrix * R_modelMatrix;
	R_modelviewProjectionMatrix = perspectiveProjectionMatrix * R_modelviewMatrix;	//ORDER IS IMPORTANT
	R_viewMatrix = lookat(vec3(xView, 12.0f, zView), vec3(xView, 12.0f, -300.0f), vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(R_gMVPUniform, 1, GL_FALSE, R_modelviewProjectionMatrix);

	glUniformMatrix4fv(R_modelUniform, 1, GL_FALSE, R_modelMatrix);
	glUniformMatrix4fv(R_viewUniform, 1, GL_FALSE, R_viewMatrix);
	glUniformMatrix4fv(R_projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	//projectionMatrix = gPerspectiveProjectionMatrix * modelMatrix;
	/*glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureKundali);
	glUniform1i(gTextureSamplerUniform, 0);*/
	glBindVertexArray(R_gVao2);

	glDrawArrays(GL_POINTS, 0, 4);
	//glDrawArraysInstanced(GL_POINTS, 0, 4, 100 * 100);
	//**** unbind vao ****
	glBindVertexArray(0);

}
void display(void)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	GLfloat checkerBoardVertices[12];
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);





	glDisable(GL_BLEND);
		glUseProgram(Flowers_gShaderProgramObject);

	if (gbLighting == 1)
	{
		//fprintf(gpLogFile, "gbLighting is TRUE!\n");
		glUniform1i(Flowers_LKeyPressed, gbLighting);
		glUniform3fv(Flowers_LDUniform, 1, lightDiffuse);
		glUniform3fv(Flowers_LAUniform, 1, lightAmbient);
		glUniform3fv(Flowers_LSUniform, 1, lightSpecular);

		glUniform3fv(Flowers_KDUniform, 1, materialDiffuse);
		glUniform3fv(Flowers_KAUniform, 1, materialAmbient);
		glUniform3fv(Flowers_KSUniform, 1, materialSpecular);
		glUniform1f(Flowers_MaterialShininessUniform, materialShininess);
		glUniform4fv(Flowers_LightPositionUniform, 1, lightPosition);
	}
	else
	{
		//fprintf(gpLogFile, "gbLighting is FALSE!\n");
		glUniform1i(Flowers_LKeyPressed, 0);
	}
//////	//declaration of matrices
	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 modelViewMatrix;
	mat4 RotationMatrix;
	mat4 TranslateMatrix;
	mat4 scaleMatrix;
//////	// intialize above matrices to identity
//////
////////	////////  RECTANGLE //////////////
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	TranslateMatrix = translate(-5.0f, 3.0f, -20.0f);
	scaleMatrix = scale(0.3f, 0.2f, 0.3f);
	TranslateMatrix *= scaleMatrix;
//
//	RotationMatrix = rotate(-90.0f, 1.0f, 0.0f, 0.0f);
//	//TranslateMatrix *= RotationMatrix; 
//
//	RotationMatrix = rotate(angle1, 0.0f, 1.0f, 0.0f);
//	TranslateMatrix *= RotationMatrix;
	viewMatrix = lookat(vec3(xView, 12.0f, zView), vec3(xView, 12.0f, -300.0f), vec3(0.0f, 1.0f, 0.0f));
////	//	RotationMatrix = rotate(angleRectangle, 1.0f, 0.0f, 0.0f);
////	//RotationMatrix = rotate(angleRectangle, angleRectangle, angleRectangle);
////	// perform necessary transformations
	modelMatrix = TranslateMatrix;

	glUniformMatrix4fv(Flowers_modelUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(Flowers_ViewUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(Flowers_projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glUniform1f(glGetUniformLocation(Flowers_gShaderProgramObject, "t"), t);
	
	myModelFlowers->Draw(Flowers_gShaderProgramObject);
   glUseProgram(0);
 ///////////////////////////////  ////// Lavender /////////////////////////////////
   glUseProgram(Flowers_gShaderProgramObject);

   if (gbLighting == 1)
   {
	   //fprintf(gpLogFile, "gbLighting is TRUE!\n");
	   glUniform1i(Flowers_LKeyPressed, gbLighting);
	   glUniform3fv(Flowers_LDUniform, 1, lightDiffuse);
	   glUniform3fv(Flowers_LAUniform, 1, lightAmbient);
	   glUniform3fv(Flowers_LSUniform, 1, lightSpecular);

	   glUniform3fv(Flowers_KDUniform, 1, materialDiffuse);
	   glUniform3fv(Flowers_KAUniform, 1, materialAmbient);
	   glUniform3fv(Flowers_KSUniform, 1, materialSpecular);
	   glUniform1f(Flowers_MaterialShininessUniform, materialShininess);
	   glUniform4fv(Flowers_LightPositionUniform, 1, lightPosition);
   }
   else
   {
	   //fprintf(gpLogFile, "gbLighting is FALSE!\n");
	   glUniform1i(Flowers_LKeyPressed, 0);
   }
	

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	TranslateMatrix = translate(.0f,6.0f, -20.0f);
	scaleMatrix = scale(15.8f, 15.8f, 15.8f);
	TranslateMatrix *= scaleMatrix;
	TranslateMatrix *= rotate(270.0f, 1.0f, 0.0f, 0.0f);
	//
	//	RotationMatrix = rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	//	//TranslateMatrix *= RotationMatrix; 
	//
	//	RotationMatrix = rotate(angle1, 0.0f, 1.0f, 0.0f);
	//	TranslateMatrix *= RotationMatrix;
	viewMatrix = lookat(vec3(xView, 12.0f, zView), vec3(xView, 12.0f, -300.0f), vec3(0.0f, 1.0f, 0.0f));
	//	//	RotationMatrix = rotate(angleRectangle, 1.0f, 0.0f, 0.0f);
	//	//RotationMatrix = rotate(angleRectangle, angleRectangle, angleRectangle);
	//	// perform necessary transformations
	modelMatrix = TranslateMatrix;

	glUniformMatrix4fv(Flowers_modelUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(Flowers_ViewUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(Flowers_projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
//	glUniform1f(glGetUniformLocation(Flowers_gShaderProgramObject, "t"), t);
	//t += 0.006f;

	
	myModelLavender->Draw(Flowers_gShaderProgramObject);






	glUseProgram(0);
//
//
//
//
//
//
////	// use shader program








	glUseProgram(gShaderProgramObject);

	if (gbLighting == 1)
	{
		//fprintf(gpLogFile, "gbLighting is TRUE!\n");
		glUniform1i(LKeyPressed, gbLighting);
		glUniform3fv(LDUniform, 1, lightDiffuse);
		glUniform3fv(LAUniform, 1, lightAmbient);
		glUniform3fv(LSUniform, 1, lightSpecular);

		glUniform3fv(KDUniform, 1, materialDiffuse);
		glUniform3fv(KAUniform, 1, materialAmbient);
		glUniform3fv(KSUniform, 1, materialSpecular);
		glUniform1f(MaterialShininessUniform, materialShininess);
		glUniform4fv(LightPositionUniform, 1, lightPosition);
	}
	else
	{
		//fprintf(gpLogFile, "gbLighting is FALSE!\n");
		glUniform1i(LKeyPressed, 0);
	}
//	//declaration of matrices
	/*mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 modelViewMatrix;

	mat4 RotationMatrix;
	mat4 TranslateMatrix;
	mat4 scaleMatrix;*/
//	// intialize above matrices to identity
//
////	////////  RECTANGLE //////////////
	modelMatrix = mat4::identity();
	//viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	TranslateMatrix = translate(0.0f, 5.0f, -05.0f);
	scaleMatrix = scale(0.003f, 0.003f, 0.003f);
	TranslateMatrix *= scaleMatrix;
//
//	RotationMatrix = rotate(-90.0f, 1.0f, 0.0f, 0.0f);
//	//TranslateMatrix *= RotationMatrix; 
//
//	RotationMatrix = rotate(angle1, 0.0f, 1.0f, 0.0f);
//	TranslateMatrix *= RotationMatrix;
	//viewMatrix = lookat(vec3(xView, 12.0f, zView), vec3(xView, 12.0f, -300.0f), vec3(0.0f, 1.0f, 0.0f));
//	//	RotationMatrix = rotate(angleRectangle, 1.0f, 0.0f, 0.0f);
//	//RotationMatrix = rotate(angleRectangle, angleRectangle, angleRectangle);
//	// perform necessary transformations
	modelMatrix = TranslateMatrix;

	glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glUniform1f(glGetUniformLocation(gShaderProgramObject, "t"), t);
	t += 0.006f;

	
	glUniform1i(gTextureSamplerUniform, 2);
	

	myModel->Draw(gShaderProgramObject);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
////	
////
////
////
////
////	/////////// TREES INSTANCING ///////////
////
////	//////	////////  RECTANGLE //////////////
////	modelMatrix = mat4::identity();
////	viewMatrix = mat4::identity();
////	scaleMatrix = mat4::identity();
////	RotationMatrix = mat4::identity();
////	TranslateMatrix = mat4::identity();
////	TranslateMatrix = translate(0.0f, 6.0f, 5.0f);
////	scaleMatrix = scale(0.003f, 0.003f, 0.003f);
////	TranslateMatrix *= scaleMatrix;
////	
////	viewMatrix = lookat(vec3(xView, 12.0f, zView), vec3(xView, 12.0f, -300.0f), vec3(0.0f, 1.0f, 0.0f));
////	
////	modelMatrix = TranslateMatrix;
////
////	glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
////	glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
////	glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
////
////	glUniform1i(gTextureSamplerUniform, 2);
////	
////
////	//myModel->Draw(gShaderProgramObject);
////	glUseProgram(0);
////
////
////
////
////
////
////
////
////
////
////
////
////////
	glUseProgram(T_gShaderProgramObject);

	modelMatrix = mat4::identity();
	//viewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	modelViewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	//viewMatrix = lookat(vec3(xView, 5.0f, zView), vec3(xView, 2.0f, -100.0f), vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = translate(-25.0f, 0.0f, -50.0f);
	modelViewMatrix = viewMatrix * modelMatrix;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, T_);
	glUniform1i(T_samplerUniform, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, T_smileyTexture);
	glUniform1i(T_tex_color, 1);
	glUniformMatrix4fv(T_mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniformMatrix4fv(T_modelViewUniform, 1, GL_FALSE, modelViewMatrix);
	glUniformMatrix4fv(T_projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glUniform1f(T_dmap_depthUniform, 10.5f);
	//glUniform1i(moduloUniform, modulo);

	glBindVertexArray(T_vao);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glPatchParameteri(GL_PATCH_VERTICES, 4);
	
	glDrawArraysInstanced(GL_PATCHES, 0, 4, modulo * modulo);
	
	glBindVertexArray(0);
	glUseProgram(0);
////////
////////	////////////////////////////////////////////////
	glUseProgram(G_gShaderProgramObject);
	modelMatrix = mat4::identity();
	//viewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	modelViewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	modelMatrix = translate(0.0f, -7.0f, 0.0f);
	vmath::vec3 vEye(xView, 10.0f, zView);
	//viewMatrix = lookat(vec3(xView, 10.0f, zView), vec3(xView, 2.0f, -100.0f), vec3(0.0f, 1.0f, 0.0f));
	glUniform1i(G_gSampler, 6);
	glUniformMatrix4fv(G_projectionUniform, 1, GL_FALSE, (GLfloat *)&perspectiveProjectionMatrix);
	glUniformMatrix4fv(G_viewUniform, 1, GL_FALSE, (GLfloat *)&viewMatrix);
	//glUniformMatrix4fv(G_modelUniform, 1, GL_FALSE, (GLfloat *)&(glm::mat4(1.0)));
	glUniformMatrix4fv(G_modelUniform, 1, GL_FALSE, modelMatrix);
	glUniform1f(fTimePassedUniform, time);
	glUniform1f(grassHeight_Uniform, grassHeight);
	time += 0.002f;
	glBindVertexArray(uiGrassVAO);
	glDrawArrays(GL_POINTS, 0, iNumGrassTriangles);
	//glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
	
	glUseProgram(0);
//////	///////////////////////////////////////////////////////
//////
////////
////
////	if (Noise == 1)
////	{
////
////		glUseProgram(N_gShaderProgramObject);
////		//viewMatrix = mat4::identity();
////		modelMatrix = mat4::identity();
////		modelViewProjectionMatrix = mat4::identity();
////		modelViewMatrix = mat4::identity();
////		modelMatrix = translate(xView + 1.2f, 12.0f, zView - 1.0f);
////		modelMatrix *= scale(5.0f, 3.0f, 2.0f);
////
////		modelViewMatrix = viewMatrix * modelMatrix;
////		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
////
////		glUniformMatrix4fv(N_modelViewUniform, 1, GL_FALSE, modelViewMatrix);
////		glUniformMatrix4fv(N_mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
////		glUniform3f(N_LightPos, 0.0f, 0.0f, 4.0f);
////		glUniform1f(N_Scale, 1.5f);
////		glUniform4f(N_SkyColor, 0.0f, 0.0f, 0.0f, 0.1f);
////		glUniform4f(N_CloudColor, 0.8f, 0.8f, 0.8f, 0.6f);
////		glUniform1f(N_animUniform, N_anim);
////
////		glEnable(GL_BLEND);
////		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
////		/*Texture Calls*/
////		glActiveTexture(GL_TEXTURE7);
////		//Here,GL_TEXTURE0 gets matched to AMC_TTRIBUTE_TEXCOORD0 
////		//For one geometry we can give 80 textures.
////
////		glBindTexture(GL_TEXTURE_3D, N_texName);
////		//now give it to Fragment Shader
////		glUniform1i(N_samplerUniform, 7);
////		glBindVertexArray(N_vao);
////		checkerBoardVertices[0] = -2.0f;
////		checkerBoardVertices[1] = -1.0f;
////		checkerBoardVertices[2] = 0.0f;
////
////		checkerBoardVertices[3] = -2.0f;
////		checkerBoardVertices[4] = 1.0f;
////		checkerBoardVertices[5] = 0.0f;
////
////		checkerBoardVertices[6] = 0.0f;
////		checkerBoardVertices[7] = 1.0f;
////		checkerBoardVertices[8] = 0.0f;
////
////		checkerBoardVertices[9] = 0.0f;
////		checkerBoardVertices[10] = -1.0f;
////		checkerBoardVertices[11] = 0.0f;
////
////		glBindBuffer(GL_ARRAY_BUFFER, N_vbo);
////		glBufferData(GL_ARRAY_BUFFER, sizeof(checkerBoardVertices), checkerBoardVertices, GL_DYNAMIC_DRAW);
////		glBindBuffer(GL_ARRAY_BUFFER, 0);
////		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
////
////		glBindVertexArray(0);
////
////
////		glUseProgram(0);
////	
////	N_anim += 0.002f;
////	}
////	
////	displayRain();
////	
	if (light >= 1)
	{
		//glEnable(GL_BLEND);
		glBindTexture(GL_TEXTURE_2D, 0);

		displayLightning();
		
		fprintf(gpLogFile, "Basically OUT\n");
		if (light >= 1)
		{

			light = light + 1;
		}
		 if(light == 20)
		{
			fprintf(gpLogFile, "Basically INNNNNNNNN\n");
			light = 0;
		}
	}
	SwapBuffers(ghdc); 
	//angle1 += 0.2f;

}
void displayLightning()
{
	//glBindTexture(GL_TEXTURE_2D, 0);
	GLfloat vertices[8];
	 const GLfloat green[] = { 0.0f,1.0f,0.0f,1.0f };
	 const GLfloat blue[] = { 0.4f, 0.5f, 0.6f, 1.0f };
	 const GLfloat L_black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
//	//static const GLfloat blue[] = { 0.4f, 0.5f, 0.6f, 1.0f };
	static const GLfloat L_one = 1.0f;
//
//	//if (!paused)
//	//	total_time += (currentTime - last_time);
//	//last_time = currentTime;
//	//float t = (float)total_time;
//
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	 //use shader program
//
//
//	/*glViewport(0, 0, T_gWidth, T_gHeight);*/
//
	glBindFramebuffer(GL_FRAMEBUFFER, L_render_fbo);
	glViewport(0, 0, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);
	glClearBufferfv(GL_COLOR, 0, L_black);
	glClearBufferfv(GL_COLOR, 1, L_black);
	glClearBufferfv(GL_DEPTH, 0, &L_one);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);




	//static GLfloat gAngle;
	/////////////// ******* Electrical Lines---- Tessellated *********** ////////////////////

	glLineWidth(20.0);
	glUseProgram(L_gShaderProgramObject);

	vmath::mat4 mat_proj;
	vmath::mat4 mat_view;
	vmath::mat4 mat_model;
	vmath::mat4 modelViewProjectionMatrix;
	mat_view = mat4::identity();
	mat_proj = mat4::identity();
	mat_model = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();


	mat_model = translate(-1.0f, 0.0f, -4.0f);
	//mat_model *= rotate(gAngle, 1.0f, 1.0f, 1.0f);
//mat_model *= scale(0.04f, 5.2f, 0.2f);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, L_ubo_material);
	glLineWidth(5.0f);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_min, L_bloom_thresh_min);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_max, L_bloom_thresh_max);

	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;
	//exposure = 5.0f;
	glUniformMatrix4fv(L_ViewUniform, 1, GL_FALSE, mat_view);
	glUniformMatrix4fv(L_projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glUniformMatrix4fv(L_modelUniform, 1, GL_FALSE, mat_model);
	glUniformMatrix4fv(L_mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	L_gNumberOfLineSegments = 5;
	glUniform1i(L_gNumberOfSegmentUniform, L_gNumberOfLineSegments);
	glUniform1f(L_b0Uniform, 4.0f);
	glUniform1f(L_b1Uniform, 5.0f);
	glUniform1f(L_lengthUniform, 2.5f);
	glUniform1i(L_gNumberOfStripsUniform, 1);

	glBindVertexArray(L_vao_Sphere);

	vertices[0] = -0.5f;
	vertices[1] = 1.0f;
	vertices[2] = 0.5f;
	vertices[3] = -1.5f;
	vertices[4] = 1.5f;
	vertices[5] = 1.5f;
	vertices[6] = 2.5f;
	vertices[7] = -1.0f;

	glBindBuffer(GL_ARRAY_BUFFER, L_vbo_position_Sphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawArrays(GL_PATCHES, 0, 4);
	glBindVertexArray(0);






	////////  LINE 2 ///////////
	glLineWidth(2.0f);
	//exposure = 1.0f;
	mat_view = mat4::identity();
	mat_proj = mat4::identity();
	mat_model = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();


	mat_model = translate(-0.4f, 0.1f, -4.0f);
	//mat_model *= rotate(gAngle, 1.0f, 1.0f, 1.0f);
	//mat_model *= scale(0.04f, 5.2f, 0.2f);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, L_ubo_material);
	glLineWidth(3.0f);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_min, L_bloom_thresh_min);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_max, L_bloom_thresh_max);

	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;

	glUniformMatrix4fv(L_ViewUniform, 1, GL_FALSE, mat_view);
	glUniformMatrix4fv(L_projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glUniformMatrix4fv(L_modelUniform, 1, GL_FALSE, mat_model);
	glUniformMatrix4fv(L_mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	L_gNumberOfLineSegments = 5;
	glUniform1i(L_gNumberOfSegmentUniform, L_gNumberOfLineSegments);
	glUniform1f(L_b0Uniform, 4.5f);
	glUniform1f(L_b1Uniform, 2.0f);
	glUniform1f(L_lengthUniform, 0.002f);
	glUniform1i(L_gNumberOfStripsUniform, 1);

	glBindVertexArray(L_vao_Sphere);

	vertices[0] = 1.0f;
	vertices[1] = 2.0f;
	vertices[2] = 0.5f;
	vertices[3] = -1.5f;
	vertices[4] = 1.5f;
	vertices[5] = 1.5f;
	vertices[6] = 2.5f;
	vertices[7] = -2.0f;

	glBindBuffer(GL_ARRAY_BUFFER, L_vbo_position_Sphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawArrays(GL_PATCHES, 0, 4);
	glBindVertexArray(0);


	//////New added line

	glLineWidth(2.0f);
	//exposure = 1.0f;
	mat_view = mat4::identity();
	mat_proj = mat4::identity();
	mat_model = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();


	mat_model = translate(-0.4f, 0.1f, -4.0f);
	//mat_model *= rotate(gAngle, 1.0f, 1.0f, 1.0f);
	//mat_model *= scale(0.04f, 5.2f, 0.2f);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, L_ubo_material);
	glLineWidth(3.0f);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_min, L_bloom_thresh_min);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_max, L_bloom_thresh_max);

	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;

	glUniformMatrix4fv(L_ViewUniform, 1, GL_FALSE, mat_view);
	glUniformMatrix4fv(L_projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glUniformMatrix4fv(L_modelUniform, 1, GL_FALSE, mat_model);
	glUniformMatrix4fv(L_mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	L_gNumberOfLineSegments = 10;
	glUniform1i(L_gNumberOfSegmentUniform, L_gNumberOfLineSegments);
	glUniform1f(L_b0Uniform, 6.5f);
	glUniform1f(L_b1Uniform, 2.0f);
	glUniform1f(L_lengthUniform, 0.002f);
	glUniform1i(L_gNumberOfStripsUniform, 1);

	glBindVertexArray(L_vao_Sphere);

	vertices[0] = 1.0f;
	vertices[1] = 2.0f;
	vertices[2] = 0.5f;
	vertices[3] = -1.5f;
	vertices[4] = 1.5f;
	vertices[5] = 1.5f;
	vertices[6] = 5.5f;
	vertices[7] = -2.0f;

	glBindBuffer(GL_ARRAY_BUFFER, L_vbo_position_Sphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawArrays(GL_PATCHES, 0, 4);
	glBindVertexArray(0);

	//////
	////////  LINE 3 ///////////
	glLineWidth(2.0f);
	//exposure = 1.0f;
	mat_view = mat4::identity();
	mat_proj = mat4::identity();
	mat_model = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();


	mat_model = translate(-1.85f, -0.1f, -4.0f);
	//mat_model *= rotate(gAngle, 1.0f, 1.0f, 1.0f);
//mat_model *= scale(0.04f, 5.2f, 0.2f);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, L_ubo_material);
	glLineWidth(3.0f);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_min, L_bloom_thresh_min);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_max, L_bloom_thresh_max);

	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;

	glUniformMatrix4fv(L_ViewUniform, 1, GL_FALSE, mat_view);
	glUniformMatrix4fv(L_projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glUniformMatrix4fv(L_modelUniform, 1, GL_FALSE, mat_model);
	glUniformMatrix4fv(L_mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	L_gNumberOfLineSegments = 8;
	glUniform1i(L_gNumberOfSegmentUniform, L_gNumberOfLineSegments);
	glUniform1f(L_b0Uniform, 5.5f);
	glUniform1f(L_b1Uniform, 2.0f);
	glUniform1f(L_lengthUniform, 2.9f);
	glUniform1i(L_gNumberOfStripsUniform, 1);

	glBindVertexArray(L_vao_Sphere);


	vertices[0] = 0.0f;
	vertices[1] = -1.0f;
	vertices[2] = -0.5f;
	vertices[3] = 1.5f;
	vertices[4] = 1.0f;
	vertices[5] = -0.5f;
	vertices[6] = 0.5f;
	vertices[7] = 1.0f;

	glBindBuffer(GL_ARRAY_BUFFER, L_vbo_position_Sphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawArrays(GL_PATCHES, 0, 4);
	glBindVertexArray(0);




	////////  LINE 4 ///////////
	glLineWidth(2.0f);
	//exposure = 1.0f;
	mat_view = mat4::identity();
	mat_proj = mat4::identity();
	mat_model = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();


	mat_model = translate(-1.0f, 0.45f, -4.0f);
	//mat_model *= rotate(gAngle, 1.0f, 1.0f, 1.0f);
//mat_model *= scale(0.04f, 5.2f, 0.2f);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, L_ubo_material);
	glLineWidth(3.0f);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_min, L_bloom_thresh_min);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_max, L_bloom_thresh_max);

	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;

	glUniformMatrix4fv(L_ViewUniform, 1, GL_FALSE, mat_view);
	glUniformMatrix4fv(L_projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glUniformMatrix4fv(L_modelUniform, 1, GL_FALSE, mat_model);
	glUniformMatrix4fv(L_mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	L_gNumberOfLineSegments = 6;
	glUniform1i(L_gNumberOfSegmentUniform, L_gNumberOfLineSegments);
	glUniform1f(L_b0Uniform, 1.8f);
	glUniform1f(L_b1Uniform, 6.0f);
	glUniform1f(L_lengthUniform, 0.002f);
	glUniform1i(L_gNumberOfStripsUniform, 1);

	glBindVertexArray(L_vao_Sphere);

	vertices[0] = -1.0f;
	vertices[1] = 2.0f;
	vertices[2] = 0.5f;
	vertices[3] = -1.5f;
	vertices[4] = 1.5f;
	vertices[5] = 1.5f;
	vertices[6] = 2.5f;
	vertices[7] = -2.0f;

	glBindBuffer(GL_ARRAY_BUFFER, L_vbo_position_Sphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawArrays(GL_PATCHES, 0, 4);
	glBindVertexArray(0);


	////////  LINE 5  ///////////
	glLineWidth(2.0f);
	//exposure = 1.0f;
	mat_view = mat4::identity();
	mat_proj = mat4::identity();
	mat_model = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();


	mat_model = translate(-1.0f, 0.45f, -4.0f);
	//mat_model *= rotate(gAngle, 1.0f, 1.0f, 1.0f);
//mat_model *= scale(0.04f, 5.2f, 0.2f);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, L_ubo_material);
	glLineWidth(3.0f);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_min, L_bloom_thresh_min);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_max, L_bloom_thresh_max);

	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;

	glUniformMatrix4fv(L_ViewUniform, 1, GL_FALSE, mat_view);
	glUniformMatrix4fv(L_projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glUniformMatrix4fv(L_modelUniform, 1, GL_FALSE, mat_model);
	glUniformMatrix4fv(L_mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	L_gNumberOfLineSegments = 8;
	glUniform1i(L_gNumberOfSegmentUniform, L_gNumberOfLineSegments);
	glUniform1f(L_b0Uniform, 1.8f);
	glUniform1f(L_b1Uniform, 2.0f);
	glUniform1f(L_lengthUniform, 0.002f);
	glUniform1i(L_gNumberOfStripsUniform, 1);

	glBindVertexArray(L_vao_Sphere);

	vertices[0] = -1.0f;
	vertices[1] = 2.0f;
	vertices[2] = 0.5f;
	vertices[3] = -1.5f;
	vertices[4] = 1.5f;
	vertices[5] = 1.5f;
	vertices[6] = 2.5f;
	vertices[7] = -2.5f;

	glBindBuffer(GL_ARRAY_BUFFER, L_vbo_position_Sphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawArrays(GL_PATCHES, 0, 4);
	glBindVertexArray(0);

	////////  LINE 5 2 ///////////
	glLineWidth(2.0f);
	//exposure = 1.0f;
	mat_view = mat4::identity();
	mat_proj = mat4::identity();
	mat_model = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();


	mat_model = translate(-1.0f, 0.45f, -4.0f);
	//mat_model *= rotate(gAngle, 1.0f, 1.0f, 1.0f);
//mat_model *= scale(0.04f, 5.2f, 0.2f);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, L_ubo_material);
	glLineWidth(3.0f);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_min, L_bloom_thresh_min);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_max, L_bloom_thresh_max);

	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;

	glUniformMatrix4fv(L_ViewUniform, 1, GL_FALSE, mat_view);
	glUniformMatrix4fv(L_projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glUniformMatrix4fv(L_modelUniform, 1, GL_FALSE, mat_model);
	glUniformMatrix4fv(L_mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	L_gNumberOfLineSegments = 8;
	glUniform1i(L_gNumberOfSegmentUniform, L_gNumberOfLineSegments);
	glUniform1f(L_b0Uniform, 0.8f);
	glUniform1f(L_b1Uniform, 4.0f);
	glUniform1f(L_lengthUniform, 0.002f);
	glUniform1i(L_gNumberOfStripsUniform, 1);

	glBindVertexArray(L_vao_Sphere);

	vertices[0] = -1.0f;
	vertices[1] = 2.0f;
	vertices[2] = 0.5f;
	vertices[3] = -1.5f;
	vertices[4] = 1.5f;
	vertices[5] = 1.5f;
	vertices[6] = 2.5f;
	vertices[7] = -2.5f;

	glBindBuffer(GL_ARRAY_BUFFER, L_vbo_position_Sphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawArrays(GL_PATCHES, 0, 4);
	glBindVertexArray(0);

	////////  LINE 6 ///////////
	glLineWidth(2.0f);
	//exposure = 1.0f;
	mat_view = mat4::identity();
	mat_proj = mat4::identity();
	mat_model = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();


	mat_model = translate(-1.25f, -0.08f, -4.0f);
	//mat_model *= rotate(gAngle, 1.0f, 1.0f, 1.0f);
//mat_model *= scale(0.04f, 5.2f, 0.2f);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, L_ubo_material);
	glLineWidth(3.0f);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_min, L_bloom_thresh_min);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_max, L_bloom_thresh_max);

	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;

	glUniformMatrix4fv(L_ViewUniform, 1, GL_FALSE, mat_view);
	glUniformMatrix4fv(L_projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glUniformMatrix4fv(L_modelUniform, 1, GL_FALSE, mat_model);
	glUniformMatrix4fv(L_mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	L_gNumberOfLineSegments = 5;
	glUniform1i(L_gNumberOfSegmentUniform, L_gNumberOfLineSegments);
	glUniform1f(L_b0Uniform, 2.5f);
	glUniform1f(L_b1Uniform, 3.0f);
	glUniform1f(L_lengthUniform, 2.9f);
	glUniform1i(L_gNumberOfStripsUniform, 1);

	glBindVertexArray(L_vao_Sphere);


	vertices[0] = 0.3f;
	vertices[1] = -1.0f;
	vertices[2] = -0.5f;
	vertices[3] = 1.5f;
	vertices[4] = 1.0f;
	vertices[5] = -0.5f;
	vertices[6] = 0.3f;
	vertices[7] = 0.5f;

	glBindBuffer(GL_ARRAY_BUFFER, L_vbo_position_Sphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawArrays(GL_PATCHES, 0, 4);
	glBindVertexArray(0);




	///////////////////// END OF ELECTRICAL LINES //////////////////////////////////


	glDisable(GL_DEPTH_TEST);


	////////////////// ******** P O S T --- P R O C E S S I N G -- E F F E C T S ******************///////////////

	glUseProgram(L_gShaderProgramObject_filter);

	glBindVertexArray(L_vao);
	glBindFramebuffer(GL_FRAMEBUFFER, L_filter_fbo[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, L_tex_brightpass);
	glUniform1f(L_samplerFilter, 1);
	glViewport(0, 0, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, L_filter_fbo[1]);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, L_tex_filter[0]);
	glUniform1f(L_samplerFilter, 0);
	glViewport(0, 0, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glUseProgram(0);
	//L_exposure = 1.0f;
	glUseProgram(L_gShaderProgramObject_resolve);
	glUniform1f(L_uniforms.L_resolve.exposure, L_exposure);

	if (L_show_prefilter)
	{
		glUniform1f(L_uniforms.L_resolve.bloom_factor, 0.0f);
		glUniform1f(L_uniforms.L_resolve.scene_factor, 1.0f);
	}
	else
	{
		glUniform1f(L_uniforms.L_resolve.bloom_factor, L_show_bloom ? L_bloom_factor : 0.0f);
		glUniform1f(L_uniforms.L_resolve.scene_factor, L_show_scene ? 1.0f : 0.0f);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glBindFramebuffer(GL_FRAMEBUFFER, L_fbo);
	glViewport(0, 0, MAX_SCENE_WIDTH, MAX_SCENE_WIDTH);
	glClearBufferfv(GL_COLOR, 0, green);
	glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, L_tex_filter[1]);
	glUniform1i(L_samplerResolve1, 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, L_show_prefilter ? L_tex_brightpass : L_tex_scene);
	glUniform1i(L_samplerResolve0, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glUseProgram(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	

	static GLfloat angle1;

	mat4 modelViewMatrix;
	//mat4 modelViewProjectionMatrix;
	mat4 scaleMatrix;
	mat4 projectionMatrix;
	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 RotationMatrix;
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glViewport(0, 0, T_gWidth, T_gHeight);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, L_color_texture);
	glUniform1i(L_samplerForLightning, 0);

	scaleMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	// intialize above matrices to identity
	modelMatrix = mat4::identity();
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	glUseProgram(L_FB_gShaderProgramObject);

	//	mat4 RotationMatrix = mat4::identity();
		// intialize above matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();


	modelMatrix = translate(-0.0f, 20.0f, -125.0f);
	modelMatrix *= scale(40.0f, 70.0f, 50.0f);

	RotationMatrix = rotate(180.0f, 0.0f, 0.0f, 1.0f);
	RotationMatrix *= rotate(50.0f, 1.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix * RotationMatrix;
	viewMatrix = lookat(vec3(xView, 12.0f, zView), vec3(xView, 12.0f, -300.0f), vec3(0.0f, 1.0f, 0.0f));
	modelViewMatrix = viewMatrix * modelMatrix;

	glUniformMatrix4fv(L_FB_mvpUniform1, 1, GL_FALSE, perspectiveProjectionMatrix);
	glUniformMatrix4fv(L_FB_modelViewUniform1, 1, GL_FALSE, modelViewMatrix);

	glBindVertexArray(L_FB_vao_triangle);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);
//	angle1 += 0.02f;
//
	//display();
	//SwapBuffers(ghdc);
}
void displayLightningV2()
{
	//glBindTexture(GL_TEXTURE_2D, 0);
	GLfloat vertices[8];
	const GLfloat green[] = { 0.0f,1.0f,0.0f,1.0f };
	const GLfloat blue[] = { 0.4f, 0.5f, 0.6f, 1.0f };
	const GLfloat L_black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	//	//static const GLfloat blue[] = { 0.4f, 0.5f, 0.6f, 1.0f };
	static const GLfloat L_one = 1.0f;
	//
	//	//if (!paused)
	//	//	total_time += (currentTime - last_time);
	//	//last_time = currentTime;
	//	//float t = (float)total_time;
	//
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//
	//	 //use shader program
	//
	//
	//	/*glViewport(0, 0, T_gWidth, T_gHeight);*/
	//
	glBindFramebuffer(GL_FRAMEBUFFER, L_render_fbo);
	glViewport(0, 0, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);
	glClearBufferfv(GL_COLOR, 0, L_black);
	glClearBufferfv(GL_COLOR, 1, L_black);
	glClearBufferfv(GL_DEPTH, 0, &L_one);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);




	//static GLfloat gAngle;
	/////////////// ******* Electrical Lines---- Tessellated *********** ////////////////////

	glLineWidth(20.0);
	glUseProgram(L_gShaderProgramObject);

	vmath::mat4 mat_proj;
	vmath::mat4 mat_view;
	vmath::mat4 mat_model;
	vmath::mat4 modelViewProjectionMatrix;
	mat_view = mat4::identity();
	mat_proj = mat4::identity();
	mat_model = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();


	mat_model = translate(-1.0f, 0.0f, -4.0f);
	//mat_model *= rotate(gAngle, 1.0f, 1.0f, 1.0f);
//mat_model *= scale(0.04f, 5.2f, 0.2f);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, L_ubo_material);
	glLineWidth(5.0f);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_min, L_bloom_thresh_min);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_max, L_bloom_thresh_max);

	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;
	//exposure = 5.0f;
	glUniformMatrix4fv(L_ViewUniform, 1, GL_FALSE, mat_view);
	glUniformMatrix4fv(L_projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glUniformMatrix4fv(L_modelUniform, 1, GL_FALSE, mat_model);
	glUniformMatrix4fv(L_mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	L_gNumberOfLineSegments = 5;
	glUniform1i(L_gNumberOfSegmentUniform, L_gNumberOfLineSegments);
	glUniform1f(L_b0Uniform, 4.0f);
	glUniform1f(L_b1Uniform, 5.0f);
	glUniform1f(L_lengthUniform, 2.5f);
	glUniform1i(L_gNumberOfStripsUniform, 1);

	glBindVertexArray(L_vao_Sphere);

	vertices[0] = -0.5f;
	vertices[1] = 1.0f;
	vertices[2] = 0.5f;
	vertices[3] = -1.5f;
	vertices[4] = 1.5f;
	vertices[5] = 1.5f;
	vertices[6] = 2.5f;
	vertices[7] = -1.0f;

	glBindBuffer(GL_ARRAY_BUFFER, L_vbo_position_Sphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawArrays(GL_PATCHES, 0, 4);
	glBindVertexArray(0);






	////////  LINE 2 ///////////
	glLineWidth(2.0f);
	//exposure = 1.0f;
	mat_view = mat4::identity();
	mat_proj = mat4::identity();
	mat_model = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();


	mat_model = translate(-0.4f, 0.1f, -4.0f);
	//mat_model *= rotate(gAngle, 1.0f, 1.0f, 1.0f);
	//mat_model *= scale(0.04f, 5.2f, 0.2f);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, L_ubo_material);
	glLineWidth(3.0f);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_min, L_bloom_thresh_min);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_max, L_bloom_thresh_max);

	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;

	glUniformMatrix4fv(L_ViewUniform, 1, GL_FALSE, mat_view);
	glUniformMatrix4fv(L_projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glUniformMatrix4fv(L_modelUniform, 1, GL_FALSE, mat_model);
	glUniformMatrix4fv(L_mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	L_gNumberOfLineSegments = 5;
	glUniform1i(L_gNumberOfSegmentUniform, L_gNumberOfLineSegments);
	glUniform1f(L_b0Uniform, 4.5f);
	glUniform1f(L_b1Uniform, 2.0f);
	glUniform1f(L_lengthUniform, 0.002f);
	glUniform1i(L_gNumberOfStripsUniform, 1);

	glBindVertexArray(L_vao_Sphere);

	vertices[0] = 1.0f;
	vertices[1] = 2.0f;
	vertices[2] = 0.5f;
	vertices[3] = -1.5f;
	vertices[4] = 1.5f;
	vertices[5] = 1.5f;
	vertices[6] = 2.5f;
	vertices[7] = -2.0f;

	glBindBuffer(GL_ARRAY_BUFFER, L_vbo_position_Sphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawArrays(GL_PATCHES, 0, 4);
	glBindVertexArray(0);





	////////  LINE 3 ///////////
	glLineWidth(2.0f);
	//exposure = 1.0f;
	mat_view = mat4::identity();
	mat_proj = mat4::identity();
	mat_model = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();


	mat_model = translate(-1.85f, -0.1f, -4.0f);
	//mat_model *= rotate(gAngle, 1.0f, 1.0f, 1.0f);
//mat_model *= scale(0.04f, 5.2f, 0.2f);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, L_ubo_material);
	glLineWidth(3.0f);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_min, L_bloom_thresh_min);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_max, L_bloom_thresh_max);

	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;

	glUniformMatrix4fv(L_ViewUniform, 1, GL_FALSE, mat_view);
	glUniformMatrix4fv(L_projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glUniformMatrix4fv(L_modelUniform, 1, GL_FALSE, mat_model);
	glUniformMatrix4fv(L_mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	L_gNumberOfLineSegments = 8;
	glUniform1i(L_gNumberOfSegmentUniform, L_gNumberOfLineSegments);
	glUniform1f(L_b0Uniform, 5.5f);
	glUniform1f(L_b1Uniform, 2.0f);
	glUniform1f(L_lengthUniform, 2.9f);
	glUniform1i(L_gNumberOfStripsUniform, 1);

	glBindVertexArray(L_vao_Sphere);


	vertices[0] = 0.0f;
	vertices[1] = -1.0f;
	vertices[2] = -0.5f;
	vertices[3] = 1.5f;
	vertices[4] = 1.0f;
	vertices[5] = -0.5f;
	vertices[6] = 0.5f;
	vertices[7] = 1.0f;

	glBindBuffer(GL_ARRAY_BUFFER, L_vbo_position_Sphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawArrays(GL_PATCHES, 0, 4);
	glBindVertexArray(0);




	////////  LINE 4 ///////////
	glLineWidth(2.0f);
	//exposure = 1.0f;
	mat_view = mat4::identity();
	mat_proj = mat4::identity();
	mat_model = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();


	mat_model = translate(-1.0f, 0.45f, -4.0f);
	//mat_model *= rotate(gAngle, 1.0f, 1.0f, 1.0f);
//mat_model *= scale(0.04f, 5.2f, 0.2f);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, L_ubo_material);
	glLineWidth(3.0f);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_min, L_bloom_thresh_min);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_max, L_bloom_thresh_max);

	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;

	glUniformMatrix4fv(L_ViewUniform, 1, GL_FALSE, mat_view);
	glUniformMatrix4fv(L_projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glUniformMatrix4fv(L_modelUniform, 1, GL_FALSE, mat_model);
	glUniformMatrix4fv(L_mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	L_gNumberOfLineSegments = 6;
	glUniform1i(L_gNumberOfSegmentUniform, L_gNumberOfLineSegments);
	glUniform1f(L_b0Uniform, 1.8f);
	glUniform1f(L_b1Uniform, 6.0f);
	glUniform1f(L_lengthUniform, 0.002f);
	glUniform1i(L_gNumberOfStripsUniform, 1);

	glBindVertexArray(L_vao_Sphere);

	vertices[0] = -1.0f;
	vertices[1] = 2.0f;
	vertices[2] = 0.5f;
	vertices[3] = -1.5f;
	vertices[4] = 1.5f;
	vertices[5] = 1.5f;
	vertices[6] = 2.5f;
	vertices[7] = -2.0f;

	glBindBuffer(GL_ARRAY_BUFFER, L_vbo_position_Sphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawArrays(GL_PATCHES, 0, 4);
	glBindVertexArray(0);


	////////  LINE 5  ///////////
	glLineWidth(2.0f);
	//exposure = 1.0f;
	mat_view = mat4::identity();
	mat_proj = mat4::identity();
	mat_model = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();


	mat_model = translate(-1.0f, 0.45f, -4.0f);
	//mat_model *= rotate(gAngle, 1.0f, 1.0f, 1.0f);
//mat_model *= scale(0.04f, 5.2f, 0.2f);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, L_ubo_material);
	glLineWidth(3.0f);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_min, L_bloom_thresh_min);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_max, L_bloom_thresh_max);

	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;

	glUniformMatrix4fv(L_ViewUniform, 1, GL_FALSE, mat_view);
	glUniformMatrix4fv(L_projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glUniformMatrix4fv(L_modelUniform, 1, GL_FALSE, mat_model);
	glUniformMatrix4fv(L_mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	L_gNumberOfLineSegments = 8;
	glUniform1i(L_gNumberOfSegmentUniform, L_gNumberOfLineSegments);
	glUniform1f(L_b0Uniform, 1.8f);
	glUniform1f(L_b1Uniform, 2.0f);
	glUniform1f(L_lengthUniform, 0.002f);
	glUniform1i(L_gNumberOfStripsUniform, 1);

	glBindVertexArray(L_vao_Sphere);

	vertices[0] = -1.0f;
	vertices[1] = 2.0f;
	vertices[2] = 0.5f;
	vertices[3] = -1.5f;
	vertices[4] = 1.5f;
	vertices[5] = 1.5f;
	vertices[6] = 2.5f;
	vertices[7] = -2.5f;

	glBindBuffer(GL_ARRAY_BUFFER, L_vbo_position_Sphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawArrays(GL_PATCHES, 0, 4);
	glBindVertexArray(0);

	////////  LINE 5 2 ///////////
	glLineWidth(2.0f);
	//exposure = 1.0f;
	mat_view = mat4::identity();
	mat_proj = mat4::identity();
	mat_model = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();


	mat_model = translate(-1.0f, 0.45f, -4.0f);
	//mat_model *= rotate(gAngle, 1.0f, 1.0f, 1.0f);
//mat_model *= scale(0.04f, 5.2f, 0.2f);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, L_ubo_material);
	glLineWidth(3.0f);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_min, L_bloom_thresh_min);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_max, L_bloom_thresh_max);

	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;

	glUniformMatrix4fv(L_ViewUniform, 1, GL_FALSE, mat_view);
	glUniformMatrix4fv(L_projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glUniformMatrix4fv(L_modelUniform, 1, GL_FALSE, mat_model);
	glUniformMatrix4fv(L_mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	L_gNumberOfLineSegments = 8;
	glUniform1i(L_gNumberOfSegmentUniform, L_gNumberOfLineSegments);
	glUniform1f(L_b0Uniform, 0.8f);
	glUniform1f(L_b1Uniform, 4.0f);
	glUniform1f(L_lengthUniform, 0.002f);
	glUniform1i(L_gNumberOfStripsUniform, 1);

	glBindVertexArray(L_vao_Sphere);

	vertices[0] = -1.0f;
	vertices[1] = 2.0f;
	vertices[2] = 0.5f;
	vertices[3] = -1.5f;
	vertices[4] = 1.5f;
	vertices[5] = 1.5f;
	vertices[6] = 2.5f;
	vertices[7] = -2.5f;

	glBindBuffer(GL_ARRAY_BUFFER, L_vbo_position_Sphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawArrays(GL_PATCHES, 0, 4);
	glBindVertexArray(0);

	////////  LINE 6 ///////////
	glLineWidth(2.0f);
	//exposure = 1.0f;
	mat_view = mat4::identity();
	mat_proj = mat4::identity();
	mat_model = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();


	mat_model = translate(-1.25f, -0.08f, -4.0f);
	//mat_model *= rotate(gAngle, 1.0f, 1.0f, 1.0f);
//mat_model *= scale(0.04f, 5.2f, 0.2f);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, L_ubo_material);
	glLineWidth(3.0f);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_min, L_bloom_thresh_min);
	glUniform1f(L_uniforms.L_scene.bloom_thresh_max, L_bloom_thresh_max);

	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;

	glUniformMatrix4fv(L_ViewUniform, 1, GL_FALSE, mat_view);
	glUniformMatrix4fv(L_projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glUniformMatrix4fv(L_modelUniform, 1, GL_FALSE, mat_model);
	glUniformMatrix4fv(L_mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	L_gNumberOfLineSegments = 5;
	glUniform1i(L_gNumberOfSegmentUniform, L_gNumberOfLineSegments);
	glUniform1f(L_b0Uniform, 2.5f);
	glUniform1f(L_b1Uniform, 3.0f);
	glUniform1f(L_lengthUniform, 2.9f);
	glUniform1i(L_gNumberOfStripsUniform, 1);

	glBindVertexArray(L_vao_Sphere);


	vertices[0] = 0.3f;
	vertices[1] = -1.0f;
	vertices[2] = -0.5f;
	vertices[3] = 1.5f;
	vertices[4] = 1.0f;
	vertices[5] = -0.5f;
	vertices[6] = 0.3f;
	vertices[7] = 0.5f;

	glBindBuffer(GL_ARRAY_BUFFER, L_vbo_position_Sphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawArrays(GL_PATCHES, 0, 4);
	glBindVertexArray(0);




	///////////////////// END OF ELECTRICAL LINES //////////////////////////////////


	glDisable(GL_DEPTH_TEST);


	////////////////// ******** P O S T --- P R O C E S S I N G -- E F F E C T S ******************///////////////

	glUseProgram(L_gShaderProgramObject_filter);

	glBindVertexArray(L_vao);
	glBindFramebuffer(GL_FRAMEBUFFER, L_filter_fbo[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, L_tex_brightpass);
	glUniform1f(L_samplerFilter, 1);
	glViewport(0, 0, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, L_filter_fbo[1]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, L_tex_filter[0]);
	glUniform1f(L_samplerFilter, 0);
	glViewport(0, 0, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glUseProgram(0);
	//L_exposure = 1.0f;
	glUseProgram(L_gShaderProgramObject_resolve);
	glUniform1f(L_uniforms.L_resolve.exposure, L_exposure);

	if (L_show_prefilter)
	{
		glUniform1f(L_uniforms.L_resolve.bloom_factor, 0.0f);
		glUniform1f(L_uniforms.L_resolve.scene_factor, 1.0f);
	}
	else
	{
		glUniform1f(L_uniforms.L_resolve.bloom_factor, L_show_bloom ? L_bloom_factor : 0.0f);
		glUniform1f(L_uniforms.L_resolve.scene_factor, L_show_scene ? 1.0f : 0.0f);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, L_fbo);
	glViewport(0, 0, MAX_SCENE_WIDTH, MAX_SCENE_WIDTH);
	glClearBufferfv(GL_COLOR, 0, green);
	glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, L_tex_filter[1]);
	glUniform1i(L_samplerResolve1, 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, L_show_prefilter ? L_tex_brightpass : L_tex_scene);
	glUniform1i(L_samplerResolve0, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glUseProgram(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
BOOL loadTexture(GLuint *texture_Smiley, TCHAR imageResourceID[])
{
	HBITMAP hBitmap = NULL;
	BITMAP bmp;
	BOOL bStatus = FALSE;

	//Win32API
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResourceID, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	if (hBitmap != NULL)
	{
		bStatus = TRUE;
		GetObject(hBitmap, sizeof(BITMAP), &bmp);
		//Now OpenGL part for texture starts from here
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glGenTextures(1, texture_Smiley);
		glBindTexture(GL_TEXTURE_2D, *texture_Smiley);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);


		//gluBuild2DMipMaps = glTexImage2D + glGenerateMipMap
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);
		//2ndParam = MipMap Level. By passing zero we say that,I am not giving any level. You(OPENGL) decide by yourself.
		//Valid Numbers to be expected over there are - 2,4,6

		glGenerateMipmap(GL_TEXTURE_2D);

		DeleteObject(hBitmap);//As hBitmap posseses Global memory and it is always better to free it after the work has been done.
		//glBindTexture(GL_TEXTURE_2D, 0);
	}
	return bStatus;


}
bool LoadHeightMapFromImageFree(string sImagePath)
{
	vRenderScale = glm::vec3(255.0f, 25.0f, 300.0f);

	if (bLoaded)
	{
		bLoaded = false;
		//	ReleaseHeightmap();
	}
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP* dib(0);

	fif = FreeImage_GetFileType(sImagePath.c_str(), 0); // Check the file signature and deduce its format

	if (fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(sImagePath.c_str());

	if (fif == FIF_UNKNOWN) // If still unknown, return failure
		return false;

	if (FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
		dib = FreeImage_Load(fif, sImagePath.c_str());
	if (!dib)
		return false;

	BYTE* bDataPointer = FreeImage_GetBits(dib); // Retrieve the image data
	iRows = FreeImage_GetHeight(dib);
	iCols = FreeImage_GetWidth(dib);

	fprintf_s(gpLogFile, "iRows : %d\n", iRows);
	fprintf_s(gpLogFile, "iCols : %d\n", iCols);
	// We also require our image to be either 24-bit (classic RGB) or 8-bit (luminance)
	if (bDataPointer == NULL || iRows == 0 || iCols == 0 || (FreeImage_GetBPP(dib) != 24 && FreeImage_GetBPP(dib) != 8))
		return false;

	// How much to increase data pointer to get to next pixel data
	unsigned int ptr_inc = FreeImage_GetBPP(dib) == 24 ? 3 : 1;
	// Length of one row in data
	unsigned int row_step = ptr_inc * iCols;

	//vboHeightmapData.CreateVBO();
	// All vertex data are here (there are iRows*iCols vertices in this heightmap), we will get to normals later
	vVertexData = vector< vector< glm::vec3> >(iRows, vector<glm::vec3>(iCols));
	vector< vector< glm::vec2> > vCoordsData(iRows, vector<glm::vec2>(iCols));

	float fTextureU = float(iCols)*0.1f;
	float fTextureV = float(iRows)*0.1f;
	int m = 0;
	FOR(i, iRows)
	{
		FOR(j, iCols)
		{
			float fScaleC = float(j) / float(iCols - 1);
			float fScaleR = float(i) / float(iRows - 1);
			float fVertexHeight = float(*(bDataPointer + row_step * i + j * ptr_inc)) / 255.0f;
			vVertexData[i][j] = glm::vec3(-0.5f + fScaleC, fVertexHeight, -0.5f + fScaleR);
			vCoordsData[i][j] = glm::vec2(fTextureU*fScaleC, fTextureV*fScaleR);
			//fprintf_s(gpLogFile, "vVertexData : %f \t %f\n", vVertexData[i][j]);
		//	fprintf(gpLogFile, "I am in loop %d", m);
			//m++;
		}
	}

	// Normals are here - the heightmap contains ( (iRows-1)*(iCols-1) quads, each one containing 2 triangles, therefore array of we have 3D array)
	vector< vector<glm::vec3> > vNormals[2];
	FOR(i, 2)vNormals[i] = vector< vector<glm::vec3> >(iRows - 1, vector<glm::vec3>(iCols - 1));

	FOR(i, iRows - 1)
	{
		FOR(j, iCols - 1)
		{
			glm::vec3 vTriangle0[] =
			{
				vVertexData[i][j],
				vVertexData[i + 1][j],
				vVertexData[i + 1][j + 1]
			};
			glm::vec3 vTriangle1[] =
			{
				vVertexData[i + 1][j + 1],
				vVertexData[i][j + 1],
				vVertexData[i][j]
			};

			glm::vec3 vTriangleNorm0 = glm::cross(vTriangle0[0] - vTriangle0[1], vTriangle0[1] - vTriangle0[2]);
			glm::vec3 vTriangleNorm1 = glm::cross(vTriangle1[0] - vTriangle1[1], vTriangle1[1] - vTriangle1[2]);

			vNormals[0][i][j] = glm::normalize(vTriangleNorm0);
			vNormals[1][i][j] = glm::normalize(vTriangleNorm1);

		}
	}

	vector< vector<glm::vec3> > vFinalNormals = vector< vector<glm::vec3> >(iRows, vector<glm::vec3>(iCols));

	FOR(i, iRows)
		FOR(j, iCols)
	{
		// Now we wanna calculate final normal for [i][j] vertex. We will have a look at all triangles this vertex is part of, and then we will make average vector
		// of all adjacent triangles' normals

		glm::vec3 vFinalNormal = glm::vec3(0.0f, 0.0f, 0.0f);

		// Look for upper-left triangles
		if (j != 0 && i != 0)
			FOR(k, 2)vFinalNormal += vNormals[k][i - 1][j - 1];
		// Look for upper-right triangles
		if (i != 0 && j != iCols - 1)vFinalNormal += vNormals[0][i - 1][j];
		// Look for bottom-right triangles
		if (i != iRows - 1 && j != iCols - 1)
			FOR(k, 2)vFinalNormal += vNormals[k][i][j];
		// Look for bottom-left triangles
		if (i != iRows - 1 && j != 0)
			vFinalNormal += vNormals[1][i][j - 1];
		vFinalNormal = glm::normalize(vFinalNormal);

		vFinalNormals[i][j] = vFinalNormal; // Store final normal of j-th vertex in i-th row

	}




	float fGrassPatchOffsetMin = 1.5f;
	float fGrassPatchOffsetMax = 2.5f;
	float fGrassPatchHeight = 5.0f;

	glm::vec3 vCurPatchPos(-vRenderScale.x*0.5f + fGrassPatchOffsetMin, 0.0f, vRenderScale.z*0.5f - fGrassPatchOffsetMin);
	iNumGrassTriangles = 0;

	while (vCurPatchPos.x < vRenderScale.x*0.5f)
	{
		vCurPatchPos.z = vRenderScale.z*0.5f - fGrassPatchOffsetMin;

		while (vCurPatchPos.z > -vRenderScale.z*0.5f)
		{
			vCurPatchPos.y = GetHeightFromRealVector(vCurPatchPos) - 0.3f;
			AddData(&vCurPatchPos, sizeof(glm::vec3));

			iNumGrassTriangles += 1;

			vCurPatchPos.z -= fGrassPatchOffsetMin + (fGrassPatchOffsetMax - fGrassPatchOffsetMin)*float(rand() % 1000)*0.001f;
		}

		vCurPatchPos.x += fGrassPatchOffsetMin + (fGrassPatchOffsetMax - fGrassPatchOffsetMin)*float(rand() % 1000)*0.001f;

	}
	glGenVertexArrays(1, &uiGrassVAO);
	glBindVertexArray(uiGrassVAO);
	glGenBuffers(1, &uiBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uiBuffer);
	glBufferData(GL_ARRAY_BUFFER, data1.size(), &data1[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	bLoaded = true; // If get here, we succeeded with generating heightmap
	return true;
}
void AddData(void* ptrData, UINT uiDataSize)
{
	data1.insert(data1.end(), (BYTE*)ptrData, (BYTE*)ptrData + uiDataSize);
	iCurrentSize += uiDataSize;
}
void LoadAllTextures()
{
	std::string sTextureNames[] = { "grass.dds" };
	glActiveTexture(GL_TEXTURE6);
	LoadTexture2D("data\\textures\\grassPack6.dds", true);
	SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_TRILINEAR);
	SetWrap();
	SetAnisotropyLevel(4.0f);
}
bool LoadTexture2D(string a_sPath, bool bGenerateMipMaps)
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP* dib(0);
	fif = FreeImage_GetFileType(a_sPath.c_str(), 0); // Check the file signature and deduce its format
	if (fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(a_sPath.c_str());

	if (fif == FIF_UNKNOWN) // If still unknown, return failure
		return false;

	if (FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
		dib = FreeImage_Load(fif, a_sPath.c_str());
	if (!dib)
		return false;
	BYTE* bDataPointer = FreeImage_GetBits(dib); // Retrieve the image data
	// If somehow one of these failed (they shouldn't), return failure
	if (bDataPointer == NULL || FreeImage_GetWidth(dib) == 0 || FreeImage_GetHeight(dib) == 0)
		return false;

	GLenum format = 0;
	int bada = FreeImage_GetBPP(dib);
	if (FreeImage_GetBPP(dib) == 32)format = GL_RGBA;
	if (FreeImage_GetBPP(dib) == 24)format = GL_BGR;
	if (FreeImage_GetBPP(dib) == 8)format = GL_LUMINANCE;
	if (format != 0)

		CreateFromData(bDataPointer, FreeImage_GetWidth(dib), FreeImage_GetHeight(dib), FreeImage_GetBPP(dib), format, bGenerateMipMaps);

	FreeImage_Unload(dib);

	if (format == 0)
		return false;

	sPath = a_sPath;

	return true; // Success


}
void SetFiltering(int a_tfMagnification, int a_tfMinification)
{
	glBindSampler(0, uiSampler);

	// Set magnification filter
	if (a_tfMagnification == TEXTURE_FILTER_MAG_NEAREST)
		glSamplerParameteri(uiSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	else if (a_tfMagnification == TEXTURE_FILTER_MAG_BILINEAR)
		glSamplerParameteri(uiSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Set minification filter
	if (a_tfMinification == TEXTURE_FILTER_MIN_NEAREST)
		glSamplerParameteri(uiSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	else if (a_tfMinification == TEXTURE_FILTER_MIN_BILINEAR)
		glSamplerParameteri(uiSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	else if (a_tfMinification == TEXTURE_FILTER_MIN_NEAREST_MIPMAP)
		glSamplerParameteri(uiSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	else if (a_tfMinification == TEXTURE_FILTER_MIN_BILINEAR_MIPMAP)
		glSamplerParameteri(uiSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	else if (a_tfMinification == TEXTURE_FILTER_MIN_TRILINEAR)
		glSamplerParameteri(uiSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	tfMinification = a_tfMinification;
	tfMagnification = a_tfMagnification;
}
void SetWrap()
{
	glBindSampler(0, uiSampler);
	glSamplerParameteri(uiSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(uiSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
void SetAnisotropyLevel(float fAnisotropyLevel)
{
	glSamplerParameterf(uiSampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, fAnisotropyLevel);
}
void CreateFromData(BYTE* bData, int a_iWidth, int a_iHeight, int a_iBPP, GLenum format, bool bGenerateMipMaps)
{
	glGenTextures(1, &uiTexture);
	glBindTexture(GL_TEXTURE_2D, uiTexture);
	if (format == GL_RGBA || format == GL_BGRA)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, a_iWidth, a_iHeight, 0, format, GL_UNSIGNED_BYTE, bData);
	// We must handle this because of internal format parameter
	else if (format == GL_RGB || format == GL_BGR)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, a_iWidth, a_iHeight, 0, format, GL_UNSIGNED_BYTE, bData);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, format, a_iWidth, a_iHeight, 0, format, GL_UNSIGNED_BYTE, bData);
	if (bGenerateMipMaps)glGenerateMipmap(GL_TEXTURE_2D);
	glGenSamplers(1, &uiSampler);

	sPath = "";
	bMipMapsGenerated = bGenerateMipMaps;
	iWidth = a_iWidth;
	iHeight = a_iHeight;
	iBPP = a_iBPP;
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




float GetHeightFromRealVector(glm::vec3 vRealPosition)
{
	int iColumn = int((vRealPosition.x + vRenderScale.x*0.5f)*float(iCols) / (vRenderScale.x));
	int iRow = int((vRealPosition.z + vRenderScale.z*0.5f)*float(iRows) / (vRenderScale.z));

	iColumn = glm::min(iColumn, iCols - 1);
	iRow = glm::min(iRow, iRows - 1);

	iColumn = glm::max(iColumn, 0);
	iRow = glm::max(iRow, 0);

	return vVertexData[iRow][iColumn].y*vRenderScale.y;
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
