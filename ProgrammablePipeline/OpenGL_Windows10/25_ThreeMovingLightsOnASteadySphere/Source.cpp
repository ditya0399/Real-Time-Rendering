#include<Windows.h>
#include<glew.h>               // This SHOULD be above GL.h header.
#include<gl/GL.h>
#include<string.h>
#include<stdio.h>
#include"vmath.h"
#include"Sphere.h"

#pragma comment(lib,"Sphere.lib")
#pragma comment(lib,"opengl32.lib")//for linking
#pragma comment(lib,"glew32.lib")//for linking


struct Light
{
	GLfloat Ambient[4];
	GLfloat Diffuse[4];
	GLfloat Specular[4];
	GLfloat Position[4];
};
Light lights[3];


GLfloat RedLightRotate = 0.0f;
GLfloat GreenLightRotate = 0.0f;
GLfloat BlueLightRotate = 0.0f;


GLfloat MaterialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat MaterialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat MaterialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat MaterialShinines = 50.0f;



//Sphere

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

int gNumVertices;
int gNumElements;

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

GLuint vao_Sphere;
GLuint vbo_position_Sphere;
GLuint vbo_Normals_Sphere;
GLuint vbo_Element_Sphere;


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

GLuint gbLighting = 0;
//Lighting Variables
GLuint projectionUniform;
GLuint modelUniform;
GLuint ViewUniform;
GLuint LDUniform;
GLuint LAUniform;
GLuint LSUniform;


GLuint KDUniform;
GLuint KAUniform;
GLuint KSUniform;
float MaterialShininessUniform;
GLuint LightPositionUniform;
GLuint LKeyPressed = 0;

//Light 1
GLuint LDUniformL1;
GLuint LAUniformL1;
GLuint LSUniformL1;
GLuint KDUniformL1;
GLuint KAUniformL1;
GLuint KSUniformL1;
GLuint LightPositionUniformL1;

