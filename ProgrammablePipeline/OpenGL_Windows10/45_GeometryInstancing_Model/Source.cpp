#include<Windows.h>
#include<glew.h>               // This SHOULD be above GL.h header.
#include<gl/GL.h>
#include<stdio.h>
#include"vmath.h"
#include"Model.h"

#pragma comment(lib,"opengl32.lib")//for linking
#pragma comment(lib,"glew32.lib")//for linking
#pragma comment(lib,"assimp-vc141-mtd.lib")//to link the assimp library

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
GLuint gbLighting = 0;
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
GLuint LKeyPressed = 0;


float xView = 0.0f;
float yView = 35.0f;
float zView = 8.0f;

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

		"float randX = 5.0 * rand(vec2(gl_InvocationID,0.0));" \
		"float randY =  rand(vec2(gl_InvocationID,0.0));" \
		"float randZ =  rand(vec2(gl_InvocationID,0.0));" \

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
	//myModel = new Model((char *)"nanosuit/RutwikFlower/rutwikFlower.obj");
	myModel = new Model((char *)"nanosuit/lavender1/Flower Hyacinth N100512.3DS");
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
	TranslateMatrix = translate(0.0f, 35.0f, -50.0f);
	scaleMatrix = scale(2.5f, 2.5f, 2.5f);
TranslateMatrix *= scaleMatrix;

	RotationMatrix = rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	//TranslateMatrix *= RotationMatrix; 

	RotationMatrix = rotate(270.0f, 1.0f, 0.0f, 0.0f);
	TranslateMatrix *= RotationMatrix;
	viewMatrix = lookat(vec3(xView, 35.0f, zView), vec3(xView, 35.0f, -100.0f), vec3(0.0f, 1.0f, 0.0f));
	//	RotationMatrix = rotate(angleRectangle, 1.0f, 0.0f, 0.0f);
	//RotationMatrix = rotate(angleRectangle, angleRectangle, angleRectangle);
	// perform necessary transformations
	modelMatrix = TranslateMatrix;

	glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);


	//glActiveTexture(GL_TEXTURE0);					
	//glBindTexture(GL_TEXTURE_2D, gTextureStone);	// 2. bind to the texture
	//glUniform1i(gTextureSamplerUniform, 0);
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
	SwapBuffers(ghdc);
	//angle1 += 0.2f;

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