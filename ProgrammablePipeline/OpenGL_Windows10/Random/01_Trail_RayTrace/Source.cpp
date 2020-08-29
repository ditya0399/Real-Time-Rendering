/* Done By Aditya Boob
	Depth of Field - SuperBible*/
//Date : 18/06/2020 

#include<Windows.h>
#include<glew.h>               // This SHOULD be above GL.h header.
#include<gl/GL.h>

#include<stdio.h>
#include"vmath.h"
#include"object.h"

#pragma comment(lib,"opengl32.lib")//for linking
#pragma comment(lib,"glew32.lib")//for linking


#define FBO_SIZE                2048
#define FRUSTUM_DEPTH           1000


int Win_Width = 800;
int Win_Height = 600;
using namespace vmath;

static GLfloat angle = 0.0f;
GLuint gShaderProgramObject;

sb7::object object;

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
GLuint mvpUniform;
mat4 perspectiveProjectionMatrix;

GLuint display_program;
GLuint filter_program;
GLuint view_program;

struct
{
	struct
	{
		GLint focal_distance;
		GLint focal_depth;

	}dof;
	struct
	{
		GLint mv_matrix;
		GLint proj_matrix;
		GLint full_shading;
		GLint diffuse_albedo;

	}view;

}uniforms;
GLuint          depth_fbo;
GLuint          depth_tex;
GLuint          color_tex;
GLuint          temp_tex;
enum {OBJECT_COUNT = 5};
struct {
	sb7::object obj;
	vmath::mat4 model_matrix;
	vmath::vec4 diffuse_albedo;
}objects[OBJECT_COUNT];
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


mat4 camera_view_matrix;
mat4 camera_proj_matrix;

GLuint quad_vao;

bool paused = false;

float focal_distance = 40.0f;
float focal_depth = 50.0f;

GLfloat Window_wid;
GLfloat Window_heig;


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void ToggleFullScreen(void);
void update(void);
void uninitialize(void);
void render_scene(double currentTime);
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

	//Wndclassinitialization
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
		TEXT("Depth Of Field(Compute Shader) - Aditya Boob "),
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
		case 'Q':
			focal_distance *= 1.1f;
			break;
		case'A':
			focal_distance /= 1.1f;
			break;
		case 'W':
			focal_depth *= 1.1f;
			break;
		case 'S':
			focal_depth /= 1.1f;
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
	
	GLuint gVertexShaderObject_render;
	GLuint gFragmentShaderObject_render;

	GLuint gVertexShaderObject_display;
	GLuint gFragmentShaderObject_display;
	
	GLuint gComputeShader;

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

	GLuint shaders[4];

	//////////////////////////////// V E R T E X - S H A D E R //////////////////////////

	/*This Vertex and Fragment shaders here just renders your object with the Phong Model.*/

	//Define Vertex Shader Object
	gVertexShaderObject_render = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar **p;
	const GLchar *vertexShaderSourceCode =

		"#version 430 core" \
		"\n"

"		uniform mat4 mv_matrix;								  " \
"	uniform mat4 proj_matrix;								  " \
"															  " \
"	layout(location = 0) in vec4 position;					  " \
"	layout(location = 1) in vec3 normal;					  " \
"															  " \
"	out VS_OUT												  " \
"	{														  " \
"		vec3 N;												  " \
"		vec3 L;												  " \
"		vec3 V;												  " \
"	} vs_out;												  " \
"															  " \

"	uniform vec3 light_pos = vec3(100.0, 100.0, 100.0);		  " \
"															  " \
"	void main(void)											  " \
"	{														  " \

"		vec4 P = mv_matrix * position;						  " \
"															  " \

"		vs_out.N = mat3(mv_matrix) * normal;				  " \
"															  " \

"		vs_out.L = light_pos - P.xyz;						  " \
"															  " \

"		vs_out.V = -P.xyz;									  " \
"															  " \

