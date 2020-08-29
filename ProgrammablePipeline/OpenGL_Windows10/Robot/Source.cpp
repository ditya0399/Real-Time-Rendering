#include<Windows.h>
#include<glew.h>               // This SHOULD be above GL.h header.
#include<gl/GL.h>
#include"Sphere.h"
#include<stdio.h>
#include"vmath.h"
#include"Cylinder.h"
#include"OGL_STACK.H"

#pragma comment(lib,"opengl32.lib")//for linking
#pragma comment(lib,"glew32.lib")//for linking
#pragma comment(lib,"Sphere.lib")

//Hand Variables

//Variables to Turn the body
int Turn = 0;

GLfloat turnAngleRightHand = 0;
GLfloat turnAngleLeftHand = 0;

//Sphere
int vertices;
bool perVertexLighting = true;
bool perFragmentLighting = false;


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
GLfloat lightPosition[] = { 0.0f,0.0f,100.0f,1.0f };

GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialShininess = 50.0f;




//////////////////       NEW  --- VARIABLES       ///////////////


GLfloat shoulder = -90;
GLfloat elbow = -30;


GLfloat leftLegUpper = -90.0f;
GLfloat leftLegLower = 30.0f;
GLfloat  RightLegUpper = -90.0f;
GLfloat  RightLegLower = 30.0f;


GLfloat LeftHandShoulder = -90.0f;
GLfloat LeftHandElbow = -0.0f;


GLfloat RightHandShoulder = -90.0f;
GLfloat RightHandElbow = 0.0f;



///////// COUNTS for Leg ////////
int RightLeg = 1;



GLfloat walkingTranslate = -8.0f;

int Walking = 0;



////////SITTING ////
int Sitting = 0;
int SittingUP = 0;
GLfloat UpperChestSitting = 0.0f;
GLfloat FaceSitting = 2.0f;

GLfloat LeftHandShoulderSitting = 1.0f;
GLfloat RightHandShoulderSitting = 1.0f;


//////////////////////////////////
/////////////Skipping

int Skipping = 0;
GLfloat UpperChestSkippingY = 0.0f;
GLfloat FaceSkippingY = 2.0f;
GLfloat LegSkippingY = -1.6f;
GLfloat HandSkippingY = 1.0f;

int Jump = 1;
//// Swimming////
int swimming = 0;
int smimmingLegsToggle = 0;


//Running
int Running = 0;










LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void ToggleFullScreen(void);
void update(void);
void uninitialize(void);
void PerVertexLighting();
void PerFragmentLighting();
void UpdateWalking();
void UpdateSitting();
void UpdateSkipping();
void UpdateRunning();
void UpdateSwimming();
int Sphere(GLfloat, int, GLfloat **, GLfloat **, GLfloat **);
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
	ToggleFullScreen();
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
		createStackFile();
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
		case 'R':
		case 'r':
			Sitting = 0;
			Skipping = 0;
			swimming = 0;
			Walking = 0;
			Turn = 0;
			if (Running == 0)
			{

				RightLeg = 1;
				walkingTranslate = -8.0f;
				leftLegUpper = -90.0f;
				leftLegLower = 30.0f;
				RightLegUpper = -90.0f;
				RightLegLower = 30.0f;


				LeftHandShoulder = -90.0f;
				LeftHandElbow = -30.0f;


				RightHandShoulder = -90.0f;
				RightHandElbow = -30.0f;
				Running  = 1;
			}
			else
				Running = 0;

			break;
		case 'M':
		case 'm':
			if (swimming == 0)
			{
				swimming = 1;
				turnAngleLeftHand = 94.044;
				leftLegLower = 30.0f;
				RightLegLower = 30.0f;
				//LeftHandElbow = 30.0f;
				//RightHandElbow = -30.0f;
			}
			else
			swimming = 0;

			break;
		case 'W':
		case 'w':
			Sitting = 0;
			Skipping = 0;
			swimming = 0;
			Running = 0;
			Turn = 0;
			if (Walking == 0)
			{
				RightLeg = 1;
				walkingTranslate = -8.0f;
				leftLegUpper = -90.0f;
				 leftLegLower = 30.0f;
				  RightLegUpper = -90.0f;
				  RightLegLower = 30.0f;


				 LeftHandShoulder = -90.0f;
				 LeftHandElbow = -30.0f;


				 RightHandShoulder = -90.0f;
				 RightHandElbow = -30.0f;
				Walking = 1;
			}
			else
				Walking = 0;
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
		case 'U':
			case 'u':
				if (swimming == 0)
					swimming = 1;
				else
					swimming = 0;
				break;



		case 'V':
		case 'v':
			perFragmentLighting = false;
			break;
		case 'F':
		case 'f':
			perFragmentLighting = true;
			break;
		case 'S':
		case 's':
			//Sitting = 0;
			Skipping = 0;
			swimming = 0;
			Running = 0;
			Turn = 0;
			Walking = 0;
			if (Sitting == 0)
			{
				/* UpperChestSitting = 0.3f;
				 FaceSitting = 2.0f;

				 LeftHandShoulderSitting = 1.0f;
				 RightHandShoulderSitting = 1.0f;




				 leftLegUpper = -90.0f;
				 leftLegLower = 30.0f;
				  RightLegUpper = -90.0f;
				  RightLegLower = 30.0f;


				 LeftHandShoulder = -90.0f;
				 LeftHandElbow = -30.0f;


				 RightHandShoulder = -90.0f;
				 RightHandElbow = -30.0f;*/

				 leftLegUpper = -90.0f;
				 leftLegLower = -90.0f;
				  RightLegUpper = -90.0f;
				  RightLegLower = 30.0f;


				 LeftHandShoulder = -90.0f;
				 LeftHandElbow = -0.0f;


				 RightHandShoulder = -90.0f;
				 RightHandElbow = 0.0f;


				walkingTranslate = -8.0f;
				Sitting = 1;
			}
			else if(Sitting == 1)
			{
			
				Sitting = 2;
			}
			else if (Sitting == 2)
			{
				Sitting = 1;
			}
			/*shoulder = (int)(shoulder + 3) % 360;
			leftLegUpper = (int)(leftLegUpper + 3) % 360;
			leftLegLower = (int)(leftLegLower + 1.5) % 360;


			RightLegUpper = (int)(RightLegUpper - 3) % 360;
			RightLegLower = (int)(RightLegLower - 1.5) % 360;*/

			break;
		case 'K':
		case 'k':
			Running = 0;
			Sitting = 0;
			Walking = 0;
			swimming = 0;
			Turn = 0;
			walkingTranslate = -8.0f;
			if (Skipping == 0)
			{
				LeftHandShoulder = -110.0f;
				RightLegUpper = -90.0F;
				RightLegLower = 30.0;
				leftLegUpper = -90.0F;
				leftLegLower = 30.0f;
				LeftHandElbow = -30.0f;

				LeftHandElbow = 150.0f;
				RightHandElbow = -150.0f;
				Skipping = 1;

			}
			else
				Skipping = 0;

			break;
		case 'A':
			
			RightHandShoulder = RightHandShoulder + 0.4f;
			RightHandElbow = RightHandElbow + 0.5f;

		

			break;
		case 'a':

			RightHandShoulder = RightHandShoulder- 0.85f;
			LeftHandShoulder = LeftHandShoulder + 0.85f;

			RightLegUpper = RightLegUpper - 0.95f;
			leftLegUpper += 0.95f;
			/*RightHandShoulder = RightHandShoulder - 0.4f;
				RightHandElbow = RightHandElbow - 0.5f;
			
				fprintf(gpLogFile, "The value of Right Hand Shoulder is %f\n", RightHandShoulder);
				fprintf(gpLogFile, "The value of Right Hand Elbow is %f\n", RightHandElbow);*/
			//leftLegUpper = leftLegUpper - 0.35f;

			fprintf(gpLogFile, "The value of leftLegUpper is %f\n", leftLegUpper);
			//LeftHandShoulder = (int)(LeftHandShoulder - 1.0) % 360;
			break;
		case 'Q':
		case 'q':
			if (Turn == 0)
				Turn = 1;
			else
				Turn = 0;

			break;
		case 'P':
		case 'p':
			turnAngleLeftHand = turnAngleLeftHand + 0.95f;
			fprintf(gpLogFile, "LeftHand is %f\n", turnAngleLeftHand);
			turnAngleRightHand = turnAngleRightHand + 0.95f;
		
			//fprintf(gpLogFile, "RightHand is %f\n", turnAngleRightHand);
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
		"out_color = vColor;" \
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
	/////////////////    F R A G M E N T  ---  S H A D E R            //////////////////////////
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
		"FragColor =  vec4(phongADSLight ,1.0);" \
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
		"if(u_LKeyPressed == 1)" \
		"{" \
		"vec4 eye_coordinates = u_viewMatrix * u_modelMatrix *  vPosition;" \
		"tNorm = mat3(u_viewMatrix * u_modelMatrix) * vNormal;" \
		"light_direction = vec3(u_LightPosition - eye_coordinates);" \
		"viewVector = vec3(-eye_coordinates.xyz);" \
		"}" \
		"out_color = vColor;" \
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
		"FragColor =vec4(phongADSLight ,1.0);" \
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
	glBindAttribLocation(gShaderProgramObjectPerFragment, AMC_ATTRIBUTE_COLOR, "vColor");
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












	/*getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();
*/
	GLfloat *SphereVertices = NULL;
	GLfloat *SphereTextures = NULL;
	GLfloat *SphereNormals = NULL;
	//Basically we are giving the vertices coordinates in the above array
	vertices = Sphere(0.6f, 400, &SphereVertices, &SphereTextures, &SphereNormals);

	//Basically we are giving the vertices coordinates in the above array
	////////////////////FOR TRIANGLE ///////////////// 
	//Create vao - Vertex array objects

	//////////////////////// FOR Sphere //////////////// 

	glGenVertexArrays(1, &vao_Sphere);
	glBindVertexArray(vao_Sphere);
	glGenBuffers(1, &vbo_position_Sphere);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_Sphere);
	glBufferData(GL_ARRAY_BUFFER, 3 * vertices * sizeof(GLfloat), SphereVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	//NORMALS 
	glGenBuffers(1, &vbo_Normals_Sphere);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Normals_Sphere);
	glBufferData(GL_ARRAY_BUFFER, 3 * vertices * sizeof(GLfloat), SphereNormals, GL_STATIC_DRAW);
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	//NORMALS 
	glGenBuffers(1, &vbo_Normals_SpherePerFragment);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Normals_SpherePerFragment);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);



	//Elements
	glGenBuffers(1, &vbo_Element_SpherePerFragment);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_SpherePerFragment);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	make_cylinder(0.4, 8.2f, 30, 30);
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
	//fprintf(gpLogFile, "The value of turn is %d\n", Turn);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//fprintf(gpLogFile, "Value of sitting is %d\n", Sitting);
