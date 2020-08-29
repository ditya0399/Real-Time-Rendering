#include<Windows.h>
#include<glew.h>               // This SHOULD be above GL.h header.
#include<gl/GL.h>
#include"Sphere.h"
#include<stdio.h>
#include<string.h>
#include"vmath.h"
#include"object.h"
#include"3DTexture.h"


#pragma comment(lib,"opengl32.lib")//for linking
#pragma comment(lib,"glew32.lib")//for linking
#pragma comment(lib,"Sphere.lib")
double currentTime = 0.0f;

enum
{
	MAX_SCENE_WIDTH = 2000,
	MAX_SCENE_HEIGHT = 2000,
	SPHERE_COUNT = 1,
};

GLfloat gWindowWidth;
GLfloat gWindowHeight;

GLfloat TreeAngle;
///////


//newly added variables


int levelmax = 4; // maximum level of recursion
int nleaf = 2; // number of leafs per branch
float openness = 3.0; // controls how open the leafs are
float factor = 1.1f; // controls how fast branch sizes decrease
float zoom = 15; // zoom of visualization
float offset = 0.05; // controls how crooked branches are
float windowx = 0.2, windowy = 0.2; // position of center of window




GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat one = 1.0f;
int i;
float last_time = 0.0;
float total_time = 0.0;



sb7::object object;





float       exposure = 1.0f;
int         mode = 0;
bool        paused = false;
float       bloom_factor = 1.0f;
bool        show_bloom = true;
bool        show_scene = true;
bool        show_prefilter = false;
float       bloom_thresh_min = 0.8f;
float       bloom_thresh_max = 1.2f;


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
} uniformsTesselleted;

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


GLuint b0Uniform;
GLuint b1Uniform;
GLuint lengthUniform;
GLuint mvpUniform;
GLuint mvpUniform_Tesselleted;

GLuint gNumberOfSegmentUniform; //MultipleLines -> Stips  | One Line divided into Multiple parts ->Segments
GLuint gNumberOfStripsUniform;
//////////////



unsigned int gNumberOfLineSegments;
int gNumVertices;
int gNumElements;

GLuint vao;






//Sphere

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
//
//int gNumVertices;
//int gNumElements;

int Win_Width = 800;
int Win_Height = 600;
using namespace vmath;

static GLfloat angle = 0.0f;
GLuint gShaderProgramObject;
GLuint gShaderProgramObject_Tesselleted;
GLuint FB_gShaderProgramObject;
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
GLuint gTextureStone;
//
GLuint vbo_texture_triangle;
GLuint vao_LightingSeed;
GLuint vao_LightingSeed_Tesselleted;
GLuint vbo_position_Lighting;
GLuint vbo_position_Lighting_Tesselleted;
GLuint vbo_Normals;
GLuint vbo_Normals_Tesselleted;
GLuint vbo_Element_Sphere;

GLuint vao_branches;
GLuint vao_Quad;
GLuint vbo_branches;
GLuint vbo_Quad;
GLuint vbo_QuadNormals;
GLuint vbo_QuadTexture;
GLuint vbo_branches_normals;

int index = 0;

GLfloat vertices[8];
GLfloat vertices1[8000];
mat4 perspectiveProjectionMatrix;


#define dimension 800 // window size
#define M_PI 3.14
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
GLuint projectionUniform_Tesselleted;
GLuint modelUniform;
GLuint modelUniform_Tesselleted;
GLuint ViewUniform;
GLuint ViewUniform_Tesselleted;
GLuint LDUniform;
GLuint LDUniform_Tesselleted;
GLuint LAUniform;
GLuint LAUniform_Tesselleted;
GLuint LSUniform;
GLuint LSUniform_Tesselleted;


GLuint KDUniform;
GLuint KDUniform_Tesselleted;
GLuint KAUniform;
GLuint KAUniform_Tesselleted;
GLuint KSUniform;
GLuint KSUniform_Tesselleted;
float MaterialShininessUniform;
float MaterialShininessUniform_Tesselleted;
GLuint LightPositionUniform;
GLuint LightPositionUniform_Tesselleted;
GLuint LKeyPressed = 0;
GLuint LKeyPressed_Tesselleted = 0;


GLfloat lightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightPosition[] = { 100.0f,100.0f,100.0f,1.0f };

GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialShininess = 50.0f;





GLuint TextureSampler_Light;



