#include<Windows.h>
#include<glew.h>               // This SHOULD be above GL.h header.
#include<gl/GL.h>

#include<stdio.h>
#include"vmath.h"
#include"Sound.h"
#pragma comment(lib,"opengl32.lib")//for linking
#pragma comment(lib,"glew32.lib")//for linking
#pragma comment(lib,"Winmm.lib")

int Win_Width = 800;
int Win_Height = 600;
using namespace vmath;

static GLfloat angle = 0.0f;
GLuint gShaderProgramObject;


enum
{
	PARTICLE_GROUP_SIZE = 128,
	PARTICLE_GROUP_COUNT = 4000,
	PARTICLE_COUNT = (PARTICLE_GROUP_SIZE * PARTICLE_GROUP_COUNT),
	MAX_ATTRACTORS = 32
};
// Posisition and velocity buffers
static union
{
	struct
	{
		GLuint position_buffer;
		GLuint velocity_buffer;
	};
	GLuint buffers[2];
};

// TBOs
static union
{
	struct
	{
		GLuint position_tbo;
		GLuint velocity_tbo;
	};
	GLuint tbos[2];
};

// Attractor UBO
GLuint  attractor_buffer;
float attractor_masses[MAX_ATTRACTORS];

float aspect_ratio;

#define STRINGIZE(a) #a

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0
};
GLuint  render_prog;
GLuint  render_vao;
GLuint  render_vbo;

//
GLuint vao;
GLuint vbo;
GLuint mvpUniform;
mat4 perspectiveProjectionMatrix;

GLuint compute_prog;
GLint   dt_location;
GLuint  output_image;



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

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void ToggleFullScreen(void);
void update(void);
void uninitialize(void);

static inline float random_float()
{
	float res;
	unsigned int tmp;
	static unsigned int seed = 0xFFFF0C59;

	seed *= 16807;

	tmp = seed ^ (seed >> 4) ^ (seed << 15);

	*((unsigned int *)&res) = (tmp >> 9) | 0x3F800000;

	return (res - 1.0f);
}

