#include<Windows.h>
#include<glew.h>               // This SHOULD be above GL.h header.
#include<gl/GL.h>
#include"Sphere.h"
#include<stdio.h>
#include<string.h>
#include"vmath.h"
#include"object.h"


#pragma comment(lib,"opengl32.lib")//for linking
#pragma comment(lib,"glew32.lib")//for linking
#pragma comment(lib,"Sphere.lib")
double currentTime = 0.0f;

enum
{
	MAX_SCENE_WIDTH = 2048,
	MAX_SCENE_HEIGHT = 2048,
	SPHERE_COUNT = 1,
};

GLfloat gWindowWidth;
GLfloat gWindowHeight;

///////



 GLfloat black[] = { 0.0f, 0.0f, 1.0f, 1.0f };
 GLfloat one = 1.0f;
int i;
float last_time = 0.0;
 float total_time = 0.0;



sb7::object object;





float       exposure;
int         mode;
bool        paused;
float       bloom_factor;
bool        show_bloom;
bool        show_scene;
bool        show_prefilter;
float       bloom_thresh_min;
float       bloom_thresh_max;


struct
{
	struct
	{
		int bloom_thresh_min;
		int bloom_thresh_max;
	} scene;
	struct
	{
		int exposure;
		int bloom_factor;
		int scene_factor;
	} resolve;
} uniforms;

GLuint      ubo_transform;
GLuint      ubo_material;


GLuint      tex_src;
GLuint      tex_lut;

GLuint      render_fbo;
GLuint      filter_fbo[2];

GLuint      tex_scene;
GLuint      tex_brightpass;
GLuint      tex_depth;
GLuint      tex_filter[2];



//////////////






GLuint vao;






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
GLuint gShaderProgramObject_filter;
GLuint gShaderProgramObject_resolve;
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
	GLuint gVertexShaderObject_filter;
	GLuint gVertexShaderObject_resolve;
	GLuint gFragmentShaderObject;
	GLuint gFragmentShaderObject_filter;
	GLuint gFragmentShaderObject_resolve;


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


	int i;
	static const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//////////////////////////////// V E R T E X - S H A D E R //////////////////////////


	//Define Vertex Shader Object
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar **p;
	const GLchar *vertexShaderSourceCode =

		"#version 430 core" \
		"\n"
		"layout(location = 0)in vec4 position;" \
		"layout(location = 1)in vec3 normal;" \
		"out VS_OUT" \
	"{" \
		"vec3 N;" \
		"vec3 L;" \
		"vec3 V;" \
		"flat int material_index;" \
	"} vs_out;" \

		"uniform vec3 light_pos = vec3(100.0, 100.0, 100.0);" \


		"uniform mat4 mat_proj;" \
		"uniform mat4  mat_view;" \
		"uniform mat4 mat_model;" \

		"void main(void)" \
		"{" \
	"	mat4 mat_mv = mat_view * mat_model;" \

	// Calculate view-space coordinate
	"vec4 P = mat_mv * position;" \

	// Calculate normal in view-space
	"vs_out.N = mat3(mat_mv) * normal;" \

	// Calculate light vector
	"vs_out.L = light_pos - P.xyz;" \

	// Calculate view vector
	"vs_out.V = -P.xyz;" \

	// Pass material index
	//"vs_out.material_index = gl_InstanceID;" \

	// Calculate the clip-space position of each vertex
	"gl_Position = mat_proj * P;" \

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
		"layout(location = 0) out vec4 color0;" \
	"layout(location = 1) out vec4 color1;" \

	"in VS_OUT" \
	"{" \
		"vec3 N;" \
		"vec3 L;" \
		"vec3 V;" \
		"flat int material_index;" \
	"} fs_in;" \

	// Material properties
	"uniform float bloom_thresh_min = 0.8;" \
	"uniform float bloom_thresh_max = 1.2;" \

	"struct material_t" \
	"{" \
		"vec3    diffuse_color;" \
		"vec3    specular_color;" \
		"float   specular_power;" \
		"vec3    ambient_color;" \
	"};" \

	"layout(binding = 1, std140) uniform MATERIAL_BLOCK" \
	"{" \
		"material_t  material[32];" \
	"} materials;" \

		"void main(void)" \
		"{" \
		// Normalize the incoming N, L and V vectors
	"	vec3 N = normalize(fs_in.N);" \
	"vec3 L = normalize(fs_in.L);" \
	"vec3 V = normalize(fs_in.V);" \

	// Calculate R locally
	"vec3 R = reflect(-L, N);" \

	"material_t m = materials.material[fs_in.material_index];" \

	// Compute the diffuse and specular components for each fragment
	"vec3 diffuse = max(dot(N, L), 0.0) * m.diffuse_color;" \
	"vec3 specular = pow(max(dot(R, V), 0.0), m.specular_power) * m.specular_color;" \
	"vec3 ambient = m.ambient_color;" \

	// Add ambient, diffuse and specular to find final color
	"vec3 color = ambient + diffuse + specular;" \

	// Write final color to the framebuffer
	"color0 = vec4(color, 1.0);" \

	// Calculate luminance
	"float Y = dot(color, vec3(0.299, 0.587, 0.144));" \

	// Threshold color based on its luminance and write it to
	// the second output
	"color = color * 4.0 * smoothstep(bloom_thresh_min, bloom_thresh_max, Y);" \
	"color1 = vec4(color, 1.0);" \
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










	//////////////////////////// FILTER ////////////////////////////////////////////
	//////////////////////////////// V E R T E X - S H A D E R //////////////////////////


	//Define Vertex Shader Object
	gVertexShaderObject_filter = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