//	fprintf(gpLogFile, "Vlue if walking is %d\n", Walking);

	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 RotationMatrix;
	mat4 TranslateMatrix;
	mat4 ScaleMatrix;
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	ScaleMatrix = mat4::identity();


	// use shader program
	if (perFragmentLighting == true)
	{
	
		PerFragmentLighting();
	}
	else
	{
		PerVertexLighting();
	}
	
	
	
		TranslateMatrix = translate(0.0f, 2.0f, walkingTranslate);
		TranslateMatrix *= rotate(0.0f, turnAngleLeftHand, 0.0f);
		modelMatrix = TranslateMatrix;
		push(modelMatrix);

		modelMatrix *= translate(0.0f, 0.5f, 0.0f);

		if (Sitting == 1 || Sitting == 2)
		{
			TranslateMatrix *= translate(0.0f, UpperChestSitting, 0.0f);
			modelMatrix = TranslateMatrix;
		}
		/*else if (Sitting == 2)
		{
			TranslateMatrix *= translate(0.0f, UpperChestSitting, 0.0f);
			modelMatrix = TranslateMatrix;
		}*/
		else if (Skipping == 1)
		{
			TranslateMatrix *= translate(0.0f, UpperChestSkippingY, 0.0f);
			modelMatrix = TranslateMatrix;
			push(modelMatrix);
		}
		else if (swimming == 1)
		{
			TranslateMatrix = translate(-1.0f, 1.0f, walkingTranslate);
			TranslateMatrix *= rotate(turnAngleLeftHand, turnAngleLeftHand, 0.0f);
			modelMatrix = TranslateMatrix;
			push(modelMatrix);
		}
	
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
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3 * vertices);
	glBindVertexArray(0);

	/////////////////////////////// UPPER BODY ---- CHEST /////////////////////


	if (Sitting == 1)
	{
		TranslateMatrix = peek();
		TranslateMatrix *= translate(0.0f, -1.6f, 0.0f);
		push(TranslateMatrix);
		TranslateMatrix *= translate(0.0f, UpperChestSitting, 0.0f);
		if (UpperChestSitting > -0.2f)
			UpperChestSitting -= 0.002f;
		ScaleMatrix = scale(1.6f, 2.6f, 1.6f);
		//TranslateMatrix *= rotate(0.0f, turnAngle, 0.0f);
		TranslateMatrix = TranslateMatrix * ScaleMatrix;
		//	RotationMatrix = rotate(angleRectangle, 1.0f, 0.0f, 0.0f);
		RotationMatrix = rotate(angleRectangle, angleRectangle, angleRectangle);

		// perform necessary transformations
		modelMatrix = TranslateMatrix;
	}
	else if (Sitting == 2)
	{
		
			TranslateMatrix = peek();
			TranslateMatrix *= translate(0.0f, -1.6f, 0.0f);
			push(TranslateMatrix);
			TranslateMatrix *= translate(0.0f, UpperChestSitting, 0.0f);
			if (UpperChestSitting < 0.0f)
				UpperChestSitting += 0.002f;
			ScaleMatrix = scale(1.6f, 2.6f, 1.6f);
			//TranslateMatrix *= rotate(0.0f, turnAngle, 0.0f);
			TranslateMatrix = TranslateMatrix * ScaleMatrix;
			//	RotationMatrix = rotate(angleRectangle, 1.0f, 0.0f, 0.0f);
			RotationMatrix = rotate(angleRectangle, angleRectangle, angleRectangle);

			// perform necessary transformations
			modelMatrix = TranslateMatrix;
		
	}
	else
	{
		TranslateMatrix = peek();
		TranslateMatrix *= translate(0.0f, -1.6f, 0.0f);
		push(TranslateMatrix);
		if (Skipping == 1)
		{
			TranslateMatrix *= translate(0.0f, UpperChestSkippingY, 0.0f);
			push(TranslateMatrix);
		}
		ScaleMatrix = scale(1.6f, 2.6f, 1.6f);
		//TranslateMatrix *= rotate(0.0f, turnAngle, 0.0f);
		TranslateMatrix = TranslateMatrix * ScaleMatrix;
		//	RotationMatrix = rotate(angleRectangle, 1.0f, 0.0f, 0.0f);
		RotationMatrix = rotate(angleRectangle, angleRectangle, angleRectangle);

		// perform necessary transformations
		modelMatrix = TranslateMatrix;
	}

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


//	/////////////////////////////////////////////////////////////////
//
//	//////// Right HAND /////////////// 

	RotationMatrix = rotate((GLfloat)RightHandShoulder, 1.0f, 0.0f, 0.0f);
	ScaleMatrix = scale(0.3f, 0.3f, 0.09f);

	if (swimming == 1)
	{
		RotationMatrix = rotate((GLfloat)RightHandShoulder, 1.0f, 0.0f, 0.0f);
		modelMatrix = peek();
		modelMatrix *= translate(0.83f, 0.85f, 0.0f);
		modelMatrix = modelMatrix * RotationMatrix;
		modelMatrix *= translate(0.0f, 0.0f, -1.0f);
		push(modelMatrix);
		modelMatrix *= ScaleMatrix;
		push(modelMatrix);
	}
	else
	{
		RotationMatrix = rotate((GLfloat)RightHandShoulder, 1.0f, 0.0f, 0.0f);
		modelMatrix = peek();
		modelMatrix *= translate(0.83f, 0.85f, 0.0f);
		modelMatrix = modelMatrix * RotationMatrix;
		modelMatrix *= translate(0.0f, 0.0f, -1.0f);
		push(modelMatrix);
		modelMatrix *= ScaleMatrix;
		push(modelMatrix);
	}

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

	draw_cylinder();
