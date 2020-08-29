#include<Windows.h>
#include<glew.h>               // This SHOULD be above GL.h header.
#include<gl/GL.h>

#include<stdio.h>
#include"vmath.h"
#pragma comment(lib,"opengl32.lib")//for linking
#pragma comment(lib,"glew32.lib")//for linking


int Win_Width = 800;
int Win_Height = 600;
using namespace vmath;
int vert;
static GLfloat angle = 0.0f;
GLuint gShaderProgramObject;
int vertices;
GLfloat VerticesOfCircle[10000];
GLfloat VerticesOfCircleOuter[10000];

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
GLuint vao_origin;
GLuint vbo_origin;
GLuint vao_RightLines;
GLuint vbo_RightLines;


GLuint vao_triangle;
GLuint vbo_triangle;
GLuint vao_InCircle;
GLuint vbo_InCircle;


GLuint vao_rectangle;
GLuint vbo_rectangle;
GLuint vao_OutCircle;
GLuint vbo_OutCircle;



float a = 2;
float b = sqrt(5);
float c = sqrt(5);
float sp;
float p, area, radius, xOffSet, yOffSet, xOffSet1, yOffSet2;

GLfloat Radius();
void DrawCircle();

GLuint mvpUniform;
mat4 perspectiveProjectionMatrix;

//float GraphVertices[];

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

float GraphVerticesVertical[80];
float GraphVerticesHorizontal[80];

