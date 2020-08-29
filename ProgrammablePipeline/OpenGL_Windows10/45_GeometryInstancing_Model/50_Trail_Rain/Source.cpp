#include <Windows.h>
#include<stdio.h>
#include<gl/glew.h>
#include<gl/GL.h>

#include"vmath.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")

using namespace vmath;

enum
{
	VDG_ATTRIBUTE_VERTEX = 0,
	VDG_ATTRIBUTE_COLOR,
	VDG_ATTRIBUTE_NORMAL,
	VDG_ATTRIBUTE_TEXTURE0,
};

//Prototype of WndProc() declared Globally
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


//Global variable declarations
FILE *gpFile = NULL;

HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;


DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullScreen = false;

GLuint gVertexShaderObject;
GLuint gGeometryShaderObject;

GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

GLuint gVao;
GLuint gVbo;
GLuint gVao2;
GLuint gVbo2;

GLuint vboNormals;

GLuint gVaoColor;
GLuint gVboColor;
GLuint gVboColor2;

GLuint gMVPUniform;
GLuint gMVPUniform2;

mat4 modelviewMatrix = mat4::identity();
mat4 modelviewProjectionMatrix = mat4::identity();

mat4 modelMatrix = mat4::identity();
mat4 viewMatrix = mat4::identity();
mat4 projectionMatrix = mat4::identity();

GLuint La_uniform;
GLuint Ld_uniform;
GLuint Ls_uniform;
GLuint light_position_uniform;

GLuint Ka_uniform;
GLuint Kd_uniform;
GLuint Ks_uniform;
GLuint material_shininess_uniform;

GLfloat lightAmbient[] = { 1.25f,1.25f,1.25f,1.0f };
GLfloat lightDiffuse[] = { 1.0f,0.0f,0.5f,1.0f };
GLfloat lightSpecular[] = { 1.0f,0.0f,0.0f,1.0f };
GLfloat lightPosition[] = { 0.0f,1.0f,4.0f,1.0f };

GLfloat material_ambient[] = { 0.20f,0.20f,0.20f,1.0f };
GLfloat material_diffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_specular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_shininess = 128.0f;
bool gbLight;

GLuint modelUniform, viewUniform, projectionUniform;

GLfloat gWidth, gHeight;

GLfloat yAnimateSlanting = 3.0f;			//-0.5

GLfloat yAnimateSlanting2 = 3.5f;			//-1.0

GLfloat yAnimateSlanting3 = 4.0f;			//-1.5

GLfloat yAnimateSlanting4 = 4.5f;			//-2.0

GLfloat yAnimateSlanting5 = 5.0f;			//-2.5

GLfloat yAnimateSlanting6 = 5.5f;			//-3.0

GLfloat yAnimateSlanting7 = 6.0f;			//-3.5

GLuint gNumberOfSegmentsUniform;
GLuint gNumberOfStripsUniform;
GLuint gLineColorUniform;
unsigned int gNumberOfLineSegments;

mat4 gPerspectiveProjectionMatrix;
GLuint L_KeyPressed_uniform;

//main()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	//function prototype
	void display();
	void initialize();
	void uninitialize(void);

	//variable declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("OpenGLPP");
	bool bDone = false;

	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Log File Can not be Created\Exitting.."), TEXT("ERROR"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log file is Successfully Openend \n");
	}

	//code
	//initializing member of WNDCLASS
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;

	//Registering Class
	RegisterClassEx(&wndclass);

	//CreateWindow
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szClassName,
		TEXT("OpenGL Programmable PipeLine Using Native Windowing: Geometry Shader White Triangles Shree Ganeshaya Namaha"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		0,
		0,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ghwnd = hwnd;

	//intitialize()
	initialize();

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	//Message Loop
	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			//display();
			if (gbActiveWindow == true)
			{
				display();

				if (gbEscapeKeyIsPressed == true)
					bDone = true;
			}
		}
	}

	uninitialize();
	return((int)msg.wParam);

}