//
	pop();
	if (Skipping == 1)
	{
		//pop();
		modelMatrix = peek();
		modelMatrix *= translate(0.0f, -0.1f, -0.1f);
		modelMatrix *= rotate(60.00f, (GLfloat)RightHandElbow, 260.0f);

		//modelMatrix *= rotate((GLfloat)LeftHandElbow, 0.0f, 1.0f, 0.0f);
		modelMatrix *= translate(0.0f, 0.0f, -0.7f);
		modelMatrix *= scale(0.3f, 0.3f, 0.09f);
	}
	//else if (Running == 1)
	//{
	//	modelMatrix = peek();
	//	modelMatrix *= translate(0.0f, 0.0f, -0.1f);
	//	modelMatrix *= rotate((GLfloat)RightHandElbow, 1.0f, 0.0f, 0.0f);
	//	modelMatrix *= translate(0.0f, 0.0f, -0.75f);
	//	modelMatrix *= scale(0.3f, 0.3f, 0.09f);
	//}
	//else if (swimming == 1)
	//{
	//	modelMatrix = peek();
	//	modelMatrix *= translate(0.0f, 0.0f, -0.1f);
	//	modelMatrix *= rotate((GLfloat)RightHandElbow, 1.0f, 0.0f, 0.0f);
	//	modelMatrix *= translate(0.0f, 0.0f, -0.75f);
	//	modelMatrix *= scale(0.3f, 0.3f, 0.09f);
	//}
	else
	{
		modelMatrix = peek();
		modelMatrix *= translate(0.0f, 0.0f, -0.1f);
		modelMatrix *= rotate((GLfloat)RightHandElbow, 1.0f, 0.0f, 0.0f);
		modelMatrix *= translate(0.0f, 0.0f, -0.75f);
		modelMatrix *= scale(0.3f, 0.3f, 0.09f);
	}
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
	//
	draw_cylinder();
//
//	
//	////////////////////////////////// WRIST Right hand ///////////////////////
//	if (Skipping == 1)
//	{
//		//pop();
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, -0.1f, -0.1f);
//		modelMatrix *= rotate(60.00f, (GLfloat)RightHandElbow, 260.0f);
//
//		//modelMatrix *= rotate((GLfloat)LeftHandElbow, 0.0f, 1.0f, 0.0f);
//		modelMatrix *= translate(0.0f, 0.0f, -0.7f);
//		modelMatrix *= scale(0.32f, 0.32f, 0.32f);
//	}
//	else if (Running == 1)
//	{
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, 0.0f, -0.75f);
//		modelMatrix *= rotate((GLfloat)RightHandElbow, 1.0f, 0.0f, 0.0f);
//		modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	else if (Sitting == 1)
//	{
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, -0.5f, -0.75f);
//		modelMatrix *= rotate((GLfloat)RightHandElbow, 1.0f, 0.0f, 0.0f);
//		modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	else if (Sitting == 2)
//	{
//
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, -0.5f, -0.75f);
//		modelMatrix *= rotate((GLfloat)RightHandElbow, 1.0f, 0.0f, 0.0f);
//		modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//
//	}
//	else
//	{
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, 0.0f, -0.75f);
//		modelMatrix *= rotate((GLfloat)RightHandElbow, 1.0f, 0.0f, 0.0f);
//		modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	if (perFragmentLighting == true)
//	{
//
//		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//	else
//	{
//		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//	glBindVertexArray(vao_Sphere);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
//	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
//	glBindVertexArray(0);
//
//
//
//
//	//////////////////////////////
//	if (Skipping == 1)
//	{
//		RotationMatrix = rotate((GLfloat)LeftHandShoulder, 1.0f, 0.0f, 0.0f);
//		ScaleMatrix = scale(0.3f, 0.3f, 0.3f);
//		modelMatrix = translate(0.9f, HandSkippingY - 0.12f, walkingTranslate -0.2f );
//		modelMatrix = modelMatrix * RotationMatrix;
//		//modelMatrix *= rotate(0.0f, 0.0f, turnAngleLeftHand);
//		modelMatrix *= translate(0.0f, 0.0f, -1.0f);
//		push(modelMatrix);
//		modelMatrix *= ScaleMatrix;
//		push(modelMatrix);
//
//	}
//	else if (Running == 1)
//	{
//		pop();
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, 0.0f, -0.1f);
//		modelMatrix *= rotate((GLfloat)LeftHandElbow, 1.0f, 0.0f, 0.0f);
//		modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	else
//	{
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, 0.0f, -0.1f);
//		modelMatrix *= rotate((GLfloat)LeftHandElbow, 1.0f, 0.0f, 0.0f);
//		modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	if (perFragmentLighting == true)
//	{
//
//		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//	else
//	{
//		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//	glBindVertexArray(vao_Sphere);
//	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 0.5f, 0.35f, 0.005f);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
//	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
//	glBindVertexArray(0);
//
//	resetStack();
//	//resetStack();
//
//
//	//////////////////// LEFT HAND /////////////////////
//
//

pop();

	RotationMatrix = rotate((GLfloat)LeftHandShoulder, 1.0f, 0.0f, 0.0f);
	ScaleMatrix = scale(0.3f, 0.3f, 0.09f);

		RotationMatrix = rotate((GLfloat)LeftHandShoulder, 0.0f,0.0f);
		
		modelMatrix = peek();
		modelMatrix *= translate(-0.85f, 0.85f, 0.0f);
		modelMatrix = modelMatrix * RotationMatrix;
		modelMatrix *= translate(0.0f, 0.0f, -1.0f);
		push(modelMatrix);
		modelMatrix *= ScaleMatrix;
		push(modelMatrix);


	
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

	draw_cylinder();
