#include<Windows.h>
#include<glew.h>               // This SHOULD be above GL.h header.
#include<gl/GL.h>
#include"TexturedSmiley.h"
#include<stdio.h>
#include"vmath.h"
#pragma comment(lib,"opengl32.lib")//for linking
#pragma comment(lib,"glew32.lib")//for linking

int Animation = 0;

int Win_Width = 800;
int Win_Height = 600;
using namespace vmath;

GLuint gTextureSamplerUniform;	// to catch uniform of sampler

GLuint gTextureStone;			// Stone texture object

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
GLuint modelUniform;
GLuint ViewUniform;
GLuint vao_rectangle;
GLuint vbo_position_rectangle;
GLuint vbo_Normals_rectangle;


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
GLuint modelViewUniform;
GLuint LDUniform;
GLuint LAUniform;
GLuint LSUniform;


GLuint KDUniform;
GLuint KAUniform;
GLuint KSUniform;
float MaterialShininessUniform;
GLuint LightPositionUniform;
GLuint LKeyPressed = 0;

GLfloat lightAmbient[] = { 125.0f,125.0f,125.0f,1.0f };
GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightPosition[] = { 100.0f,100.0f,100.0f,1.0f };

GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialShininess = 128.0f;

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
				//  LKeyPressed = 0;
			}
			else
			{

				gbLighting = 0;
			}
			break;
		case 'A':
		case 'a':
			if (Animation == 0)
			{
				Animation = 1;
			}
			else
			{
				Animation = 0;
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

	int LoadGLTextures(GLuint *, TCHAR[]);
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
		"out vec3 out_Color;" \
		"in vec2 vTextureCoord;" \
		"out vec2 out_texture_coord;"	\

		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"out vec3 tNorm;" \
		"out vec3 light_direction;" \
		"out vec3 viewVector;" \
		"uniform vec4 u_LightPosition;" \
	
		"uniform int u_LKeyPressed;" \
		
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
		"gl_Position = u_projectionMatrix * u_viewMatrix *u_modelMatrix * vPosition;" \
		"out_texture_coord = vTextureCoord;"	\
		"out_Color = vColor;" \
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
		
		"out vec4 FragColor;" \
		"in vec3 out_Color;" \
		"in vec2 out_texture_coord;" \
		"uniform int u_LKeyPressed;"	\
		"out vec4 textureColor;" \
		"uniform sampler2D u_texture_sampler;" \
		"in vec3 phongADSLight;" \
		"void main(void)" \
		"{" \
		"textureColor = texture(u_texture_sampler, out_texture_coord);"	\
		"if(u_LKeyPressed == 1)" \
		"{" \
		"FragColor = vec4(out_Color,1.0) * textureColor * vec4(phongADSLight ,1.0);" \
		"}" \
		"else" \
		"{" \
		"FragColor =vec4(out_Color,1.0) * textureColor;" \
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
	glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_TEXCOORD0, "vTextureCoord");
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
	gTextureSamplerUniform = glGetUniformLocation(gShaderProgramObject, "u_texture_sampler");
	MaterialShininessUniform = glGetUniformLocation(gShaderProgramObject, "u_MaterialShininess");
	//Here we have done all the preparations of data transfer from CPU to GPU


	const GLfloat InterleavedArray[] =
	{
1.0f, 1.0f, -1.0f,1.0f, 0.0f, 0.0f,0.0f, 1.0f, 0.0f,0.0f, 1.0f,
-1.0f, 1.0f, -1.0f,1.0f, 0.0f, 0.0f,0.0f, 1.0f, 0.0f,0.0f, 0.0f,
-1.0f, 1.0f, 1.0f,1.0f, 0.0f, 0.0f,0.0f, 1.0f, 0.0f,1.0f, 0.0f,
1.0f, 1.0f, 1.0f,1.0f, 0.0f, 0.0f,0.0f, 1.0f, 0.0f,1.0f, 1.0f,
1.0f, -1.0f, -1.0f,0.0f, 1.0f, 0.0f,0.0f, -1.0f, 0.0f,1.0f, 1.0f,
-1.0f, -1.0f, -1.0f,0.0f, 1.0f, 0.0f,0.0f, -1.0f, 0.0f,0.0f, 1.0f,
-1.0f, -1.0f, 1.0f,0.0f, 1.0f, 0.0f,0.0f, -1.0f, 0.0f,0.0f, 0.0f,
1.0f, -1.0f, 1.0f,0.0f, 1.0f, 0.0f,0.0f, -1.0f, 0.0f,1.0f, 0.0f,
1.0f, 1.0f, 1.0f,0.0f, 0.0f, 1.0f,0.0f, 0.0f, 1.0f,0.0f, 0.0f,
-1.0f, 1.0f, 1.0f,0.0f, 0.0f, 1.0f,0.0f, 0.0f, 1.0f,1.0f, 0.0f,
-1.0f, -1.0f, 1.0f,0.0f, 0.0f, 1.0f,0.0f, 0.0f, 1.0f,1.0f, 1.0f,
1.0f, -1.0f, 1.0f,0.0f, 0.0f, 1.0f,0.0f, 0.0f, 1.0f,0.0f, 1.0f,
1.0f, 1.0f, -1.0f,0.0f, 1.0f, 1.0f,0.0f, 0.0f, -1.0f,1.0f, 0.0f,
-1.0f, 1.0f, -1.0f,0.0f, 1.0f, 1.0f,0.0f, 0.0f, -1.0f,1.0f, 1.0f,
-1.0f, -1.0f, -1.0f,0.0f, 1.0f, 1.0f,0.0f, 0.0f, -1.0f,0.0f, 1.0f,
1.0f, -1.0f, -1.0f,0.0f, 1.0f, 1.0f,0.0f, 0.0f, -1.0f,0.0f, 0.0f,
1.0f, 1.0f, -1.0f,1.0f, 1.0f, 0.0f,1.0f, 0.0f, 0.0f,1.0f, 0.0f,
1.0f, 1.0f, 1.0f,1.0f, 1.0f, 0.0f,1.0f, 0.0f, 0.0f,1.0f, 1.0f,
1.0f, -1.0f, 1.0f,1.0f, 1.0f, 0.0f,1.0f, 0.0f, 0.0f,0.0f, 1.0f,
1.0f, -1.0f, -1.0f,1.0f, 1.0f, 0.0f,1.0f, 0.0f, 0.0f,0.0f, 0.0f,
-1.0f, 1.0f, -1.0f,1.0f, 0.0f, 1.0f,-1.0f, 0.0f, 0.0f,0.0f, 0.0f,
-1.0f, 1.0f, 1.0f,1.0f, 0.0f, 1.0f,-1.0f, 0.0f, 0.0f,1.0f, 0.0f,
-1.0f, -1.0f, 1.0f,1.0f, 0.0f, 1.0f,-1.0f, 0.0f, 0.0f,1.0f, 1.0f,
-1.0f, -1.0f, -1.0f,1.0f, 0.0f, 1.0f,-1.0f, 0.0f, 0.0f,0.0f, 1.0f

	};
	
	//Basically we are giving the vertices coordinates in the above array
	////////////////////FOR TRIANGLE ///////////////// 
	//Create vao - Vertex array objects

	//////////////////////// FOR RECTANGLE //////////////// 

	glGenVertexArrays(1, &vao_rectangle);
	glBindVertexArray(vao_rectangle);
	glGenBuffers(1, &vbo_position_rectangle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_rectangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(InterleavedArray), InterleavedArray, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),(void *)0);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(3 *sizeof(float)));
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(9 * sizeof(float)));
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	//NORMALS 
	




	//Depth Lines
	glClearDepth(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//initializing Perspective ProjectionMatrix to identity.


	LoadGLTextures(&gTextureStone, MAKEINTRESOURCE(IDBITMAP_MARBLE));
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
	modelViewProjectionMatrix  =mat4::identity();
	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	TranslateMatrix = translate(0.0f, 0.0f, -6.0f);
	//	RotationMatrix = rotate(angleRectangle, 1.0f, 0.0f, 0.0f);
	RotationMatrix = rotate(angleRectangle, angleRectangle, angleRectangle);
	// perform necessary transformations
	modelMatrix = TranslateMatrix * RotationMatrix;

	glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glActiveTexture(GL_TEXTURE0);					// 1. 0th texture (corresponds to YSG_ATTRIBUTE_TEXTURE0)
	glBindTexture(GL_TEXTURE_2D, gTextureStone);	// 2. bind to the texture
	glUniform1i(gTextureSamplerUniform, 0);




	glBindVertexArray(vao_rectangle);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	glBindVertexArray(0);
	// unuse program
	glUseProgram(0);
	SwapBuffers(ghdc);
	if (Animation == 1)
	{
		angleRectangle = angleRectangle - 0.1f;
		if (angleRectangle <= -360.0f)
			angleRectangle = 0.0f;
	}
}

void uninitialize(void)
{

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