//	const GLchar **p;
	const GLchar *vertexShaderSourceCode_filter =

		"#version 430 core" \
		"\n"
		
		"void main(void)" \
		"{" \
		"const vec4 vertices[] = vec4[](vec4(-1.0, -1.0, 0.5, 1.0)," \
			"vec4(1.0, -1.0, 0.5, 1.0)," \
			"vec4(-1.0, 1.0, 0.5, 1.0)," \
			"vec4(1.0, 1.0, 0.5, 1.0));" \

	"gl_Position = vertices[gl_VertexID];" \

		"}";

	// GPU will run the above code. And GPU WILL RUN FOR PER VERTEX. If there are 1000 vertex. Then GPU will run this shader for
	//1000 times. We are Multiplying each vertex with the Model View Matrix.
	//And how does the GPU gets to know about at what offset the array has to be taken . Go to glVertexAttribPointer() in Display.
	// in = Input. 

	//p = &vertexShaderSourceCode;
		//Specify above source code to the vertex shader object
	glShaderSource(gVertexShaderObject_filter, 1, (const GLchar **)&vertexShaderSourceCode_filter, NULL);

	//Compile the vertex shader 
	glCompileShader(gVertexShaderObject_filter);

	//////////////// Error Checking//////////////////
	//Code for catching the errors 
 iShaderCompileStatus = 0;
 iInfoLogLength = 0;