int GenerateGraphVertices(int);
void DrawOuterCircle();
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
		TEXT("PP-Graph"),
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
		fprintf_s(gpLogFile, "choosePixelFormat() Failed\n");
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
		"in vec4 vColor;" 
		"out vec4 out_color;" 
		"uniform mat4 u_mvp_matrix;" \

		"void main(void)" \
		"{" \
		"gl_Position = u_mvp_matrix* vPosition;" \
		"out_color =  vColor;" \
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
		"in vec4 out_color;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		
		"FragColor = out_color;" \
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
				fprintf(gpLogFile, "%s\n", szInfoLog);
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
	glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_COLOR, "vColor");
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
	//Here we have done all the preparations of data transfer from CPU to GPU
	
	int num;

	vert = GenerateGraphVertices(5);

	const GLfloat OriginVertices[] =
	{
		1.0f, 0.0f, 0.0f, //X-Axis
		-1.0f, 0.0f, 0.0f,
		0.0f,1.0f,0.0f,
		0.0f,-1.0f,0.0f
	};



	//Basically we are giving the vertices coordinates in the above array

	//Create vao - Vertex array objects
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//in the above statement we have accesed the GL-ARRAY_BUFFER using vbo. Without it wouldn't be possible to get GL_ARRAY_BUFFER
	//For the sake of understanding . GL_ARRAY_BUFFER is in the GPU side ad=nd we have bind our CPU side vbo with it like a Pipe to get the access.
	glBufferData(GL_ARRAY_BUFFER, sizeof(GraphVerticesVertical), GraphVerticesVertical, GL_STATIC_DRAW);
	//The above statement states that , we are passing our vertices array to the GPU and GL_STATIC_DRAW means draw it now only. Don't draw it in Runtime. 
	//The below statement states that after storing the data in the GPU'S buffer . We are passing it to the vPosition now . 

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	//GL_FALSE = We are not giving normalized coordinates as our coordinates are not converted in 0 - 1 range.
	//3 = This is the thing I was talking about in initialize. Here, we are telling GPU to break our array in 3 parts . 
	//0 and Null are for the Interleaved. 
	//GL_FLOAT- What is the type? .
	//AMC_ATTRIBUTE_POSITION. here we are passing data to vPosition. 

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 0.0f, 1.0f, 0.0f);

	glBindVertexArray(0);

	//ORIGIN LINES//
	//glLineWidth(5.0f);
	glGenVertexArrays(1, &vao_origin);
	glBindVertexArray(vao_origin);
	glGenBuffers(1, &vbo_origin);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_origin);
	glBufferData(GL_ARRAY_BUFFER, sizeof(OriginVertices), OriginVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 0.0f, 1.0f, 0.0f);

	glBindVertexArray(0);

	///////HORIZONTAL lINES //////

	glGenVertexArrays(1, &vao_RightLines);
	glBindVertexArray(vao_RightLines);
	glGenBuffers(1, &vbo_RightLines);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_RightLines);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GraphVerticesHorizontal), GraphVerticesHorizontal, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 0.0f, 1.0f, 0.0f);
	glBindVertexArray(0);



	const GLfloat triangleVertices[] =
	{
		0.0f,1.0f,0.0f,
		-1.0f,-1.0f,0.0f,
		1.0f,-1.0f,0.0f,

		1.0f,-1.0f,0.0f,
		0.0f,1.0f,0.0f
	};

	const GLfloat rectangleVertices[] =
	{
		-1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		
		1.0f,1.0f,0.0f,
		1.0f,-1.75f,0.0f,

		1.0f,-1.75f,0.0f,
		-1.0f,-1.75f,0.0f,

		-1.0f,-1.75f,0.0f,
		-1.0f,1.0f,0.0f,
	};
	//Basically we are giving the vertices coordinates in the above array
	Radius();

	DrawCircle();


	DrawOuterCircle();
	//Create vao - Vertex array objects
	glGenVertexArrays(1, &vao_triangle);
	glBindVertexArray(vao_triangle);
	glGenBuffers(1, &vbo_triangle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);
	glBindVertexArray(0);

	//// INCIRCLE /////////////

	glGenVertexArrays(1, &vao_InCircle);
	glBindVertexArray(vao_InCircle);
	glGenBuffers(1, &vbo_InCircle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_InCircle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VerticesOfCircle), VerticesOfCircle, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);
	glBindVertexArray(0);
	


	///// Rectangle ///
	glGenVertexArrays(1, &vao_rectangle);
	glBindVertexArray(vao_rectangle);
	glGenBuffers(1, &vbo_rectangle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_rectangle);

	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);


	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);
	glBindVertexArray(0);

	//////////Outer Circle  //////




	glGenVertexArrays(1, &vao_OutCircle);
	glBindVertexArray(vao_OutCircle);
	glGenBuffers(1, &vbo_OutCircle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_OutCircle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VerticesOfCircleOuter), VerticesOfCircleOuter, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);
	glBindVertexArray(0);
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

	//declaration of matrices
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;

	// intialize above matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	// perform necessary transformations
	modelViewMatrix = translate(0.0f, 0.0f, -1.3f);

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glBindVertexArray(vao);
	glLineWidth(1.0f);
	glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 0.0f, 1.0f, 0.0f);
	glDrawArrays(GL_LINES, 0, 80);
	glBindVertexArray(0);

	///////// ORIGIN //////////////////
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	modelViewMatrix = translate(0.0f, 0.0f, -1.3f);
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	// send necessary matrices to shader in respective uniforms
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glBindVertexArray(vao_origin);
	glLineWidth(5.0f);
	glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 0.0f, 1.0f, 0.0f);
	glDrawArrays(GL_LINES, 0, 4);
	glBindVertexArray(0);
	glLineWidth(0.0f);
	///////////////// HORIZONTAL LINES //////////////////

	
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	modelViewMatrix = translate(0.0f, 0.0f, -1.3f);
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glBindVertexArray(vao_RightLines);
	glLineWidth(1.0f);
	glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 0.0f, 1.0f, 0.0f);
	glDrawArrays(GL_LINES, 0,80);
	glBindVertexArray(0);


	//Triangle
	glLineWidth(0.0f);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	modelViewMatrix = translate(0.0f, 0.2f, -5.0f);
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glBindVertexArray(vao_triangle);
	glLineWidth(3.0f);
	glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);
	glDrawArrays(GL_LINE_LOOP, 0, 3);
	glBindVertexArray(0);

	////Circle
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	modelViewMatrix = translate(0.0f, 0.2f, -5.0f);
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glBindVertexArray(vao_InCircle);
	glPointSize(3.0f);
	glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);
	// bind with textures

	// draw necessary scene
	glDrawArrays(GL_LINE_LOOP, 0, 3000 * 1);
	glBindVertexArray(0);

	//Outer Circle
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	modelViewMatrix = translate(0.0f, 0.2f, -5.0f);
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glBindVertexArray(vao_OutCircle);
	glPointSize(3.0f);
	glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);
	// bind with textures

	// draw necessary scene
	glDrawArrays(GL_LINE_LOOP, 0, 1250 * 1);
	glBindVertexArray(0);

	//Rectangle
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	modelViewMatrix = translate(0.0f, 0.2f, -5.0f);
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glBindVertexArray(vao_rectangle);
	glLineWidth(3.0f);
	glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);
	glDrawArrays(GL_LINES, 0, 12);
	glBindVertexArray(0);


	// unuse program
	glUseProgram(0);

	// unuse program
	glUseProgram(0);
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
int GenerateGraphVertices(int Vertices)
{
	int n, j, k;
	n = 0;
	j = 1;
	k = 3;
	int l = 0;
	
	for (GLfloat iFloat = -1.0f; iFloat <= 1.0f; iFloat = iFloat + 0.1f)
	{
		GraphVerticesVertical[n + 0] = iFloat;
	//	fprintf(gpLogFile, "The value of %d iFloat vertices are %d\n", n, ArrayOfVertices[n]);
		n = n + 2;
		GraphVerticesVertical[n + 0] = iFloat;
		GraphVerticesVertical[j] = 1.0f;
	//	fprintf(gpLogFile, "The value of j %d and vertices are %d\n", j, ArrayOfVertices[j]);
		j = j + 4;
		GraphVerticesVertical[k] = -1.0f;
		//fprintf(gpLogFile, "The value of k %d and vertices are %d\n", k, ArrayOfVertices[k]);
		k = k + 4;

		n = n + 2;
	}
	l = n;
	///////////////// HORIZONTAL VERTICES /////////////
	int i = 0;
	int m = 2;
	int a = 1;
	for (GLfloat hFloat = 1.0f; hFloat >= -1.0f; hFloat = hFloat - 0.1f)
	{
		GraphVerticesHorizontal[a + 0] = hFloat;
		a = a + 2;
		GraphVerticesHorizontal[a + 0] = hFloat;
		a = a + 2;
		GraphVerticesHorizontal[i] = -1.0f;
		i = i + 4;

		GraphVerticesHorizontal[m] = 1.0f;
		m = m + 4;

	}
	vertices = a;
	fprintf(gpLogFile, "The value of vertices is : %d\n", l);
	return l;

}
GLfloat Radius()
{

	p = a + b + c;
	sp = p / 2.0f;
	area = sqrt(sp* (sp - a)*(sp - b)*(sp - c));
	//area = 0.5f * 2.0f * 1.0f;
	radius = (area / sp);
	xOffSet = (0 * a) + ((-1)* (b)) + (1 * (c));
	xOffSet1 = (xOffSet / p);
	yOffSet = ((1 * a) + (-1 * b) + (-1 * c));
	yOffSet2 = (yOffSet / p);

	return (radius);
}
void DrawCircle()
{

	int n = 0;
	//int k = 0;


	for (GLfloat fAngle = 0.0f; fAngle < (2.0f * M_PI); fAngle = fAngle + 0.002f)
	{
		VerticesOfCircle[(3 * n) + 0] = (radius)*(GLfloat)cos(fAngle) + xOffSet1;
		VerticesOfCircle[(3 * n) + 1] = (radius)*(GLfloat)sin(fAngle) + yOffSet2;
		VerticesOfCircle[(3 * n) + 2] = 0.0f;
		n = n + 1;
	}
	fprintf(gpLogFile, "n : %d\n", n);

	//return n;
}
void DrawOuterCircle()
{

	int n = 0;
	//int k = 0;


	for (GLfloat fAngle = 0.0f; fAngle < (2.0f * M_PI); fAngle = fAngle + 0.005f)
	{
		VerticesOfCircleOuter[(3 * n) + 0] = (1.7f)*(GLfloat)cos(fAngle) + xOffSet1;
		VerticesOfCircleOuter[(3 * n) + 1] = (1.7f)*(GLfloat)sin(fAngle) + yOffSet2;
		VerticesOfCircleOuter[(3 * n) + 2] = 0.0f;
		n = n + 1;
	}
	fprintf(gpLogFile, "n : %d\n", n);

	//return n;
}