//Light 2
GLuint LDUniformL2;
GLuint LAUniformL2;
GLuint LSUniformL2;
GLuint KDUniformL2;
GLuint KAUniformL2;
GLuint KSUniformL2;
GLuint LightPositionUniformL2;





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
		TEXT("Red and Blue Light on a Pyramid"),
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

	case WM_CHAR:

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

	lights[0].Ambient[0] = 0.0f;
	lights[0].Ambient[1] = 0.0f;
	lights[0].Ambient[2] = 0.0f;
	lights[0].Ambient[3] = 1.0f;

	lights[0].Diffuse[0] = 1.0f;
	lights[0].Diffuse[1] = 0.0f;
	lights[0].Diffuse[2] = 0.0f;
	lights[0].Diffuse[3] = 1.0f;

	lights[0].Specular[0] = 1.0f;
	lights[0].Specular[1] = 0.0f;
	lights[0].Specular[2] = 0.0f;
	lights[0].Specular[3] = 1.0f;

	lights[0].Position[0] = 0.0f;
	lights[0].Position[1] = 0.0f;
	lights[0].Position[2] = 0.0f;
	lights[0].Position[3] = 1.0f;



	//light 1

	lights[1].Ambient[0] = 0.0f;
	lights[1].Ambient[1] = 0.0f;
	lights[1].Ambient[2] = 0.0f;
	lights[1].Ambient[3] = 1.0f;

	lights[1].Diffuse[0] = 0.0f;
	lights[1].Diffuse[1] = 1.0f;
	lights[1].Diffuse[2] = 0.0f;
	lights[1].Diffuse[3] = 1.0f;

	lights[1].Specular[0] = 0.0f;
	lights[1].Specular[1] = 1.0f;
	lights[1].Specular[2] = 0.0f;
	lights[1].Specular[3] = 1.0f;

	lights[1].Position[0] = 0.0f;
	lights[1].Position[1] = 0.0f;
	lights[1].Position[2] = 0.0f;
	lights[1].Position[3] = 1.0f;

	//light 2
	

	lights[2].Ambient[0] = 0.0f;
	lights[2].Ambient[1] = 0.0f;
	lights[2].Ambient[2] = 0.0f;
	lights[2].Ambient[3] = 1.0f;

	lights[2].Diffuse[0] = 0.0f;
	lights[2].Diffuse[1] = 0.0f;
	lights[2].Diffuse[2] = 1.0f;
	lights[2].Diffuse[3] = 1.0f;

	lights[2].Specular[0] = 0.0f;
	lights[2].Specular[1] = 0.0f;
	lights[2].Specular[2] = 1.0f;
	lights[2].Specular[3] = 1.0f;

	lights[2].Position[0] = 0.0f;
	lights[2].Position[1] = 0.0f;
	lights[2].Position[2] = 0.0f;
	lights[2].Position[3] = 1.0f;





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
		"in vec3 vNormal;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform int u_LKeyPressed;" \
		"uniform vec3 u_LDUniform;" \
		"uniform vec3 u_LAUniform;" \
		"uniform vec3 u_LSUniform;" \
		"uniform vec3 u_KDUniform;" \
		"uniform vec3 u_KAUniform;" \
		"uniform vec3 u_KSUniform;" \
		"uniform float u_MaterialShininess;" \
		"uniform vec4 u_LightPosition;" \
		"uniform vec3 u_LDUniformL1;" \
		"uniform vec3 u_LAUniformL1;" \
		"uniform vec3 u_LSUniformL1;" \
		"uniform vec4 u_LightPositionL1;" \
		"uniform vec3 u_LDUniformL2;" \
		"uniform vec3 u_LAUniformL2;" \
		"uniform vec3 u_LSUniformL2;" \
		"uniform vec4 u_LightPositionL2;" \
		"out vec3  phongADSLight;" \
		"void main(void)" \
		"{" \
		"if(u_LKeyPressed == 1)" \
		"{" \
		"vec4 eye_coordinates = u_viewMatrix * u_modelMatrix *  vPosition;" \
		"vec3 tNorm = normalize(mat3(u_viewMatrix * u_modelMatrix) * vNormal);" \

		"vec3 light_direction = normalize(vec3(u_LightPosition - eye_coordinates));" \
		"float tNormal_dot_lightDirection = max(dot(light_direction,tNorm),0.0);" \
		"vec3 reflectionVector = reflect(-light_direction,tNorm);" \
		"vec3 viewVector = normalize(vec3(-eye_coordinates.xyz));" \
		"vec3 ambient = u_LAUniform * u_KAUniform;" \
		"vec3 diffuse = u_LDUniform * u_KDUniform * tNormal_dot_lightDirection;" \
		"vec3 specular = u_LSUniform * u_KSUniform * pow(max(dot(reflectionVector,viewVector),0.0),u_MaterialShininess);" \



		"vec3 light_directionL1 = normalize(vec3(u_LightPositionL1 - eye_coordinates));" \
		"float tNormal_dot_lightDirectionL1 = max(dot(light_directionL1,tNorm),0.0);" \
		"vec3 reflectionVectorL1 = reflect(-light_directionL1,tNorm);" \
		"vec3 ambientL1 = u_LAUniformL1 * u_KAUniform;" \
		"vec3 diffuseL1 = u_LDUniformL1 * u_KDUniform * tNormal_dot_lightDirectionL1;" \
		"vec3 specularL1 = u_LSUniformL1 * u_KSUniform * pow(max(dot(reflectionVectorL1,viewVector),0.0),u_MaterialShininess);" \

		"vec3 light_directionL2 = normalize(vec3(u_LightPositionL2 - eye_coordinates));" \
		"float tNormal_dot_lightDirectionL2 = max(dot(light_directionL2,tNorm),0.0);" \
		"vec3 reflectionVectorL2 = reflect(-light_directionL2,tNorm);" \
		"vec3 ambientL2 = u_LAUniformL2 * u_KAUniform;" \
		"vec3 diffuseL2 = u_LDUniformL2 * u_KDUniform * tNormal_dot_lightDirectionL2;" \
		"vec3 specularL2 = u_LSUniformL2 * u_KSUniform * pow(max(dot(reflectionVectorL2,viewVector),0.0),u_MaterialShininess);" \
		"phongADSLight = ambient + diffuse + specular +ambientL1 + diffuseL1 + specularL1 + ambientL2 + diffuseL2 + specularL2 ;" \
		"}" \
		"else" \
		"{" \
		"phongADSLight = vec3(1.0,1.0,1.0);" \
		"}" \
		"gl_Position = u_projectionMatrix * u_viewMatrix *u_modelMatrix * vPosition;" \
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
	/////////////////    F R A G M E N T S H A D E R            //////////////////////////
	//Define Vertex Shader Object
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar *fragmentShaderSourceCode =
		"#version 430 core" \
		"\n" \
		"in vec3 phongADSLight;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = vec4(phongADSLight ,1.0);" \
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
	// CREATE SHADER PROGRAM OBJECT
	gShaderProgramObject = glCreateProgram();
	//attach vertex shader to the gShaderProgramObject
	glAttachShader(gShaderProgramObject, gVertexShaderObject);

	//attach fragment shader to the gShaderProgramObject
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);


	//Pre-Linking  binding to vertexAttributes
	glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_NORMAL, "vNormal");
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
	LightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_LightPosition");
	MaterialShininessUniform = glGetUniformLocation(gShaderProgramObject, "u_MaterialShininess");
	LDUniformL1 = glGetUniformLocation(gShaderProgramObject, "u_LDUniformL1");
	LAUniformL1 = glGetUniformLocation(gShaderProgramObject, "u_LAUniformL1");
	LSUniformL1 = glGetUniformLocation(gShaderProgramObject, "u_LSUniformL1");


	LightPositionUniformL1 = glGetUniformLocation(gShaderProgramObject, "u_LightPositionL1");

	LDUniformL2 = glGetUniformLocation(gShaderProgramObject, "u_LDUniformL2");
	LAUniformL2 = glGetUniformLocation(gShaderProgramObject, "u_LAUniformL2");
	LSUniformL2 = glGetUniformLocation(gShaderProgramObject, "u_LSUniformL2");
	LightPositionUniformL2 = glGetUniformLocation(gShaderProgramObject, "u_LightPositionL2");
	//Here we have done all the preparations of data transfer from CPU to GPU


	//Basically we are giving the vertices coordinates in the above array
	////////////////////FOR TRIANGLE ///////////////// 
	//Create vao - Vertex array objects

	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();


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

	const GLfloat pyramidNormals[] =
	{
		0.0f, 0.447214f, 0.894427f,
		0.0f, 0.447214f, 0.894427f,
		0.0f, 0.447214f, 0.894427f,

		// Right face
		0.894427f, 0.447214f, 0.0f,
		0.894427f, 0.447214f, 0.0f,
		0.894427f, 0.447214f, 0.0f,

		// Back face
		0.0f, 0.447214f, -0.894427f,
		0.0f, 0.447214f, -0.894427f,
		0.0f, 0.447214f, -0.894427f,

		// Left face
		-0.894427f, 0.447214f, 0.0f,
		-0.894427f, 0.447214f, 0.0f,
		-0.894427f, 0.447214f, 0.0f
	};
	glGenVertexArrays(1, &vao_Sphere);
	glBindVertexArray(vao_Sphere);
	glGenBuffers(1, &vbo_position_Sphere);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_Sphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	//NORMALS 
	glGenBuffers(1, &vbo_Normals_Sphere);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Normals_Sphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);



	//Elements
	glGenBuffers(1, &vbo_Element_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);






	//Depth Lines
	glClearDepth(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

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



void display(void)
{


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use shader program
	glUseProgram(gShaderProgramObject);
	if (gbLighting == 1)
	{
		//fprintf(gpLogFile, "gbLighting is TRUE!\n");
		glUniform1i(LKeyPressed, gbLighting);
		glUniform3fv(LDUniform, 1, lights[0].Diffuse);
		glUniform3fv(LAUniform, 1, lights[0].Ambient);
		glUniform3fv(LSUniform, 1, lights[0].Specular);

		glUniform3fv(LDUniformL1, 1, lights[1].Diffuse);
		glUniform3fv(LAUniformL1, 1, lights[1].Ambient);
		glUniform3fv(LSUniformL1, 1, lights[1].Specular);

		glUniform3fv(LDUniformL2, 1, lights[2].Diffuse);
		glUniform3fv(LAUniformL2, 1, lights[2].Ambient);
		glUniform3fv(LSUniformL2, 1, lights[2].Specular);


		glUniform3fv(KDUniform, 1, MaterialDiffuse);
		glUniform3fv(KAUniform, 1, MaterialAmbient);
		glUniform3fv(KSUniform, 1, MaterialSpecular);
		glUniform1f(MaterialShininessUniform, MaterialShinines);
		lights[0].Position[0] = 0.0f;
		lights[0].Position[1] = sin(RedLightRotate) * 50.0f;
		lights[0].Position[2] = cos(RedLightRotate) * 50.0f - 3.0f;

		glUniform4fv(LightPositionUniform, 1, lights[0].Position);

		lights[1].Position[0] = sin(GreenLightRotate) * 100.0f;
		lights[1].Position[1] = 0.0f;
		lights[1].Position[2] = cos(GreenLightRotate) * 100.0f - 3.0f;

		glUniform4fv(LightPositionUniformL1, 1, lights[1].Position);

		lights[2].Position[0] = cos(BlueLightRotate) * 100.0f;
		lights[2].Position[1] = sin(BlueLightRotate) * 100.0f;
		lights[2].Position[2] = -3.0f;
		glUniform4fv(LightPositionUniformL2, 1, lights[2].Position);
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
	// intialize above matrices to identity

	////////  RECTANGLE //////////////
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();

	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	TranslateMatrix = translate(0.0f, 0.0f, -2.0f);
	//	RotationMatrix = rotate(angleRectangle, 1.0f, 0.0f, 0.0f);
	RotationMatrix = rotate(angleRectangle, 0.0f, 1.0f, 0.0f);
	// perform necessary transformations
	modelMatrix = TranslateMatrix;

	glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);




	glBindVertexArray(vao_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	// unuse program
	glUseProgram(0);
	SwapBuffers(ghdc);
	angleRectangle = angleRectangle - 0.1f;
	if (angleRectangle <= -360.0f)
		angleRectangle = 0.0f;

	GreenLightRotate = GreenLightRotate + 0.002f;;
	RedLightRotate = RedLightRotate + 0.002f;
	BlueLightRotate = BlueLightRotate + 0.02f;
}

void uninitialize(void)
{

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
	/*if (vbo_Normals_rectangle)
	{
		glDeleteBuffers(1, &vbo_Normals_rectangle);
		vbo_Normals_rectangle = 0;
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