#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif



#include<Windows.h>
#include<glew.h>               // This SHOULD be above GL.h header.
#include<gl/GL.h>
#include"3DTexture.h"

#include<stdio.h>
#include"vmath.h"
#pragma comment(lib,"opengl32.lib")//for linking
#pragma comment(lib,"glew32.lib")//for linking

double currentTime;

int Win_Width = 800;
int Win_Height = 600;
using namespace vmath;

GLfloat gWidth;
GLfloat gHeight;
 GLfloat angle1 = 0.0f;
GLuint gShaderProgramObject;
GLuint gShaderProgramObject1;

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0
};

GLuint gVertexShaderObject_Model, gFragmentShaderObject_Model, gProgramObject_Model;
GLuint gVao_Model, gVbo_Model,gVbo_Texture,gVbo_TextureIndices, gElementBuffer_Model;

GLuint gSamplerUniform_Model;
GLuint gMVPUniform;
//
GLuint vao_triangle;
GLuint vao_rectangle;
GLuint vbo_position_triangle;
GLuint vbo_position_rectangle;
GLuint vbo_color_rectangle;
GLuint vbo_color_triangle;
GLuint mvpUniform;
GLuint mvpUniform1;
GLuint modelViewUniform;
GLuint modelViewUniform1;
mat4 perspectiveProjectionMatrix;
GLuint index_buffer;
GLuint fbo;
GLuint color_texture;
GLuint depth_texture;
GLuint vbo_texture_rectangle;
GLuint gTextureKundali;
GLuint samplerUniform;
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

struct vec_int
{
	int *p;
	int size;
};

struct vec_float
{
	float *pf;
	int size;
};

#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE];

FILE *gp_mesh_file;
struct vec_float *gp_vertex, *gp_texture, *gp_normal;
struct vec_float *gp_vertex_sorted, *gp_texture_sorted, *gp_normal_sorted;
struct vec_int *gp_vertex_indices, *gp_texture_indices, *gp_normal_indices;
void initModel(void);
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
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;

		case 'F':
		case 'f':
			ToggleFullScreen();
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
	GLuint gFragmentShaderObject1;
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

//
//
//	//////////////////////////////// V E R T E X - S H A D E R //////////////////////////
//

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
	/////////////////    F R A G M E N T S H A D E R            //////////////////////////
	//Define Vertex Shader Object
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar *fragmentShaderSourceCode =
		"#version 430 core" \
		"\n" \

		"in VS_OUT" \
		"{" \
			"vec4 color;" \
			"vec2 texcoord;" \
		"} fs_in;" \

		"uniform sampler2D u_texture_sampler;"
		"out vec4 FragColor;" \
		
		"void main(void)" \
		"{" \
		"FragColor = texture(u_texture_sampler, fs_in.texcoord);" \
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
				fprintf(gpLogFile, "Fragment Shader %s\n", szInfoLog);
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

	// CREATE SHADER PROGRAM OBJECT
	gShaderProgramObject = glCreateProgram();
	
	//attach vertex shader to the gShaderProgramObject
	glAttachShader(gShaderProgramObject, gVertexShaderObject);
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);

	

	//Pre-Linking  binding to vertexAttributes
	glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_COLOR, "vColor");
	glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_TEXCOORD0, "vTexCoord");

	