//
	pop();

		if (Skipping == 1)
	{
		//pop();
		modelMatrix = peek();
		modelMatrix *= translate(0.00f, -0.1f, -0.1f);
		modelMatrix *= rotate(60.00f, (GLfloat)LeftHandElbow, -260.0f);

		//modelMatrix *= rotate((GLfloat)LeftHandElbow, 0.0f, 1.0f, 0.0f);
		modelMatrix *= translate(0.0f, 0.0f, -0.7f);
		modelMatrix *= scale(0.3f, 0.3f, 0.09f);
	}
		else
		{

			modelMatrix = peek();
			modelMatrix *= translate(0.0f, 0.0f, -0.1f);
			modelMatrix *= rotate((GLfloat)LeftHandElbow, 1.0f, 0.0f, 0.0f);
			modelMatrix *= translate(0.0f, 0.0f, -0.7f);
			modelMatrix *= scale(0.3f, 0.3f, 0.09f);
		}

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
	draw_cylinder();
//
//	//////////////////////////////////// WRIST left hand
//	 if (Skipping == 1)
//	{
//		//pop();
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, -0.1f, -0.1f);
//		modelMatrix *= rotate(60.00f, (GLfloat)LeftHandElbow, -260.0f);
//
//		//modelMatrix *= rotate((GLfloat)LeftHandElbow, 0.0f, 1.0f, 0.0f);
//		modelMatrix *= translate(0.0f, 0.0f, -0.7f);
//		modelMatrix *= scale(0.32f, 0.32f, 0.32f);
//	}
//	 else if (Sitting == 1)
//	 {
//		 modelMatrix = peek();
//		 modelMatrix *= translate(0.0f, -0.5f, -0.75f);
//		 modelMatrix *= rotate((GLfloat)LeftHandElbow, 1.0f, 0.0f, 0.0f);
//		 modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		 modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	 }
//	 else if (Sitting == 2)
//	 {
//
//		 modelMatrix = peek();
//		 modelMatrix *= translate(0.0f, -0.5f, -0.75f);
//		 modelMatrix *= rotate((GLfloat)LeftHandElbow, 1.0f, 0.0f, 0.0f);
//		 modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		 modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//
//	 }
//	 else if (Running == 1)
//	 {
//		 modelMatrix = peek();
//		 modelMatrix *= translate(0.0f, 0.0f, -0.75f);
//		 modelMatrix *= rotate((GLfloat)LeftHandElbow * 0.8f, 1.0f, 0.0f, 0.0f);
//		 modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		 modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	 }
//	else
//	{
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, 0.0f, -0.75f);
//		modelMatrix *= rotate((GLfloat)LeftHandElbow, 1.0f, 0.0f, 0.0f);
//		modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	if (perFragmentLighting == true)
//	{
//
//		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//	else
//	{
//		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//	glBindVertexArray(vao_Sphere);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
//	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
//	glBindVertexArray(0);
//
//
//
//
//	//////////////////////////////
//	if (Skipping == 1)
//	{
//		RotationMatrix = rotate((GLfloat)LeftHandShoulder, 1.0f, 0.0f, 0.0f);
//		ScaleMatrix = scale(0.3f, 0.3f, 0.3f);
//		modelMatrix = translate(-0.9f, HandSkippingY , walkingTranslate + 0.12f);
//		modelMatrix = modelMatrix * RotationMatrix;
//		//modelMatrix *= rotate(0.0f, 0.0f, turnAngleLeftHand);
//		modelMatrix *= translate(0.0f, 0.0f, -1.0f);
//		push(modelMatrix);
//		modelMatrix *= ScaleMatrix;
//		push(modelMatrix);
//
//	}
//	else if (Running == 1)
//	{
//		pop();
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, 0.0f, -0.1f);
//		modelMatrix *= rotate((GLfloat)LeftHandElbow, 1.0f, 0.0f, 0.0f);
//		modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	else 
//	{
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, 0.0f, -0.1f);
//		modelMatrix *= rotate((GLfloat)LeftHandElbow, 1.0f, 0.0f, 0.0f);
//		modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	if (perFragmentLighting == true)
//	{
//
//		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//	else
//	{
//		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//	glBindVertexArray(vao_Sphere);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
//	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
//	glBindVertexArray(0);
//
//	resetStack();
//
//
//
//	//////////////////// Left Leg ////////////////////
//
	pop();
	RotationMatrix = rotate((GLfloat)leftLegUpper, 1.0f, 0.0f, 0.0f);

	
 if(Sitting == 1)
	{
		modelMatrix = peek();
		modelMatrix *= translate(0.0f, -1.85f, 0.0f);
		modelMatrix *= translate(-0.4f, 0.0f, 0.3f);
		push(modelMatrix);
		if (Walking == 1)
			RotationMatrix = rotate((GLfloat)leftLegUpper, 1.0f, 0.0f, 0.0f);
		else if (Sitting == 1)
			RotationMatrix = rotate((GLfloat)leftLegUpper, 1.0f, 0.0f, 0.0f);
		ScaleMatrix = scale(0.3f, 0.3f, 0.1f);


		//	push(modelMatrix);
		modelMatrix = modelMatrix * RotationMatrix;
		modelMatrix *= translate(0.0f, 0.0f, 0.0f);
	//	push(modelMatrix);
		modelMatrix *= ScaleMatrix;
		push(modelMatrix);
	}
	else if (Sitting == 2)
	{
		modelMatrix = peek();
		modelMatrix *= translate(0.0f, -1.85f, 0.0f);
		modelMatrix *= translate(-0.4f, 0.0f, 0.3f);
		push(modelMatrix);
		if (Walking == 1)
			RotationMatrix = rotate((GLfloat)leftLegUpper, 1.0f, 0.0f, 0.0f);
		else if (Sitting == 1)
			RotationMatrix = rotate((GLfloat)leftLegUpper, 1.0f, 0.0f, 0.0f);
		ScaleMatrix = scale(0.3f, 0.3f, 0.1f);


		//	push(modelMatrix);
		modelMatrix = modelMatrix * RotationMatrix;
		modelMatrix *= translate(0.0f, 0.0f, 0.0f);
		//	push(modelMatrix);
		modelMatrix *= ScaleMatrix;
		push(modelMatrix);
	}
	
	

	else
	{
		modelMatrix = peek();
		modelMatrix *= translate(0.0f, -0.85f, 0.0f);
		modelMatrix *= translate(-0.4f, 0.0f, 0.3f);
		
			RotationMatrix = rotate((GLfloat)leftLegUpper, 1.0f, 0.0f, 0.0f);
	
		ScaleMatrix = scale(0.3f, 0.3f, 0.1f);


		//	push(modelMatrix);
		modelMatrix = modelMatrix * RotationMatrix;
		modelMatrix *= translate(0.0f, 0.0f, -1.0f);
		push(modelMatrix);
		modelMatrix *= ScaleMatrix;
		push(modelMatrix);
	}

	
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

	draw_cylinder();