static vmath::vec3 random_vector(float minmag = 0.0f, float maxmag = 1.0f)
{
	vmath::vec3 randomvec(random_float() * 2.0f - 1.0f, random_float() * 2.0f - 1.0f, random_float() * 2.0f - 1.0f);
	randomvec = normalize(randomvec);
	randomvec *= (random_float() * (maxmag - minmag) + minmag);

	return randomvec;
}

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
		TEXT("Aditya Boob - Particle Simulation - Compute Shader - OpenGL"),
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
	GLuint gComputeShader;
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

	//////////// COMPUTE SHADER ////////////
	//Define Vertex Shader Object
	gComputeShader = glCreateShader(GL_COMPUTE_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar **p;
	const GLchar *computeShaderSourceCode =

		"#version 430 core\n"
		"\n"
"		layout(std140, binding = 0) uniform attractor_block												  \n" 
"	{																									  \n" 
"		vec4 attractor[64]; // xyz = position, w = mass													  \n" 
"	};																									  \n" 
"		\n																								  \n" 
"	layout(local_size_x = 256) in;																		  \n" 
"																										  \n" 
"	layout(rgba32f, binding = 0) uniform imageBuffer velocity_buffer;									  \n" 
"	layout(rgba32f, binding = 1) uniform imageBuffer position_buffer;									  \n" 
"																										  \n" 
"	uniform float dt = 1.0;																				  \n" 
"																										  \n" 
"	void main(void)																						  \n" 
"	{																									  \n" 
"		vec4 vel = imageLoad(velocity_buffer, int(gl_GlobalInvocationID.x));							  \n" 
"		vec4 pos = imageLoad(position_buffer, int(gl_GlobalInvocationID.x));							  \n" 
"																										  \n" 
"		int i;																							  \n" 
"																										  \n" 
"		pos.xyz += vel.xyz * dt;																		  \n" 
"		pos.w -= 0.0001 * dt;																			  \n" 
"																										  \n" 
"		for (i = 0; i < 8; i++)																			  \n" 
"		{																								  \n" 
"			vec3 dist = (attractor[i].xyz - pos.xyz);													  \n" 
"			vel.xyz += dt * dt * attractor[i].w * normalize(dist) / (dot(dist, dist) + 10.0);			  \n" 
"		}																								  \n" 
"																										  \n" 
"		if (pos.w <= 0.0)																				  \n" 
"		{																								  \n" 
"			pos.xyz = -pos.xyz * 0.01;																	  \n" 
"			vel.xyz *= 0.01;																			  \n" 
"			pos.w += 1.0f;																				  \n" 
"		}																								  \n" 
"																										  \n" 
"		imageStore(position_buffer, int(gl_GlobalInvocationID.x), pos);									  \n" 
"		imageStore(velocity_buffer, int(gl_GlobalInvocationID.x), vel);									  \n" 
"	}																							  \n" 
																										  
		;
	// GPU will run the above code. And GPU WILL RUN FOR PER VERTEX. If there are 1000 vertex. Then GPU will run this shader for
	//1000 times. We are Multiplying each vertex with the Model View Matrix.
	//And how does the GPU gets to know about at what offset the array has to be taken . Go to glVertexAttribPointer() in Display.
	// in = Input. 

	//p = &vertexShaderSourceCode;
		//Specify above source code to the vertex shader object
	glShaderSource(gComputeShader, 1, (const GLchar **)&computeShaderSourceCode, NULL);

	//Compile the vertex shader 
	glCompileShader(gComputeShader);

	//////////////// Error Checking//////////////////
	//Code for catching the errors 
	GLint iShaderCompileStatus = 0;
	GLint iInfoLogLength = 0;
	GLchar *szInfoLog = NULL;


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
	
	//////////////////////////////// V E R T E X - S H A D E R //////////////////////////


	//Define Vertex Shader Object
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code

	const GLchar *vertexShaderSourceCode =

		"#version 430 core\n"
		"\n"
		"in vec4 vert;\n"
		"\n"
		"uniform mat4 mvp;\n"
		"\n"
		"out float intensity;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"    intensity = vert.w;\n"
		"    gl_Position = mvp * vec4(vert.xyz, 1.0);\n"
		"}\n";

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
				fprintf(gpLogFile, "VS : %s\n", szInfoLog);
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
		"#version 430 core\n"
		"\n"
		"layout (location = 0) out vec4 color;\n"
		"\n"
		"in float intensity;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"    color = mix(vec4(0.0f, 0.2f, 1.0f, 1.0f), vec4(0.2f, 0.05f, 0.0f, 1.0f), intensity);\n"
		"}\n";
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
				fprintf(gpLogFile, "FS : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(hwnd);
				exit(0);


			}
		}
	}
	// CREATE SHADER PROGRAM OBJECT
	gShaderProgramObject = glCreateProgram();
	compute_prog = glCreateProgram();
	//attach vertex shader to the gShaderProgramObject
	glAttachShader(gShaderProgramObject, gVertexShaderObject);

	//attach fragment shader to the gShaderProgramObject
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);
	glAttachShader(compute_prog, gComputeShader);


	//Pre-Linking  binding to vertexAttributes
	//glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_POSITION, "vert");
	//Here the above line means that we are linking the GPU's variable vPosition with the CPU's  enum member  i.e AMC_ATTRIBUTE_POSITION .
	//So whatever changes will be done in AMC_ATTRIBUTE_POSITION , those will also reflect in vPosition

	//RULE : ALWAYS BIND THE ATTRIBUTES BEFORE LINKING AND BIND THE UNIFORM AFTER LINKING.

	//Link the shader program 
	glLinkProgram(gShaderProgramObject);
	glLinkProgram(compute_prog);

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
	int size[3];
	glGetProgramiv(compute_prog, GL_COMPUTE_WORK_GROUP_SIZE, size);
	fprintf(gpLogFile, "Value oof work group size is x : %d   y : %d  z : %d \n ", size[0], size[1], size[2]);

	//POST Linking
	//Retrieving uniform locations 
	dt_location = glGetUniformLocation(compute_prog, "dt");
	mvpUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
	//Here we have done all the preparations of data transfer from CPU to GPU
	/*glGenTextures(1, &output_image);
	glBindTexture(GL_TEXTURE_2D, output_image);
	glTexStorage2D(GL_TEXTURE_2D, 8, GL_RGBA32F, 256, 256);
	glBindTexture(GL_TEXTURE_2D, 0);
	const GLfloat triangleVertices[] =
	{


		-1.0f, -1.0f, 0.5f, 1.0f,
		 1.0f, -1.0f, 0.5f, 1.0f,
		 1.0f,  1.0f, 0.5f, 1.0f,
		-1.0f,  1.0f, 0.5f, 1.0f,
	};
	//Basically we are giving the vertices coordinates in the above array

	//Create vao - Vertex array objects
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//in the above statement we have accesed the GL-ARRAY_BUFFER using vbo. Without it wouldn't be possible to get GL_ARRAY_BUFFER
	//For the sake of understanding . GL_ARRAY_BUFFER is in the GPU side ad=nd we have bind our CPU side vbo with it like a Pipe to get the access.
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
	//The above statement states that , we are passing our vertices array to the GPU and GL_STATIC_DRAW means draw it now only. Don't draw it in Runtime. 
	//The below statement states that after storing the data in the GPU'S buffer . We are passing it to the vPosition now . 

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	//GL_FALSE = We are not giving normalized coordinates as our coordinates are not converted in 0 - 1 range.
	//3 = This is the thing I was talking about in initialize. Here, we are telling GPU to break our array in 3 parts . 
	//0 and Null are for the Interleaved. 
	//GL_FLOAT- What is the type? .
	//AMC_ATTRIBUTE_POSITION. here we are passing data to vPosition. 

	glEnableVertexAttribArray(0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);


	*/




	//////////////////// Compute /////

	glGenVertexArrays(1, &render_vao);
	glBindVertexArray(render_vao);

	glGenBuffers(2, buffers);
	glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
	glBufferData(GL_ARRAY_BUFFER, PARTICLE_COUNT * sizeof(vmath::vec4), NULL, GL_DYNAMIC_COPY);

	vmath::vec4 * positions = (vmath::vec4 *)glMapBufferRange(GL_ARRAY_BUFFER,
		0,
		PARTICLE_COUNT * sizeof(vmath::vec4),
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	for (int i = 0; i < PARTICLE_COUNT; i++)
	{
		positions[i] = vmath::vec4(random_vector(-10.0f, 10.0f), random_float());
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, velocity_buffer);
	glBufferData(GL_ARRAY_BUFFER, PARTICLE_COUNT * sizeof(vmath::vec4), NULL, GL_DYNAMIC_COPY);

	vmath::vec4 * velocities = (vmath::vec4 *)glMapBufferRange(GL_ARRAY_BUFFER,
		0,
		PARTICLE_COUNT * sizeof(vmath::vec4),
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	for (int i = 0; i < PARTICLE_COUNT; i++)
	{
		velocities[i] = vmath::vec4(random_vector(-0.1f, 0.1f), 0.0f);
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	glGenTextures(2, tbos);

	for (int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_BUFFER, tbos[i]);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, buffers[i]);
	}

	glGenBuffers(1, &attractor_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, attractor_buffer);
	glBufferData(GL_UNIFORM_BUFFER, 32 * sizeof(vmath::vec4), NULL, GL_STATIC_DRAW);

	for (int i = 0; i < MAX_ATTRACTORS; i++)
	{
		attractor_masses[i] = 0.5f + random_float() * 0.5f;
	}

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, attractor_buffer);












	PlaySound(MAKEINTRESOURCE(MYWAV), NULL, SND_NODEFAULT | SND_RESOURCE | SND_ASYNC);


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
	aspect_ratio = (float)width / (float)height;
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


	
//
//	// use shader program
//	
//
//	//declaration of matrices
//	mat4 modelViewMatrix;
//	mat4 modelViewProjectionMatrix;
//
//	// intialize above matrices to identity
//	modelViewMatrix = mat4::identity();
//	modelViewProjectionMatrix = mat4::identity();
//
//	// perform necessary transformations
//	modelViewMatrix = translate(0.0f, 0.0f, -3.0f);
//	// do necessary matrix multiplication
//	// this was internally done by glOrtho() in FFP.	
//
//	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
//
//	// send necessary matrices to shader in respective uniforms
////	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
//	//GL_FALSE = Should we transpose the matrix?
//	//DirectX is roj major so there we will have to transpose but OpenGL is Colomn major so no need to transpose. 
//
//	glUseProgram(compute_prog);
//	glBindImageTexture(0, output_image, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
//	glDispatchCompute(8, 16, 1);
//	glBindTexture(GL_TEXTURE_2D, output_image);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	//
//	glUseProgram(gShaderProgramObject);
//	// bind with vao
//	//this will avoid many binding to vbo
//	//glBindVertexArray(vao);
//
//	// bind with textures
//
//	// draw necessary scene
//	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
//	//  0 =  From where to start in the array. 
//	// We have to start from 0th element i.e 0. if the 0th element would be 50.0f then we would have given the 2nd parameter as 50.
//	//3 = How many Vertices? 
//	//GL_TRIANGLES is the thing between glBegin() and glEnd()
//
//
//	// unbind vao
//	//glBindVertexArray(0);
//
//	// unuse program
//	glUseProgram(0);







	//////////

	static const GLuint start_ticks = ::GetTickCount() - 100000;
	GLuint current_ticks = ::GetTickCount();
	static GLuint last_ticks = current_ticks;
	float time = ((start_ticks - current_ticks) & 0xFFFFF) / float(0xFFFFF);
	float delta_time = (float)(current_ticks - last_ticks) * 0.075f;

	vmath::vec4 * attractors = (vmath::vec4 *)glMapBufferRange(GL_UNIFORM_BUFFER,
		0,
		32 * sizeof(vmath::vec4),
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	int i;

	for (i = 0; i < 32; i++)
	{
		attractors[i] = vmath::vec4(sinf(time * (float)(i + 4) * 7.5f * 20.0f) * 50.0f,
			cosf(time * (float)(i + 7) * 3.9f * 20.0f) * 50.0f,
			sinf(time * (float)(i + 3) * 5.3f * 20.0f) * cosf(time * (float)(i + 5) * 9.1f) * 100.0f,
			attractor_masses[i]);
	}

	glUnmapBuffer(GL_UNIFORM_BUFFER);

	// If dt is too large, the system could explode, so cap it to
	// some maximum allowed value
	if (delta_time >= 2.0f)
	{
		delta_time = 2.0f;
	}

	// Activate the compute program and bind the position and velocity buffers
	glUseProgram(compute_prog);
	glBindImageTexture(0, velocity_tbo, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(1, position_tbo, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	
	// Set delta time
	glUniform1f(dt_location, delta_time);


	// Dispatch
	glDispatchCompute(PARTICLE_GROUP_COUNT, 1, 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	vmath::mat4 mvp = vmath::perspective(60.0f, aspect_ratio, 0.1f, 1000.0f) *
		vmath::translate(0.0f, 0.0f, -150.0f) *
		vmath::rotate(time * 1000.0f, vmath::vec3(0.0f, 1.0f, 0.0f));

	// Clear, select the rendering program and draw a full screen quad
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glUseProgram(gShaderProgramObject);
	glUniformMatrix4fv(0, 1, GL_FALSE, mvp);
	glBindVertexArray(render_vao);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	// glPointSize(2.0f);
	glDrawArrays(GL_POINTS, 0, PARTICLE_COUNT);
	//glBindVertexArray(0);
	last_ticks = current_ticks;

	SwapBuffers(ghdc);

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