//	glBindAttribLocation(gShaderProgramObject1, AMC_ATTRIBUTE_TEXCOORD0, "vTexCoord");
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
	mvpUniform = glGetUniformLocation(gShaderProgramObject, "u_p_matrix");
	modelViewUniform = glGetUniformLocation(gShaderProgramObject, "u_modelView_matrix");
	samplerUniform = glGetUniformLocation(gShaderProgramObject, "u_texture_sampler");
	//Here we have done all the preparations of data transfer from CPU to GPU
	







	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////// shader Program Object 1
	gShaderProgramObject1 = glCreateProgram();

	//attach vertex shader to the gShaderProgramObject
	
	glAttachShader(gShaderProgramObject1, gVertexShaderObject);
	glAttachShader(gShaderProgramObject1, gFragmentShaderObject1);

	//Pre-Linking  binding to vertexAttributes
	

	glBindAttribLocation(gShaderProgramObject1, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObject1, AMC_ATTRIBUTE_COLOR, "vColor");
	glBindAttribLocation(gShaderProgramObject1, AMC_ATTRIBUTE_TEXCOORD0, "vTexCoord");
	//	glBindAttribLocation(gShaderProgramObject1, AMC_ATTRIBUTE_TEXCOORD0, "vTexCoord");
		//Here the above line means that we are linking the GPU's variable vPosition with the CPU's  enum member  i.e AMC_ATTRIBUTE_POSITION .
		//So whatever changes will be done in AMC_ATTRIBUTE_POSITION , those will also reflect in vPosition

		//RULE : ALWAYS BIND THE ATTRIBUTES BEFORE LINKING AND BIND THE UNIFORM AFTER LINKING.

		//Link the shader program 
	
	glLinkProgram(gShaderProgramObject1);

	//Code for catching the errors 
	iProgramLinkStatus = 0;



	glGetProgramiv(gShaderProgramObject1, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject1, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written3;
				glGetProgramInfoLog(gShaderProgramObject1, iInfoLogLength, &written3, szInfoLog);
				fprintf(gpLogFile, "%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}

	
	
	mvpUniform1 = glGetUniformLocation(gShaderProgramObject1, "u_p_matrix");
	modelViewUniform1 = glGetUniformLocation(gShaderProgramObject1, "u_modelView_matrix");
	const GLushort vertex_Indices[] =
	{
			 0, 1, 2,
			2, 1, 3,
			2, 3, 4,
			4, 3, 5,
			4, 5, 6,
			6, 5, 7,
			6, 7, 0,
			0, 7, 1,
			6, 0, 2,
			2, 4, 6,
			7, 5, 3,
			7, 3, 1
	};
	const GLfloat vertex_data[] =
	{
		// Position                 Tex Coord
			1.0f, 1.0f, -1.0f,	     0.0f, 0.0f,
			-1.0f, 1.0f, -1.0f,      1.0f, 0.0f,
			-1.0f, 1.0f, 1.0f,	     1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,		 0.0f, 1.0f,

			1.0f, -1.0f, -1.0f,	     0.0f, 0.0f,
			-1.0f, -1.0f, -1.0f,     1.0f, 0.0f,
			-1.0f, -1.0f, 1.0f,		 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,	     0.0f, 1.0f,

			1.0f, 1.0f, 1.0f,	     0.0f, 0.0f,
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
	
	//Basically we are giving the vertices coordinates in the above array
	////////////////////FOR TRIANGLE ///////////////// 
	//Create vao - Vertex array objects
	glGenVertexArrays(1, &vao_triangle);
	glBindVertexArray(vao_triangle);
	glGenBuffers(1, &vbo_position_triangle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_triangle);
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
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),(GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);
	
	/*glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(vertex_Indices),
		vertex_Indices,
		GL_STATIC_DRAW);
*/
	//glEnable(GL_CULL_FACE);

	//Depth Lines
	glClearDepth(1.0f);
	glClearColor(0.0f, 0.0f,0.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);



	//// FRAMEBUFFER /////////



	glGenFramebuffers(1, &fbo);   //create a frameBuffer Object 
	glBindFramebuffer(GL_FRAMEBUFFER, fbo); //Bind it 
	glGenTextures(1, &color_texture);//Create a texture for our color buffer.
	glBindTexture(GL_TEXTURE_2D, color_texture); //Bind it 
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1024, 1024);
	// we are going to read from this but it won't have MipMaps. TurnOff MipMaps for this texture.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &depth_texture);//Create a texture that will be ours FBO's depth Buffer
	glBindTexture(GL_TEXTURE_2D, depth_texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, 1024, 1024);


	//Attach BOTH COLOR AND TEXTURE with the FBO
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_texture, 0); //here 0 specifies the mipmap level 
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture, 0);


	//Tell OpenGL what we want to draw into the framebuffer's memory first and Only Color Attachment
	static const GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, draw_buffers);

	GLenum fboStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (fboStatus == GL_FRAMEBUFFER_COMPLETE)
		fprintf(gpLogFile, "fbo status %d\n", fboStatus);
	glBindFramebuffer(GL_FRAMEBUFFER, 0); //Bind it 
	glBindTexture(GL_TEXTURE_2D, 0); //Bind it 
	//initializing Perspective ProjectionMatrix to identity.






	initModel();



	perspectiveProjectionMatrix = mat4::identity();
	LoadGLTextures(&gTextureKundali, MAKEINTRESOURCE(IDBITMAP_VIJAY_KUNDALI));

	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	resize(Win_Width, Win_Height);

	return(0);
}
void initModel()
{
	void load_mesh(void);
	// Common compilation and linking variables 
	GLint iShaderCompilationStatus = -1;
	GLint iProgramLinkStatus = -1;
	GLint iInfoLogLength = 0;
	GLint iWritten = 0;
	GLchar *szInfoLog = NULL;

	/* Vertex Shader Block */
	static const GLchar *szvecVertexShaderSource1[] =
	{
		"#version 460 core                          \n"
		"                                           \n"
		"in vec4 vPosition;                         \n"
		"in vec2 vTexCoord;" \
		"out vec2 out_texCoord;" \
		"uniform mat4 u_mvp_matrix;                 \n"
		"void main(void)                            \n"
		"{                                          \n"
		"out_texCoord = vTexCoord;" \
		"   gl_Position = u_mvp_matrix * vPosition; \n"
		"}                                          \n"
	};

	gVertexShaderObject_Model = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(gVertexShaderObject_Model, 1, szvecVertexShaderSource1, NULL);
	glCompileShader(gVertexShaderObject_Model);

	glGetShaderiv(gVertexShaderObject_Model, GL_COMPILE_STATUS, &iShaderCompilationStatus);
	if (iShaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_Model, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (!szInfoLog)
			{
				fprintf(gpLogFile, "malloc:Out of memory\n");
				uninitialize();
				exit(EXIT_FAILURE);
			}
			memset(szInfoLog, 0, iInfoLogLength);
			glGetShaderInfoLog(gVertexShaderObject_Model, iInfoLogLength, &iWritten, szInfoLog);
			fprintf(gpLogFile, "CPA:VERTEX SHADER 1 INFO LOG:%s", szInfoLog);
			free(szInfoLog);
			uninitialize();
			exit(EXIT_FAILURE);
		}
	}

	/* Fragment Shader Block */
	static const GLchar *szvecFragmentShaderSource1[] =
	{
		"#version 460 core                          \n"
		  
		"uniform sampler2D tex;" \
		"in vec2 out_texCoord;" \
		"out vec4 FragColor;                        \n"
		"void main(void)                            \n"
		"{                                          \n"
		"   FragColor =texture(tex,out_texCoord);  \n"
		"}                                          \n"
	};

	gFragmentShaderObject_Model = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(gFragmentShaderObject_Model, 1, szvecFragmentShaderSource1, NULL);
	glCompileShader(gFragmentShaderObject_Model);

	iShaderCompilationStatus = 0;

	glGetShaderiv(gFragmentShaderObject_Model, GL_COMPILE_STATUS, &iShaderCompilationStatus);
	if (iShaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_Model, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (!szInfoLog)
			{
				fprintf(gpLogFile, "malloc:out of virtual memory\n");
				uninitialize();
				exit(EXIT_FAILURE);
			}

			memset(szInfoLog, 0, iInfoLogLength);
			glGetShaderInfoLog(gFragmentShaderObject_Model, iInfoLogLength, &iWritten, szInfoLog);
			fprintf(gpLogFile, "CPA:FRAGMENT SHADER 1 INFO LOG:%s", szInfoLog);
			free(szInfoLog);
			uninitialize();
			exit(EXIT_FAILURE);
		}
	}

	gProgramObject_Model = glCreateProgram();
	glAttachShader(gProgramObject_Model, gVertexShaderObject_Model);
	glAttachShader(gProgramObject_Model, gFragmentShaderObject_Model);
	glBindAttribLocation(gProgramObject_Model, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gProgramObject_Model, AMC_ATTRIBUTE_TEXCOORD0, "vTexCoord");
	glLinkProgram(gProgramObject_Model);

	glGetProgramiv(gProgramObject_Model, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gProgramObject_Model, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (!szInfoLog)
			{
				fprintf(gpLogFile, "malloc:out of virtual memory\n");
				uninitialize();
				exit(EXIT_FAILURE);
			}

			glGetProgramInfoLog(gProgramObject_Model, iInfoLogLength, &iWritten, szInfoLog);
			fprintf(gpLogFile, "CPA:PROGRAM INFO LOG:%s", szInfoLog);
			uninitialize();
			exit(EXIT_FAILURE);
		}
	}

	gMVPUniform = glGetUniformLocation(gProgramObject_Model, "u_mvp_matrix");
	gSamplerUniform_Model = glGetUniformLocation(gProgramObject_Model, "tex");

	load_mesh();

	glGenVertexArrays(1, &gVao_Model);
	glBindVertexArray(gVao_Model); /*BIND VAO*/

	glGenBuffers(1, &gVbo_Model); /* BIND VBO */
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Model);
	glBufferData(GL_ARRAY_BUFFER, (gp_vertex_sorted->size) * sizeof(GLfloat), gp_vertex_sorted->pf, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0); /*UNBIND VBO */


	glGenBuffers(1, &gVbo_Texture); /* BIND VBO */
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Texture);
	glBufferData(GL_ARRAY_BUFFER, (gp_vertex_sorted->size) * sizeof(GLfloat), gp_texture->pf, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);
	glBindBuffer(GL_ARRAY_BUFFER, 0); /*UNBIND VBO */



	//glGenBuffers(1, &gVbo_TextureIndices);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_TextureIndices);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, gp_vertex_indices->size * sizeof(int), gp_texture_indices->p,
	//	GL_STATIC_DRAW);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	glGenBuffers(1, &gElementBuffer_Model);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gElementBuffer_Model);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, gp_vertex_indices->size * sizeof(int), gp_vertex_indices->p,
		GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0); /*UNBIND VAO */

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
void resize(int width, int height)
{


	if (height == 0)
	{
		height = 1;
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);


	gWidth = width;
	gHeight = height;
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
	perspectiveProjectionMatrix = perspective(50.0f,
		(GLfloat)width / (GLfloat)height,
		0.1f,
		1000.0f);

}