///////////// CUBE VARIABLES ////////////
GLuint FB_vao_triangle;
GLuint FB_vbo_position_triangle;
GLuint FB_mvpUniform1;
GLuint FB_modelViewUniform1;
GLuint fbo;
GLuint color_texture;
GLuint depth_texture;
GLuint vbo_texture_rectangle;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void ToggleFullScreen(void);
void update(void);
void uninitialize(void);
void initializeShader();
void draw_fractal(double, double, double, int);
void draw_fractal1(double, double, double, int);


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
		case 'e':
			factor *= 1 / 1.1;

			break;
		case 'q':
			factor *= 1.1;

			break;
		case VK_ESCAPE:
			ToggleFullScreen();
			DestroyWindow(hwnd);
			break;
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
		case 'R':
		case 'r':
			TreeAngle -= 0.4f;
			fprintf(gpLogFile, "The value of TreeAngle is %f\n", TreeAngle);
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
		case '1':
		case '2':
		case '3':

			break;
		case 'B':
			show_bloom = !show_bloom;
			break;
		case 'V':
			show_scene = !show_scene;
			break;
		case 'A':
			bloom_factor += 0.1f;
			break;
		case 'Z':
			bloom_factor -= 0.1f;
			break;
		case 'S':
			bloom_thresh_min += 0.1f;
			break;
		case 'X':
			bloom_thresh_min -= 0.1f;
			break;
		case 'D':
			bloom_thresh_max += 0.1f;
			break;
		case 'C':
			bloom_thresh_max -= 0.1f;
			break;

		case 'N':
			show_prefilter = !show_prefilter;
			break;
		case 'M':
			mode = (mode + 1) % 3;
			break;
		case 'P':
			paused = !paused;
			break;
		case 'W':
		case 'w':
			exposure *= 1.1f;

			break;
		case VK_SPACE:
			exposure /= 1.1f;
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
void draw_fractal(double x0, double y0, double angle0, int level)
{
	if (level > levelmax) return;
	float r = 0.15 / pow(factor, level - 1);
	float angle;
	float x, y, coss, sinn;
	int i;
	int n = 0;
	for (i = 0; i < nleaf; i++)
	{
		angle = offset + angle0 + i * 2 * M_PI / nleaf / openness - (nleaf - 1) * 2 * M_PI / nleaf / openness / 2;
		coss = cos(angle);
		sinn = sin(angle);
		x = x0 + r * coss;
		y = y0 + r * sinn;
		/*glBegin(GL_LINES);
		glVertex2f(x0, y0);
		glVertex2f(x, y);
		glEnd();*/
		vertices[n] = x0;
		n = n + 1;
		vertices[n] = y0;
		n = n + 1;
		vertices[n] = x;
		n = n + 1;
		vertices[n] = y;
		n = n + 1;

		if (index < 4000)
			draw_fractal1(x, y, atan2((y - y0), (x - x0)), level + 1);
	}
	fprintf(gpLogFile, "Value of index is %d\n", index);
}
void draw_fractal1(double x0, double y0, double angle0, int level)
{
	if (level > levelmax) return;
	float r = 0.15 / pow(factor, level - 1);
	float angle;
	float x, y, coss, sinn;
	int i;
	int n = 0;
	for (i = 0; i < nleaf; i++)
	{
		angle = offset + angle0 + i * 2 * M_PI / nleaf / openness - (nleaf - 1) * 2 * M_PI / nleaf / openness / 2;
		coss = cos(angle);
		sinn = sin(angle);
		x = x0 + r * coss;
		y = y0 + r * sinn;
		/*glBegin(GL_LINES);
		glVertex2f(x0, y0);
		glVertex2f(x, y);
		glEnd();*/
		vertices1[index] = x0;
		//fprintf(gpLogFile, "Value of %d nth element is %f", n, vertices1[n]);
		index = index + 1;
		vertices1[index] = y0;
		//fprintf(gpLogFile, "Value of %d nth element is %f", n, vertices1[n]);
		index = index + 1;
		vertices1[index] = x;
		//fprintf(gpLogFile, "Value of %d nth element is %f", n, vertices1[n]);
		index = index + 1;
		vertices1[index] = y;
		//fprintf(gpLogFile, "Value of %d nth element is %f", n, vertices1[n]);
		index = index + 1;


		draw_fractal1(x, y, atan2((y - y0), (x - x0)), level + 1);
	}
}
int initialize(void)
{

	int LoadGLTextures(GLuint *, TCHAR[]);
	GLuint gVertexShaderObject;
	GLuint gVertexShaderObject_FractalTree;

	GLuint gVertexShaderObject_filter;
	GLuint gVertexShaderObject_resolve;
	GLuint gFragmentShaderObject;
	GLuint gFragmentShaderObject_Tesselleted;
	GLuint gFragmentShaderObject_filter;
	GLuint gFragmentShaderObject_resolve;
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
	gVertexShaderObject_FractalTree = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar **p;
	const GLchar *vertexShaderSourceCodeFractalTree =

		"#version 430 core" \
		"\n"
		"layout(location = 0)in vec3 position;" \
		"layout(location = 1)in vec3 normal;" \
		"layout(location = 2)in vec2 texCoord;" \
		"out VS_OUT" \
		"{" \
		"vec3 N;" \
		"vec3 L;" \
		"vec3 V;" \
		"vec2 T;" \
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
		"vec4 P = mat_mv * vec4(position,1.0);" \

		// Calculate normal in view-space
		"vs_out.N = mat3(mat_mv) * normal;" \

		//// Calculate light vector
		"vs_out.L = light_pos - P.xyz;" \

		//// Calculate view vector
		"vs_out.V = -P.xyz;" \

		// Pass material index
		//"vs_out.material_index = gl_InstanceID;" \

		// Calculate the clip-space position of each vertex
		"gl_Position = mat_proj * P;" \
		"vs_out.T = texCoord;" \
		"}";


	glShaderSource(gVertexShaderObject_FractalTree, 1, (const GLchar **)&vertexShaderSourceCodeFractalTree, NULL);

	//Compile the vertex shader 
	glCompileShader(gVertexShaderObject_FractalTree);

	//////////////// Error Checking//////////////////
	//Code for catching the errors 
	GLint iShaderCompileStatus = 0;
	GLint iInfoLogLength = 0;
	GLchar *szInfoLog = NULL;


	glGetShaderiv(gVertexShaderObject_FractalTree, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_FractalTree, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject_FractalTree, iInfoLogLength, &written, szInfoLog);
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
		"layout(location = 2) out vec4 color3;" \
		"out vec4 FragColor;" \
		"in VS_OUT" \
		"{" \
		"vec3 N;" \
		"vec3 L;" \
		"vec3 V;" \
		"vec2 T;" \
		"flat int material_index;" \
		"} fs_in;" \

		// Material properties
		"uniform float bloom_thresh_min = 0.8;" \
		"uniform float bloom_thresh_max = 1.2;" \
		"uniform sampler2D u_texture_sampler;"
		"struct material_t" \
		"{" \
		"vec3    diffuse_color;" \
		"vec3    specular_color;" \
		"float   specular_power;" \
		"vec3    ambient_color;" \
		"};" \

		"layout(binding = 1, std140) uniform MATERIAL_BLOCK" \
		"{" \
		"material_t  material[1];" \
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
		"vec4 tex = texture(u_texture_sampler, fs_in.T);" \
		
		// Compute the diffuse and specular components for each fragment
		"vec3 diffuse = max(dot(N, L), 0.0) * m.diffuse_color *tex.rgb;" \
		"vec3 specular = pow(max(dot(R, V), 0.0), m.specular_power) * m.specular_color *tex.rgb;" \
		"vec3 ambient = m.ambient_color *tex.rgb;" \

		// Add ambient, diffuse and specular to find final color
		"vec3 color = ambient + diffuse + specular;" \

		// Write final color to the framebuffer
		//color0 = vec4(color, 1.0);" \

		// Calculate luminance
		"float Y = dot(color, vec3(0.299, 0.587, 0.144));" \

		// Threshold color based on its luminance and write it to
		// the second output
		"color =  color * 4.0 * smoothstep(bloom_thresh_min, bloom_thresh_max, Y);"   \
		"vec4 tex1 = tex;" \


		/*"if(tex1.g < 0.8)" \
		"discard;" \*/

		"FragColor = vec4(1.0);" \

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





	///// Tessellated Lighting Shaders

	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
//	const GLchar **p;
	const GLchar *vertexShaderSourceCode =

		"#version 430 core" \
		"\n"
		"layout(location = 0)in vec2 position;" \
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
		"vec4 P = mat_mv * vec4(position,0.0,1.0);" \

		// Calculate normal in view-space
		"vs_out.N = mat3(mat_mv) * normal;" \

		//// Calculate light vector
		"vs_out.L = light_pos - P.xyz;" \

		//// Calculate view vector
		"vs_out.V = -P.xyz;" \

		// Pass material index
		//"vs_out.material_index = gl_InstanceID;" \

		// Calculate the clip-space position of each vertex
		"gl_Position = vec4(position,0.0,1.0);" \

		"}";


	glShaderSource(gVertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);

	//Compile the vertex shader 
	glCompileShader(gVertexShaderObject);

	//////////////// Error Checking//////////////////
	//Code for catching the errors 
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;


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
		"vec3 N;" \
		"vec3 L;" \
		"vec3 V;" \
		"flat int material_index;" \
		"} tcs_in[];" \

		"out TCS_OUT" \
		"{" \
		"vec3 N;" \
		"vec3 L;" \
		"vec3 V;" \
		"flat int material_index;" \
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
		"tcs_out[gl_InvocationID].N = tcs_in[gl_InvocationID].N;" \
		"tcs_out[gl_InvocationID].L = tcs_in[gl_InvocationID].L;" \
		"tcs_out[gl_InvocationID].V = tcs_in[gl_InvocationID].V;" \
		"tcs_out[gl_InvocationID].material_index = tcs_in[gl_InvocationID].material_index;" \
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
		"uniform float length;" \

		"in TCS_OUT" \
		"{" \
		"vec3 N;" \
		"vec3 L;" \
		"vec3 V;" \
		"flat int material_index;" \
		"} tes_in[];" \

		"out TES_OUT" \
		"{" \
		"vec3 N;" \
		"vec3 L;" \
		"vec3 V;" \
		"flat int material_index;" \
		"}tes_out;" \

		"uniform float b2Constant;" \
		"uniform float b1Constant;" \
		"void main(void)" \
		"{" \


		"float u =  gl_TessCoord.x;" \
		"vec3 p0 = gl_in[0].gl_Position.xyz;" \
		"vec3 p1 = gl_in[1].gl_Position.xyz;" \
		"vec3 p2 = gl_in[2].gl_Position.xyz;" \
		"vec3 p3 = gl_in[3].gl_Position.xyz;" \
		"float u1 = (1.0 - u);" \
		"float u2 = u * u;" \
		"float b3 =  u2 * u;" \
		"float b2 = b2Constant * u2 * u1;" \
		"float b1 = b1Constant * u * u1 * u1;" \
		"float b0 =   u1* u1 * u1;" \
		"vec3 p =  length * p0 * b0 + p1 * b1 + p2 * b2 +p3 * b3 ;" \

		"gl_Position = u_mvp_matrix * vec4(p , 1.0);" \
		"tes_out.N = tes_in[0].N;" \
		"tes_out.L = tes_in[0].L;" \
		"tes_out.V = tes_in[0].V;" \
		"tes_out.material_index = tes_in[0].material_index;" \


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
	gFragmentShaderObject_Tesselleted = glCreateShader(GL_FRAGMENT_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar *fragmentShaderSourceCode_tesselleted =
		"#version 430 core" \
		"\n" \
		"layout(location = 0) out vec4 color0;" \
		"layout(location = 1) out vec4 color1;" \
		"out vec4 FragColor;" \
		"in TES_OUT" \
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
		"material_t  material[1];" \
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
		//color0 = vec4(color, 1.0);" \

		// Calculate luminance
		"float Y = dot(color, vec3(0.299, 0.587, 0.144));" \

		// Threshold color based on its luminance and write it to
		// the second output
		"color = color * 4.0 * smoothstep(bloom_thresh_min, bloom_thresh_max, Y);"   \
		"FragColor = vec4(color,1.0);" \
		"}";

	//FragColor = vec4(1,1,1,1) = White Color
	//this means here we are giving color to the Triangle.




	//Specify above source code to the vertex shader object
	glShaderSource(gFragmentShaderObject_Tesselleted, 1, (const GLchar **)&fragmentShaderSourceCode_tesselleted, NULL);

	//Compile the vertex shader 
	glCompileShader(gFragmentShaderObject_Tesselleted);
	//Code for catching the errors 
		   /*iShaderCompileStatus = 0;
		   iInfoLogLength = 0;*/
	szInfoLog = NULL;


	glGetShaderiv(gFragmentShaderObject_Tesselleted, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_Tesselleted, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written1;
				glGetShaderInfoLog(gFragmentShaderObject_Tesselleted, iInfoLogLength, &written1, szInfoLog);
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
	gFragmentShaderObject_resolve = glCreateShader(GL_FRAGMENT_SHADER); //This command will create the Shader Object
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
	gShaderProgramObject_Tesselleted = glCreateProgram();
	gShaderProgramObject_filter = glCreateProgram();
	gShaderProgramObject_resolve = glCreateProgram();
	//attach vertex shader to the gShaderProgramObject
	glAttachShader(gShaderProgramObject, gVertexShaderObject_FractalTree);
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);


	glAttachShader(gShaderProgramObject_Tesselleted, gVertexShaderObject);
	glAttachShader(gShaderProgramObject_Tesselleted, gTesselationControlShaderObject);
	glAttachShader(gShaderProgramObject_Tesselleted, gTesselationEvaluationShaderObject);
	glAttachShader(gShaderProgramObject_Tesselleted, gFragmentShaderObject_Tesselleted);


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
	glLinkProgram(gShaderProgramObject_Tesselleted);
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

	//
	mvpUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
	TextureSampler_Light = glGetUniformLocation(gShaderProgramObject, "u_texture_sampler");

	//Here we have done all the preparations of data transfer from CPU to GPU

	/*gNumberOfSegmentUniform = glGetUniformLocation(gShaderProgramObject, "numberOfSegments");
	gNumberOfStripsUniform = glGetUniformLocation(gShaderProgramObject, "numberOfStrips");
	lengthUniform = glGetUniformLocation(gShaderProgramObject, "length");*/

	//Tesselleted 
	uniformsTesselleted.scene.bloom_thresh_min = glGetUniformLocation(gShaderProgramObject_Tesselleted, "bloom_thresh_min");
	uniformsTesselleted.scene.bloom_thresh_max = glGetUniformLocation(gShaderProgramObject_Tesselleted, "bloom_thresh_max");



	projectionUniform_Tesselleted = glGetUniformLocation(gShaderProgramObject_Tesselleted, "mat_proj");
	ViewUniform_Tesselleted = glGetUniformLocation(gShaderProgramObject_Tesselleted, "mat_view");
	modelUniform_Tesselleted = glGetUniformLocation(gShaderProgramObject_Tesselleted, "mat_model");

	//
	mvpUniform_Tesselleted = glGetUniformLocation(gShaderProgramObject_Tesselleted, "u_mvp_matrix");
	b0Uniform = glGetUniformLocation(gShaderProgramObject_Tesselleted, "b2Constant");
	b1Uniform = glGetUniformLocation(gShaderProgramObject_Tesselleted, "b1Constant");
	//Here we have done all the preparations of data transfer from CPU to GPU

	gNumberOfSegmentUniform = glGetUniformLocation(gShaderProgramObject_Tesselleted, "numberOfSegments");
	gNumberOfStripsUniform = glGetUniformLocation(gShaderProgramObject_Tesselleted, "numberOfStrips");
	lengthUniform = glGetUniformLocation(gShaderProgramObject_Tesselleted, "length");

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




	gNumberOfLineSegments = 1;





	int i;
	static const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);







	//load_shaders();

	static const GLfloat exposureLUT[20] = { 11.0f, 6.0f, 3.2f, 2.8f, 2.2f, 1.90f, 1.80f, 1.80f, 1.70f, 1.70f,  1.60f, 1.60f, 1.50f, 1.50f, 1.40f, 1.40f, 1.30f, 1.20f, 1.10f, 1.00f };
	//static const GLfloat exposureLUT[20] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.00f };

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
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT16, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, tex_depth, 0);
	glDrawBuffers(2, buffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &filter_fbo[0]);
	glGenTextures(1, &tex_filter[0]);

	glBindFramebuffer(GL_FRAMEBUFFER, filter_fbo[0]);
	glBindTexture(GL_TEXTURE_2D, tex_filter[0]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex_filter[0], 0);
	glDrawBuffers(1, buffers);


	glGenFramebuffers(1, &filter_fbo[1]);
	glGenTextures(1, &tex_filter[1]);
	glBindFramebuffer(GL_FRAMEBUFFER, filter_fbo[1]);
	glBindTexture(GL_TEXTURE_2D, tex_filter[1]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex_filter[1], 0);
	glDrawBuffers(1, buffers);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenTextures(1, &tex_lut);
	glBindTexture(GL_TEXTURE_1D, tex_lut);
	glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, 20);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 20, GL_RED, GL_FLOAT, exposureLUT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);




	const GLfloat rectangleVertices[] =
	{
		// Perspective square (Top face)
	/*	1.0f, 1.0f, -1.0f,
-1.0f, 1.0f, -1.0f,
-1.0f, 1.0f, 1.0f,
1.0f, 1.0f, 1.0f,

1.0f, -1.0f, -1.0f,
-1.0f, -1.0f, -1.0f,
-1.0f, -1.0f, 1.0f,
1.0f, -1.0f, 1.0f,*/

/////////////////////
//1.0f, 1.0f, 1.0f,
//-1.0f, 1.0f, 1.0f,
//-1.0f, -1.0f, 1.0f,
//1.0f, -1.0f, 1.0f,
//////////////////////////


-1.0f, 1.0f, 0.0f,
-1.0f, -1.0f, 0.0f,








//
//1.0f, 1.0f, -1.0f,
//-1.0f, 1.0f, -1.0f,
//-1.0f, -1.0f, -1.0f,
//1.0f, -1.0f, -1.0f,

//1.0f, 1.0f, -1.0f,
//1.0f, 1.0f, 1.0f,
//1.0f, -1.0f, 1.0f,
//1.0f, -1.0f, -1.0f,


//-1.0f, 1.0f, -1.0f,
//-1.0f, 1.0f, 1.0f,
//-1.0f, -1.0f, 1.0f,
//-1.0f, -1.0f, -1.0f

	};
	const GLfloat cubeNormals[] =
	{
		/*0.0f, 1.0f, 0.0f,
0.0f, 1.0f, 0.0f,
0.0f, 1.0f, 0.0f,
0.0f, 1.0f, 0.0f,

0.0f, -1.0f, 0.0f,
0.0f, -1.0f, 0.0f,
0.0f, -1.0f, 0.0f,
0.0f, -1.0f, 0.0f,*/

0.0f, 0.0f, 1.0f,
0.0f, 0.0f, 1.0f,
//0.0f, 0.0f, 1.0f,
//0.0f, 0.0f, 1.0f,
//
//0.0f, 0.0f, -1.0f,
//0.0f, 0.0f, -1.0f,
//0.0f, 0.0f, -1.0f,
//0.0f, 0.0f, -1.0f,
//
//1.0f, 0.0f, 0.0f,
//1.0f, 0.0f, 0.0f,
//1.0f, 0.0f, 0.0f,
//1.0f, 0.0f, 0.0f,
//
//-1.0f, 0.0f, 0.0f,
//-1.0f, 0.0f, 0.0f,
//-1.0f, 0.0f, 0.0f,
//-1.0f, 0.0f, 0.0f

	};

	const GLfloat lineVertices[] = {
		-0.5f,1.0f,0.5f,-1.0f,1.0f,1.0f,2.0f,-1.0f,
	};
	const GLfloat lineNormals_Tesselleted[] = {
		0.0f, 0.0f, 1.0f,
0.0f, 0.0f, 1.0f,
	};
	const GLfloat lineNormals[] = {
	
	0.0f, 0.0f, 1.0f,
0.0f, 0.0f, 1.0f,

0.0f, 0.0f, 1.0f,
0.0f, 0.0f, 1.0f,

0.0f, 0.0f, 1.0f,
0.0f, 0.0f, 1.0f,
0.0f, 0.0f, 1.0f,
0.0f, 0.0f, 1.0f,
0.0f, 0.0f, 1.0f,
0.0f, 0.0f, 1.0f,
0.0f, 0.0f, 1.0f,
0.0f, 0.0f, 1.0f,
0.0f, 0.0f, 1.0f,
0.0f, 0.0f, 1.0f,


	};
	const GLfloat vertex_data[] =
	{
		// Position                 Tex Coord
			2.0f, 1.0f, -1.0f,	     0.0f, 0.0f,
			-2.0f, 1.0f, -1.0f,      1.0f, 0.0f,
			-2.0f, 1.0f, 1.0f,	     1.0f, 1.0f,
			2.0f, 1.0f, 1.0f,		 0.0f, 1.0f,

			2.0f, -1.0f, -1.0f,	     0.0f, 0.0f,
			-2.0f, -1.0f, -1.0f,     1.0f, 0.0f,
			-2.0f, -1.0f, 1.0f,		 1.0f, 1.0f,
			2.0f, -1.0f, 1.0f,	     0.0f, 1.0f,

			1.0f, 1.0f,1.0f,	     0.0f, 0.0f,
			-1.0f, 1.0f, 1.0f,		 1.0f, 0.0f,
			-1.0f, -1.0f, 1.0f,      1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,	     0.0f, 1.0f,

			1.0f, 1.0f, -1.0f,		 0.0f, 0.0f,
			-1.0f, 1.0f, -1.0f,	     1.0f, 0.0f,
			-1.0f, -1.0f, -1.0f,     1.0f, 1.0f,
			1.0f, -1.0f, -1.0f,	     0.0f, 1.0f,

			1.0f, 1.0f, -1.0f,	     0.0f, 0.0f,
			1.0f, 1.0f, 1.0f,	     1.0f, 0.0f,
			1.0f, -1.0f, 1.0f, 	     1.0f, 1.0f,
			1.0f, -1.0f, -1.0f,		 0.0f, 1.0f,

			-1.0f, 1.0f, 1.0f,	     0.0f, 0.0f,
			-1.0f, 1.0f, -1.0f,	     1.0f, 0.0f,
			-1.0f, -1.0f, -1.0f,	 1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,	     0.0f, 1.0f,


	};
	draw_fractal(0.5, 0.5, M_PI / 2, 1);

	glGenVertexArrays(1, &vao_LightingSeed);
	glBindVertexArray(vao_LightingSeed);



	glGenBuffers(1, &vbo_position_Lighting);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_Lighting);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(0);
	

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//NORMALS 
	glGenBuffers(1, &vbo_Normals);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineNormals), lineNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenVertexArrays(1, &vao_branches);
	glBindVertexArray(vao_branches);
	glGenBuffers(1, &vbo_branches);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_branches);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);



	glGenBuffers(1, &vbo_branches_normals);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_branches_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineNormals), lineNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);

	//Tesselleted calls

	glGenVertexArrays(1, &vao_LightingSeed_Tesselleted);
	glBindVertexArray(vao_LightingSeed_Tesselleted);



	glGenBuffers(1, &vbo_position_Lighting_Tesselleted);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_Lighting_Tesselleted);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	//NORMALS 
	glGenBuffers(1, &vbo_Normals_Tesselleted);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Normals_Tesselleted);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineNormals_Tesselleted), lineNormals_Tesselleted, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	////Elements
	/*glGenBuffers(1, &vbo_Element_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);*/
	//object.load("torus.sbm");



	/*glGenBuffers(1, &ubo_transform);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_transform);
	glBufferData(GL_UNIFORM_BUFFER, (2 + SPHERE_COUNT) * sizeof(vmath::mat4), NULL, GL_DYNAMIC_DRAW);*/

	struct material
	{
		vmath::vec3     diffuse_color;
		unsigned int : 1;           // pad
		vmath::vec3     specular_color;
		float           specular_power;
		vmath::vec3     ambient_color;
		unsigned int : 1;           // pad
	};

	glGenBuffers(1, &ubo_material);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_material);
	glBufferData(GL_UNIFORM_BUFFER, SPHERE_COUNT * sizeof(material), NULL, GL_STATIC_DRAW);

	material * m = (material *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, SPHERE_COUNT * sizeof(material), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	float ambient = 0.2f;
	for (i = 0; i < SPHERE_COUNT; i++)
	{
		float fi = 3.14159267f * (float)i / 8.0f;
		m[i].diffuse_color = vmath::vec3(sinf(fi) * 0.5f + 0.5f, sinf(fi + 1.345f) * 0.5f + 0.5f, sinf(fi + 2.567f) * 0.5f + 0.5f);
		m[i].specular_color = vmath::vec3(2.0f, 0.0f, 40.0f);
		m[i].specular_power = 1.0f;
		m[i].ambient_color = vmath::vec3(ambient * 0.025f);
		ambient *= 1.0f;
	}
	glUnmapBuffer(GL_UNIFORM_BUFFER);


	LoadGLTextures(&gTextureStone, MAKEINTRESOURCE(IDBITMAP_STONE));

	initializeShader();


	//Depth Lines
	//glClearDepth(1.0f);
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);
	//initializing Perspective ProjectionMatrix to identity.
	perspectiveProjectionMatrix = mat4::identity();

	resize(Win_Width, Win_Height);

	return(0);
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
void initializeShader()
{
	GLuint gVertexShaderObject;
	GLuint gFragmentShaderObject1;

	//////////////////////////////// V E R T E X - S H A D E R //////////////////////////


	//Define Vertex Shader Object
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar **p;
	const GLchar *vertexShaderSourceCode =

		"#version 430 core" \
		"\n"
		"in vec4 vPosition;" \
		"in vec2 vTexCoord;" \

		"out VS_OUT" \
		"{" \
		"vec4 color;" \
		"vec2 texcoord;" \
		"} vs_out;" \

		"uniform mat4 u_modelView_matrix;"  \
		"uniform mat4 u_p_matrix;" \

		"void main(void)" \
		"{" \
		"gl_Position = u_p_matrix * u_modelView_matrix * vPosition;" \

		"vs_out.texcoord = vTexCoord;" \
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
				fprintf(gpLogFile, " Vertex Shader :%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}
	///////////////////  FRAGMENT SHADER 1 ////////////
	gFragmentShaderObject1 = glCreateShader(GL_FRAGMENT_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar *fragmentShaderSourceCode1 =
		"#version 430 core" \
		"\n" \

		"uniform sampler2D tex;" \

		"out vec4 FragColor;" \

		"in VS_OUT" \
		"{" \
		"vec4 color;" \
		"vec2 texcoord;" \
		"} fs_in;" \
		"void main(void)" \
		"{" \
		"FragColor = texture(tex,fs_in.texcoord);" \
		"}";

	//FragColor = vec4(1,1,1,1) = White Color
	//this means here we are giving color to the Triangle.




	//Specify above source code to the vertex shader object
	glShaderSource(gFragmentShaderObject1, 1, (const GLchar **)&fragmentShaderSourceCode1, NULL);

	//Compile the vertex shader 
	glCompileShader(gFragmentShaderObject1);
	//Code for catching the errors 
		   /*iShaderCompileStatus = 0;
		   iInfoLogLength = 0;*/
	szInfoLog = NULL;


	glGetShaderiv(gFragmentShaderObject1, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject1, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written1;
				glGetShaderInfoLog(gFragmentShaderObject1, iInfoLogLength, &written1, szInfoLog);
				fprintf(gpLogFile, "FRAGMENT  1   SHADER :%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}

	FB_gShaderProgramObject = glCreateProgram();
	glAttachShader(FB_gShaderProgramObject, gVertexShaderObject);
	glAttachShader(FB_gShaderProgramObject, gFragmentShaderObject1);

	glBindAttribLocation(FB_gShaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(FB_gShaderProgramObject, AMC_ATTRIBUTE_COLOR, "vColor");
	glBindAttribLocation(FB_gShaderProgramObject, AMC_ATTRIBUTE_TEXCOORD0, "vTexCoord");

	//	glBindAttribLocation(gShaderProgramObject1, AMC_ATTRIBUTE_TEXCOORD0, "vTexCoord");
		//Here the above line means that we are linking the GPU's variable vPosition with the CPU's  enum member  i.e AMC_ATTRIBUTE_POSITION .
		//So whatever changes will be done in AMC_ATTRIBUTE_POSITION , those will also reflect in vPosition

		//RULE : ALWAYS BIND THE ATTRIBUTES BEFORE LINKING AND BIND THE UNIFORM AFTER LINKING.

		//Link the shader program 

	glLinkProgram(FB_gShaderProgramObject);

	//Code for catching the errors 
	GLint iProgramLinkStatus = 0;



	glGetProgramiv(FB_gShaderProgramObject, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(FB_gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written3;
				glGetProgramInfoLog(FB_gShaderProgramObject, iInfoLogLength, &written3, szInfoLog);
				fprintf(gpLogFile, "%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}


	const GLfloat vertex_data[] =
	{
		// Position                 Tex Coord
			2.0f, 1.0f, -1.0f,	     0.0f, 0.0f,
			-2.0f, 1.0f, -1.0f,      1.0f, 0.0f,
			-2.0f, 1.0f, 1.0f,	     1.0f, 1.0f,
			2.0f, 1.0f, 1.0f,		 0.0f, 1.0f,

			2.0f, -1.0f, -1.0f,	     0.0f, 0.0f,
			-2.0f, -1.0f, -1.0f,     1.0f, 0.0f,
			-2.0f, -1.0f, 1.0f,		 1.0f, 1.0f,
			2.0f, -1.0f, 1.0f,	     0.0f, 1.0f,

			1.0f, 1.0f,1.0f,	     0.0f, 0.0f,
			-1.0f, 1.0f, 1.0f,		 1.0f, 0.0f,
			-1.0f, -1.0f, 1.0f,      1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,	     0.0f, 1.0f,

			1.0f, 1.0f, -1.0f,		 0.0f, 0.0f,
			-1.0f, 1.0f, -1.0f,	     1.0f, 0.0f,
			-1.0f, -1.0f, -1.0f,     1.0f, 1.0f,
			1.0f, -1.0f, -1.0f,	     0.0f, 1.0f,

			1.0f, 1.0f, -1.0f,	     0.0f, 0.0f,
			1.0f, 1.0f, 1.0f,	     1.0f, 0.0f,
			1.0f, -1.0f, 1.0f, 	     1.0f, 1.0f,
			1.0f, -1.0f, -1.0f,		 0.0f, 1.0f,

			-1.0f, 1.0f, 1.0f,	     0.0f, 0.0f,
			-1.0f, 1.0f, -1.0f,	     1.0f, 0.0f,
			-1.0f, -1.0f, -1.0f,	 1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,	     0.0f, 1.0f,


	};
	FB_mvpUniform1 = glGetUniformLocation(FB_gShaderProgramObject, "u_p_matrix");
	FB_modelViewUniform1 = glGetUniformLocation(FB_gShaderProgramObject, "u_modelView_matrix");

	//Basically we are giving the vertices coordinates in the above array
	////////////////////FOR TRIANGLE ///////////////// 
	//Create vao - Vertex array objects
	glGenVertexArrays(1, &FB_vao_triangle);
	glBindVertexArray(FB_vao_triangle);
	glGenBuffers(1, &FB_vbo_position_triangle);
	glBindBuffer(GL_ARRAY_BUFFER, FB_vbo_position_triangle);
	//in the above statement we have accesed the GL-ARRAY_BUFFER using vbo. Without it wouldn't be possible to get GL_ARRAY_BUFFER
	//For the sake of understanding . GL_ARRAY_BUFFER is in the GPU side ad=nd we have bind our CPU side vbo with it like a Pipe to get the access.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
	//The above statement states that , we are passing our vertices array to the GPU and GL_STATIC_DRAW means draw it now only. Don't draw it in Runtime. 
	//The below statement states that after storing the data in the GPU'S buffer . We are passing it to the vPosition now . 

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);
	//GL_FALSE = We are not giving normalized coordinates as our coordinates are not converted in 0 - 1 range.
	//3 = This is the thing I was talking about in initialize. Here, we are telling GPU to break our array in 3 parts . 
	//0 and Null are for the Interleaved. 
	//GL_FLOAT- What is the type? .
	//AMC_ATTRIBUTE_POSITION. here we are passing data to vPosition. 

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);

	//// FRAMEBUFFER /////////

	glGenFramebuffers(1, &fbo);   //create a frameBuffer Object 
	glBindFramebuffer(GL_FRAMEBUFFER, fbo); //Bind it 
	glGenTextures(1, &color_texture);//Create a texture for our color buffer.
	glBindTexture(GL_TEXTURE_2D, color_texture); //Bind it 
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, MAX_SCENE_WIDTH, MAX_SCENE_WIDTH);
	// we are going to read from this but it won't have MipMaps. TurnOff MipMaps for this texture.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &depth_texture);//Create a texture that will be ours FBO's depth Buffer
	glBindTexture(GL_TEXTURE_2D, depth_texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, MAX_SCENE_WIDTH, MAX_SCENE_WIDTH);


	//Attach BOTH COLOR AND TEXTURE with the FBO
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_texture, 0); //here 0 specifies the mipmap level 
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture, 0);


	//Tell OpenGL what we want to draw into the framebuffer's memory first and Only Color Attachment
	static const GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, draw_buffers);

	GLenum fboStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (fboStatus == GL_FRAMEBUFFER_COMPLETE)
		fprintf(gpLogFile, "fbo status %d\n", fboStatus);
	glBindFramebuffer(GL_FRAMEBUFFER, 0); //UnBind it 
	glBindTexture(GL_TEXTURE_2D, 0); //UnBind it 

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


	fprintf(gpLogFile, "width :%f\n", gWindowWidth);
	fprintf(gpLogFile, "height :%f\n", gWindowHeight);

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



	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	GLfloat vertices[8];
	static const GLfloat green[] = { 0.0f,0.0f,0.0f,1.0f };
	static const GLfloat blue[] = { 0.4f, 0.5f, 0.6f, 1.0f };


	if (!paused)
		total_time += (currentTime - last_time);
	last_time = currentTime;
	float t = (float)total_time;

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	 //use shader program


	glViewport(0, 0, gWindowWidth, gWindowHeight);

	glBindFramebuffer(GL_FRAMEBUFFER, render_fbo);
	glViewport(0, 0, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);
	glClearBufferfv(GL_COLOR, 0, black);
	glClearBufferfv(GL_COLOR, 1, black);
	glClearBufferfv(GL_DEPTH, 0, &one);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);


	glLineWidth(125.0f);

	//static GLfloat gAngle;


	glUseProgram(gShaderProgramObject);

	vmath::mat4 mat_proj;
	vmath::mat4 mat_view;
	vmath::mat4 mat_model;
	vmath::mat4 modelViewProjectionMatrix;
	mat_view = mat4::identity();
	mat_proj = mat4::identity();
	mat_model = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();


	mat_model = translate(0.0f, -0.5f, -3.2f);
	mat_model *= rotate(180.0f, 0.0f, 0.0f, 1.0f);
	mat_model *= translate(0.0f, -1.0f, -0.0f);
	//mat_model *= scale(0.04f, 5.2f, 0.2f);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_material);
	//glLineWidth(15.0f);
	glUniform1f(uniforms.scene.bloom_thresh_min, bloom_thresh_min);
	glUniform1f(uniforms.scene.bloom_thresh_max, bloom_thresh_max);
	glActiveTexture(GL_TEXTURE0);					
	glBindTexture(GL_TEXTURE_2D, gTextureStone);	
	glUniform1i(TextureSampler_Light, 0);

	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;
	//exposure = 5.0f;
	glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, mat_view);
	glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glUniformMatrix4fv(modelUniform, 1, GL_FALSE, mat_model);
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);


	glBindVertexArray(vao_LightingSeed);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);


	//glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
	////

	///
	//mat_view = mat4::identity();
	//mat_proj = mat4::identity();
	//mat_model = mat4::identity();
	//modelViewProjectionMatrix = mat4::identity();


	//mat_model = translate(1.1f, -0.75f, -2.0f);
	//mat_model *= rotate(180.0f, 0.0f, 0.0f, 1.0f);
	//mat_model *= translate(0.0f, -1.0f, -0.0f);

	//glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_material);
	////glLineWidth(5.0f);
	//glUniform1f(uniforms.scene.bloom_thresh_min, bloom_thresh_min);
	//glUniform1f(uniforms.scene.bloom_thresh_max, bloom_thresh_max);

	//modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;
	////exposure = 5.0f;
	//glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, mat_view);
	//glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	//glUniformMatrix4fv(modelUniform, 1, GL_FALSE, mat_model);
	//glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

