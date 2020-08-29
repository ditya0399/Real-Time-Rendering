#include<Windows.h>
#include<glew.h>               // This SHOULD be above GL.h header.
#include<gl/GL.h>
#include"Sphere.h"
#include<stdio.h>
#include"vmath.h"
#include "vbm.h"

#pragma comment(lib,"opengl32.lib")//for linking
#pragma comment(lib,"glew32.lib")//for linking
#pragma comment(lib,"Sphere.lib")



#define FRUSTUM_DEPTH 800.0f
#define DEPTH_TEXTURE_SIZE  2048
VBObject object;
float aspect;

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
GLuint render_scene_prog;
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

GLuint vao_Cube;
GLuint vbo_position_Cube;
GLuint vbo_Normals_Cube;
GLuint vbo_Element_Sphere;
GLuint render_light_prog;
struct
{
	GLint model_view_projection_matrix;
} render_light_uniforms;

// FBO to render depth with
GLuint  depth_fbo;
GLuint  depth_texture;

// Ground plane
GLuint  ground_vbo;
GLuint  ground_vao;

GLint current_width;
GLint current_height;



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


struct
{
	GLint model_matrix;
	GLint view_matrix;
	GLint projection_matrix;
	GLint shadow_matrix;
	GLint light_position;
	GLint material_ambient;
	GLint material_diffuse;
	GLint material_specular;
	GLint material_specular_power;
} render_scene_uniforms;


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
void initializeLightShaders();
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

	initializeLightShaders();

	//////////////////////////////// V E R T E X - S H A D E R //////////////////////////


	//Define Vertex Shader Object
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar **p;
	const GLchar *vertexShaderSourceCode =

		"#version 430 core" \
		"\n"
	"	uniform mat4 model_matrix;" \
	"uniform mat4 view_matrix;" \
	"uniform mat4 projection_matrix;" \

	"uniform mat4 shadow_matrix;" \

	"layout(location = 0) in vec4 position;" \
	"layout(location = 1) in vec3 normal;" \

	"out VS_FS_INTERFACE" \
	"{" \
		"vec4 shadow_coord;" \
		"vec3 world_coord;" \
		"vec3 eye_coord;" \
		"vec3 normal;" \
	"} vertex;" \

	"void main(void)" \
	"{" \
		"vec4 world_pos = model_matrix * position;" \
		"vec4 eye_pos = view_matrix * world_pos;" \
		"vec4 clip_pos = projection_matrix * eye_pos;" \

		"vertex.world_coord = world_pos.xyz;" \
		"vertex.eye_coord = eye_pos.xyz;" \
		"vertex.shadow_coord = shadow_matrix * world_pos;"  \
		"vertex.normal = mat3(view_matrix * model_matrix) * normal;" \

		"gl_Position = clip_pos;" \
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

		"uniform sampler2DShadow depth_texture;" \
		"uniform vec3 light_position;" \

		"uniform vec3 material_ambient;	" \
		"uniform vec3 material_diffuse;	" \
		"uniform vec3 material_specular;" \
		"uniform float material_specular_power;" \

		"layout(location = 0) out vec4 color;" \

		"in VS_FS_INTERFACE" \
		"{" \
		"vec4 shadow_coord;" \
		"vec3 world_coord;" \
		"vec3 eye_coord;" \
		"vec3 normal;" \
		"} fragment;" \

		"void main(void)" \
		"{" \
		"vec3 N = fragment.normal;" \
		"vec3 L = normalize(light_position - fragment.world_coord);" \
		"float LdotN = dot(N, L);" \
		"vec3 R = reflect(-L, N);" \

		"float diffuse = max(LdotN, 0.0);" \
		"float specular = max(pow(dot(normalize(-fragment.eye_coord), R), material_specular_power), 0.0);" \

		"float f = textureProj(depth_texture, fragment.shadow_coord);" \

		"color = vec4(material_ambient + f * (material_diffuse * diffuse + material_specular * specular), 1.0);" \
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
	render_scene_prog = glCreateProgram();
	//attach vertex shader to the gShaderProgramObject
	glAttachShader(render_scene_prog, gVertexShaderObject);

	//attach fragment shader to the gShaderProgramObject
	glAttachShader(render_scene_prog, gFragmentShaderObject);


	//Pre-Linking  binding to vertexAttributes
	glBindAttribLocation(render_scene_prog, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(render_scene_prog, AMC_ATTRIBUTE_NORMAL, "vNormal");
	//Here the above line means that we are linking the GPU's variable vPosition with the CPU's  enum member  i.e AMC_ATTRIBUTE_POSITION .
	//So whatever changes will be done in AMC_ATTRIBUTE_POSITION , those will also reflect in vPosition

	//RULE : ALWAYS BIND THE ATTRIBUTES BEFORE LINKING AND BIND THE UNIFORM AFTER LINKING.

	//Link the shader program 
	glLinkProgram(render_scene_prog);

	//Code for catching the errors 
	GLint iProgramLinkStatus = 0;



	glGetProgramiv(render_scene_prog, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(render_scene_prog, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written3;
				glGetProgramInfoLog(render_scene_prog, iInfoLogLength, &written3, szInfoLog);
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
	render_scene_uniforms.model_matrix = glGetUniformLocation(render_scene_prog, "model_matrix");
	render_scene_uniforms.view_matrix = glGetUniformLocation(render_scene_prog, "view_matrix");
	render_scene_uniforms.projection_matrix = glGetUniformLocation(render_scene_prog, "projection_matrix");
	render_scene_uniforms.shadow_matrix = glGetUniformLocation(render_scene_prog, "shadow_matrix");
	render_scene_uniforms.light_position = glGetUniformLocation(render_scene_prog, "light_position");
	render_scene_uniforms.material_ambient = glGetUniformLocation(render_scene_prog, "material_ambient");
	render_scene_uniforms.material_diffuse = glGetUniformLocation(render_scene_prog, "material_diffuse");
	render_scene_uniforms.material_specular = glGetUniformLocation(render_scene_prog, "material_specular");
	render_scene_uniforms.material_specular_power = glGetUniformLocation(render_scene_prog, "material_specular_power");

	//Here we have done all the preparations of data transfer from CPU to GPU







	glUseProgram(render_scene_prog);
	glUniform1i(glGetUniformLocation(render_scene_prog, "depth_texture"), 0);

	glGenTextures(1, &depth_texture);
	glBindTexture(GL_TEXTURE_2D, depth_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Create FBO to render depth into
	glGenFramebuffers(1, &depth_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture, 0);
	glDrawBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);














	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();


	//Basically we are giving the vertices coordinates in the above array
	////////////////////FOR TRIANGLE ///////////////// 
	//Create vao - Vertex array objects

	//////////////////////// FOR RECTANGLE //////////////// 



	const GLfloat CubeVertices[] =
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



	static const float ground_vertices[] =
	{
		-500.0f, -50.0f, -500.0f, 1.0f,
		-500.0f, -50.0f,  500.0f, 1.0f,
		500.0f, -50.0f,  500.0f, 1.0f,
		500.0f, -50.0f, -500.0f, 1.0f,
	};

	static const float ground_normals[] =
	{
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &ground_vao);
	glGenBuffers(1, &ground_vbo);
	glBindVertexArray(ground_vao);
	glBindBuffer(GL_ARRAY_BUFFER, ground_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ground_vertices) + sizeof(ground_normals), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ground_vertices), ground_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(ground_vertices), sizeof(ground_normals), ground_normals);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)sizeof(ground_vertices));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);




	//////////////////////////////////////////////////////////////////////////////////////////



	//glGenVertexArrays(1, &vao_Sphere);
	//glBindVertexArray(vao_Sphere);
	//glGenBuffers(1, &vbo_position_Sphere);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo_position_Sphere);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glEnableVertexAttribArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);


	////NORMALS 
	//glGenBuffers(1, &vbo_Normals_Sphere);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo_Normals_Sphere);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glEnableVertexAttribArray(1);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);



	////Elements
	//glGenBuffers(1, &vbo_Element_Sphere);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);


	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glBindVertexArray(0);



	object.LoadFromVBM("armadillo_low.vbm", 0, 1, 2);

	glGenVertexArrays(1, &vao_Cube);
	glBindVertexArray(vao_Cube);
	glGenBuffers(1, &vbo_position_Cube);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_Cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertices), CubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	//NORMALS 
	glGenBuffers(1, &vbo_Normals_Cube);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Normals_Cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormals), cubeNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glBindVertexArray(0);


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
void initializeLightShaders()
{
	GLuint gVertexShaderObject;
	GLuint gFragmentShaderObject;
	//////////////////////////////// V E R T E X - S H A D E R //////////////////////////


	//Define Vertex Shader Object
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
//	const GLchar **p;
	const GLchar *vertexShaderSourceCode =

		"#version 430 core" \
		"\n" \
		"uniform mat4 model_view_projection_matrix;"\

	"layout(location = 0) in vec4 position;" \

	"void main(void)" \
	"{" \
		"gl_Position = model_view_projection_matrix * position;" \
		//gl_Position.z += 0.1;
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
		"layout(location = 0) out vec4 color;" \

		"uniform vec3 material_ambient;" \

		"void main(void)" \
		"{" \
		"	color = vec4(material_ambient*3.0f, 1.0);" \
	

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
	render_light_prog = glCreateProgram();
	//attach vertex shader to the gShaderProgramObject
	glAttachShader(render_light_prog, gVertexShaderObject);

	//attach fragment shader to the gShaderProgramObject
	glAttachShader(render_light_prog, gFragmentShaderObject);


	//Pre-Linking  binding to vertexAttributes
	glBindAttribLocation(render_light_prog, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(render_light_prog, AMC_ATTRIBUTE_NORMAL, "vNormal");
	//Here the above line means that we are linking the GPU's variable vPosition with the CPU's  enum member  i.e AMC_ATTRIBUTE_POSITION .
	//So whatever changes will be done in AMC_ATTRIBUTE_POSITION , those will also reflect in vPosition

	//RULE : ALWAYS BIND THE ATTRIBUTES BEFORE LINKING AND BIND THE UNIFORM AFTER LINKING.

	//Link the shader program 
	glLinkProgram(render_light_prog);

	//Code for catching the errors 
	GLint iProgramLinkStatus = 0;



	glGetProgramiv(render_light_prog, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(render_light_prog, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written3;
				glGetProgramInfoLog(render_light_prog, iInfoLogLength, &written3, szInfoLog);
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
	render_light_uniforms.model_view_projection_matrix = glGetUniformLocation(render_light_prog, "model_view_projection_matrix");
	//Here we have done all the preparations of data transfer from CPU to GPU

	

}
void resize(int width, int height)
{


	if (height == 0)
	{
		height = 1;
	}

	//glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	current_width = width;
	current_height = height;
 aspect = float(height) / float(width);

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
	//perspectiveProjectionMatrix = perspective(45.0f,
	//	(GLfloat)width / (GLfloat)height,
	//	0.1f,
	//	100.0f);

}



void display(void)
{


//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	float t = float(GetTickCount() & 0xFFFF) / float(0xFFFF);
	static float q = 0.0f;
	static const vec3 X(1.0f, 0.0f, 0.0f);
	static const vec3 Y(0.0f, 1.0f, 0.0f);
	static const vec3 Z(0.0f, 0.0f, 1.0f);

	vec3 light_position = vec3(sinf(t * 6.0f * 3.141592f) * 300.0f, 200.0f, cosf(t * 4.0f * 3.141592f) * 100.0f + 250.0f);

	// Setup
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Matrices for rendering the scene
	mat4 scene_model_matrix = rotate(t * 720.0f, Y);
	//mat4 scene_model_matrix1 = translate(0.0f,-5.0f);
	mat4 scene_view_matrix = translate(0.0f, 0.0f, -200.0f);
	mat4 scene_projection_matrix = frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, FRUSTUM_DEPTH);
	const mat4 scale_bias_matrix = mat4(vec4(0.5f, 0.0f, 0.0f, 0.0f),
		vec4(0.0f, 0.5f, 0.0f, 0.0f),
		vec4(0.0f, 0.0f, 0.5f, 0.0f),
		vec4(0.5f, 0.5f, 0.5f, 1.0f));

	// Matrices used when rendering from the light's position
	mat4 light_view_matrix = lookat(light_position, vec3(0.0f), Y);
	mat4 light_projection_matrix(frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, FRUSTUM_DEPTH));


	glUseProgram(render_light_prog);
	glUniformMatrix4fv(render_light_uniforms.model_view_projection_matrix, 1, GL_FALSE,
		light_projection_matrix * light_view_matrix * scene_model_matrix);

	// Bind the 'depth only' FBO and set the viewport to the size of the depth texture
	glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
	glViewport(0, 0, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);

	// Clear
	glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);

	// Enable polygon offset to resolve depth-fighting isuses
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 2.0f);
	

	//glBindVertexArray(vao_Sphere);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
	//glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//glBindVertexArray(0);
	object.Render();


	/*glBindVertexArray(vao_Cube);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	glBindVertexArray(0);

*/
	glBindVertexArray(ground_vao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindVertexArray(0);

	glDisable(GL_POLYGON_OFFSET_FILL);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, current_width, current_height);


	glUseProgram(render_scene_prog);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	// Setup all the matrices
	glUniformMatrix4fv(render_scene_uniforms.model_matrix, 1, GL_FALSE, scene_model_matrix);
	glUniformMatrix4fv(render_scene_uniforms.view_matrix, 1, GL_FALSE, scene_view_matrix);
	glUniformMatrix4fv(render_scene_uniforms.projection_matrix, 1, GL_FALSE, scene_projection_matrix);
	glUniformMatrix4fv(render_scene_uniforms.shadow_matrix, 1, GL_FALSE,
		scale_bias_matrix * light_projection_matrix * light_view_matrix);
	glUniform3fv(render_scene_uniforms.light_position, 1, light_position);

	// Bind the depth texture
	glBindTexture(GL_TEXTURE_2D, depth_texture);
	glGenerateMipmap(GL_TEXTURE_2D);

	glUniform3fv(render_scene_uniforms.material_ambient, 1, vec3(0.1f, 0.0f, 0.2f));
	glUniform3fv(render_scene_uniforms.material_diffuse, 1, vec3(0.3f, 0.2f, 0.8f));
	glUniform3fv(render_scene_uniforms.material_specular, 1, vec3(1.0f, 1.0f, 1.0f));
	glUniform1f(render_scene_uniforms.material_specular_power, 25.0f);

	/*glBindVertexArray(vao_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);*/


	/*glBindVertexArray(vao_Cube);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	glBindVertexArray(0);*/


	object.Render();


	glUniform3fv(render_scene_uniforms.material_ambient, 1, vec3(0.1f, 0.1f, 0.1f));
	glUniform3fv(render_scene_uniforms.material_diffuse, 1, vec3(0.1f, 0.5f, 0.1f));
	glUniform3fv(render_scene_uniforms.material_specular, 1, vec3(0.1f, 0.1f, 0.1f));
	glUniform1f(render_scene_uniforms.material_specular_power, 3.0f);

glBindVertexArray(ground_vao);
glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
glBindVertexArray(0);



	// unuse program
	glUseProgram(0);
	SwapBuffers(ghdc);
	angleRectangle = angleRectangle - 0.1f;
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

	//if (gShaderProgramObject)
	//{
	//	GLsizei shaderCount;
	//	GLsizei shaderNumber;

	//	glUseProgram(gShaderProgramObject);
	//	glGetProgramiv(gShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);

	//	GLuint *pShaders = (GLuint *)malloc(sizeof(GLuint) * shaderCount);
	//	if (pShaders)
	//	{
	//		glGetAttachedShaders(gShaderProgramObject, shaderCount, &shaderCount, pShaders);

	//		for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++)
	//		{
	//			// detach shader
	//			glDetachShader(gShaderProgramObject, pShaders[shaderNumber]);

	//			// delete shader
	//			glDeleteShader(pShaders[shaderNumber]);
	//			pShaders[shaderNumber] = 0;
	//		}
	//		free(pShaders);
	//	}

	//	glDeleteProgram(gShaderProgramObject);
	//	gShaderProgramObject = 0;
	//	glUseProgram(0);

	//}
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