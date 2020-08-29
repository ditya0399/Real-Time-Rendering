#include<Windows.h>
#include<glew.h>               // This SHOULD be above GL.h header.
#include<gl/GL.h>
#include"Sphere.h"
#include<stdio.h>
#include"vmath.h"
#pragma comment(lib,"opengl32.lib")//for linking
#pragma comment(lib,"glew32.lib")//for linking
#pragma comment(lib,"Sphere.lib")

bool Blur = true;
bool WireFrame = true;

//Sphere
GLuint mvpUniform;
float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

int gNumVertices;
int gNumElements;



GLuint gNumberOfSegmentUniform; //MultipleLines -> Stips  | One Line divided into Multiple parts ->Segments
GLuint gNumberOfStripsUniform;
GLuint gLineColorUniform;
unsigned int gNumberOfLineSegments;


int Win_Width = 800;
int Win_Height = 600;
using namespace vmath;

static GLfloat angle = 0.0f;
GLuint gShaderProgramObject;
GLuint gShaderProgramObjectGodRays;
GLfloat angleTriangle = 0.0f;
GLfloat angleRectangle = 0.0f;

GLuint factorb0;
GLuint factorb1;
GLuint factorb2;

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0
};

//

GLuint ColorBuffer;
GLuint DepthBuffer;
GLuint FBO;

GLuint vao_Sphere;
GLuint vbo_position_Sphere;
GLuint vbo_Normals_Sphere;
GLuint vbo_Element_Sphere;


GLuint vao_GodRays;
GLuint vbo_GodRays;

int gWidth, gHeight;

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


GLfloat lightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightPosition[] = { 100.0f,100.0f,100.0f,1.0f };

GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialShininess = 50.0f;

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
		case VK_UP:
			gNumberOfLineSegments++;
			if (gNumberOfLineSegments >= 50)
				gNumberOfLineSegments = 50;
			fprintf(gpLogFile, "Value of gNumberOfLineSegments is %d\n", gNumberOfLineSegments);
			break;

		case VK_DOWN:
			gNumberOfLineSegments <= 0;
			gNumberOfLineSegments = 1;

			break;
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
	GLuint gVertexShaderObjectGodRays;
	GLuint gFragmentShaderObjectGodRays;
	GLuint gFragmentShaderObject;
	GLuint gTesselationControlShaderObject;
	GLuint gTesselationEvaluationShaderObject;


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
		"in vec2 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform int u_LKeyPressed;" \
		"out VS_OUT" \
		"{" \
		" vec3 tNorm;" \
		" vec3 light_direction;" \
		" vec3 viewVector;" \
		"}vs_out;" \
		"uniform vec4 u_LightPosition;" \
		"out vec3 LDN;" \
		"void main(void)" \
		"{" \
		"if(u_LKeyPressed == 1)" \
		"{" \
		"vec4 eye_coordinates = u_viewMatrix * u_modelMatrix *  vec4(vPosition,0.0,1.0);" \
		"vs_out.tNorm= mat3(u_viewMatrix * u_modelMatrix) * vNormal;" \
		"vs_out.light_direction = vec3(u_LightPosition - eye_coordinates);" \
		"vs_out.viewVector = vec3(-eye_coordinates.xyz);" \
		"}" \
		"gl_Position = vec4(vPosition,0.0,1.0);" \

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
		" vec3 tNorm;" \
		" vec3 light_direction;" \
		" vec3 viewVector;" \
		"}tcs_in[];" \

		"out TCS_OUT" \
		"{" \
		" vec3 tNorm;" \
		" vec3 light_direction;" \
		" vec3 viewVector;" \
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
		"tcs_out[gl_InvocationID].tNorm = tcs_in[gl_InvocationID].tNorm;" \
		"tcs_out[gl_InvocationID].light_direction = tcs_in[gl_InvocationID].light_direction;" \
		"tcs_out[gl_InvocationID].viewVector = tcs_in[gl_InvocationID].viewVector;" \
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


		"in TCS_OUT" \
		"{" \
		" vec3 tNorm;" \
		" vec3 light_direction;" \
		" vec3 viewVector;" \
		"}tes_in[];" \


		"out TES_OUT" \
		"{" \
		" vec3 tNorm;" \
		" vec3 light_direction;" \
		" vec3 viewVector;" \
		"}tes_out;" \
		"uniform float factorb2;" \
		"uniform float factorb1;" \
		"uniform float factorb0;" \
		"void main(void)" \
		"{" \

	
		"float u =  gl_TessCoord.x;" \
		"vec3 p0 = gl_in[0].gl_Position.xyz;" \
		"vec3 p1 = gl_in[1].gl_Position.xyz;" \
		"vec3 p2 = gl_in[2].gl_Position.xyz;" \
		"vec3 p3 = gl_in[3].gl_Position.xyz;" \
		"float u1 = (1.0 - u);" \
		"float u2 = u * u;" \
		"float b3 =  (u2 * u);" \
		"float b2 = factorb2 * u2 * u1;" \
		"float b1 = factorb1 * u * u1 * u1;" \
		"float b0 =  (u1 * u1 * u1)/factorb0;" \
		"vec3 p = p0 * b0 + p1 * b1 + p2 * b2 +p3 * b3;" \
		
		"gl_Position = u_mvp_matrix * vec4(p , 1.0);" \
		"tes_out.tNorm = tes_in[0].tNorm;" \
		"tes_out.light_direction = tes_in[0].light_direction;" \
		"tes_out.viewVector = tes_in[0].viewVector;" \


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
		
		
		"in TES_OUT" \
		"{" \
		" vec3 tNorm;" \
		" vec3 light_direction;" \
		" vec3 viewVector;" \
		"}fs_in;" \

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
		"vec3 normalizedTNorm = normalize(fs_in.tNorm);" \
		"vec3 normalizedLightDirection = normalize(fs_in.light_direction);" \
		"vec3 normalizedViewVector = normalize(fs_in.viewVector);" \
		"float tNormal_dot_lightDirection = max(dot(normalizedViewVector,normalizedTNorm),0.0);" \
		"vec3 reflectionVector = reflect(-normalizedLightDirection,normalizedTNorm);" \
		
		"vec3 ambient = u_LAUniform * u_KAUniform;" \
		"vec3 diffuse = u_LDUniform * u_KDUniform * tNormal_dot_lightDirection;" \
		"float EVdotLDR = pow(max(dot(reflectionVector,normalizedViewVector),0.0),32.0);" \
		"vec3 specular = u_LSUniform * u_KSUniform * pow(max(dot(reflectionVector,normalizedViewVector),0.0),u_MaterialShininess);" \
		"vec3 color = vec3(0.4,0.2,0.8);" \
		"phongADSLight = color * tNormal_dot_lightDirection + vec3(EVdotLDR);" \
		"}" \
		"else" \
		"{" \
		"phongADSLight = vec3(0.4,0.2,0.8);" \
		"}" \
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
	glAttachShader(gShaderProgramObject, gTesselationControlShaderObject);
	glAttachShader(gShaderProgramObject, gTesselationEvaluationShaderObject);
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
	mvpUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
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

	gNumberOfSegmentUniform = glGetUniformLocation(gShaderProgramObject, "numberOfSegments");
	gNumberOfStripsUniform = glGetUniformLocation(gShaderProgramObject, "numberOfStrips");

	///////////////////// G O D --- R A Y S ///////////////////////////
	//Define Vertex Shader Object
	gVertexShaderObjectGodRays = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code

	const GLchar *vertexShaderSourceCodeGodRays =

		"#version 430 core" \
		"\n"
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"varying vec4 vTexCoord;" \
		"out vec4 out_texCoord;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform int u_LKeyPressed;" \
		"out vec3 tNorm;" \
		"out vec3 light_direction;" \
		"out vec3 viewVector;" \
		"uniform vec4 u_LightPosition;" \
		"out vec3 LDN;" \
		"void main(void)" \
		"{" \
		 "vTexCoord = vPosition;" \
		"out_texCoord = vTexCoord;" \
		"gl_Position = vPosition * 2.0 - 1.0;" \

		"}";

	// GPU will run the above code. And GPU WILL RUN FOR PER VERTEX. If there are 1000 vertex. Then GPU will run this shader for
	//1000 times. We are Multiplying each vertex with the Model View Matrix.
	//And how does the GPU gets to know about at what offset the array has to be taken . Go to glVertexAttribPointer() in Display.
	// in = Input. 

	//p = &vertexShaderSourceCode;
		//Specify above source code to the vertex shader object
	glShaderSource(gVertexShaderObjectGodRays, 1, (const GLchar **)&vertexShaderSourceCodeGodRays, NULL);

	//Compile the vertex shader 
	glCompileShader(gVertexShaderObjectGodRays);

	//////////////// Error Checking//////////////////
	//Code for catching the errors 
	 iShaderCompileStatus = 0;
	 iInfoLogLength = 0;
	szInfoLog = NULL;


	glGetShaderiv(gVertexShaderObjectGodRays, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObjectGodRays, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObjectGodRays, iInfoLogLength, &written, szInfoLog);
				fprintf(gpLogFile, "VertexShaderGodRays :\n");
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
	gFragmentShaderObjectGodRays = glCreateShader(GL_FRAGMENT_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar *fragmentShaderSourceCodeGodRays =
		"#version 430 core" \
		"\n" \
		"in vec3 tNorm;" \
		"in vec3 light_direction;" \
		"in vec3 viewVector;" \
		"in vec4 out_texCoord;" \
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

		"uniform sampler2D ColorBuffer;" \
		"uniform sampler2D ColorBuffer1;" \
		"void main(void)" \
		"{" \
			"int Samples = 200;" \
			"float Intensity = 2.225;" \
			"float Decay = 0.86875;" \
			"vec2 TexCoord = out_texCoord.xy;" \
			"vec2 TexCoord1 = out_texCoord.xy;" \
			"vec2 Direction = vec2(0.5) * TexCoord.xy;" \
			"vec2 Direction1 = vec2(0.5) * TexCoord1.xy;" \
			"Direction = Direction / Samples;" \
			"Direction1 = Direction1 / Samples;" \

			"vec3 Color = texture(ColorBuffer,TexCoord).rgb;" \
			"vec3 Color1 = texture(ColorBuffer1,TexCoord1).rgb;" \
			"for(int Sample = 0;Sample < Samples;Sample++)" \
			"{" \
				"Color = Color +Color1 + texture(ColorBuffer,TexCoord).rgb * Intensity;" \
				"Intensity = Intensity *Decay;" \
				"TexCoord = TexCoord  - Direction/2;" \
				"TexCoord1 = TexCoord1 * Direction1/24;" \
			"}" \
			"FragColor = vec4(Color,1.0);" \
		"}";

	//FragColor = vec4(1,1,1,1) = White Color
	//this means here we are giving color to the Triangle.
	//Specify above source code to the vertex shader object.

	glShaderSource(gFragmentShaderObjectGodRays, 1, (const GLchar **)&fragmentShaderSourceCodeGodRays, NULL);
	//Compile the vertex shader 
	glCompileShader(gFragmentShaderObjectGodRays);
	//Code for catching the errors 
	/*iShaderCompileStatus = 0;
	iInfoLogLength = 0;*/
	szInfoLog = NULL;


	glGetShaderiv(gFragmentShaderObjectGodRays, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObjectGodRays, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written1;
				glGetShaderInfoLog(gFragmentShaderObjectGodRays, iInfoLogLength, &written1, szInfoLog);
				fprintf(gpLogFile, "%sFragment Shader GR : \n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}

	// CREATE SHADER PROGRAM OBJECT
	gShaderProgramObjectGodRays = glCreateProgram();
	//attach vertex shader to the gShaderProgramObject
	glAttachShader(gShaderProgramObjectGodRays, gVertexShaderObjectGodRays);

	//attach fragment shader to the gShaderProgramObject
	glAttachShader(gShaderProgramObjectGodRays, gFragmentShaderObjectGodRays);


	//Pre-Linking  binding to vertexAttributes
	glBindAttribLocation(gShaderProgramObjectGodRays, AMC_ATTRIBUTE_POSITION, "vPosition");
//	glBindAttribLocation(gShaderProgramObjectGodRays, AMC_ATTRIBUTE_NORMAL, "vNormal");
	//Here the above line means that we are linking the GPU's variable vPosition with the CPU's  enum member  i.e AMC_ATTRIBUTE_POSITION .
	//So whatever changes will be done in AMC_ATTRIBUTE_POSITION , those will also reflect in vPosition

	//RULE : ALWAYS BIND THE ATTRIBUTES BEFORE LINKING AND BIND THE UNIFORM AFTER LINKING.

	//Link the shader program 
	glLinkProgram(gShaderProgramObjectGodRays);

	//Code for catching the errors 
//	GLint iProgramLinkStatus = 0;



	glGetProgramiv(gShaderProgramObjectGodRays, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObjectGodRays, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written3;
				glGetProgramInfoLog(gShaderProgramObjectGodRays, iInfoLogLength, &written3, szInfoLog);
				fprintf(gpLogFile, "%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}

	factorb0 = glGetUniformLocation(gShaderProgramObject, "factorb0");
	factorb1 = glGetUniformLocation(gShaderProgramObject, "factorb1");
	factorb2 = glGetUniformLocation(gShaderProgramObject, "factorb2");
	////POST Linking
	////Retrieving uniform locations 
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
	////Here we have done all the preparations of data transfer from CPU to GPU

const GLfloat rectangleVertices[] =
{
		// Perspective square (Top face)
1.0f, 1.0f, -1.0f,
-1.0f, 1.0f, -1.0f,
-1.0f, 1.0f, 1.0f,
1.0f, 1.0f, 1.0f,

1.0f, -1.0f, -1.0f,
-1.0f, -1.0f, -1.0f,
-1.0f, -1.0f, 1.0f,
1.0f, -1.0f, 1.0f,


1.0f, 1.0f, 1.0f,
-1.0f, 1.0f, 1.0f,
-1.0f, -1.0f, 1.0f,
1.0f, -1.0f, 1.0f,


1.0f, 1.0f, -1.0f,
-1.0f, 1.0f, -1.0f,
-1.0f, -1.0f, -1.0f,
1.0f, -1.0f, -1.0f,
1.0f, 1.0f, -1.0f,
1.0f, 1.0f, 1.0f,
1.0f, -1.0f, 1.0f,
1.0f, -1.0f, -1.0f,
-1.0f, 1.0f, -1.0f,
-1.0f, 1.0f, 1.0f,
-1.0f, -1.0f, 1.0f,
-1.0f, -1.0f, -1.0f

	};
	const GLfloat cubeNormals[] =
	{
		0.0f, 1.0f, 0.0f,
0.0f, 1.0f, 0.0f,
0.0f, 1.0f, 0.0f,
0.0f, 1.0f, 0.0f,

0.0f, -1.0f, 0.0f,
0.0f, -1.0f, 0.0f,
0.0f, -1.0f, 0.0f,
0.0f, -1.0f, 0.0f,

0.0f, 0.0f, 1.0f,
0.0f, 0.0f, 1.0f,
0.0f, 0.0f, 1.0f,
0.0f, 0.0f, 1.0f,

0.0f, 0.0f, -1.0f,
0.0f, 0.0f, -1.0f,
0.0f, 0.0f, -1.0f,
0.0f, 0.0f, -1.0f,

1.0f, 0.0f, 0.0f,
1.0f, 0.0f, 0.0f,
1.0f, 0.0f, 0.0f,
1.0f, 0.0f, 0.0f,

-1.0f, 0.0f, 0.0f,
-1.0f, 0.0f, 0.0f,
-1.0f, 0.0f, 0.0f,
-1.0f, 0.0f, 0.0f

	};
	const GLfloat lineVertices[] = {
		-1.0f,-1.0f,-0.5f,1.0f,0.5f,-1.0f,1.0f,1.0f
	};
	const GLfloat lineNormals[] = {
		-1.0,0.5,1.0,
		1.0,-0.5,1.0,
		1.0,1.0,1.0,
		-1.0,-1.0,1.0,
		-1.0,0.5,1.0,
		1.0,-0.5,1.0
	};
	const GLfloat GodRaysVertices[] =
	{
		0.0f,0.0f,
		1.0,0.0,
		1.0,1.0,
		0.0,1.0

	};

	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();

	//Basically we are giving the vertices coordinates in the above array
	////////////////////FOR TRIANGLE ///////////////// 
	//Create vao - Vertex array objects

	//////////////////////// FOR RECTANGLE //////////////// 

	glGenVertexArrays(1, &vao_rectangle);
	glBindVertexArray(vao_rectangle);
	glGenBuffers(1, &vbo_position_rectangle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_rectangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//NORMALS 
	glGenBuffers(1, &vbo_Normals_rectangle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Normals_rectangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormals), cubeNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glGenVertexArrays(1, &vao_GodRays);
	glBindVertexArray(vao_GodRays);
	glGenBuffers(1, &vbo_GodRays);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_GodRays);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GodRaysVertices), GodRaysVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	glGenVertexArrays(1, &vao_Sphere);
	glBindVertexArray(vao_Sphere);
	glGenBuffers(1, &vbo_position_Sphere);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_Sphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	//NORMALS 
	glGenBuffers(1, &vbo_Normals_Sphere);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Normals_Sphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineNormals), lineNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);



	////Elements
	//glGenBuffers(1, &vbo_Element_Sphere);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
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




	//////// GENERATE Textures
	glGenTextures(1, &ColorBuffer);
	glGenTextures(1, &DepthBuffer);

	//generate FBO
	//glGenFramebuffersEXT(1, &FBO);
	glGenFramebuffers(1, &FBO);




	resize(Win_Width, Win_Height);



	gNumberOfLineSegments = 1;






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
	gWidth = width;
	gHeight = height;


}



void display(void)
{
	//gbLighting = 1;
	glBindTexture(GL_TEXTURE_2D, ColorBuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, gWidth, gHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, DepthBuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, gWidth, gHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);




	if (Blur)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorBuffer, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthBuffer, 0);

	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);



	// use shader program
	glUseProgram(gShaderProgramObject);
	if (gbLighting == 1)
	{
		//fprintf(gpLogFile, "gbLighting is TRUE!\n");
		glUniform1i(LKeyPressed, gbLighting);
		glUniform3fv(LDUniform, 1, lightDiffuse);
		glUniform3fv(LAUniform, 1, lightAmbient);
		glUniform3fv(LSUniform, 1, lightSpecular);

		/*glUniform3fv(KDUniform, 1, materialDiffuse);
		glUniform3fv(KAUniform, 1, materialAmbient);
		glUniform3fv(KSUniform, 1, materialSpecular);
		glUniform1f(MaterialShininessUniform, materialShininess);
		glUniform4fv(LightPositionUniform, 1, lightPosition);*/
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
	// intialize above matrices to identity

	////////  RECTANGLE //////////////
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();

	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	TranslateMatrix = translate(-1.0f, 0.0f, -4.0f);
	//	RotationMatrix = rotate(angleRectangle, 1.0f, 0.0f, 0.0f);
//	RotationMatrix = rotate(angleRectangle, angleRectangle, 0.0f);
	// perform necessary transformations
	modelMatrix = TranslateMatrix * RotationMatrix;

	modelViewMatrix = viewMatrix * modelMatrix;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glUniform1i(gNumberOfSegmentUniform, gNumberOfLineSegments);
	glUniform1i(gNumberOfStripsUniform, 1);
	glUniform4fv(gLineColorUniform, 1, vmath::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform1f(factorb0, 1.0f);
	glUniform1f(factorb1, 1.0f);
	glUniform1f(factorb2, 6.0f);

	/*glBindVertexArray(vao_rectangle);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	glBindVertexArray(0);*/

	glBindVertexArray(vao_Sphere);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
	//glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	// bind with textures

	glDrawArrays(GL_PATCHES , 0, 4);
	glBindVertexArray(0);
	
	
	//glLineWidth(15.0f);
	/////////////////////////////////////////////////////////////////////////////
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();

	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	TranslateMatrix = translate(-1.0f, 0.0f, -4.0f);
	RotationMatrix = rotate(180.0f, 0.0f, 1.0f, 0.0f);
	modelMatrix = TranslateMatrix;

	modelViewMatrix = viewMatrix * modelMatrix;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glUniform1i(gNumberOfSegmentUniform, gNumberOfLineSegments);
	glUniform1i(gNumberOfStripsUniform, 1);
	glUniform4fv(gLineColorUniform, 1, vmath::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform1f(factorb0, 1.3f);
	glUniform1f(factorb1, 5.0f);
	glUniform1f(factorb2, 6.0f);

	glBindVertexArray(vao_Sphere);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
	//glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	// bind with textures

	// draw necessary scene
	glDrawArrays(GL_PATCHES, 0, 4);
	glBindVertexArray(0);


	///////////////////////////////////////////////////////////////////////
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();

	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	TranslateMatrix = translate(1.0f, 0.0f, -4.0f);
	RotationMatrix = rotate(180.0f, 0.0f, 1.0f, 0.0f);
	modelMatrix = TranslateMatrix * RotationMatrix;

	modelViewMatrix = viewMatrix * modelMatrix;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glUniform1i(gNumberOfSegmentUniform, gNumberOfLineSegments);
	glUniform1i(gNumberOfStripsUniform, 1);
	glUniform4fv(gLineColorUniform, 1, vmath::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform1f(factorb0, 19.0f);
	glUniform1f(factorb1, 1.0f);
	glUniform1f(factorb2, 2.0f);

	glBindVertexArray(vao_Sphere);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
	//glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	// bind with textures

	// draw necessary scene
	glDrawArrays(GL_PATCHES, 0, 4);
	glBindVertexArray(0);


	////////////////////////////////////////////////////////////////////////
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();

	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	TranslateMatrix = translate(0.2f, 0.2f, -4.0f);
	RotationMatrix = rotate(100.0f, 0.0f, 1.0f, 0.0f);
	modelMatrix = TranslateMatrix * RotationMatrix;

	modelViewMatrix = viewMatrix * modelMatrix;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glUniform1i(gNumberOfSegmentUniform, gNumberOfLineSegments);
	glUniform1i(gNumberOfStripsUniform, 1);
	glUniform4fv(gLineColorUniform, 1, vmath::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform1f(factorb0, 2.0f);
	glUniform1f(factorb1, 2.0f);
	glUniform1f(factorb2, 1.0f);

	glBindVertexArray(vao_Sphere);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
	//glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	// bind with textures

	// draw necessary scene
	glDrawArrays(GL_PATCHES, 0, 4);
	glBindVertexArray(0);

	////////////////////////////////////////////////////////////////////////////
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();

	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	TranslateMatrix = translate(0.5f, 0.2f, -4.0f);
	RotationMatrix = rotate(120.0f, 0.0f, 1.0f, 0.0f);
	modelMatrix = TranslateMatrix * RotationMatrix;

	modelViewMatrix = viewMatrix * modelMatrix;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glUniform1i(gNumberOfSegmentUniform, gNumberOfLineSegments);
	glUniform1i(gNumberOfStripsUniform, 1);
	glUniform4fv(gLineColorUniform, 1, vmath::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform1f(factorb0, 1.0f);
	glUniform1f(factorb1, 5.0f);
	glUniform1f(factorb2, 1.0f);

	glBindVertexArray(vao_Sphere);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
	//glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	// bind with textures

	// draw necessary scene
	glDrawArrays(GL_PATCHES, 0, 4);
	glBindVertexArray(0);



	// unuse program
	glUseProgram(0);



	//glDisable(GL_DEPTH_TEST);

	if (Blur)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER, 0);

		glBindTexture(GL_TEXTURE_2D, ColorBuffer);

		glUseProgram(gShaderProgramObjectGodRays);

		glBindVertexArray(vao_GodRays);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glUseProgram(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	SwapBuffers(ghdc);
	
	angleRectangle = angleRectangle - 0.5f;
	if (angleRectangle <= -360.0f)
		angleRectangle = 0.0f;
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