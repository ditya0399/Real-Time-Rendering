#include<Windows.h>
#include<glew.h>               // This SHOULD be above GL.h header.
#include<gl/GL.h>
#include"Sphere.h"
#include<stdio.h>
#include"vmath.h"

#pragma comment(lib,"opengl32.lib")//for linking
#pragma comment(lib,"glew32.lib")//for linking
#pragma comment(lib,"Sphere.lib")

int vertices;
int coneVertices;
//Sphere
bool perVertexLighting = false;
bool perFragmentLighting = false;


GLfloat Vert[15000];

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
GLuint gShaderProgramObjectPerFragment;

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


GLuint vao_SpherePerFragment;
GLuint vbo_position_SpherePerFragment;
GLuint vbo_Normals_SpherePerFragment;
GLuint vbo_Element_SpherePerFragment;


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

GLuint gbLighting = 1;
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
GLuint LKeyPressedPerFragment = 0;


GLuint vao_cone;
GLuint vbo_Normals_Cone;
GLuint vbo_cone;


//	Per fragment
GLuint projectionUniformPerFragment;
GLuint modelUniformPerFragment;
GLuint ViewUniformPerFragment;
GLuint LDUniformPerFragment;
GLuint LAUniformPerFragment;
GLuint LSUniformPerFragment;
GLuint KDUniformPerFragment;
GLuint KAUniformPerFragment;
GLuint KSUniformPerFragment;
float MaterialShininessUniformPerFragment;
GLuint LightPositionUniformPerFragment;


GLfloat lightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightPosition[] = { 0.0f,00.0f,2.0f,1.0f };

GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialShininess = 50.0f;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void ToggleFullScreen(void);
void update(void);
void uninitialize(void);
void PerVertexLighting();
void PerFragmentLighting();
int Sphere(GLfloat, int, GLfloat **, GLfloat **, GLfloat **);
int Cylinder(GLfloat **, GLfloat, GLfloat);
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
		TEXT("AB : PerVertexPerFragment Lighting on a Sphere(Toggling)"),
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

	case WM_CHAR:

		switch (wParam)
		{
		case 'E':
		case 'e':
			ToggleFullScreen();
			DestroyWindow(hwnd);
			break;

		case 'H':
		case 'h':
			angleRectangle += 0.8f;
			break;

		case VK_ESCAPE:
			ToggleFullScreen();
			break;

		case 'L':
		case 'l':
			if (gbLighting == 0)
			{
				gbLighting = 1;
			}
			else
			{
				gbLighting = 0;
			}


			break;
		case 'V':
		case 'v':
			perFragmentLighting = false;
			break;
		case 'F':
		case 'f':
			perFragmentLighting = true;
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
	GLuint gVertexShaderObjectPerFragment;
	GLuint gFragmentShaderObjectPerFragment;


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
		"in vec3 vColor;" \
		"out vec3 out_color;" \
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
		"out vec3  phongADSLight;" \
		"void main(void)" \
		"{" \
		"out_color = vColor;" \
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
		"phongADSLight = ambient + diffuse + specular;" \
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
		"in vec3 out_color;" \
		"void main(void)" \
		"{" \
		"FragColor = vec4(vec3(out_color) * phongADSLight ,1.0);" \
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



	//Here we have done all the preparations of data transfer from CPU to GPU





	//////////////// PER - FRAGMENT ////////////////////

	//////////////////////////////// V E R T E X - S H A D E R //////////////////////////


	//Define Vertex Shader Object
	gVertexShaderObjectPerFragment = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	
	const GLchar *vertexShaderSourceCodePerFragment =

		"#version 430 core" \
		"\n"
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"in vec3 vColor;" \
		"out vec3 out_color;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform int u_LKeyPressed;" \
		"out vec3 tNorm;" \
		"out vec3 light_direction;" \
		"out vec3 viewVector;" \
		"uniform vec4 u_LightPosition;" \

		"void main(void)" \
		"{" \
		"out_color = vColor;" \
		"if(u_LKeyPressed == 1)" \
		"{" \
		"vec4 eye_coordinates = u_viewMatrix * u_modelMatrix *  vPosition;" \
		"tNorm = mat3(u_viewMatrix * u_modelMatrix) * vNormal;" \
		"light_direction = vec3(u_LightPosition - eye_coordinates);" \
		"viewVector = vec3(-eye_coordinates.xyz);" \
		"}" \
		"gl_Position = u_projectionMatrix * u_viewMatrix *u_modelMatrix * vPosition;" \
		"}";

	// GPU will run the above code. And GPU WILL RUN FOR PER VERTEX. If there are 1000 vertex. Then GPU will run this shader for
	//1000 times. We are Multiplying each vertex with the Model View Matrix.
	//And how does the GPU gets to know about at what offset the array has to be taken . Go to glVertexAttribPointer() in Display.
	// in = Input. 

	//p = &vertexShaderSourceCode;
		//Specify above source code to the vertex shader object
	glShaderSource(gVertexShaderObjectPerFragment, 1, (const GLchar **)&vertexShaderSourceCodePerFragment, NULL);

	//Compile the vertex shader 
	glCompileShader(gVertexShaderObjectPerFragment);

	//////////////// Error Checking//////////////////
	//Code for catching the errors 
	GLint iShaderCompileStatusPerFragment = 0;
	GLint iInfoLogLengthPerFragment = 0;
	GLchar *szInfoLogPerFragment = NULL;


	glGetShaderiv(gVertexShaderObjectPerFragment, GL_COMPILE_STATUS, &iShaderCompileStatusPerFragment);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObjectPerFragment, GL_INFO_LOG_LENGTH, &iInfoLogLengthPerFragment);
		if (iInfoLogLengthPerFragment > 0)
		{
			szInfoLogPerFragment = (GLchar *)malloc(iInfoLogLengthPerFragment);
			if (szInfoLogPerFragment != NULL)
			{
				GLsizei writtenPerFragment;
				glGetShaderInfoLog(gVertexShaderObjectPerFragment, iInfoLogLengthPerFragment, &writtenPerFragment, szInfoLogPerFragment);
				fprintf(gpLogFile, "VertexShader :\n");
				fprintf(gpLogFile, "%s\n", szInfoLogPerFragment);
				free(szInfoLogPerFragment);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}
	/////////////////    F R A G M E N T S H A D E R            //////////////////////////
	//Define Vertex Shader Object
	gFragmentShaderObjectPerFragment = glCreateShader(GL_FRAGMENT_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar *fragmentShaderSourceCodePerFragment =
		"#version 430 core" \
		"\n" \
		"in vec3 tNorm;" \
		"in vec3 light_direction;" \
		"in vec3 viewVector;" \
		"in vec3 out_color;" \
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
		"vec3 normalizedTNorm = normalize(tNorm);" \
		"vec3 normalizedLightDirection = normalize(light_direction);" \
		"vec3 normalizedViewVector = normalize(viewVector);" \
		"float tNormal_dot_lightDirection = max(dot(normalizedLightDirection,normalizedTNorm),0.0);" \
		"vec3 reflectionVector = reflect(-normalizedLightDirection,normalizedTNorm);" \

		"vec3 ambient = u_LAUniform * u_KAUniform;" \
		"vec3 diffuse = u_LDUniform * u_KDUniform * tNormal_dot_lightDirection;" \
		"vec3 specular = u_LSUniform * u_KSUniform * pow(max(dot(reflectionVector,normalizedViewVector),0.0),u_MaterialShininess);" \
		"phongADSLight = ambient + diffuse + specular;" \
		"}" \
		"else" \
		"{" \
		"phongADSLight = vec3(1.0,1.0,1.0);" \
		"}" \
		"FragColor = vec4(vec3(out_color) * phongADSLight ,1.0);" \
		"}";

	//FragColor = vec4(1,1,1,1) = White Color
	//this means here we are giving color to the Triangle.




	//Specify above source code to the vertex shader object
	glShaderSource(gFragmentShaderObjectPerFragment, 1, (const GLchar **)&fragmentShaderSourceCodePerFragment, NULL);

	//Compile the vertex shader 
	glCompileShader(gFragmentShaderObjectPerFragment);
	//Code for catching the errors 
		   /*iShaderCompileStatus = 0;
		   iInfoLogLength = 0;*/
	szInfoLogPerFragment = NULL;


	glGetShaderiv(gFragmentShaderObjectPerFragment, GL_COMPILE_STATUS, &iShaderCompileStatusPerFragment);
	if (iShaderCompileStatusPerFragment == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObjectPerFragment, GL_INFO_LOG_LENGTH, &iInfoLogLengthPerFragment);
		if (iInfoLogLengthPerFragment > 0)
		{
			szInfoLogPerFragment = (GLchar *)malloc(iInfoLogLengthPerFragment);
			if (szInfoLogPerFragment != NULL)
			{
				GLsizei written1PerFragment;
				glGetShaderInfoLog(gFragmentShaderObjectPerFragment, iInfoLogLengthPerFragment, &written1PerFragment, szInfoLogPerFragment);
				fprintf(gpLogFile, "%sFragment Shader : \n", szInfoLogPerFragment);
				free(szInfoLogPerFragment);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}
	// CREATE SHADER PROGRAM OBJECT
	gShaderProgramObjectPerFragment = glCreateProgram();
	//attach vertex shader to the gShaderProgramObject
	glAttachShader(gShaderProgramObjectPerFragment, gVertexShaderObjectPerFragment);

	//attach fragment shader to the gShaderProgramObject
	glAttachShader(gShaderProgramObjectPerFragment, gFragmentShaderObjectPerFragment);


	//Pre-Linking  binding to vertexAttributes
	glBindAttribLocation(gShaderProgramObjectPerFragment, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObjectPerFragment, AMC_ATTRIBUTE_NORMAL, "vNormal");
	//Here the above line means that we are linking the GPU's variable vPosition with the CPU's  enum member  i.e AMC_ATTRIBUTE_POSITION .
	//So whatever changes will be done in AMC_ATTRIBUTE_POSITION , those will also reflect in vPosition

	//RULE : ALWAYS BIND THE ATTRIBUTES BEFORE LINKING AND BIND THE UNIFORM AFTER LINKING.

	//Link the shader program 
	glLinkProgram(gShaderProgramObjectPerFragment);

	//Code for catching the errors 
	GLint iProgramLinkStatusPerFragment = 0;



	glGetProgramiv(gShaderProgramObjectPerFragment, GL_LINK_STATUS, &iProgramLinkStatusPerFragment);
	if (iProgramLinkStatusPerFragment == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObjectPerFragment, GL_INFO_LOG_LENGTH, &iInfoLogLengthPerFragment);
		if (iInfoLogLength > 0)
		{
			szInfoLogPerFragment = (GLchar *)malloc(iInfoLogLengthPerFragment);
			if (szInfoLogPerFragment != NULL)
			{
				GLsizei written3PerFragment;
				glGetProgramInfoLog(gShaderProgramObjectPerFragment, iInfoLogLengthPerFragment, &written3PerFragment, szInfoLogPerFragment);
				fprintf(gpLogFile, "%s\n", szInfoLogPerFragment);
				free(szInfoLogPerFragment);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}


	//POST Linking
	//Retrieving uniform locations 
	modelUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_modelMatrix");
	ViewUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_viewMatrix");
	projectionUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_projectionMatrix");

	LKeyPressedPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_LKeyPressed");
	LDUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_LDUniform");
	LAUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_LAUniform");
	LSUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_LSUniform");

	KDUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_KDUniform");
	KAUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_KAUniform");
	KSUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_KSUniform");
	LightPositionUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_LightPosition");
	MaterialShininessUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_MaterialShininess");


	const GLfloat triangleVertices[] =
	{
		// Perspective triangle (Front face)
		0.0f, 1.0f, 0.0f,		// Apex
		-1.0f, -1.0f, 1.0f,		// Left bottom
		1.0f, -1.0f, 1.0f,		// Right bottom
								// Perspective triangle (Right face)
		//0.0f, 1.0f, 0.0f,		// Apex
		//1.0f, -1.0f, 1.0f,		// Left bottom
		//1.0f, -1.0f, -1.0f,		// Right bottom
		//						// Perspective triangle (Back face)
		//0.0f, 1.0f, 0.0f,		// Apex
		//1.0f, -1.0f, -1.0f,		// Left bottom
		//-1.0f, -1.0f, -1.0f,	// Right bottom
		//						// Perspective triangle (Left face)
		//0.0f, 1.0f, 0.0f,		// Apex
		//-1.0f, -1.0f, -1.0f,	// Left bottom
		//-1.0f, -1.0f, 1.0f
	};

	const GLfloat pyramidNormals[] =
	{
		0.0f, 0.447214f, 0.894427f,
		0.0f, 0.447214f, 0.894427f,
		0.0f, 0.447214f, 0.894427f,

		//// Right face
		//0.894427f, 0.447214f, 0.0f,
		//0.894427f, 0.447214f, 0.0f,
		//0.894427f, 0.447214f, 0.0f,

		//// Back face
		//0.0f, 0.447214f, -0.894427f,
		//0.0f, 0.447214f, -0.894427f,
		//0.0f, 0.447214f, -0.894427f,

		//// Left face
		//-0.894427f, 0.447214f, 0.0f,
		//-0.894427f, 0.447214f, 0.0f,
		//-0.894427f, 0.447214f, 0.0f
	};

	GLfloat *Vertices = NULL;
	coneVertices = Cylinder(&Vertices, 0.9f, 1.0f);
	glGenVertexArrays(1, &vao_cone);
	glBindVertexArray(vao_cone);
	glGenBuffers(1, &vbo_cone);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cone);
	//in the above statement we have accesed the GL-ARRAY_BUFFER using vbo. Without it wouldn't be possible to get GL_ARRAY_BUFFER
	//For the sake of understanding . GL_ARRAY_BUFFER is in the GPU side ad=nd we have bind our CPU side vbo with it like a Pipe to get the access.
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
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


	glGenBuffers(1, &vbo_Normals_Cone);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Normals_Cone);
	 
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidNormals), pyramidNormals, GL_STATIC_DRAW);
	
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);



	GLfloat *SphereVertices = NULL;
	GLfloat *SphereTextures = NULL;
	GLfloat *SphereNormals = NULL;

	vertices = Sphere(0.5f, 50, &SphereVertices, &SphereTextures, &SphereNormals);
	/*getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();
*/

	//Basically we are giving the vertices coordinates in the above array
	////////////////////FOR TRIANGLE ///////////////// 
	//Create vao - Vertex array objects

	//////////////////////// FOR RECTANGLE //////////////// 

	glGenVertexArrays(1, &vao_Sphere);
	glBindVertexArray(vao_Sphere);
	glGenBuffers(1, &vbo_position_Sphere);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_Sphere);
	glBufferData(GL_ARRAY_BUFFER, 3 * vertices * sizeof(GLfloat), SphereVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	////NORMALS 
	glGenBuffers(1, &vbo_Normals_Sphere);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Normals_Sphere);
	glBufferData(GL_ARRAY_BUFFER, 6 * vertices * sizeof(GLfloat), SphereVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);



	////Elements
	//glGenBuffers(1, &vbo_Element_Sphere);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);



	///////////// PER FRAGMENT //////////

	glGenVertexArrays(1, &vao_SpherePerFragment);
	glBindVertexArray(vao_SpherePerFragment);
	glGenBuffers(1, &vbo_position_SpherePerFragment);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_SpherePerFragment);
	glBufferData(GL_ARRAY_BUFFER, 3 * vertices * sizeof(GLfloat), SphereVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	//NORMALS 
	glGenBuffers(1, &vbo_Normals_SpherePerFragment);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Normals_SpherePerFragment);
	glBufferData(GL_ARRAY_BUFFER, 6 * vertices * sizeof(GLfloat), SphereVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);



	////Elements
	//glGenBuffers(1, &vbo_Element_SpherePerFragment);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_SpherePerFragment);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);


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
	if (perFragmentLighting == true)
	{
	
		PerFragmentLighting();
	}
	else
	{
		PerVertexLighting();
	}

	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 RotationMatrix;
	mat4 TranslateMatrix;

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();

	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	TranslateMatrix = translate(0.05f, 0.2f, -2.5f);
	TranslateMatrix *= scale(0.3f, 0.3f, 0.3f);
	//	RotationMatrix = rotate(angleRectangle, 1.0f, 0.0f, 0.0f);
	RotationMatrix = rotate(angleRectangle, angleRectangle, angleRectangle);
	// perform necessary transformations
	modelMatrix = TranslateMatrix;
	if (perFragmentLighting == true)
	{
		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);

	}
	else
	{
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	}
	glBindVertexArray(vao_Sphere);
	glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 0.4f, 0.9f, 0.2f);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3 * vertices);
	glBindVertexArray(0);

	////////

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();

	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	TranslateMatrix = translate(0.05f, 0.19f, -2.3f);
	TranslateMatrix *= scale(0.04f, 0.04f, 0.04f);
	//	RotationMatrix = rotate(angleRectangle, 1.0f, 0.0f, 0.0f);
	RotationMatrix = rotate(angleRectangle, angleRectangle, angleRectangle);
	// perform necessary transformations
	modelMatrix = TranslateMatrix;
	if (perFragmentLighting == true)
	{
		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);

	}
	else
	{
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	}
	glBindVertexArray(vao_Sphere);
	glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 0.0f, 0.0f);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3 * vertices);
	glBindVertexArray(0);

	/////


	///////////////////////////////////////////
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();

	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	TranslateMatrix = translate(-0.22f, -0.16f, -2.5f);
	TranslateMatrix *= scale(0.65f, 0.65f, 0.6f);
	RotationMatrix = rotate(angleRectangle, 0.0f, angleRectangle);