//
	pop();

	
	 if(Sitting == 1 || Sitting == 2)
	{
		
		modelMatrix = peek();
		modelMatrix *= translate(0.0f, 0.0f, 0.1f);
		modelMatrix *= rotate((GLfloat)leftLegLower, 1.0f, 0.0f, 0.0f);
		modelMatrix *= translate(0.0f, 0.0f, -0.8f);
		modelMatrix *= scale(0.3f, 0.3f, 0.09f);
	}
	
	else
	{
		modelMatrix = peek();
		modelMatrix *= translate(0.0f, 0.0f, -0.1f);
		modelMatrix *= rotate((GLfloat)leftLegLower, 1.0f, 0.0f, 0.0f);
		modelMatrix *= translate(0.0f, 0.0f, -0.7f);
		modelMatrix *= scale(0.3f, 0.3f, 0.09f);
	}
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
	draw_cylinder();
//	if (Walking == 1)
//	{
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, -0.0f, -0.1f);
//		modelMatrix *= rotate((GLfloat)leftLegLower, 1.0f, 0.0f, 0.0f);
//		modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	else if (Sitting == 1)
//	{
//		//pop();
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, -0.02f, 0.025f);
//		modelMatrix *= rotate((GLfloat)leftLegLower, 1.0f, 0.0f, 0.0f);
//		//modelMatrix *= translate(0.0f, 0.0f, 0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	else if (Sitting == 2)
//	{
//		//pop();
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, -0.02f, 0.025f);
//		modelMatrix *= rotate((GLfloat)leftLegLower, 1.0f, 0.0f, 0.0f);
//		//modelMatrix *= translate(0.0f, 0.0f, 0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	else if (Skipping == 1)
//	{
//	//	pop();
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, -0.1f, -0.0f);
//		modelMatrix *= rotate((GLfloat)leftLegLower, 1.0f, 0.0f, 0.0f);
//		//modelMatrix *= translate(0.0f, 0.0f, 0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	else
//	{
//
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, -0.0f, -0.1f);
//		modelMatrix *= rotate((GLfloat)leftLegLower, 1.0f, 0.0f, 0.0f);
//		push(modelMatrix);
//		modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//
//	}
//	if (perFragmentLighting == true)
//	{
//
//		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//	else
//	{
//		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//
//
//	glBindVertexArray(vao_Sphere);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
//	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
//	glBindVertexArray(0);
//
//
//
//	//////////////////////// SHOES /////////////////////////
//
	
//	modelMatrix = peek();
//	modelMatrix *= translate(0.0f, -0.0f, -0.8f);
//	modelMatrix *= rotate((GLfloat)leftLegLower, 1.0f, 0.0f, 0.0f);
//	modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//	modelMatrix *= scale(0.3f, 0.7f, 0.3f);
//
//	
//	if (perFragmentLighting == true)
//	{
//
//		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//	else
//	{
//		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//
//
//	glBindVertexArray(vao_Sphere);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
//	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
//	glBindVertexArray(0);
//
//
//
//
//
//
//
//
//	resetStack();
//
//
//	////////////////////////// RIGHT LEG ////////////////////////////////
//
//	
//
pop();


if(Sitting == 1 || Sitting == 2)
{
	modelMatrix = peek();
	modelMatrix *= translate(0.0f, -1.85f, 0.0f);
	modelMatrix *= translate(0.4f, 0.0f, 0.3f);
	push(modelMatrix);
	if (Walking == 1)
		RotationMatrix = rotate((GLfloat)leftLegUpper, 1.0f, 0.0f, 0.0f);
	else if (Sitting == 1)
		RotationMatrix = rotate((GLfloat)leftLegUpper, 1.0f, 0.0f, 0.0f);
	ScaleMatrix = scale(0.3f, 0.3f, 0.1f);


	//	push(modelMatrix);
	modelMatrix = modelMatrix * RotationMatrix;
	modelMatrix *= translate(0.0f, 0.0f, 0.0f);
	//	push(modelMatrix);
	modelMatrix *= ScaleMatrix;
	push(modelMatrix);
}
else
{
	modelMatrix = peek();
	modelMatrix *= translate(0.0f, -0.85f, 0.0f);
	modelMatrix *= translate(0.4f, 0.0f, 0.3f);

	RotationMatrix = rotate((GLfloat)RightLegUpper, 1.0f, 0.0f, 0.0f);

	ScaleMatrix = scale(0.3f, 0.3f, 0.1f);


	//	push(modelMatrix);
	modelMatrix = modelMatrix * RotationMatrix;
	modelMatrix *= translate(0.0f, 0.0f, -1.0f);
	push(modelMatrix);
	modelMatrix *= ScaleMatrix;
	push(modelMatrix);
}
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

	draw_cylinder();
//
	pop();