void display(void)
{


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	static const GLfloat blue[] = { 0.0f, 0.0f, 0.1f, 1.0f };
	static const GLfloat one = 1.0f;
	static const GLfloat green[] = { 0.0f,0.53f,0.0f,1.0f };
	// use shader program
	

	//declaration of matrices
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 ScaleMatrix;
	mat4 projectionMatrix;
	ScaleMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	// intialize above matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	float f = (float)currentTime * 0.3f;
	
	// perform necessary transformations
	modelViewMatrix = translate(-0.0f, 0.0f, -45.0f) *
		translate(sinf(2.1f * f) * 0.5f,
			cosf(1.7f * f) * 0.5f,
			sinf(1.3f * f) * cosf(1.5f * f) * 2.0f) *
		vmath::rotate((float)currentTime * 45.0f, 0.0f, 1.0f, 0.0f) *
		vmath::rotate((float)currentTime * 81.0f, 1.0f, 0.0f, 0.0f);
		f += 0.5f;
	// do necessary matrix multiplication
	// this was internally done by gluPerspective() in FFP.	
	
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	
	glViewport(0, 0, 1024, 1024);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0f, 0.83f, 0.0f, 1.0f);
	//glClearBufferfv(GL_COLOR, 0,green);
	glClear(GL_DEPTH_BUFFER_BIT);
	//glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);
	glUseProgram(gProgramObject_Model);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureKundali);
	glUniform1f(gSamplerUniform_Model, 0);

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glBindVertexArray(gVao_Model);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gElementBuffer_Model);
	glDrawElements(GL_TRIANGLES, (gp_vertex_indices->size), GL_UNSIGNED_INT, NULL);
	//glBindVertexArray(0);
	//glUseProgram(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, gWidth, gHeight);
	glClearBufferfv(GL_COLOR, 0, blue);
	//glClearBufferfv(GL_DEPTH, 0, &one);
	glBindTexture(GL_TEXTURE_2D, color_texture);

	glUseProgram(gShaderProgramObject1);


	ScaleMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	mat4 RotationMatrix = mat4::identity();
	// intialize above matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	//float f = (float)currentTime * 0.3f;

	// perform necessary transformations
	modelViewMatrix = translate(-0.0f, 0.0f, -6.0f);
	RotationMatrix = rotate (angle1, angle1, 0.0f);
	modelViewMatrix = modelViewMatrix * RotationMatrix;
		/*translate(sinf(2.1f * f) * 0.5f,
			cosf(1.7f * f) * 0.5f,
			sinf(1.3f * f) * cosf(1.5f * f) * 2.0f) *
		vmath::rotate((float)currentTime * 45.0f, 0.0f, 1.0f, 0.0f) *
		vmath::rotate((float)currentTime * 81.0f, 1.0f, 0.0f, 0.0f);*/

	glUniformMatrix4fv(mvpUniform1, 1, GL_FALSE, perspectiveProjectionMatrix);
	glUniformMatrix4fv(modelViewUniform1, 1, GL_FALSE, modelViewMatrix);
	
	glBindVertexArray(vao_triangle);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	//glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);


	glUseProgram(0);