//	RotationMatrix = rotate(angleRectangle, angleRectangle, angleRectangle);
	// perform necessary transformations
	modelMatrix = TranslateMatrix * RotationMatrix;
	//modelMatrix *= translate(0.0f, 0.8f, 0.0f);
	if (perFragmentLighting == true)
	{
		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);

	}
	else
	{
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	}
	glBindVertexArray(vao_Sphere);
	glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 0.8f, 0.8f, 0.2f);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3 * vertices);
	glBindVertexArray(0);


	///////// CONE /////////
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();

	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	TranslateMatrix = translate(0.13f, 0.23f, -2.8f);
//	TranslateMatrix *= scale(0.1f, 0.1f, 0.5f);
	RotationMatrix = rotate(20.0f, 0.0f, 0.0f);
	//TranslateMatrix *= rotate(0.0f, 0.0f, 0.0f);
	TranslateMatrix *= scale(0.1f, 0.08f, 0.5f);
	//	RotationMatrix = rotate(angleRectangle, angleRectangle, angleRectangle);
		// perform necessary transformations
	modelMatrix = TranslateMatrix * RotationMatrix;
	//modelMatrix *= translate(0.0f, 0.8f, 0.0f);
	if (perFragmentLighting == true)
	{
		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);

	}
	else
	{
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	}
	glBindVertexArray(vao_cone);
	glDrawArrays(GL_TRIANGLE_FAN, 0,  3);
	glBindVertexArray(0);



	/////////////////////////////////////////////
	//modelMatrix = mat4::identity();
	//viewMatrix = mat4::identity();

	//RotationMatrix = mat4::identity();
	//TranslateMatrix = mat4::identity();
	//TranslateMatrix = translate(0.2f, 0.22f, -2.0f);
	//TranslateMatrix *= scale(0.12f, 0.05f, 0.6f);
	////RotationMatrix = rotate(0.0f, 0.0f, angleRectangle);
	////	RotationMatrix = rotate(angleRectangle, angleRectangle, angleRectangle);
	//	// perform necessary transformations
	//modelMatrix = TranslateMatrix * RotationMatrix;
	////modelMatrix *= translate(0.0f, 0.8f, 0.0f);
	//if (perFragmentLighting == true)
	//{
	//	glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
	//	glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
	//	glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);

	//}
	//else
	//{
	//	glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
	//	glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
	//	glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	//}
	//glBindVertexArray(vao_Sphere);
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 3 * vertices);
	//glBindVertexArray(0);

	/////////
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();

	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	TranslateMatrix = translate(-0.22f, -0.48f, -2.7f);
	TranslateMatrix *= scale(0.04f, 0.2f, 0.6f);
	RotationMatrix = rotate(0.0f, 0.0f, angleRectangle);
	//	RotationMatrix = rotate(angleRectangle, angleRectangle, angleRectangle);
		// perform necessary transformations
	modelMatrix = TranslateMatrix * RotationMatrix;
	//modelMatrix *= translate(0.0f, 0.8f, 0.0f);
	if (perFragmentLighting == true)
	{
		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);
		
	}
	else
	{
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	}
	glBindVertexArray(vao_Sphere);
	glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 0.6f, 0.3f, 0.1f);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3 * vertices);
	glBindVertexArray(0);


	/////////
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();

	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	TranslateMatrix = translate(-0.39f, -0.47f, -2.7f);
	TranslateMatrix *= scale(0.04f, 0.2f, 0.6f);
	//RotationMatrix = rotate(0.0f, 0.0f, angleRectangle);
	//	RotationMatrix = rotate(angleRectangle, angleRectangle, angleRectangle);
		// perform necessary transformations
	modelMatrix = TranslateMatrix * RotationMatrix;
	//modelMatrix *= translate(0.0f, 0.8f, 0.0f);
	if (perFragmentLighting == true)
	{
		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);

	}
	else
	{
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	}
	glBindVertexArray(vao_Sphere);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3 * vertices);
	glBindVertexArray(0);
	

	//Legs 
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();

	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	TranslateMatrix = translate(-0.43f, -0.57f, -2.7f);
	TranslateMatrix *= rotate(182.4f, 0.0f, 182.4f);
	TranslateMatrix *= scale(0.001f, 0.04f, 0.6f);
	RotationMatrix = rotate(0.0f, 0.0f, angleRectangle);
	//fprintf(gpLogFile, "%f\n", angleRectangle);
	//	RotationMatrix = rotate(angleRectangle, angleRectangle, angleRectangle);
		// perform necessary transformations
	modelMatrix = TranslateMatrix;
	//modelMatrix *= translate(0.0f, 0.8f, 0.0f);
	if (perFragmentLighting == true)
	{
		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);

	}
	else
	{
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	}
	glBindVertexArray(vao_Sphere);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3 * vertices);
	glBindVertexArray(0);

	
	//
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();

	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	TranslateMatrix = translate(-0.43f, -0.57f, -2.7f);
	TranslateMatrix *= rotate(123.0f, 0.0f, -123.0f);
	TranslateMatrix *= scale(0.04f, 0.01f, 0.2f);
	RotationMatrix = rotate(0.0f, 0.0f, angleRectangle);
	fprintf(gpLogFile, "%f\n", angleRectangle);
	//	RotationMatrix = rotate(angleRectangle, angleRectangle, angleRectangle);
		// perform necessary transformations
	modelMatrix = TranslateMatrix;
	//modelMatrix *= translate(0.0f, 0.8f, 0.0f);
	if (perFragmentLighting == true)
	{
		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);

	}
	else
	{
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	}
	glBindVertexArray(vao_Sphere);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3 * vertices);
	glBindVertexArray(0);


	////////// Right leg
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();

	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	TranslateMatrix = translate(-0.23f, -0.57f, -2.7f);
	TranslateMatrix *= rotate(123.0f, 0.0f, -123.0f);
	TranslateMatrix *= scale(0.04f, 0.01f, 0.2f);
	RotationMatrix = rotate(0.0f, 0.0f, angleRectangle);
	fprintf(gpLogFile, "%f\n", angleRectangle);
	//	RotationMatrix = rotate(angleRectangle, angleRectangle, angleRectangle);
		// perform necessary transformations
	modelMatrix = TranslateMatrix;
	//modelMatrix *= translate(0.0f, 0.8f, 0.0f);
	if (perFragmentLighting == true)
	{
		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);

	}
	else
	{
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	}
	glBindVertexArray(vao_Sphere);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3 * vertices);
	glBindVertexArray(0);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();

	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	TranslateMatrix = translate(-0.23f, -0.57f, -2.7f);
	TranslateMatrix *= rotate(182.4f, 0.0f, 182.4f);
	TranslateMatrix *= scale(0.001f, 0.04f, 0.6f);
	RotationMatrix = rotate(0.0f, 0.0f, angleRectangle);
	//fprintf(gpLogFile, "%f\n", angleRectangle);
	//	RotationMatrix = rotate(angleRectangle, angleRectangle, angleRectangle);
		// perform necessary transformations
	modelMatrix = TranslateMatrix;
	//modelMatrix *= translate(0.0f, 0.8f, 0.0f);
	if (perFragmentLighting == true)
	{
		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);

	}
	else
	{
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	}
	glBindVertexArray(vao_Sphere);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3 * vertices);
	glBindVertexArray(0);


	glUseProgram(0);


	SwapBuffers(ghdc);

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