szInfoLog = NULL;


	glGetShaderiv(gVertexShaderObject_filter, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_filter, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject_filter, iInfoLogLength, &written, szInfoLog);
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
	gFragmentShaderObject_filter = glCreateShader(GL_FRAGMENT_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar *fragmentShaderSourceCode_filter =
		"#version 430 core" \
		"\n" \
		"layout(binding = 0) uniform sampler2D hdr_image;" \

	"out vec4 color;" \

	"const float weights[] = float[](0.0024499299678342," \
		"0.0043538453346397," \
		"0.0073599963704157," \
		"0.0118349786570722," \
		"0.0181026699707781," \
		"0.0263392293891488," \
		"0.0364543006660986," \
		"0.0479932050577658," \
		"0.0601029809166942," \
		"0.0715974486241365," \
		"0.0811305381519717," \
		"0.0874493212267511," \
		"0.0896631113333857," \
		"0.0874493212267511," \
		"0.0811305381519717," \
		"0.0715974486241365," \
		"0.0601029809166942," \
		"0.0479932050577658," \
		"0.0364543006660986," \
		"0.0263392293891488," \
		"0.0181026699707781," \
		"0.0118349786570722," \
		"0.0073599963704157," \
		"0.0043538453346397," \
		"0.0024499299678342);" \

		"void main(void)" \
		"{" \
	"	vec4 c = vec4(0.0);" \
	"ivec2 P = ivec2(gl_FragCoord.yx) - ivec2(0, weights.length() >> 1);" \
	"int i;" \

	"for (i = 0; i < weights.length(); i++)" \
	"{" \
		"c += texelFetch(hdr_image, P + ivec2(0, i), 0) * weights[i];" \
	"}" \

	"color = c;" \
		"}";

	//FragColor = vec4(1,1,1,1) = White Color
	//this means here we are giving color to the Triangle.




	//Specify above source code to the vertex shader object
	glShaderSource(gFragmentShaderObject_filter, 1, (const GLchar **)&fragmentShaderSourceCode_filter, NULL);

	//Compile the vertex shader 
	glCompileShader(gFragmentShaderObject_filter);
	//Code for catching the errors 
		   /*iShaderCompileStatus = 0;
		   iInfoLogLength = 0;*/
	szInfoLog = NULL;


	glGetShaderiv(gFragmentShaderObject_filter, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_filter, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written1;
				glGetShaderInfoLog(gFragmentShaderObject_filter, iInfoLogLength, &written1, szInfoLog);
				fprintf(gpLogFile, "%sFragment Shader : \n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}











	////////////////////////////// RESOLVE //////////////////////////////////////////////
	//////////////////////////////// V E R T E X - S H A D E R //////////////////////////


	//Define Vertex Shader Object
	gVertexShaderObject_resolve = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
//	const GLchar **p;
	const GLchar *vertexShaderSourceCode_resolve =

		"#version 430 core" \
		"\n"

		"void main(void)" \
		"{" \
		"const vec4 vertices[] = vec4[](vec4(-1.0, -1.0, 0.5, 1.0)," \
		"vec4(1.0, -1.0, 0.5, 1.0)," \
		"vec4(-1.0, 1.0, 0.5, 1.0)," \
		"vec4(1.0, 1.0, 0.5, 1.0));" \

		"gl_Position = vertices[gl_VertexID];" \

		"}";

	// GPU will run the above code. And GPU WILL RUN FOR PER VERTEX. If there are 1000 vertex. Then GPU will run this shader for
	//1000 times. We are Multiplying each vertex with the Model View Matrix.
	//And how does the GPU gets to know about at what offset the array has to be taken . Go to glVertexAttribPointer() in Display.
	// in = Input. 

	//p = &vertexShaderSourceCode;
		//Specify above source code to the vertex shader object
	glShaderSource(gVertexShaderObject_resolve, 1, (const GLchar **)&vertexShaderSourceCode_resolve, NULL);

	//Compile the vertex shader 
	glCompileShader(gVertexShaderObject_resolve);

	//////////////// Error Checking//////////////////
	//Code for catching the errors 
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;


	glGetShaderiv(gVertexShaderObject_resolve, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_resolve, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject_resolve, iInfoLogLength, &written, szInfoLog);
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
	gFragmentShaderObject_resolve= glCreateShader(GL_FRAGMENT_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar *fragmentShaderSourceCode_resolve =
		"#version 430 core" \
		"\n" \
		"layout(binding = 0) uniform sampler2D hdr_image;" \
	"layout(binding = 1) uniform sampler2D bloom_image;" \

	"uniform float exposure = 0.9;" \
	"uniform float bloom_factor = 1.0;" \
	"uniform float scene_factor = 1.0;" \

	"out vec4 color;" \

	"void main(void)" \
	"{" \
		"vec4 c = vec4(0.0);" \

		"c += texelFetch(hdr_image, ivec2(gl_FragCoord.xy), 0) * scene_factor;" \
		"c += texelFetch(bloom_image, ivec2(gl_FragCoord.xy), 0) * bloom_factor;" \

		"c.rgb = vec3(1.0) - exp(-c.rgb * exposure);" \
	"	color = c;" \
	

		"}";

	//FragColor = vec4(1,1,1,1) = White Color
	//this means here we are giving color to the Triangle.




	//Specify above source code to the vertex shader object
	glShaderSource(gFragmentShaderObject_resolve, 1, (const GLchar **)&fragmentShaderSourceCode_resolve, NULL);

	//Compile the vertex shader 
	glCompileShader(gFragmentShaderObject_resolve);
	//Code for catching the errors 
		   /*iShaderCompileStatus = 0;
		   iInfoLogLength = 0;*/
	szInfoLog = NULL;


	glGetShaderiv(gFragmentShaderObject_resolve, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_resolve, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written1;
				glGetShaderInfoLog(gFragmentShaderObject_resolve, iInfoLogLength, &written1, szInfoLog);
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
	gShaderProgramObject_filter = glCreateProgram();
	gShaderProgramObject_resolve = glCreateProgram();
	//attach vertex shader to the gShaderProgramObject
	glAttachShader(gShaderProgramObject, gVertexShaderObject);
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);


	glAttachShader(gShaderProgramObject_filter, gVertexShaderObject_filter);
	glAttachShader(gShaderProgramObject_filter, gFragmentShaderObject_filter);


	glAttachShader(gShaderProgramObject_resolve, gVertexShaderObject_resolve);
	glAttachShader(gShaderProgramObject_resolve, gFragmentShaderObject_resolve);
	//Pre-Linking  binding to vertexAttributes
	//glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPosition");
	//glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_NORMAL, "vNormal");
	//Here the above line means that we are linking the GPU's variable vPosition with the CPU's  enum member  i.e AMC_ATTRIBUTE_POSITION .
	//So whatever changes will be done in AMC_ATTRIBUTE_POSITION , those will also reflect in vPosition

	//RULE : ALWAYS BIND THE ATTRIBUTES BEFORE LINKING AND BIND THE UNIFORM AFTER LINKING.

	//Link the shader program 
	glLinkProgram(gShaderProgramObject);
	glLinkProgram(gShaderProgramObject_filter);
	glLinkProgram(gShaderProgramObject_resolve);

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



	uniforms.scene.bloom_thresh_min = glGetUniformLocation(gShaderProgramObject, "bloom_thresh_min");
	uniforms.scene.bloom_thresh_max = glGetUniformLocation(gShaderProgramObject, "bloom_thresh_max");



	uniforms.resolve.exposure = glGetUniformLocation(gShaderProgramObject_resolve, "exposure");
	uniforms.resolve.bloom_factor = glGetUniformLocation(gShaderProgramObject_resolve, "bloom_factor");
	uniforms.resolve.scene_factor = glGetUniformLocation(gShaderProgramObject_resolve, "scene_factor");

	projectionUniform = glGetUniformLocation(gShaderProgramObject, "mat_proj");
	ViewUniform = glGetUniformLocation(gShaderProgramObject, "mat_view");
	modelUniform = glGetUniformLocation(gShaderProgramObject, "mat_model");
 

		


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
	//Here we have done all the preparations of data transfer from CPU to GPU

	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();


	//Basically we are giving the vertices coordinates in the above array
	////////////////////FOR TRIANGLE ///////////////// 
	//Create vao - Vertex array objects

	//////////////////////// FOR RECTANGLE //////////////// 

	//glGenVertexArrays(1, &vao_Sphere);
	//glBindVertexArray(vao_Sphere);
	















	
	//load_shaders();

	static const GLfloat exposureLUT[20] = { 11.0f, 6.0f, 3.2f, 2.8f, 2.2f, 1.90f, 1.80f, 1.80f, 1.70f, 1.70f,  1.60f, 1.60f, 1.50f, 1.50f, 1.40f, 1.40f, 1.30f, 1.20f, 1.10f, 1.00f };

	glGenFramebuffers(1, &render_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, render_fbo);

	glGenTextures(1, &tex_scene);
	glBindTexture(GL_TEXTURE_2D, tex_scene);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex_scene, 0);
	glGenTextures(1, &tex_brightpass);
	glBindTexture(GL_TEXTURE_2D, tex_brightpass);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, tex_brightpass, 0);
	glGenTextures(1, &tex_depth);
	glBindTexture(GL_TEXTURE_2D, tex_depth);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, tex_depth, 0);
	glDrawBuffers(2, buffers);

	glGenFramebuffers(2, &filter_fbo[0]);
	glGenTextures(2, &tex_filter[0]);
	for (i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, filter_fbo[i]);
		glBindTexture(GL_TEXTURE_2D, tex_filter[i]);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, i ? MAX_SCENE_WIDTH : MAX_SCENE_HEIGHT, i ? MAX_SCENE_HEIGHT : MAX_SCENE_WIDTH);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex_filter[i], 0);
		glDrawBuffers(1, buffers);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenTextures(1, &tex_lut);
	glBindTexture(GL_TEXTURE_1D, tex_lut);
	glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, 20);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 20, GL_RED, GL_FLOAT, exposureLUT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);




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
	object.load("torus.sbm");



	glGenBuffers(1, &ubo_transform);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_transform);
	glBufferData(GL_UNIFORM_BUFFER, (2 + SPHERE_COUNT) * sizeof(vmath::mat4), NULL, GL_DYNAMIC_DRAW);

	struct material
	{
		vmath::vec3     diffuse_color;
		unsigned int : 32;           // pad
		vmath::vec3     specular_color;
		float           specular_power;
		vmath::vec3     ambient_color;
		unsigned int : 32;           // pad
	};

	glGenBuffers(1, &ubo_material);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_material);
	glBufferData(GL_UNIFORM_BUFFER, SPHERE_COUNT * sizeof(material), NULL, GL_STATIC_DRAW);

	material * m = (material *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, SPHERE_COUNT * sizeof(material), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	float ambient = 0.002f;
	for (i = 0; i < SPHERE_COUNT; i++)
	{
		float fi = 3.14159267f * (float)i / 8.0f;
		m[i].diffuse_color = vmath::vec3(sinf(fi) * 0.5f + 0.5f, sinf(fi + 1.345f) * 0.5f + 0.5f, sinf(fi + 2.567f) * 0.5f + 0.5f);
		m[i].specular_color = vmath::vec3(2.8f, 2.8f, 2.9f);
		m[i].specular_power = 30.0f;
		m[i].ambient_color = vmath::vec3(ambient * 0.025f);
		ambient *= 1.5f;
	}
	glUnmapBuffer(GL_UNIFORM_BUFFER);







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
	gWindowWidth = width;
	gWindowHeight = height;
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

	


	if (!paused)
		total_time += (currentTime - last_time);
	last_time = currentTime;
	float t = (float)total_time;

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use shader program







	glViewport(0, 0, gWindowWidth, gWindowHeight);

	glBindFramebuffer(GL_FRAMEBUFFER, render_fbo);
	glClearBufferfv(GL_COLOR, 0, black);
	glClearBufferfv(GL_COLOR, 1, black);
	glClearBufferfv(GL_DEPTH, 0, &one);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);




	glUseProgram(gShaderProgramObject);
	
		vmath::mat4 mat_proj;
		vmath::mat4 mat_view;
		vmath::mat4 mat_model;
	
	mat_proj = perspective(50.0f, (float)gWindowWidth / (float)gWindowHeight, 1.0f, 1000.0f);
	mat_view = translate(0.0f, 0.0f, -5.0f);

	for (i = 0; i < SPHERE_COUNT; i++)
	{
		float fi = 3.141592f * (float)i / 16.0f;
		// float r = cosf(fi * 0.25f) * 0.4f + 1.0f;
		float r = (i & 2) ? 0.6f : 1.5f;
		//mat_model[i] = vmath::translate(cosf(t + fi) * 5.0f * r, sinf(t + fi * 4.0f) * 4.0f, sinf(t + fi) * 5.0f * r) *
			//vmath::rotate(sinf(t + fi * 2.13f) * 75.0f, cosf(t + fi * 1.37f) * 92.0f, 0.0f);
		//glUniformMatrix4fv(modelUniform, 32, GL_FALSE, mat_model[i]);
	}



	//glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_material);

	glUniform1f(uniforms.scene.bloom_thresh_min, bloom_thresh_min);
	glUniform1f(uniforms.scene.bloom_thresh_max, bloom_thresh_max);
	glUniformMatrix4fv(modelUniform, 1, GL_FALSE, mat_model);
	glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, mat_view);
	glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, mat_proj);
	
	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