//	glBindVertexArray(vao_branches);
//
//	//glDrawArrays(GL_LINES, 0, index/8);
//	glBindVertexArray(0);
//	fprintf(gpLogFile, "Value of index is %d\n", index);
//
//	//
//	mat_view = mat4::identity();
//	mat_proj = mat4::identity();
//	mat_model = mat4::identity();
//	modelViewProjectionMatrix = mat4::identity();
//
//
//	mat_model = translate(0.89f, 0.1f, -2.0f);
//	mat_model *= rotate(-60.0f, 0.0f, 0.0f, 1.0f);
//
//	mat_model *= translate(0.0f, -1.0f, -0.0f);
//
//	glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_material);
//	//glLineWidth(5.0f);
//	glUniform1f(uniforms.scene.bloom_thresh_min, bloom_thresh_min);
//	glUniform1f(uniforms.scene.bloom_thresh_max, bloom_thresh_max);
//
//	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;
//	//exposure = 5.0f;
//	glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, mat_view);
//	glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
//	glUniformMatrix4fv(modelUniform, 1, GL_FALSE, mat_model);
//	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
//
//	glBindVertexArray(vao_branches);
//
//	//glDrawArrays(GL_LINES, 0, 25);
//	glBindVertexArray(0);
//	fprintf(gpLogFile, "Value of index is %d\n", index);
//
//	//
//	mat_view = mat4::identity();
//	mat_proj = mat4::identity();
//	mat_model = mat4::identity();
//	modelViewProjectionMatrix = mat4::identity();
//
//
//	mat_model = translate(1.46f, -1.2f, -2.0f);
//	mat_model *= rotate(180.0f, 0.0f, 0.0f, 1.0f);
//	mat_model *= translate(0.0f, -2.0f, -0.0f);
//	mat_model *= scale(2.8f, 1.0f, 1.0f);
//
//	glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_material);
//	//glLineWidth(5.0f);
//	glUniform1f(uniforms.scene.bloom_thresh_min, bloom_thresh_min);
//	glUniform1f(uniforms.scene.bloom_thresh_max, bloom_thresh_max);
//
//	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;
//	//exposure = 5.0f;
//	glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, mat_view);
//	glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
//	glUniformMatrix4fv(modelUniform, 1, GL_FALSE, mat_model);
//	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
//
//	glBindVertexArray(vao_branches);
//
//	glDrawArrays(GL_LINES, 0, index / 10);
//	glBindVertexArray(0);
//	fprintf(gpLogFile, "Value of index is %d\n", index);
//
//	mat_view = mat4::identity();
//	mat_proj = mat4::identity();
//	mat_model = mat4::identity();
//	modelViewProjectionMatrix = mat4::identity();
//
//
//	mat_model = translate(1.75f, -0.8f, -2.0f);
//	mat_model *= rotate(200.0f, 0.0f, 0.0f, 1.0f);
//	mat_model *= translate(0.0f, -2.0f, -0.0f);
//	mat_model *= scale(2.5f, 1.0f, 1.0f);
//
//	glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_material);
//	//glLineWidth(5.0f);
//	glUniform1f(uniforms.scene.bloom_thresh_min, bloom_thresh_min);
//	glUniform1f(uniforms.scene.bloom_thresh_max, bloom_thresh_max);
//
//	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;
//	//exposure = 5.0f;
//	glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, mat_view);
//	glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
//	glUniformMatrix4fv(modelUniform, 1, GL_FALSE, mat_model);
//	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
//
//	glBindVertexArray(vao_branches);
//
//	glDrawArrays(GL_LINES, 0, index / 4);
//	glBindVertexArray(0);
//
//	//
//	mat_view = mat4::identity();
//	mat_proj = mat4::identity();
//	mat_model = mat4::identity();
//	modelViewProjectionMatrix = mat4::identity();
//
//
//	mat_model = translate(3.05f, -0.06f, -2.0f);
//	mat_model *= rotate(220.0f, 0.0f, 0.0f, 1.0f);
//	mat_model *= translate(0.0f, -2.0f, -0.0f);
//	mat_model *= scale(2.0f, 1.0f, 1.0f);
//
//	glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_material);
//	//glLineWidth(5.0f);
//	glUniform1f(uniforms.scene.bloom_thresh_min, bloom_thresh_min);
//	glUniform1f(uniforms.scene.bloom_thresh_max, bloom_thresh_max);
//
//	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;
//	//exposure = 5.0f;
//	glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, mat_view);
//	glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
//	glUniformMatrix4fv(modelUniform, 1, GL_FALSE, mat_model);
//	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
//
//	glBindVertexArray(vao_branches);
//
//	glDrawArrays(GL_LINES, 0, index / 10);
//	glBindVertexArray(0);
//
//	//
//		//
//	mat_view = mat4::identity();
//	mat_proj = mat4::identity();
//	mat_model = mat4::identity();
//	modelViewProjectionMatrix = mat4::identity();
//
//
//	mat_model = translate(1.4f, 0.06f, -2.0f);
//	mat_model *= rotate(260.0f, 0.0f, 0.0f, 1.0f);
//	mat_model *= translate(0.0f, -2.0f, -0.0f);
//	//mat_model *= scale(2.0f, 1.0f, 1.0f);
//
//	glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_material);
//	//glLineWidth(5.0f);
//	glUniform1f(uniforms.scene.bloom_thresh_min, bloom_thresh_min);
//	glUniform1f(uniforms.scene.bloom_thresh_max, bloom_thresh_max);
//
//	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;
//	//exposure = 5.0f;
//	glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, mat_view);
//	glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
//	glUniformMatrix4fv(modelUniform, 1, GL_FALSE, mat_model);
//	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
//
//	glBindVertexArray(vao_branches);
//
//	glDrawArrays(GL_LINES, 0, index / 10);
//	glBindVertexArray(0);
//
//	//
//	mat_view = mat4::identity();
//	mat_proj = mat4::identity();
//	mat_model = mat4::identity();
//	modelViewProjectionMatrix = mat4::identity();
//
//
//	mat_model = translate(2.63f, 0.25f, -2.0f);
//	mat_model *= rotate(260.0f, 0.0f, 0.0f, 1.0f);
//	mat_model *= translate(0.0f, -2.0f, -0.0f);
//	//mat_model *= scale(2.0f, 1.0f, 1.0f);
//
//	glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_material);
//	//glLineWidth(5.0f);
//	glUniform1f(uniforms.scene.bloom_thresh_min, bloom_thresh_min);
//	glUniform1f(uniforms.scene.bloom_thresh_max, bloom_thresh_max);
//
//	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;
//	//exposure = 5.0f;
//	glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, mat_view);
//	glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
//	glUniformMatrix4fv(modelUniform, 1, GL_FALSE, mat_model);
//	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
//
//	glBindVertexArray(vao_branches);
//
//	glDrawArrays(GL_LINES, 0, index / 10);
//	glBindVertexArray(0);
//
//	//mat_view = mat4::identity();
//	//mat_proj = mat4::identity();
//	//mat_model = mat4::identity();
//	//modelViewProjectionMatrix = mat4::identity();
//
//
//	//mat_model = translate(1.1f, -2.0f, -2.0f);
//	//mat_model *= rotate(180.0f, 0.0f, 0.0f, 1.0f);
//	//mat_model *= translate(0.0f, -2.0f, -0.0f);
//	//mat_model *= scale(1.5f, 1.0f, 1.0f);
//
//	//glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_material);
//	////glLineWidth(5.0f);
//	//glUniform1f(uniforms.scene.bloom_thresh_min, bloom_thresh_min);
//	//glUniform1f(uniforms.scene.bloom_thresh_max, bloom_thresh_max);
//
//	//modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;
//	////exposure = 5.0f;
//	//glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, mat_view);
//	//glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
//	//glUniformMatrix4fv(modelUniform, 1, GL_FALSE, mat_model);
//	//glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
//
//	//glBindVertexArray(vao_branches);
//
//	//glDrawArrays(GL_LINES, 0, index/14);
//	//glBindVertexArray(0);
//	//glUseProgram(0);
//	//
//
//	///////////////////////////////////////////////////////////////////
//		/////////////// ******* Electrical Lines---- Tessellated *********** ////////////////////
//
//	glUseProgram(gShaderProgramObject_Tesselleted);
//
//
//	mat_view = mat4::identity();
//	mat_proj = mat4::identity();
//	mat_model = mat4::identity();
//	modelViewProjectionMatrix = mat4::identity();
//
//
//	mat_model = translate(-0.25f, 0.0f, -2.0f);
//	//mat_model *= rotate(180.0f, 0.0f, 0.0f, 1.0f);
////mat_model *= scale(0.04f, 5.2f, 0.2f);
//
//	glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_material);
//	//glLineWidth(15.0f);
//	glUniform1f(uniformsTesselleted.scene.bloom_thresh_min, bloom_thresh_min);
//	glUniform1f(uniformsTesselleted.scene.bloom_thresh_max, bloom_thresh_max);
//
//	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;
//	//exposure = 5.0f;
//	glUniformMatrix4fv(ViewUniform_Tesselleted, 1, GL_FALSE, mat_view);
//	glUniformMatrix4fv(projectionUniform_Tesselleted, 1, GL_FALSE, perspectiveProjectionMatrix);
//	glUniformMatrix4fv(modelUniform_Tesselleted, 1, GL_FALSE, mat_model);
//	glUniformMatrix4fv(mvpUniform_Tesselleted, 1, GL_FALSE, modelViewProjectionMatrix);
//	gNumberOfLineSegments = 15;
//	glUniform1i(gNumberOfSegmentUniform, gNumberOfLineSegments);
//	glUniform1f(b0Uniform, 6.0f);
//	glUniform1f(b1Uniform, 5.0f);
//	glUniform1f(lengthUniform, 2.5f);
//	glUniform1i(gNumberOfStripsUniform, 1);
//
//	glBindVertexArray(vao_LightingSeed_Tesselleted);
//
//	vertices[0] = -0.5f;
//	vertices[1] = 1.0f;
//	vertices[2] = 0.5f;
//	vertices[3] = -1.5f;
//	vertices[4] = 1.5f;
//	vertices[5] = 1.5f;
//	vertices[6] = 1.5f;
//	vertices[7] = -2.0f;
//
//	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_Lighting_Tesselleted);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
//	glPatchParameteri(GL_PATCH_VERTICES, 4);
//	glDrawArrays(GL_PATCHES, 0, 4);
//	glBindVertexArray(0);
//
//
//
//	//
////
//////
//////
//////	////////  LINE 2 ///////////
//	mat_view = mat4::identity();
//	mat_proj = mat4::identity();
//	mat_model = mat4::identity();
//	modelViewProjectionMatrix = mat4::identity();
//
//
//	mat_model = translate(0.0f, 0.0f, -2.0f);
//	//mat_model *= rotate(180.0f, 0.0f, 0.0f, 1.0f);
////mat_model *= scale(0.04f, 5.2f, 0.2f);
//
//	glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_material);
//	//glLineWidth(15.0f);
//	glUniform1f(uniformsTesselleted.scene.bloom_thresh_min, bloom_thresh_min);
//	glUniform1f(uniformsTesselleted.scene.bloom_thresh_max, bloom_thresh_max);
//
//	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;
//	//exposure = 5.0f;
//	glUniformMatrix4fv(ViewUniform_Tesselleted, 1, GL_FALSE, mat_view);
//	glUniformMatrix4fv(projectionUniform_Tesselleted, 1, GL_FALSE, perspectiveProjectionMatrix);
//	glUniformMatrix4fv(modelUniform_Tesselleted, 1, GL_FALSE, mat_model);
//	glUniformMatrix4fv(mvpUniform_Tesselleted, 1, GL_FALSE, modelViewProjectionMatrix);
//	gNumberOfLineSegments = 10;
//	glUniform1i(gNumberOfSegmentUniform, gNumberOfLineSegments);
//	glUniform1f(b0Uniform, 2.0f);
//	glUniform1f(b1Uniform, 5.0f);
//	glUniform1f(lengthUniform, 1.5f);
//	glUniform1i(gNumberOfStripsUniform, 1);
//
//	glBindVertexArray(vao_LightingSeed_Tesselleted);
//
//	vertices[0] = -0.5f;
//	vertices[1] = 1.0f;
//	vertices[2] = 0.5f;
//	vertices[3] = -1.5f;
//	vertices[4] = 2.8f;
//	vertices[5] = 0.5f;
//	vertices[6] = 15.2f;
//	vertices[7] = -2.0f;
//
//	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_Lighting_Tesselleted);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
//	glPatchParameteri(GL_PATCH_VERTICES, 4);
//	glDrawArrays(GL_PATCHES, 0, 4);
//	glBindVertexArray(0);
//	//////
//	//////
//	//////	////////////
//	mat_view = mat4::identity();
//	mat_proj = mat4::identity();
//	mat_model = mat4::identity();
//	modelViewProjectionMatrix = mat4::identity();
//
//
//	mat_model = translate(0.0f, 0.0f, -2.0f);
//	//mat_model *= rotate(180.0f, 0.0f, 0.0f, 1.0f);
////mat_model *= scale(0.04f, 5.2f, 0.2f);
//
//	glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_material);
//	//glLineWidth(15.0f);
//	glUniform1f(uniformsTesselleted.scene.bloom_thresh_min, bloom_thresh_min);
//	glUniform1f(uniformsTesselleted.scene.bloom_thresh_max, bloom_thresh_max);
//
//	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;
//	//exposure = 5.0f;
//	glUniformMatrix4fv(ViewUniform_Tesselleted, 1, GL_FALSE, mat_view);
//	glUniformMatrix4fv(projectionUniform_Tesselleted, 1, GL_FALSE, perspectiveProjectionMatrix);
//	glUniformMatrix4fv(modelUniform_Tesselleted, 1, GL_FALSE, mat_model);
//	glUniformMatrix4fv(mvpUniform_Tesselleted, 1, GL_FALSE, modelViewProjectionMatrix);
//	gNumberOfLineSegments = 6;
//	glUniform1i(gNumberOfSegmentUniform, gNumberOfLineSegments);
//	glUniform1f(b0Uniform, 2.0f);
//	glUniform1f(b1Uniform, 9.0f);
//	glUniform1f(lengthUniform, 1.5f);
//	glUniform1i(gNumberOfStripsUniform, 1);
//
//	glBindVertexArray(vao_LightingSeed_Tesselleted);
//
//	vertices[0] = -0.5f;
//	vertices[1] = 1.0f;
//	vertices[2] = 0.5f;
//	vertices[3] = -1.5f;
//	vertices[4] = 3.8f;
//	vertices[5] = 0.5f;
//	vertices[6] = 2.2f;
//	vertices[7] = -2.0f;
//
//	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_Lighting_Tesselleted);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
//	glPatchParameteri(GL_PATCH_VERTICES, 4);
//	glDrawArrays(GL_PATCHES, 0, 4);
//	glBindVertexArray(0);
//
//	//
//
//	mat_view = mat4::identity();
//	mat_proj = mat4::identity();
//	mat_model = mat4::identity();
//	modelViewProjectionMatrix = mat4::identity();
//
//
//	mat_model = translate(-0.0f, -0.0f, -2.0f);
//	//mat_model *= rotate(180.0f, 0.0f, 0.0f, 1.0f);
////mat_model *= scale(0.04f, 5.2f, 0.2f);
//
//	glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_material);
//	//glLineWidth(15.0f);
//	glUniform1f(uniformsTesselleted.scene.bloom_thresh_min, bloom_thresh_min);
//	glUniform1f(uniformsTesselleted.scene.bloom_thresh_max, bloom_thresh_max);
//
//	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;
//	//exposure = 5.0f;
//	glUniformMatrix4fv(ViewUniform_Tesselleted, 1, GL_FALSE, mat_view);
//	glUniformMatrix4fv(projectionUniform_Tesselleted, 1, GL_FALSE, perspectiveProjectionMatrix);
//	glUniformMatrix4fv(modelUniform_Tesselleted, 1, GL_FALSE, mat_model);
//	glUniformMatrix4fv(mvpUniform_Tesselleted, 1, GL_FALSE, modelViewProjectionMatrix);
//	gNumberOfLineSegments = 10;
//	glUniform1i(gNumberOfSegmentUniform, gNumberOfLineSegments);
//	glUniform1f(b0Uniform, 2.0f);
//	glUniform1f(b1Uniform, 5.0f);
//	glUniform1f(lengthUniform, 1.5f);
//	glUniform1i(gNumberOfStripsUniform, 1);
//
//	glBindVertexArray(vao_LightingSeed_Tesselleted);
//
//	vertices[0] = -0.5f;
//	vertices[1] = 1.0f;
//	vertices[2] = 0.5f;
//	vertices[3] = -1.5f;
//	vertices[4] = 1.5f;
//	vertices[5] = 1.5f;
//	vertices[6] = 1.5f;
//	vertices[7] = -2.0f;
//
//	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_Lighting_Tesselleted);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
//	glPatchParameteri(GL_PATCH_VERTICES, 4);
//	glDrawArrays(GL_PATCHES, 0, 4);
//	glBindVertexArray(0);
//
//	//
//	mat_view = mat4::identity();
//	mat_proj = mat4::identity();
//	mat_model = mat4::identity();
//	modelViewProjectionMatrix = mat4::identity();
//
//
//	mat_model = translate(-0.0f, -0.0f, -2.0f);
//	//mat_model *= rotate(180.0f, 0.0f, 0.0f, 1.0f);
////mat_model *= scale(0.04f, 5.2f, 0.2f);
//
//	glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_material);
//	//glLineWidth(15.0f);
//	glUniform1f(uniformsTesselleted.scene.bloom_thresh_min, bloom_thresh_min);
//	glUniform1f(uniformsTesselleted.scene.bloom_thresh_max, bloom_thresh_max);
//
//	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;
//	//exposure = 5.0f;
//	glUniformMatrix4fv(ViewUniform_Tesselleted, 1, GL_FALSE, mat_view);
//	glUniformMatrix4fv(projectionUniform_Tesselleted, 1, GL_FALSE, perspectiveProjectionMatrix);
//	glUniformMatrix4fv(modelUniform_Tesselleted, 1, GL_FALSE, mat_model);
//	glUniformMatrix4fv(mvpUniform_Tesselleted, 1, GL_FALSE, modelViewProjectionMatrix);
//	gNumberOfLineSegments = 10;
//	glUniform1i(gNumberOfSegmentUniform, gNumberOfLineSegments);
//	glUniform1f(b0Uniform, 2.0f);
//	glUniform1f(b1Uniform, 5.0f);
//	glUniform1f(lengthUniform, 0.4f);
//	glUniform1i(gNumberOfStripsUniform, 1);
//
//	glBindVertexArray(vao_LightingSeed_Tesselleted);
//
//	vertices[0] = -0.5f;
//	vertices[1] = 1.0f;
//	vertices[2] = 0.5f;
//	vertices[3] = -1.5f;
//	vertices[4] = 1.2f;
//	vertices[5] = 1.5f;
//	vertices[6] = 0.9f;
//	vertices[7] = -2.0f;
//
//	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_Lighting_Tesselleted);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
//	glPatchParameteri(GL_PATCH_VERTICES, 4);
//	glDrawArrays(GL_PATCHES, 0, 4);
//	glBindVertexArray(0);
//
//	//
//	mat_view = mat4::identity();
//	mat_proj = mat4::identity();
//	mat_model = mat4::identity();
//	modelViewProjectionMatrix = mat4::identity();
//
//
//	mat_model = translate(-0.0f, -0.0f, -2.0f);
//	//mat_model *= rotate(180.0f, 0.0f, 0.0f, 1.0f);
////mat_model *= scale(0.04f, 5.2f, 0.2f);
//
//	glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_material);
//	//glLineWidth(15.0f);
//	glUniform1f(uniformsTesselleted.scene.bloom_thresh_min, bloom_thresh_min);
//	glUniform1f(uniformsTesselleted.scene.bloom_thresh_max, bloom_thresh_max);
//
//	modelViewProjectionMatrix = perspectiveProjectionMatrix * mat_view * mat_model;
//	//exposure = 5.0f;
//	glUniformMatrix4fv(ViewUniform_Tesselleted, 1, GL_FALSE, mat_view);
//	glUniformMatrix4fv(projectionUniform_Tesselleted, 1, GL_FALSE, perspectiveProjectionMatrix);
//	glUniformMatrix4fv(modelUniform_Tesselleted, 1, GL_FALSE, mat_model);
//	glUniformMatrix4fv(mvpUniform_Tesselleted, 1, GL_FALSE, modelViewProjectionMatrix);
//	gNumberOfLineSegments = 10;
//	glUniform1i(gNumberOfSegmentUniform, gNumberOfLineSegments);
//	glUniform1f(b0Uniform, 6.0f);
//	glUniform1f(b1Uniform, 2.0f);
//	glUniform1f(lengthUniform, 0.4f);
//	glUniform1i(gNumberOfStripsUniform, 1);
//
//	glBindVertexArray(vao_LightingSeed_Tesselleted);
//
//	vertices[0] = -0.5f;
//	vertices[1] = 1.0f;
//	vertices[2] = 0.5f;
//	vertices[3] = -1.5f;
//	vertices[4] = 1.2f;
//	vertices[5] = -2.5f;
//	vertices[6] = 0.9f;
//	vertices[7] = -2.0f;
//
//	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_Lighting_Tesselleted);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
//	glPatchParameteri(GL_PATCH_VERTICES, 4);
//	glDrawArrays(GL_PATCHES, 0, 4);
	//glBindVertexArray(0);

	
		///////////////////// END OF ELECTRICAL LINES //////////////////////////////////


	glDisable(GL_DEPTH_TEST);


	////////////////// ******** P O S T --- P R O C E S S I N G -- E F F E C T S ******************///////////////

	glUseProgram(gShaderProgramObject_filter);

	glBindVertexArray(vao);
	glBindFramebuffer(GL_FRAMEBUFFER, filter_fbo[0]);
	glBindTexture(GL_TEXTURE_2D, tex_brightpass);
	glViewport(0, 0, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, filter_fbo[1]);
	glBindTexture(GL_TEXTURE_2D, tex_filter[0]);
	glViewport(0, 0, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glUseProgram(0);

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
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, MAX_SCENE_WIDTH, MAX_SCENE_WIDTH);
	glClearBufferfv(GL_COLOR, 0, green);
	glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_filter[1]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, show_prefilter ? tex_brightpass : tex_scene);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glUseProgram(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, gWindowWidth, gWindowHeight);
	glClearBufferfv(GL_COLOR, 0, green);
	glClearBufferfv(GL_DEPTH, 0, &one);
	glBindTexture(GL_TEXTURE_2D, color_texture);



	static GLfloat angle1;

	mat4 modelViewMatrix;
	//mat4 modelViewProjectionMatrix;
	mat4 ScaleMatrix;
	mat4 projectionMatrix;
	ScaleMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	// intialize above matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	glUseProgram(FB_gShaderProgramObject);


	ScaleMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	mat4 RotationMatrix = mat4::identity();
	// intialize above matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	//float f = (float)currentTime * 0.3f;

	// perform necessary transformations
	modelViewMatrix = translate(-0.0f, -0.0f, -125.0f);
	RotationMatrix = rotate(180.0f, 0.0f, 0.0f, 1.0f);
	modelViewMatrix = modelViewMatrix * RotationMatrix *scale(50.0f,100.0f,50.0f);
	/*translate(sinf(2.1f * f) * 0.5f,
		cosf(1.7f * f) * 0.5f,
		sinf(1.3f * f) * cosf(1.5f * f) * 2.0f) *
	vmath::rotate((float)currentTime * 45.0f, 0.0f, 1.0f, 0.0f) *
	vmath::rotate((float)currentTime * 81.0f, 1.0f, 0.0f, 0.0f);*/

	glUniformMatrix4fv(FB_mvpUniform1, 1, GL_FALSE, perspectiveProjectionMatrix);
	glUniformMatrix4fv(FB_modelViewUniform1, 1, GL_FALSE, modelViewMatrix);

	glBindVertexArray(FB_vao_triangle);

	//glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	//glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	//glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	//glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	//glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	//glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);
	angle1 += 0.02f;

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