//WndProc()
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//function prototypes
	//	void display();
	void resize(int, int);
	void ToggleFullScreen(void);
	void uninitialize(void);

	//code
	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
			gbActiveWindow = true;
		else
			gbActiveWindow = false;
		break;
	case WM_PAINT:
		//		display();
		break;

	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{

		case VK_UP:
			gNumberOfLineSegments++;
			if (gNumberOfLineSegments >= 50)
				gNumberOfLineSegments = 50; // reset
			break;
		case VK_DOWN:
			gNumberOfLineSegments--;
			if (gNumberOfLineSegments <= 0)
				gNumberOfLineSegments = 1; // reset
			break;
		case VK_ESCAPE:
			gbEscapeKeyIsPressed = true;
			break;

		case 0x46:			//for 'f' or 'F'
			if (gbFullScreen == false)
			{
				ToggleFullScreen();
				gbFullScreen = true;
			}
			else
			{
				ToggleFullScreen();
				gbFullScreen = false;
			}
			break;
		default:
			break;
		}
		break;


	case WM_LBUTTONDOWN:
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));

}


void ToggleFullScreen(void)
{
	//variable declarations
	MONITORINFO mi;

	//code
	if (gbFullScreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left,
					mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}

		}
		ShowCursor(FALSE);
	}
	else
	{
		//code
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
}