//


 if(Sitting == 1 || Sitting == 2)
	{

		modelMatrix = peek();
		modelMatrix *= translate(0.0f, 0.0f, 0.1f);
		modelMatrix *= rotate((GLfloat)leftLegLower, 1.0f, 0.0f, 0.0f);
		modelMatrix *= translate(0.0f, 0.0f, -0.8f);
		modelMatrix *= scale(0.3f, 0.3f, 0.09f);
	}
	else
	{
		modelMatrix = peek();
		modelMatrix *= translate(0.0f, 0.0f, -0.1f);
		modelMatrix *= rotate((GLfloat)RightLegLower, 1.0f, 0.0f, 0.0f);
		modelMatrix *= translate(0.0f, 0.0f, -0.7f);
		modelMatrix *= scale(0.3f, 0.3f, 0.09f);
	}
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
	draw_cylinder();
//	if (Walking == 1)
//	{
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, -0.0f, -0.1f);
//		modelMatrix *= rotate((GLfloat)RightLegLower, 1.0f, 0.0f, 0.0f);
//		modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	else if (Sitting == 1)
//	{
//		//pop();
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, -0.02f, 0.025f);
//		modelMatrix *= rotate((GLfloat)RightLegLower, 1.0f, 0.0f, 0.0f);
//		//modelMatrix *= translate(0.0f, 0.0f, 0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	else if (Sitting == 2)
//	{
//		//pop();
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, -0.02f, 0.025f);
//		modelMatrix *= rotate((GLfloat)RightLegLower, 1.0f, 0.0f, 0.0f);
//		//modelMatrix *= translate(0.0f, 0.0f, 0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	else if (Skipping == 1)
//	{
//		//	pop();
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, -0.1f, -0.0f);
//		modelMatrix *= rotate((GLfloat)RightLegLower, 1.0f, 0.0f, 0.0f);
//		//modelMatrix *= translate(0.0f, 0.0f, 0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	else
//	{
//		
//			modelMatrix = peek();
//			modelMatrix *= translate(0.0f, -0.0f, -0.1f);
//			modelMatrix *= rotate((GLfloat)RightLegLower, 1.0f, 0.0f, 0.0f);
//			push(modelMatrix);
//			modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//			modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//		
//	}
//	if (perFragmentLighting == true)
//	{
//
//		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//	else
//	{
//		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//
//
//	glBindVertexArray(vao_Sphere);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
//	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
//	glBindVertexArray(0);
//
//
//
//	///////////// SHOES RIGHT LEG /////////
//	modelMatrix = peek();
//	modelMatrix *= translate(0.0f, -0.0f, -0.8f);
//	modelMatrix *= rotate((GLfloat)RightLegLower, 1.0f, 0.0f, 0.0f);
//	modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//	modelMatrix *= scale(0.3f, 0.7f, 0.3f);
//
//
//	if (perFragmentLighting == true)
//	{
//
//		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//	else
//	{
//		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//
//
//	glBindVertexArray(vao_Sphere);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
//	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
//	glBindVertexArray(0);
//
//

	

	resetStack();

















	//fprintf(gpLogFile, "Value of RIGHTLEGUPPER IS %f\n", RightLegUpper);
	//fprintf(gpLogFile, "Value of RIGHTLEGLOWER IS %f\n", RightLegLower);

	// unuse program
	glUseProgram(0);


	

	UpdateSitting();
	UpdateSkipping();
	UpdateWalking();
	UpdateRunning();
	UpdateSwimming();
	

	SwapBuffers(ghdc); 
	angleRectangle = angleRectangle - 0.1f;
	if (angleRectangle <= -360.0f)
		angleRectangle = 0.0f;
}
void UpdateSkipping()
{
	if (Skipping == 1)
	{
		if (Jump == 1)
		{
			if (leftLegLower <= 70.000153)
			{
				RightLegLower = RightLegLower + 0.14;
				leftLegLower = leftLegLower + 0.14;


				LeftHandElbow = LeftHandElbow - 0.45;
				RightHandElbow = RightHandElbow + 0.45;
				//fprintf(gpLogFile, "The value of leftLegLower is %f\n", leftLegLower);

				if (UpperChestSkippingY <= 0.2)
					UpperChestSkippingY += 0.0006f;
				if (FaceSkippingY <= 2.2)
					FaceSkippingY+= 0.0006f;
				if (LegSkippingY <= -1.4)
					LegSkippingY += 0.0006f;
				if (HandSkippingY <= 1.2)
					HandSkippingY += 0.0006f;
				fprintf(gpLogFile, "UpperChestSkippingY is %f\n", UpperChestSkippingY);
			}
		}
		if (leftLegLower >= 70.0001253)
		{
			Jump = 0;
			//fprintf(gpLogFile, "Here is JUMP is 0");
		}
		if (Jump == 0)
		{
			if (leftLegLower >= 30.0000)
			{
				RightLegLower = RightLegLower - 0.14;
				leftLegLower = leftLegLower - 0.14;
				//fprintf(gpLogFile, "The value of leftLegLower is %f\n", leftLegLower);
				LeftHandElbow = LeftHandElbow - 0.45;
				RightHandElbow = RightHandElbow + 0.45;
				if(UpperChestSkippingY >= 0.0)
					UpperChestSkippingY -= 0.0006f;
				if (FaceSkippingY >= 2.0)
					FaceSkippingY -= 0.0006f;
				if (LegSkippingY >= -1.6)
					LegSkippingY -= 0.0006f;
				if (HandSkippingY >= 1.0)
					HandSkippingY -= 0.0006f;
				if (leftLegLower <= 30.0000)
				{
					Jump = 1;
				}
				
			}
		}


	}
}
void UpdateSitting()
{
	if (Sitting == 1)
	{
		
		
			if (leftLegUpper >= -161.0)
			{
				

				/*	LeftHandShoulder = (int)(LeftHandShoulder - 1.0) % 360;
					LeftHandElbow = (int)(LeftHandElbow - 0.5) % 360;

					RightHandShoulder = (int)(RightHandShoulder + 1.0) % 360;
					RightHandElbow = (int)(RightHandElbow + 0.5) % 360;
		*/

				leftLegUpper = (int)(leftLegUpper - 1.0) % 360;
				//leftLegLower = -90.0;


				//RightLegUpper = (int)(RightLegUpper - 1.0) % 360;
				//RightLegLower = -90.0;
			}
	
				
		
	}
	else if (Sitting == 2)
	{
		if (leftLegUpper <= -90.0)
		{
			//fprintf(gpLogFile, "Value of RightLegUpper is %f\n", RightLegUpper);
			leftLegUpper = (int)(leftLegUpper + 1.0) % 360;
			/*leftLegLower = -100.0;


			RightLegUpper = (int)(RightLegUpper + 1.0) % 360;
			RightLegLower = -100.0;*/

		}
		//if()
	}
	
}
void UpdateWalking()
{



	if (Walking == 1)
	{
		//Sitting = 0;

		/*if (walkingTranslate <= -5.0f)
			walkingTranslate += 0.02f;
		else
			Walking = 0;
*/
		if (RightLeg == 1)
		{
			if (RightLegUpper >= -141.0)
			{


				LeftHandShoulder = (int)(LeftHandShoulder - 1.0) % 360;
				LeftHandElbow = (int)(LeftHandElbow - 0.5) % 360;

				RightHandShoulder = (int)(RightHandShoulder + 1.0) % 360;
				RightHandElbow = (int)(RightHandElbow + 0.5) % 360;


				leftLegUpper = (int)(leftLegUpper + 1.0) % 360;
				leftLegLower = (int)(leftLegLower + 0.5) % 360;


				RightLegUpper = (int)(RightLegUpper - 1.0) % 360;
				RightLegLower = (int)(RightLegLower - 0.5) % 360;
			}
		}

		if (RightLegUpper <= -141.0)
			RightLeg = 0;


		if (RightLeg == 0)
		{
			if (RightLegUpper <= -50.0)
			{

				LeftHandShoulder = (int)(LeftHandShoulder + 1.0) % 360;
				LeftHandElbow = (int)(LeftHandElbow + 0.5) % 360;

				RightHandShoulder = (int)(RightHandShoulder - 1.0) % 360;
				RightHandElbow = (int)(RightHandElbow - 0.5) % 360;



				leftLegUpper = (int)(leftLegUpper - 1.0) % 360;
				leftLegLower = (int)(leftLegLower - 0.5) % 360;


				RightLegUpper = (int)(RightLegUpper + 1.0) % 360;
				RightLegLower = (int)(RightLegLower + 0.5) % 360;
			}
			if (RightLegUpper >= -50.0)
			{
				RightLeg = 1;
			}
		}


	}


}
void UpdateRunning()
{
	if (Running == 1)
	{
		if (RightLeg == 1)
		{
			if (RightHandShoulder >= -132.000076)
			{


				RightHandShoulder = RightHandShoulder - 0.6f;
				RightHandElbow = RightHandElbow - 0.7f;


				LeftHandShoulder = LeftHandShoulder + 0.6f;
				LeftHandElbow = LeftHandElbow + 0.7f;

				leftLegUpper = leftLegUpper - 0.7;
				leftLegLower = leftLegLower - 0.6;


				RightLegUpper = RightLegUpper + 0.7;
				RightLegLower = RightLegLower + 0.6;

			}
			if (RightHandShoulder <= -132.000076)
				RightLeg = 0;
		}
		
		if (RightLeg == 0)
		{
			if (RightHandShoulder <= -50.0)
			{
				RightHandShoulder = RightHandShoulder + 0.6f;
				RightHandElbow = RightHandElbow + 0.7f;


				LeftHandShoulder = LeftHandShoulder - 0.6f;
				LeftHandElbow = LeftHandElbow - 0.7f;

				leftLegUpper = leftLegUpper + 0.7;
				leftLegLower = leftLegLower + 0.6;


				RightLegUpper = RightLegUpper - 0.7;
				RightLegLower = RightLegLower - 0.6;
			}
			if (RightHandShoulder >= -50.0)
				RightLeg = 1;
		}

	}
}
void UpdateSwimming()
{
	if (swimming == 1)
	{
		if (smimmingLegsToggle == 0)
		{
			if (leftLegUpper >= -103.9)
			{
				RightLegUpper = RightLegUpper + 0.25f;
				leftLegUpper -= 0.25f;
				fprintf(gpLogFile, "I am here\n");
			}
			if (leftLegUpper <= -103.9)
			{
				smimmingLegsToggle = 1;
				fprintf(gpLogFile, "Value of leftLegUpper is %f\n", leftLegUpper);
			}
		}
		if(smimmingLegsToggle == 1)
		{

			if (leftLegUpper <= -70.0)
			{

				RightLegUpper = RightLegUpper - 0.25f;
				leftLegUpper += 0.25f;
			}
			if (leftLegUpper >= -70.0)
			{
			//	fprintf(gpLogFile, "I am here\n");
				smimmingLegsToggle = 0;
			}

		}
		RightHandShoulder = RightHandShoulder - 0.45f;
		LeftHandShoulder = LeftHandShoulder + 0.45f;
	}
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


}
void update(void)
{


}
int Sphere(GLfloat r, int n, GLfloat **Vertices, GLfloat **Textures, GLfloat **Normals)
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
