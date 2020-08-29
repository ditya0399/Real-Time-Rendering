#include<Windows.h>
#include<glew.h>               // This SHOULD be above GL.h header.
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



#pragma comment(lib,"opengl32.lib")//for linking
#pragma comment(lib,"glew32.lib")//for linking
#pragma comment(lib,"assimp-vc141-mtd.lib")//to link the assimp library
#pragma comment(lib, "FreeImage.lib")

#define FOR(q,n) for(int q=0;q<n;q++)


//int iBufferType;
int Win_Width = 800; 
int Win_Height = 600;
using namespace vmath;

float z = -10.0f;
Model *myModel = NULL;

GLfloat angle1 = 0.0f;
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

int modulo = 64;

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





LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void ToggleFullScreen(void);
void update(void);
void uninitialize(void);
void displayTerrain();
void initializeTerrain();


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
				//  LKeyPressed = 3;
			}
			else
			{

				gbLighting = 0;
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
		"gl_Position =   u_viewMatrix *u_modelMatrix * vPosition;" \
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

		"float randX = 35 *rand(vec2(gl_InvocationID,0.0));" \
		"float randY =  rand(vec2(gl_InvocationID,0.0));" \
		"float randZ =  35 *  rand(vec2(gl_InvocationID,0.0));" \

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


	//firtree1.3ds

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

	//Depth Lines
	glClearDepth(1.0f);
	glClearColor(0.85f, 0.95f, 1.0f, 1.0f);

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

		"float fGrassPatchSize = 5.0;" \
		"float fWindStrength = 4.0;" \

		"vec3 vWindDirection = vec3(1.0, 0.0, 1.0);" \
		"vWindDirection = normalize(vWindDirection);" \

		"for (int i = 0; i < 3; i++)" \
		"{" \


		"vec3 vBaseDirRotated = (rotationMatrix(vec3(0, 1, 0), sin(fTimePassed*0.7f)*0.1f)*vec4(vBaseDir[i], 1.0)).xyz;" \

		"vLocalSeed = vGrassFieldPos * float(i);" \
		"int iGrassPatch = randomInt(0, 3);" \

		"float fGrassPatchHeight = 3.5 + randZeroOne()*2.0;" \

		"float fTCStartX = float(iGrassPatch)*0.25f;" \
		"float fTCEndX = fTCStartX + 0.25f;" \

		"float fWindPower = 0.5f + sin(vGrassFieldPos.x / 30 + vGrassFieldPos.z / 30 + fTimePassed * (1.2f + fWindStrength / 20.0f));" \
		"if (fWindPower < 0.0f)" \
		"fWindPower = fWindPower * 0.2f;" \
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
		"uniform vec4 vColor = vec4(1.0, 1.0, 1.0,1.0);" \

		"uniform vec3 vEyePosition = vec3(0.0,9.0,85.0);" \



		"uniform float fAlphaTest = 0.05;" \
		"uniform float fAlphaMultiplier = 3.0;" \


		"void main(void)" \
		"{" \

		"vec4 vTexColor = texture2D(gSampler, vTexCoord);" \


		"float fNewAlpha = vTexColor.a*fAlphaMultiplier;" \
		"if (fNewAlpha < fAlphaTest)" \
		"discard;" \

		"if (fNewAlpha > 1.0f)" \
		"fNewAlpha = 0.5f;" \

		"vec4 vMixedColor = vTexColor;" \


		"outputColor = vec4(vMixedColor.xyz, fNewAlpha);" \

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



void display(void)
{


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use shader program
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
	//declaration of matrices
	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 modelViewMatrix;

	mat4 RotationMatrix;
	mat4 TranslateMatrix;
	mat4 scaleMatrix;
	// intialize above matrices to identity

	////////  RECTANGLE //////////////
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	TranslateMatrix = translate(0.0f, 6.0f, -05.0f);
	scaleMatrix = scale(0.003f, 0.003f, 0.003f);
	TranslateMatrix *= scaleMatrix;

	RotationMatrix = rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	//TranslateMatrix *= RotationMatrix; 

	RotationMatrix = rotate(angle1, 0.0f, 1.0f, 0.0f);
	TranslateMatrix *= RotationMatrix;
	viewMatrix = lookat(vec3(xView, 12.0f, zView), vec3(xView, 12.0f, -300.0f), vec3(0.0f, 1.0f, 0.0f));
	//	RotationMatrix = rotate(angleRectangle, 1.0f, 0.0f, 0.0f);
	//RotationMatrix = rotate(angleRectangle, angleRectangle, angleRectangle);
	// perform necessary transformations
	modelMatrix = TranslateMatrix;

	glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);


	//glActiveTexture(GL_TEXTURE0);					
	//glBindTexture(GL_TEXTURE_2D, gTextureSamplerUniform);	// 2. bind to the texture
	glUniform1i(gTextureSamplerUniform, 2);
	//GL_FALSE = Should we transpose the matrix?
	//DirectX is roj major so there we will have to transpose but OpenGL is Colomn major so no need to transpose. 
	// bind with vao
	//this will avoid many binding to vbo
//	glBindVertexArray(vao);

	// bind with textures

	// draw necessary scene
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	//  0 =  From where to start in the array. 
	// We have to start from 0th element i.e 0. if the 0th element would be 50.0f then we would have given the 2nd parameter as 50.
	//3 = How many Vertices? 
	//GL_TRIANGLES is the thing between glBegin() and glEnd()


	// unbind vao
	//glBindVertexArray(0);

	// unuse program

	myModel->Draw(gShaderProgramObject);
	glUseProgram(0);

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



//	displayTerrain();
	glUseProgram(0);

	////////////////////////////////////////////////
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
	time += 0.002f;
	glBindVertexArray(uiGrassVAO);
	glDrawArrays(GL_POINTS, 0, iNumGrassTriangles);
	//glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);

	glUseProgram(0);


	SwapBuffers(ghdc);
	//angle1 += 0.2f;

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
	LoadTexture2D("data\\textures\\grassPack.dds", true);
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