void initialize()
{
	//function prototype
	void uninitialize(void);
	void resize(int, int);

	//variable declaration
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	//code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	//Initialization code
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 8;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;

	ghdc = GetDC(ghwnd);

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	//******** VERTEX SHADER ******
	//create shader
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to shader
	const GLchar *vertexShaderSourceCode =
		"#version 430" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \

		"out VertexData{"\
		"vec3 transformed_normals;" \
		"vec3 light_direction;" \
		"vec3 viewer_vector;" \
		"}vs_out;"\

		"uniform mat4 u_projection_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_model_matrix;" \


		"uniform vec4 u_light_position;" \
		"uniform int u_lighting_enabled;" \

		"void main(void)" \
		"{" \
		"	if(u_lighting_enabled == 1)" \
		"	{" \
		"		vec4 eye_coordinates = u_view_matrix*u_model_matrix*vPosition;" \
		"		vs_out.transformed_normals = mat3(u_view_matrix * u_model_matrix) * vNormal;" \
		"		vs_out.light_direction = vec3(u_light_position) - eye_coordinates.xyz;" \
		"		vs_out.viewer_vector = -eye_coordinates.xyz;" \
		"	}" \
		"gl_Position = u_view_matrix * u_model_matrix * vPosition;" \
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
				fprintf(gpFile, "Vertex Shader Compilation Log:%s\n", szInfoLog);
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
		"layout(line_strip,max_vertices = 2)out;" \

		"uniform mat4 u_projection_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_model_matrix;" \

		"in VertexData{"\
		"vec3 transformed_normals;" \
		"vec3 light_direction;" \
		"vec3 viewer_vector;" \
		"}gs_in[];"\


		"out VertexData{"\
		"vec3 transformed_normals;" \
		"vec3 light_direction;" \
		"vec3 viewer_vector;" \
		"}vertexOut;"\


		"float rand(vec2 co)"\
		"{"\
		"	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);"\
		"}"\
		"float randx = 100*rand(vec2(gl_InvocationID,0.0f));"\
		"float randy = rand(vec2(gl_InvocationID,0.0f));"\
		"float randz =  rand(vec2(gl_InvocationID,0.0f));"\

		"void main(void)" \
		"{"\

		"gl_Position = u_projection_matrix * ((gl_in[0].gl_Position )+vec4(0.0, -0.5, 0.0, 0.0)+vec4(randx,randy,randz,0.0) );"\
		"EmitVertex();"\
		"vertexOut.transformed_normals=gs_in[0].transformed_normals;"	\
		"vertexOut.light_direction=gs_in[0].light_direction;"	\
		"vertexOut.viewer_vector=gs_in[0].viewer_vector;"	\


		"gl_Position = u_projection_matrix *( (gl_in[0].gl_Position )+vec4(0.0, 0.2, 0.0, 0.0)+vec4(randx,randy,randz,0.0));"\
		"EmitVertex();"\
		"vertexOut.transformed_normals=gs_in[0].transformed_normals;"	\
		"vertexOut.light_direction=gs_in[0].light_direction;"	\
		"vertexOut.viewer_vector=gs_in[0].viewer_vector;"	\

		"EndPrimitive();"\


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
				fprintf(gpFile, "Geometry Shader Compilation Log:%s\n", szInfoLog);
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
		"out vec4 FragColor;" \
		"uniform vec3 u_La;" \
		"uniform vec3 u_Ld;" \
		"uniform vec3 u_Ls;" \
		"uniform vec3 u_Ka;" \
		"uniform vec3 u_Kd;" \
		"uniform vec3 u_Ks;" \
		"uniform float u_material_shininess;" \
		"uniform int u_lighting_enabled;" \

		"in VertexData{"\
		"vec3 transformed_normals;" \
		"vec3 light_direction;" \
		"vec3 viewer_vector;" \
		"}fragout;"\

		"void main(void)" \
		"{" \

		"vec3 phong_ads_color;" \
		"vec4 FragLight;"\


		"if(u_lighting_enabled==1)"\
		"{" \
		"	vec3 normalized_transformed_normals=normalize(fragout.transformed_normals);" \
		"	vec3 normalized_light_direction=normalize(fragout.light_direction);" \
		"	vec3 normalized_viewer_vector=normalize(fragout.viewer_vector);" \
		"	vec3 ambient = u_La * u_Ka;" \
		"	float tn_dot_ld = max(dot(normalized_transformed_normals, normalized_light_direction),0.0);" \
		"	vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;" \
		"	vec3 reflection_vector = reflect(-normalized_light_direction,normalized_transformed_normals);" \
		"	vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector,normalized_viewer_vector),0.0),u_material_shininess);" \
		"	phong_ads_color = ambient + diffuse + specular;" \
		"	FragLight = vec4(phong_ads_color,1.0);"\
		"	FragColor = FragLight;" \
		"}"\
		"else"\
		"{"\
		"	FragColor = vec4(1,1,1,1);" \
		"}"\
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
				fprintf(gpFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}




	//********** SHADER PROGRAM *********
	//create
	gShaderProgramObject = glCreateProgram();

	// attach vertex shader to shader program
	glAttachShader(gShaderProgramObject, gVertexShaderObject);

	// attach tessellation control shader to shader program
	glAttachShader(gShaderProgramObject, gGeometryShaderObject);

	// attach fragment shader to shader program
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);

	// pre-link binding of shader program object with vertex shader position attribute
	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_VERTEX, "vPosition");

	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_NORMAL, "vNormal");

	// link shader
	glLinkProgram(gShaderProgramObject);
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject, iInfoLength, &written, szInfoLog);
				fprintf(gpFile, "Shader Program Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}


	//get MVP uniform locaion
	gMVPUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
	gMVPUniform2 = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix2");
	modelUniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
	viewUniform = glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
	projectionUniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");

	L_KeyPressed_uniform = glGetUniformLocation(gShaderProgramObject, "u_lighting_enabled");
	La_uniform = glGetUniformLocation(gShaderProgramObject, "u_La");
	//diffuse color intensity of LIGHT
	Ld_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ld");
	//specular color intensity of LIGHT
	Ls_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ls");
	//position of light
	light_position_uniform = glGetUniformLocation(gShaderProgramObject, "u_light_position");

	//ambient reflective color intensity of MATERIAL
	Ka_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ka");
	//diffuse reflective color intensity of MATERIAL
	Kd_uniform = glGetUniformLocation(gShaderProgramObject, "u_Kd");

	//specular reflective color intensity of MATERIAL
	Ks_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ks");
	material_shininess_uniform = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	////**** vertices, colors, shader attribs, vbo, vao initializations ****//

	const GLfloat points[] =
	{

1.0f, 1.0f, 1.0f,
-1.0f, 1.0f, 1.0f,
-1.0f, -1.0f, 1.0f,
1.0f, -1.0f, 1.0f,

	};

	const GLfloat normal[] =
	{
		0.0f, 0.0f, 1.0f,
0.0f, 0.0f, 1.0f,
0.0f, 0.0f, 1.0f,
0.0f, 0.0f, 1.0f,
	};


	glGenVertexArrays(1, &gVao2);
	glBindVertexArray(gVao2);

	glGenBuffers(1, &gVbo2);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vboNormals);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normal), normal, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//glEnable(GL_CULL_FACE);


	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	gPerspectiveProjectionMatrix = mat4::identity();

	resize(WIN_WIDTH, WIN_HEIGHT);

}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	void drawGeometry();
	//start using OpenGL program Object
	glUseProgram(gShaderProgramObject);


	glUniform1i(L_KeyPressed_uniform, 1);

	//setting lights properties
	glUniform3fv(La_uniform, 1, lightAmbient);
	glUniform3fv(Ld_uniform, 1, lightDiffuse);
	glUniform3fv(Ls_uniform, 1, lightSpecular);
	glUniform4fv(light_position_uniform, 1, lightPosition);

	//setting material's properties
	glUniform3fv(Ka_uniform, 1, material_ambient);
	glUniform3fv(Kd_uniform, 1, material_diffuse);
	glUniform3fv(Ks_uniform, 1, material_specular);
	glUniform1f(material_shininess_uniform, material_shininess);

	glPointSize(4.0f);

	gPerspectiveProjectionMatrix = perspective(45.0f, (GLfloat)gWidth / (GLfloat)gHeight, 0.1f, 100.0f);

	//OpenL Drawing 
	//set modelview & modelviewprojection matrices to identity 
	///////////////////////////////////////////// CENTER FRONT ///////////////////////////////////////////////////
	mat4 rotationMatrix = mat4::identity();

	//modelviewProjectionMatrix = mat4::identity();
	//modelviewMatrix = translate(-8.0f, 0.0f, -8.0f);

	//drawGeometry();

	/////////////////////////////////////////////////CENTER BACK //////////////////////

	/*modelMatrix = mat4::identity();
	modelMatrix = translate(-8.0f, yAnimateSlanting, -10.0f);
	drawGeometry();

	/////////////////////////////////////////////////CENTER UP BACK Y=2.5//////////////////////

	modelMatrix = mat4::identity();
	modelMatrix = translate(-6.0f, yAnimateSlanting2, -10.0f);

	drawGeometry();


	///////////////////////////////////////////////////CENTER UP BACK Y=2.0//////////////////////

	modelMatrix = mat4::identity();
	modelMatrix = translate(-6.0f, yAnimateSlanting3, -10.0f);

	drawGeometry();
	/////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	modelMatrix = mat4::identity();
	modelMatrix = translate(-6.0f, yAnimateSlanting4, -10.0f);
	drawGeometry();

	///////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	modelMatrix = mat4::identity();
	modelviewMatrix = translate(-6.0f, yAnimateSlanting5, -10.0f);
	drawGeometry();


	/////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

	modelMatrix = mat4::identity();
	modelMatrix = translate(-6.0f, yAnimateSlanting6, -5.0f);

	drawGeometry();
	/////////////////////////////////////////////////CENTER UP BACK Y=1.0 //////////////////////

	modelMatrix = mat4::identity();
	modelMatrix = translate(-6.0f, yAnimateSlanting7, -10.0f);
	drawGeometry();


	///////////////////////////////////////////////////CENTER UP BACK Y=0.5 //////////////////////

	//modelMatrix = mat4::identity();
	//modelMatrix = translate(-6.0f, 0.5f, -10.0f);
	//drawGeometry();

	//////////////////////////////////////////////////CENTER DOWN BACK Y=-0.5 //////////////////////

	//modelMatrix = mat4::identity();
	//modelMatrix = translate(-6.0f, -0.5f, -10.0f);
	//drawGeometry();

	//////////////////////////////////////////////////CENTER DOWN BACK Y=-1.0 //////////////////////

	//modelMatrix = mat4::identity();
	//modelMatrix = translate(-6.0f, -1.0f, -10.0f);
	//drawGeometry();

	//////////////////////////////////////////////////CENTER DOWN BACK Y=-1.5 //////////////////////

	//modelMatrix = mat4::identity();
	//modelMatrix = translate(-6.0f, -1.5f, -10.0f);
	//drawGeometry();
	//////////////////////////////////////////////////CENTER DOWN BACK Y=-2.0//////////////////////

	//modelMatrix = mat4::identity();
	//modelMatrix = translate(-6.0f, -2.0f, -10.0f);
	//drawGeometry();

	//////////////////////////////////////////////////CENTER DOWN BACK Y=-2.5//////////////////////

	//modelMatrix = mat4::identity();
	//modelMatrix = translate(-6.0f, -2.5f, -10.0f);
	//drawGeometry();
	*/

	/////////////////////////////////////////////////////////////Z FRONT UP///////////////////////////////////////////////////////////


	///////////////////////////////////////////////  FRONT Top y=0.5///////////////////////////////////////////////////

	modelMatrix = mat4::identity();
	modelMatrix = translate(-6.0f, 0.5f, -8.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	modelMatrix = mat4::identity();
	modelMatrix = translate(-6.0f, 1.0f, -8.0f);

	drawGeometry();

	/////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

	modelMatrix = mat4::identity();
	modelMatrix = translate(-6.0f, 1.5f, -8.0f);

	drawGeometry();


	/////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

	modelMatrix = mat4::identity();
	modelMatrix = translate(-6.0f, 2.0f, -8.0f);

	drawGeometry();








	///////////////////////////////////////////////////////////Z FRONT DOWN///////////////////////////////////////////////////////////


	/////////////////////////////////////////////  FRONT Down y=-0.5///////////////////////////////////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-2.0f, -0.5f, -8.0f);

		//drawGeometry();

		///////////////////////////////////////////////  FRONT Down y=-1.0///////////////////////////////////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-2.0f, -1.0f, -8.0f);

		//drawGeometry();

		///////////////////////////////////////////////  FRONT Down y=-1.0///////////////////////////////////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-2.0f, -1.5f, -8.0f);

		//drawGeometry();


		///////////////////////////////////////////////  FRONT Down y=-2.0///////////////////////////////////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-2.0f, -2.0f, -8.0f);

		//drawGeometry();


		///////////////////////////////////////////////  FRONT Down y=-2.5///////////////////////////////////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-2.0f, -2.5f, -8.0f);

		//drawGeometry();



		/////////////////////////////////////////////////   X NEGATIVE SIDE //////////////////////////////////////////////////

		//



		/////////////////////////////////////////////// CENTER FRONT ///////////////////////////////////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-5.8f, 0.0f, -8.0f);

		//drawGeometry();

		///////////////////////////////////////////////////CENTER BACK //////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-5.0f, 0.0f, -10.0f);
		//drawGeometry();

		///////////////////////////////////////////////////CENTER UP BACK Y=2.5//////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-5.0f, 2.5f, -10.0f);

		//drawGeometry();


		///////////////////////////////////////////////////CENTER UP BACK Y=2.0//////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-5.0f, 2.0f, -10.0f);

		//drawGeometry();
		///////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-5.0f, 1.5f, -10.0f);
		//drawGeometry();

		///////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-5.0f, 1.5f, -10.0f);
		//drawGeometry();


		///////////////////////////////////////////////////CENTER UP BACK Y=1.5 //////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-5.0f, 1.5f, -5.0f);

		//drawGeometry();
		///////////////////////////////////////////////////CENTER UP BACK Y=1.0 //////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-5.0f, 1.0f, -10.0f);
		//drawGeometry();


		///////////////////////////////////////////////////CENTER UP BACK Y=0.5 //////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-5.0f, 0.5f, -10.0f);
		//drawGeometry();

		//////////////////////////////////////////////////CENTER DOWN BACK Y=-0.5 //////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-5.0f, -0.5f, -10.0f);
		//drawGeometry();

		//////////////////////////////////////////////////CENTER DOWN BACK Y=-1.0 //////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-5.0f, -1.0f, -10.0f);
		//drawGeometry();

		//////////////////////////////////////////////////CENTER DOWN BACK Y=-1.5 //////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-5.0f, -1.5f, -10.0f);
		//drawGeometry();
		//////////////////////////////////////////////////CENTER DOWN BACK Y=-2.0//////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-5.0f, -2.0f, -10.0f);
		//drawGeometry();

		//////////////////////////////////////////////////CENTER DOWN BACK Y=-2.5//////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-5.0f, -2.5f, -10.0f);
		//drawGeometry();


		/////////////////////////////////////////////////////////////Z FRONT UP///////////////////////////////////////////////////////////


		///////////////////////////////////////////////  FRONT Top y=0.5///////////////////////////////////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-3.8f, 0.5f, -8.0f);

		//drawGeometry();

		///////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-3.8f, 1.0f, -8.0f);

		//drawGeometry();

		///////////////////////////////////////////////  FRONT Top y=1.0///////////////////////////////////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-3.8f, 1.5f, -8.0f);

		//drawGeometry();


		///////////////////////////////////////////////  FRONT Top y=1.5///////////////////////////////////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-3.8f, 2.0f, -8.0f);

		//drawGeometry();








		////////////////////////////////////////////////////Z FRONT DOWN///////////////////////////////////////////////////////////


		///////////////////////////////////////////////  FRONT Down y=-0.5///////////////////////////////////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-3.8f, yAnimateSlanting, -8.0f);

		//drawGeometry();

		///////////////////////////////////////////////  FRONT Down y=-1.0///////////////////////////////////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-3.8f, yAnimateSlanting2, -8.0f);

		//drawGeometry();

		///////////////////////////////////////////////  FRONT Down y=-1.0///////////////////////////////////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-3.8f, yAnimateSlanting3, -8.0f);

		//drawGeometry();


		///////////////////////////////////////////////  FRONT Down y=-2.0///////////////////////////////////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-3.8f, yAnimateSlanting4, -8.0f);

		//drawGeometry();

		/////////////////////////////////////////////////////// slanting ////////////////////////////////////////////////////////

		/////////////////////////////////////////////  FRONT Down y= negative///////////////////////////////////////////////////

		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-3.8f, -yAnimateSlanting5, -8.0f);

		//drawGeometry();


		/////////////////////////////////////////
		//rotationMatrix = vmath::rotate(20.0f, 0.0f, 0.0f, 1.0f);
		//modelviewProjectionMatrix = mat4::identity();
		//modelMatrix = translate(-4.8f, yAnimateSlanting, -10.0f);
		//modelMatrix = modelMatrix * rotationMatrix;
		//drawGeometry();

		//
		//modelMatrix = mat4::identity();
		//modelMatrix = translate(-3.8f, yAnimateSlanting2, -10.0f);								//-1.0
		//modelMatrix = modelMatrix * rotationMatrix;
		//drawGeometry();

		//
		//modelviewProjectionMatrix = mat4::identity();
		//modelMatrix = translate(-3.8f, yAnimateSlanting3, -10.0f);								//-1.5
		//modelMatrix = modelMatrix * rotationMatrix;
		//drawGeometry();
		//
		//modelviewProjectionMatrix = mat4::identity();
		//modelMatrix = translate(-3.8f, yAnimateSlanting4, -10.0f);							   //-2.0
		//modelMatrix = modelMatrix * rotationMatrix;
		//drawGeometry();

		//modelviewProjectionMatrix = mat4::identity();
		//modelMatrix = translate(-3.8f, yAnimateSlanting5, -10.0f);							  //-2.5	
		//modelMatrix = modelMatrix * rotationMatrix;
		//drawGeometry();

		/////////////////////////////////////////////  FRONT Down y= positive///////////////////////////////////////////////////

	//modelviewProjectionMatrix = mat4::identity();
	//modelMatrix = translate(-3.8f, -2.5f, -8.0f);

	//drawGeometry();


	///////////////////////////////////////
		/*rotationMatrix = vmath::rotate(20.0f, 0.0f, 0.0f, 1.0f);
		modelviewProjectionMatrix = mat4::identity();
		modelMatrix = translate(-3.8f, 0.005f, -10.0f);
		modelMatrix = modelMatrix * rotationMatrix;
		drawGeometry();


		modelviewProjectionMatrix = mat4::identity();
		modelMatrix = translate(-3.8f, 0.5f, -10.0f);
		modelMatrix = modelMatrix * rotationMatrix;
		drawGeometry();


		modelviewProjectionMatrix = mat4::identity();
		modelMatrix = translate(-3.8f, 1.5f, -10.0f);
		modelMatrix = modelMatrix * rotationMatrix;
		drawGeometry();


		modelviewProjectionMatrix = mat4::identity();
		modelMatrix = translate(-3.8f, 2.5f, -10.0f);
		modelMatrix = modelMatrix * rotationMatrix;
		drawGeometry();

		modelviewProjectionMatrix = mat4::identity();
		modelMatrix = translate(-3.8f, 4.5f, -10.0f);
		modelMatrix = modelMatrix * rotationMatrix;
		drawGeometry();*/
		//stop using OpenGL program Object
	glUseProgram(0);


	SwapBuffers(ghdc);

	//////////////////////////////////////////////// 
	yAnimateSlanting = yAnimateSlanting - 0.01f;
	if (yAnimateSlanting <= -0.5f)
	{
		yAnimateSlanting = 3.0f;
	}

	yAnimateSlanting2 = yAnimateSlanting2 - 0.01f;
	if (yAnimateSlanting2 <= -1.0f)
	{
		yAnimateSlanting2 = 3.5f;
	}


	yAnimateSlanting3 = yAnimateSlanting3 - 0.01f;
	if (yAnimateSlanting3 <= -1.5f)
	{
		yAnimateSlanting3 = 4.0f;
	}


	yAnimateSlanting4 = yAnimateSlanting4 - 0.01f;
	if (yAnimateSlanting4 <= -2.0f)
	{
		yAnimateSlanting4 = 4.5f;
	}


	yAnimateSlanting5 = yAnimateSlanting5 - 0.01f;
	if (yAnimateSlanting5 <= -2.5f)
	{
		yAnimateSlanting5 = 5.0f;
	}

	yAnimateSlanting6 = yAnimateSlanting6 - 0.01f;
	if (yAnimateSlanting6 <= -3.0f)
	{
		yAnimateSlanting5 = 5.5f;
	}

	yAnimateSlanting7 = yAnimateSlanting7 - 0.01f;
	if (yAnimateSlanting7 <= -3.5f)
	{
		yAnimateSlanting7 = 6.0f;
	}
}