void PerVertexLighting()
{
	glUseProgram(gShaderProgramObject);

	//fprintf(gpLogFile, "gbLighting is TRUE!\n");

	if (gbLighting == 1)
	{
		glUniform1i(LKeyPressed, 1);
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
		glUniform1i(LKeyPressed, 0);
	}
	

	// intialize above matrices to identity

	////////  RECTANGLE //////////////
	
	// unuse program
	//glUseProgram(0);

}
void PerFragmentLighting()
{
	glUseProgram(gShaderProgramObjectPerFragment);

	//fprintf(gpLogFile, "gbLighting is TRUE!\n");
	if (gbLighting == 1)
	{
		glUniform1i(LKeyPressedPerFragment, 1);
		glUniform3fv(LDUniformPerFragment, 1, lightDiffuse);
		glUniform3fv(LAUniformPerFragment, 1, lightAmbient);
		glUniform3fv(LSUniformPerFragment, 1, lightSpecular);

		glUniform3fv(KDUniformPerFragment, 1, materialDiffuse);
		glUniform3fv(KAUniformPerFragment, 1, materialAmbient);
		glUniform3fv(KSUniformPerFragment, 1, materialSpecular);
		glUniform1f(MaterialShininessUniformPerFragment, materialShininess);
		glUniform4fv(LightPositionUniformPerFragment, 1, lightPosition);
	}
	else
	{
		glUniform1i(LKeyPressedPerFragment, 0);

	}
	
	
	
	// unuse program
	

}
void update(void)
{


}