*/
	object.render(SPHERE_COUNT);


	glDisable(GL_DEPTH_TEST);
	glUseProgram(gShaderProgramObject_filter);


	//glBindVertexArray(vao_Sphere);
	glBindVertexArray(vao);
	glBindFramebuffer(GL_FRAMEBUFFER, filter_fbo[0]);
	glBindTexture(GL_TEXTURE_2D, tex_brightpass);
	glViewport(0, 0, gWindowWidth, gWindowHeight);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindFramebuffer(GL_FRAMEBUFFER, filter_fbo[1]);
	glBindTexture(GL_TEXTURE_2D, tex_filter[0]);
	glViewport(0, 0, gWindowWidth, gWindowHeight);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glUseProgram(gShaderProgramObject_resolve);

	glUniform1f(uniforms.resolve.exposure, exposure);
	if (show_prefilter)
	{
		glUniform1f(uniforms.resolve.bloom_factor, 0.0f);
		glUniform1f(uniforms.resolve.scene_factor, 1.0f);
	}
	else
	{
		glUniform1f(uniforms.resolve.bloom_factor, show_bloom ? bloom_factor : 0.0f);
		glUniform1f(uniforms.resolve.scene_factor, show_scene ? 1.0f : 0.0f);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_filter[1]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, show_prefilter ? tex_brightpass : tex_scene);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);



	//glUseProgram(0);









	////declaration of matrices
	//mat4 modelMatrix;
	//mat4 viewMatrix;
	//mat4 modelViewProjectionMatrix;
	//mat4 RotationMatrix;
	//mat4 TranslateMatrix;
	//// intialize above matrices to identity

	//////////  RECTANGLE //////////////
	//modelMatrix = mat4::identity();
	//viewMatrix = mat4::identity();

	//RotationMatrix = mat4::identity();
	//TranslateMatrix = mat4::identity();
	//TranslateMatrix = translate(0.0f, 0.0f, -2.0f);
	////	RotationMatrix = rotate(angleRectangle, 1.0f, 0.0f, 0.0f);
	//RotationMatrix = rotate(angleRectangle, angleRectangle, angleRectangle);
	//// perform necessary transformations
	//modelMatrix = TranslateMatrix;

	//glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
	//glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
	//glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);





	
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