//////////////////////////////////////////////////////////// Draw Geometry ///////////////////////////////////////////////////////////////

void drawGeometry()
{
	//modelviewProjectionMatrix = gPerspectiveProjectionMatrix * modelviewMatrix;	//ORDER IS IMPORTANT

	//glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelviewProjectionMatrix);

	glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, projectionMatrix);

	projectionMatrix = gPerspectiveProjectionMatrix * modelMatrix;

	glBindVertexArray(gVao2);

	glDrawArrays(GL_POINTS, 0, 4);

	//**** unbind vao ****
	glBindVertexArray(0);

}



void resize(int width, int height)
{
	gWidth = width;
	gHeight = height;
	//code
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	//gPerspectiveProjectionMatrix = perspective(60.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

}

void uninitialize(void)
{
	//UNINITIALIZATION CODE
	if (gbFullScreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}

	//destroy vao
	if (gVao)
	{
		glDeleteVertexArrays(1, &gVao);
		gVao = 0;
	}

	//destroy vbo
	if (gVbo)
	{
		glDeleteBuffers(1, &gVbo);
		gVbo = 0;
	}


	//detach vertex shader from shader program object
	glDetachShader(gShaderProgramObject, gVertexShaderObject);

	glDetachShader(gShaderProgramObject, gGeometryShaderObject);

	//detach fragment shader from shader program object
	glDetachShader(gShaderProgramObject, gFragmentShaderObject);

	//delete vertex shader object 
	glDeleteShader(gVertexShaderObject);
	gVertexShaderObject = 0;

	glDeleteShader(gGeometryShaderObject);
	gGeometryShaderObject = 0;


	//delete fragment shader object
	glDeleteShader(gFragmentShaderObject);
	gFragmentShaderObject = 0;

	//unlink shader program
	glUseProgram(0);

	wglMakeCurrent(NULL, NULL);

	wglDeleteContext(ghrc);
	ghrc = NULL;

	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;

	DestroyWindow(ghwnd);
	ghwnd = NULL;

}



























