int Sphere(GLfloat r, int n, GLfloat **Vertices, GLfloat **Normals, GLfloat **Textures)
{

	int i, j;
	int NoOfVertices = 0;
	GLdouble phi1, phi2, theta, s, t;
	GLfloat ex, ey, ez, px, py, pz;
	GLfloat x0 = 0.0f;
	GLfloat y0 = 0.0f;
	GLfloat z0 = 0.0f;
	*Vertices = (GLfloat *)malloc(3 * sizeof(GLfloat) * n * (n + 1) * 2);
	*Normals = (GLfloat *)malloc(3 * sizeof(GLfloat) * n * (n + 1) * 2);
	*Textures = (GLfloat *)malloc(2 * sizeof(GLfloat) * n * (n + 1) * 2);
	GLfloat *Vert = *Vertices;
	GLfloat *TexCoords = *Textures;
	GLfloat *NormalsSphere = *Normals;
	if (Vertices)
	{
		for (j = 0; j < n; j++)
		{

			phi1 = j * M_PI * 2 / n;
			phi2 = (j + 1) * M_PI * 2 / n;
			for (i = 0; i <= n; i++)
			{
				theta = i * M_PI / n;
				ex = sin(theta) * cos(phi2);
				ey = sin(theta) * sin(phi2);
				ez = cos(theta);
				px = r * ex;
				py = r * ey;
				pz = r * ez;

				NormalsSphere[(NoOfVertices * 3) + 0] = ex;
				NormalsSphere[(NoOfVertices * 3) + 1] = ey;
				NormalsSphere[(NoOfVertices * 3) + 2] = ez;

				s = phi2 / (M_PI * 2);
				t = 1 - (theta / M_PI);
				TexCoords[(NoOfVertices * 2) + 0] = s;
				TexCoords[(NoOfVertices * 2) + 1] = t;

				Vert[(NoOfVertices * 3) + 0] = px;
				Vert[(NoOfVertices * 3) + 1] = py;
				Vert[(NoOfVertices * 3) + 2] = pz;

				///////////////////////





				ex = sin(theta) * cos(phi1);
				ey = sin(theta) * sin(phi1);
				ez = cos(theta);
				px = r * ex;
				py = r * ey;
				pz = r * ez;
				NormalsSphere[(NoOfVertices * 3) + 3] = ex;
				NormalsSphere[(NoOfVertices * 3) + 4] = ey;
				NormalsSphere[(NoOfVertices * 3) + 5] = ez;


				//
				s = phi1 / (M_PI * 2);
				t = 1 - (theta / M_PI);
				TexCoords[(NoOfVertices * 2) + 2] = s;
				TexCoords[(NoOfVertices * 2) + 3] = t;

				//
				Vert[(NoOfVertices * 3) + 3] = px;
				Vert[(NoOfVertices * 3) + 4] = py;
				Vert[(NoOfVertices * 3) + 5] = pz;
				NoOfVertices = NoOfVertices + 2;
			}
		}
	}
	fprintf(gpLogFile, "No of vertices %d\n", NoOfVertices);
	return NoOfVertices;

}
int Cylinder(GLfloat **Vertices, GLfloat radius, GLfloat height)
{
	radius = 0.5f;
	int NoOfVertices = 0;
	GLfloat x = 0.0;
	GLfloat y = 0.0;
	GLfloat angle = 0.0;
	GLfloat angle_stepsize = 0.15;

	//*Vertices = (GLfloat *)malloc(3 * sizeof(GLfloat) *18000);
	//GLfloat *Vert = *Vertices;
	angle = 0.0;
	while (height <= 2.0f)
	{
		while (angle < 2 * M_PI)
		{
			x = radius * cos(angle);
			y = radius * sin(angle);

			Vert[(NoOfVertices * 3) + 0] = x;
			Vert[(NoOfVertices * 3) + 1] = y;
			Vert[(NoOfVertices * 3) + 2] = height;

			/*Vert[(NoOfVertices * 3) + 3] = x;
			Vert[(NoOfVertices * 3) + 4] = y;
			Vert[(NoOfVertices * 3) + 5] = 0.0f;
	*/
			angle = angle + angle_stepsize;
			NoOfVertices = NoOfVertices + 1;

		}
		angle = 0.0f;
		radius = radius - 0.01f;
		height += 0.02f;
	}

	fprintf(gpLogFile, "The number of vertices are %d\n", NoOfVertices);
	return NoOfVertices;
}