/*

glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

glUseProgram(gProgramObject_Model);

vmath::mat4 modelViewMatrix = vmath::mat4::identity();
vmath::mat4 modelViewProjectionMatrix = vmath::mat4::identity();

modelViewMatrix = vmath::translate(0.0f, 0.0f, -45.0f);
modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, gTextureKundali);
glUniform1f(gSamplerUniform_Model, 0);


glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);
glBindVertexArray(gVao_Model);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gElementBuffer_Model);
glDrawElements(GL_TRIANGLES, (gp_vertex_indices->size), GL_UNSIGNED_INT, NULL);
glBindVertexArray(0);
glUseProgram(0);*/


	SwapBuffers(ghdc);
	currentTime = currentTime + 0.005;
	angle1 = angle1 + 0.35f;
}

void uninitialize(void)
{

	int destroy_vec_int(struct vec_int *p_vec_int);
	int destroy_vec_float(struct vec_float *p_vec_float);

	if (vbo_position_triangle)
	{
		glDeleteBuffers(1, &vbo_position_triangle);
		vbo_position_triangle = 0;
	}

	if (vao_triangle)
	{
		glDeleteBuffers(1, &vao_triangle);
		vao_triangle = 0;
	}
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
	destroy_vec_float(gp_vertex);
	gp_vertex = NULL;

	destroy_vec_float(gp_texture);
	gp_texture = NULL;

	destroy_vec_float(gp_normal);
	gp_normal = NULL;

	destroy_vec_float(gp_vertex_sorted);
	gp_vertex_sorted = NULL;
	destroy_vec_float(gp_texture_sorted);
	gp_texture_sorted = NULL;
	destroy_vec_float(gp_normal_sorted);
	gp_normal_sorted = NULL;

	destroy_vec_int(gp_vertex_indices);
	gp_vertex_indices = NULL;
	destroy_vec_int(gp_texture_indices);
	gp_texture_indices = NULL;
	destroy_vec_int(gp_normal_indices);
	gp_normal_indices = NULL;

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

void load_mesh(void)
{
	struct vec_int *create_vec_int();
	struct vec_float *create_vec_float();
	int push_back_vec_int(struct vec_int *p_vec_int, int data);
	int push_back_vec_float(struct vec_float *p_vec_int, float data);
	void show_vec_float(struct vec_float *p_vec_float);
	void show_vec_int(struct vec_int *p_vec_int);
	int destroy_vec_float(struct vec_float *p_vec_float);


	const char *space = " ", *slash = "/", *first_token = NULL, *token;
	char *f_entries[3] = { NULL, NULL, NULL };
	int nr_pos_cords = 0, nr_tex_cords = 0, nr_normal_cords = 0, nr_faces = 0;
	int i, vi;

	gp_mesh_file = fopen("teapot.OBJ", "r");
	if (gp_mesh_file == NULL)
	{
		fprintf(stderr, "error in opening file\n");
		exit(EXIT_FAILURE);
	}

	gp_vertex = create_vec_float();
	gp_texture = create_vec_float();
	gp_normal = create_vec_float();

	gp_vertex_indices = create_vec_int();
	gp_texture_indices = create_vec_int();
	gp_normal_indices = create_vec_int();

	while (fgets(buffer, BUFFER_SIZE, gp_mesh_file) != NULL)
	{
		first_token = strtok(buffer, space);

		if (strcmp(first_token, "v") == 0)
		{
			nr_pos_cords++;
			while ((token = strtok(NULL, space)) != NULL)
				push_back_vec_float(gp_vertex, atof(token));

		}
		else if (strcmp(first_token, "vt") == 0)
		{
			nr_tex_cords++;
			while ((token = strtok(NULL, space)) != NULL)
				push_back_vec_float(gp_texture, atof(token));
		}
		else if (strcmp(first_token, "vn") == 0)
		{
			nr_normal_cords++;
			while ((token = strtok(NULL, space)) != NULL)
				push_back_vec_float(gp_normal, atof(token));
		}
		else if (strcmp(first_token, "f") == 0)
		{
			nr_faces++;
			for (i = 0; i < 3; i++)
				f_entries[i] = strtok(NULL, space);

			for (i = 0; i < 3; i++)
			{
				token = strtok(f_entries[i], slash);
				push_back_vec_int(gp_vertex_indices, atoi(token) - 1);
				token = strtok(NULL, slash);
				push_back_vec_int(gp_texture_indices, atoi(token) - 1);
				token = strtok(NULL, slash);
				push_back_vec_int(gp_normal_indices, atoi(token) - 1);
			}
		}
	}

	gp_vertex_sorted = create_vec_float();
	for (int i = 0; i < gp_vertex_indices->size; i++)
		push_back_vec_float(gp_vertex_sorted, gp_vertex->pf[i]);

	gp_texture_sorted = create_vec_float();
	for (int i = 0; i < gp_texture_indices->size; i++)
		push_back_vec_float(gp_texture_sorted, gp_texture->pf[i]);

	gp_normal_sorted = create_vec_float();
	for (int i = 0; i < gp_normal_indices->size; i++)
		push_back_vec_float(gp_normal_sorted, gp_normal->pf[i]);


	fclose(gp_mesh_file);
	gp_mesh_file = NULL;


	/*fprintf(gpFile, "vertices:%d texture:%d normal:%d faces:%d\n", nr_pos_cords, nr_tex_cords, nr_normal_cords, nr_faces);
	fprintf(gpFile, "vert size:%d texture size:%d normal size:%d\n", gp_vertex->size, gp_texture->size, gp_normal->size);
	fprintf(gpFile, "vert indices :%d texture indices :%d normal indices:%d\n", gp_vertex_indices->size, gp_texture_indices->size,
		gp_normal_indices->size);

	fprintf(gpFile, "Vertex Array\n");
	show_vec_float(gp_vertex);

	fprintf(gpFile, "Normal Array\n");
	show_vec_float(gp_normal);

	fprintf(gpFile, "Vertex Array Indices\n");
	show_vec_int(gp_vertex_indices);

	fprintf(gpFile, "Normal Array Indices\n");
	show_vec_int(gp_normal_indices);
	*/

}

struct vec_int *create_vec_int()
{
	struct vec_int *p = (struct vec_int*)malloc(sizeof(struct vec_int));
	memset(p, 0, sizeof(struct vec_int));
	return p;
}

struct vec_float *create_vec_float()
{
	struct vec_float *p = (struct vec_float*)malloc(sizeof(struct vec_float));
	memset(p, 0, sizeof(struct vec_float));
	return p;
}

int push_back_vec_int(struct vec_int *p_vec_int, int data)
{
	p_vec_int->p = (int*)realloc(p_vec_int->p, (p_vec_int->size + 1) * sizeof(int));
	p_vec_int->size = p_vec_int->size + 1;
	p_vec_int->p[p_vec_int->size - 1] = data;
	return (0);
}

int push_back_vec_float(struct vec_float *p_vec_float, float data)
{
	p_vec_float->pf = (float*)realloc(p_vec_float->pf, (p_vec_float->size + 1) * sizeof(float));
	p_vec_float->size = p_vec_float->size + 1;
	p_vec_float->pf[p_vec_float->size - 1] = data;
	return (0);
}

int destroy_vec_int(struct vec_int *p_vec_int)
{
	free(p_vec_int->p);
	free(p_vec_int);
	return (0);
}

int destroy_vec_float(struct vec_float *p_vec_float)
{
	free(p_vec_float->pf);
	free(p_vec_float);
	return (0);
}

void show_vec_float(struct vec_float *p_vec_float)
{
	int i;
	for (i = 0; i < p_vec_float->size; i++)
		fprintf(gpLogFile, "%f\n", p_vec_float->pf[i]);
}

void show_vec_int(struct vec_int *p_vec_int)
{
	int i;
	for (i = 0; i < p_vec_int->size; i++)
		fprintf(gpLogFile, "%d\n", p_vec_int->p[i]);
}