"		gl_Position = proj_matrix * P;                        " \



		"}";
	// GPU will run the above code. And GPU WILL RUN FOR PER VERTEX. If there are 1000 vertex. Then GPU will run this shader for
	//1000 times. We are Multiplying each vertex with the Model View Matrix.
	//And how does the GPU gets to know about at what offset the array has to be taken . Go to glVertexAttribPointer() in Display.
	// in = Input. 

	//p = &vertexShaderSourceCode;
		//Specify above source code to the vertex shader object
	glShaderSource(gVertexShaderObject_render, 1, (const GLchar **)&vertexShaderSourceCode, NULL);

	//Compile the vertex shader 
	glCompileShader(gVertexShaderObject_render);

	//////////////// Error Checking//////////////////
	//Code for catching the errors 
	GLint iShaderCompileStatus = 0;
	GLint iInfoLogLength = 0;
	GLchar *szInfoLog = NULL;


	glGetShaderiv(gVertexShaderObject_render, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_render, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject_render, iInfoLogLength, &written, szInfoLog);
				fprintf(gpLogFile, "Render VS%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}
	/////////////////    F R A G M E N T S H A D E R            //////////////////////////
	//Define Fragment Shader Object
	gFragmentShaderObject_render = glCreateShader(GL_FRAGMENT_SHADER); //This command will create the Shader Object
	//Now Write fragment shader code
	const GLchar *fragmentShaderSourceCode =
		"#version 430 core" \
		"\n" \
		"	layout(location = 0) out vec4 color;													" \
		"																							" \
		"in VS_OUT																					" \
		"{																							" \
		"	vec3 N;																					" \
		"	vec3 L;																					" \
		"	vec3 V;																					" \
		"} fs_in;																					" \
		"																							" \
		"uniform vec3 diffuse_albedo = vec3(0.9, 0.8, 1.0);											" \
		"uniform vec3 specular_albedo = vec3(0.7);													" \
		"uniform float specular_power = 300.0;														" \
		"uniform bool full_shading = true;															" \
		"																							" \
		"void main(void)																			" \
		"{																							" \

		"	vec3 N = normalize(fs_in.N);															" \
		"	vec3 L = normalize(fs_in.L);															" \
		"	vec3 V = normalize(fs_in.V);															" \
		"																							" \
	
		"	vec3 R = reflect(-L, N);																" \
		"																							" \
	
		"	vec3 diffuse = max(dot(N, L), 0.0) * diffuse_albedo;									" \
		"	vec3 specular = pow(max(dot(R, V), 0.0), specular_power) * specular_albedo;				" \
		"																							" \
	
		"	color = vec4(diffuse + specular, fs_in.V.z);                                           " \
			"}";

	//FragColor = vec4(1,1,1,1) = White Color
	//this means here we are giving color to the Triangle.




	//Specify above source code to the vertex shader object
	glShaderSource(gFragmentShaderObject_render, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);

	//Compile the vertex shader 
	glCompileShader(gFragmentShaderObject_render);
	//Code for catching the errors 
		   /*iShaderCompileStatus = 0;
		   iInfoLogLength = 0;*/
	szInfoLog = NULL;


	glGetShaderiv(gFragmentShaderObject_render, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_render, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written1;
				glGetShaderInfoLog(gFragmentShaderObject_render, iInfoLogLength, &written1, szInfoLog);
				fprintf(gpLogFile, "Render FS %s\n", szInfoLog);
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
	glAttachShader(gShaderProgramObject, gVertexShaderObject_render);

	//attach fragment shader to the gShaderProgramObject
	glAttachShader(gShaderProgramObject, gFragmentShaderObject_render);


	//Pre-Linking  binding to vertexAttributes
	//glBindAttribLocation(view_program, AMC_ATTRIBUTE_POSITION, "vPosition");
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
	uniforms.view.proj_matrix = glGetUniformLocation(gShaderProgramObject, "proj_matrix");
	uniforms.view.mv_matrix = glGetUniformLocation(gShaderProgramObject, "mv_matrix");
	uniforms.view.full_shading = glGetUniformLocation(gShaderProgramObject, "full_shading");
	uniforms.view.diffuse_albedo = glGetUniformLocation(gShaderProgramObject, "diffuse_albedo");






	/* This Vertex and Fragment shaders will take the Image from your from your framebuffer in which you have rendered those objects
		and will apply it on the screen. The vertex positions for the quad are defined in the Vertex Shader Itself.*/


//	//Define Vertex Shader Object
	gVertexShaderObject_display = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	
	const GLchar *vertexShaderSourceCode_d =

		"#version 430 core" \
		"\n"

		"void main(void)											 " \
		"{											                 " \
		"	const vec4 vertex[] = vec4[](vec4(-1.0, -1.0, 0.5, 1.0), " \
		"		vec4(1.0, -1.0, 0.5, 1.0),							 " \
		"		vec4(-1.0, 1.0, 0.5, 1.0),							 " \
		"		vec4(1.0, 1.0, 0.5, 1.0));							 " \
		"															 " \
		"	gl_Position = vertex[gl_VertexID];						 " \


		"}";

	glShaderSource(gVertexShaderObject_display, 1, (const GLchar **)&vertexShaderSourceCode_d, NULL);

	//Compile the vertex shader 
	glCompileShader(gVertexShaderObject_display);

	//////////////// Error Checking//////////////////
	//Code for catching the errors 
		iShaderCompileStatus = 0;
		iInfoLogLength = 0;
		 szInfoLog = NULL;


	glGetShaderiv(gVertexShaderObject_display, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_display, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject_display, iInfoLogLength, &written, szInfoLog);
				fprintf(gpLogFile, "%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}
	/////////////////    F R A G M E N T S H A D E R            //////////////////////////
	//Define Fragment Shader Object
	gFragmentShaderObject_display = glCreateShader(GL_FRAGMENT_SHADER); //This command will create the Shader Object
	//Now Write Fragment shader code
	const GLchar *fragmentShaderSourceCode_d =
		"#version 430 core" \
		"\n" \
        "layout(binding = 0) uniform sampler2D input_image;" \
        "layout(location = 0) out vec4 color; " \
		"uniform float focal_distance = 50.0;" \
		"uniform float focal_depth = 30.0;" \

		"void main(void)																			" \
		"{																							" \

" vec2 s = 1.0 / textureSize(input_image, 0);                                                " \
	"																						 " \
	"vec2 C = gl_FragCoord.xy;																 " \
	"																						 " \
	"																						 " \
	"vec4 v = texelFetch(input_image, ivec2(gl_FragCoord.xy), 0).rgba;						 " \
	"																						 " \
	"																						 " \
	"float m;																				 " \
	"																						 " \
	"																						 " \
	"if (v.w == 0.0)																		 " \
	"{																						 " \
	"	m = 0.5;																			 " \
	"}																						 " \
	"else																					 " \
	"{																						 " \
	"																						 " \
	"	m = abs(v.w - focal_distance);														 " \
	"	m = 0.5 + smoothstep(0.0, focal_depth, m) * 7.5;									 " \
	"}																						 " \
	"vec2 P0 = vec2(C * 1.0) + vec2(-m, -m);												 " \
	"vec2 P1 = vec2(C * 1.0) + vec2(-m, m);													 " \
	"vec2 P2 = vec2(C * 1.0) + vec2(m, -m);													 " \
	"vec2 P3 = vec2(C * 1.0) + vec2(m, m);													 " \

	"P0 *= s;																				 " \
	"P1 *= s;																				 " \
	"P2 *= s;																				 " \
	"P3 *= s;																				 " \
	"																						 " \
	"																						 " \
	"vec3 a = textureLod(input_image, P0, 0).rgb;											 " \
	"vec3 b = textureLod(input_image, P1, 0).rgb;											 " \
	"vec3 c = textureLod(input_image, P2, 0).rgb;											 " \
	"vec3 d = textureLod(input_image, P3, 0).rgb;											 " \
	"																						 " \
	"																						 " \
	"vec3 f = a - b - c + d;																 " \
	"																						 " \
	"																						 " \
	"m *= 2;																				 " \
	"																						 " \
	"																						 " \
	"f /= float(m * m);																		 " \
	"																						 " \
	"																						 " \
	"color = vec4(f, 1.0);																	 " \
																						
		"}";

	



	
	glShaderSource(gFragmentShaderObject_display, 1, (const GLchar **)&fragmentShaderSourceCode_d, NULL);

	
	glCompileShader(gFragmentShaderObject_display);
	//Code for catching the errors 
		   /*iShaderCompileStatus = 0;
		   iInfoLogLength = 0;*/
	szInfoLog = NULL;


	glGetShaderiv(gFragmentShaderObject_display, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_display, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written1;
				glGetShaderInfoLog(gFragmentShaderObject_display, iInfoLogLength, &written1, szInfoLog);
				fprintf(gpLogFile, "DISPLAY FS%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}
	// CREATE SHADER PROGRAM OBJECT
	display_program = glCreateProgram();
	//attach vertex shader to the shaderProgramObject
	glAttachShader(display_program, gVertexShaderObject_display);

	//attach fragment shader to the shaderProgramObject
	glAttachShader(display_program, gFragmentShaderObject_display);


	//Pre-Linking  binding to vertexAttributes
	//glBindAttribLocation(view_program, AMC_ATTRIBUTE_POSITION, "vPosition");
	//Here the above line means that we are linking the GPU's variable vPosition with the CPU's  enum member  i.e AMC_ATTRIBUTE_POSITION .
	//So whatever changes will be done in AMC_ATTRIBUTE_POSITION , those will also reflect in vPosition

	//RULE : ALWAYS BIND THE ATTRIBUTES BEFORE LINKING AND BIND THE UNIFORM AFTER LINKING.

	//Link the shader program 
	glLinkProgram(display_program);

	//Code for catching the errors 
	iProgramLinkStatus = 0;



	glGetProgramiv(display_program, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(display_program, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written3;
				glGetProgramInfoLog(display_program, iInfoLogLength, &written3, szInfoLog);
				fprintf(gpLogFile, "Display FS%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}


	//POST Linking
	//Retrieving uniform locations 
	uniforms.dof.focal_distance = glGetUniformLocation(display_program, "focal_distance");
	uniforms.dof.focal_depth = glGetUniformLocation(display_program, "focal_depth");




	//////////// COMPUTE SHADER ////////////
	//Define Compute Shader Object
	gComputeShader = glCreateShader(GL_COMPUTE_SHADER); //This command will create the Shader Object
	//Now Write Compute shader code
	
	const GLchar *computeShaderSourceCode =

		"#version 430 core" \
		"\n"                   \
		"layout (local_size_x = 1024) in; " \

		"shared vec3 shared_data[gl_WorkGroupSize.x *2];" \

		"layout(binding = 0, rgba32f) readonly uniform image2D input_image;" \
		"layout(binding = 1, rgba32f) writeonly uniform image2D output_image;" \

		"void main(void)" \
		"{" \

		"	uint id = gl_LocalInvocationID.x;" \
		"   uint rd_id;" \
		"   uint wr_id;" \
		"	uint mask;"  \
		"		ivec2 P0 = ivec2(id * 2, gl_WorkGroupID.x);					" \
		"	    ivec2 P1 = ivec2(id * 2 + 1, gl_WorkGroupID.x);					" \
		"																	" \
		"	const uint steps = uint(log2(gl_WorkGroupSize.x)) + 1;			" \
		"	uint step = 0;													" \
		"																	" \
		"	vec4 i0 = imageLoad(input_image, P0);							" \
		"	vec4 i1 = imageLoad(input_image, P1);							" \
		"																	" \
		"	shared_data[P0.x] = i0.rgb;										" \
		"	shared_data[P1.x] = i1.rgb;                                     " \

		"	barrier();" \

		"for (step = 0; step < steps; step++)		 " \
		"{											 " \
		"	mask = (1 << step) - 1;                  " \
		"     rd_id = ((id >> step) << (step + 1)) + mask;"		 \
		"       wr_id = rd_id + 1 + (id & mask);"		 \
		"       shared_data[wr_id] += shared_data[rd_id];"  \
		"		barrier();" \
		"    }" \

		" imageStore(output_image, P0.yx, vec4(shared_data[P0.x], i0.a)); " \
		" imageStore(output_image, P1.yx, vec4(shared_data[P1.x], i1.a)); " \
		"}";
	
	glShaderSource(gComputeShader, 1, (const GLchar **)&computeShaderSourceCode, NULL);

	
	glCompileShader(gComputeShader);

	//////////////// Error Checking//////////////////
	//Code for catching the errors 
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;


	glGetShaderiv(gComputeShader, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gComputeShader, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gComputeShader, iInfoLogLength, &written, szInfoLog);
				fprintf(gpLogFile, "CS : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}
	filter_program = glCreateProgram();
	glAttachShader(filter_program, gComputeShader);
	glLinkProgram(filter_program);






	//Here we have done all the preparations of data transfer from CPU to GPU
	
	int i;

	static const char * const object_names[] =
	{
		"dragon.sbm",
		"asteroids.sbm",
		"cube.sbm",
		"ladybug.sbm",
		"dragon.sbm",
	};

	
	static const vmath::vec4 object_colors[] =
	{
		vmath::vec4(1.0f, 0.7f, 0.8f, 1.0f),
		vmath::vec4(0.7f, 0.8f, 1.0f, 1.0f),
		vmath::vec4(0.3f, 0.9f, 0.4f, 1.0f),
		vmath::vec4(0.6f, 0.4f, 0.9f, 1.0f),
		vmath::vec4(0.8f, 0.2f, 0.1f, 1.0f),
	};
	for (i = 0; i < OBJECT_COUNT; i++)
	{
		objects[i].obj.load(object_names[i]);
		objects[i].diffuse_albedo = object_colors[i];
	}

	glGenFramebuffers(1, &depth_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);

	glGenTextures(1, &depth_tex);
	glBindTexture(GL_TEXTURE_2D, depth_tex);
	glTexStorage2D(GL_TEXTURE_2D, 11, GL_DEPTH_COMPONENT32F, FBO_SIZE, FBO_SIZE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &color_tex);
	glBindTexture(GL_TEXTURE_2D, color_tex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, FBO_SIZE, FBO_SIZE);

	glGenTextures(1, &temp_tex);
	glBindTexture(GL_TEXTURE_2D, temp_tex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, FBO_SIZE, FBO_SIZE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_tex, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_tex, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_DEPTH_TEST);

	glGenVertexArrays(1, &quad_vao);
	glBindVertexArray(quad_vao);
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
	Window_wid = width;
	Window_heig = height;
	fprintf(gpLogFile,"Height : %f   Width %f\n", Window_heig, Window_wid);
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


double currentTime;
void display()
{


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	static const GLfloat zeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	static double last_time = 0.0;
	static double total_time = 0.0;

	if (!paused)
		total_time += (currentTime - last_time);
	last_time = currentTime;

	const float f = (float)total_time + 30.0f;

	vmath::vec3 view_position = vmath::vec3(0.0f, 0.0f, 40.0f);

	camera_proj_matrix = vmath::perspective(50.0f,
		(float)Window_wid / (float)Window_heig,
		2.0f,
		300.0f);

	camera_view_matrix = vmath::lookat(view_position,
		vmath::vec3(0.0f),
		vmath::vec3(0.0f, 1.0f, 0.0f));

	objects[0].model_matrix = vmath::translate(5.0f, 0.0f, 20.0f) *
		vmath::rotate(f * 14.5f, 0.0f, 1.0f, 0.0f) *
		vmath::rotate(20.0f, 1.0f, 0.0f, 0.0f) *
		vmath::translate(0.0f, -4.0f, 0.0f);

	objects[1].model_matrix = vmath::translate(-5.0f, 0.0f, 0.0f) *
		vmath::rotate(f * 14.5f, 0.0f, 1.0f, 0.0f) *
		vmath::rotate(20.0f, 1.0f, 0.0f, 0.0f) *
		vmath::translate(0.0f, -4.0f, 0.0f);

	objects[2].model_matrix = vmath::translate(-15.0f, 0.0f, -20.0f) *
		vmath::rotate(f * 14.5f, 0.0f, 1.0f, 0.0f) *
		vmath::rotate(20.0f, 1.0f, 0.0f, 0.0f) *
		vmath::translate(0.0f, -4.0f, 0.0f);

	objects[3].model_matrix = vmath::translate(-25.0f, 0.0f, -40.0f) *
		vmath::rotate(f * 14.5f, 0.0f, 1.0f, 0.0f) *
		vmath::rotate(20.0f, 1.0f, 0.0f, 0.0f) *
		vmath::translate(0.0f, -4.0f, 0.0f);

	objects[4].model_matrix = vmath::translate(-35.0f, 0.0f, -60.0f) *
		vmath::rotate(f * 14.5f, 0.0f, 1.0f, 0.0f) *
		vmath::rotate(20.0f, 1.0f, 0.0f, 0.0f) *
		vmath::translate(0.0f, -4.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	render_scene(total_time);
	glUseProgram(filter_program);

	glBindImageTexture(0, color_tex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(1, temp_tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glDispatchCompute(Window_heig, 1, 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glBindImageTexture(0, temp_tex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(1, color_tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glDispatchCompute(Window_wid, 1, 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, color_tex);
	glDisable(GL_DEPTH_TEST);
	glUseProgram(display_program);
	glUniform1f(uniforms.dof.focal_distance, focal_distance);
	glUniform1f(uniforms.dof.focal_depth, focal_depth);
	glBindVertexArray(quad_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glUseProgram(0);

	SwapBuffers(ghdc);
	currentTime += 0.001f;

}
void render_scene(double currentTime)
{
	static const GLfloat ones[] = { 1.0f };
	static const GLfloat zero[] = { 0.0f };
	static const GLfloat gray[] = { 0.f, 0.1f, 0.1f, 0.0f };
	static const GLenum attachments[] = { GL_COLOR_ATTACHMENT0 };
	static const vmath::mat4 scale_bias_matrix = vmath::mat4(vmath::vec4(0.5f, 0.0f, 0.0f, 0.0f),
		vmath::vec4(0.0f, 0.5f, 0.0f, 0.0f),
		vmath::vec4(0.0f, 0.0f, 0.5f, 0.0f),
		vmath::vec4(0.5f, 0.5f, 0.5f, 1.0f));

	glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);

	glDrawBuffers(1, attachments);
	glViewport(0, 0, Window_wid, Window_heig);
	glClearBufferfv(GL_COLOR, 0, gray);
	glClearBufferfv(GL_DEPTH, 0, ones);
	glUseProgram(gShaderProgramObject);
	glUniformMatrix4fv(uniforms.view.proj_matrix, 1, GL_FALSE, camera_proj_matrix);

	glClearBufferfv(GL_DEPTH, 0, ones);

	int i;
	for (i = 0; i < OBJECT_COUNT; i++)
	{
		vmath::mat4& model_matrix = objects[i].model_matrix;
		glUniformMatrix4fv(uniforms.view.mv_matrix, 1, GL_FALSE, camera_view_matrix * objects[i].model_matrix);
		glUniform3fv(uniforms.view.diffuse_albedo, 1, objects[i].diffuse_albedo);
		objects[0].obj.render();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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