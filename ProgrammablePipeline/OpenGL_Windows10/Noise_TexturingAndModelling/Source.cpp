#include<Windows.h>
#include<glew.h>               // This SHOULD be above GL.h header.
#include<gl/GL.h>
//#include"Header.h"
#include<stdio.h>
#include"vmath.h"
#pragma comment(lib,"opengl32.lib")//for linking
#pragma comment(lib,"glew32.lib")//for linking


#define noiseWidth 128
#define noiseHeight 128
#define SIZE 64

//Variables for textures
GLuint vbo_texture;
GLuint smileyTexture;
GLuint samplerUniform;

#define CheckImageWidth 64
#define CheckImageHeight 64

int Win_Width = 800;
int Win_Height = 600;
//
//PROCEDURAL TEXTURE VARIABLES
GLubyte checkImage[CheckImageHeight][CheckImageWidth][4];
GLuint texName;

//Normal variables

//
using namespace vmath;

static GLfloat angle = 0.0f;

//Shader Variable.
GLuint gShaderProgramObject;

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

double noise[noiseHeight][noiseWidth];
float noise1[SIZE + 1][SIZE + 1][SIZE + 1];
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

//Noise Texture Related Variables
int noise3DTexSize = 128;
GLuint noise3DTexName = 0;
GLubyte *noise3DTexPtr;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

typedef struct xyz_td 
{
	float x,
		y,
		z;
}xyz_td;
float calc_noise(xyz_td ); 
float turbulence();
void ToggleFullScreen(void);
void update(void);
void make3DNoiseTexture(void);
void generateNoise();
void Noise();
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
		TEXT("OrthoTriangle"),
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
			ToggleFullScreen();
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
	//Functions declarations
	void loadTexture();



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
		"in vec2 vTexCoord;" \
		"out vec2 out_TexCoord;" \
		"in vec4 vPosition;" \
		"uniform mat4 u_mvp_matrix;" \

		"void main(void)" \
		"{" \
		"gl_Position = u_mvp_matrix * vPosition;" \
		"out_TexCoord =  vTexCoord;" \
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
		"in vec2 out_TexCoord;" \
		"out vec4 FragColor;" \
		"uniform sampler2D u_sampler;" \
		"void main(void)" \
		"{" \
		"FragColor = texture(u_sampler,out_TexCoord);" \
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
	glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_TEXCOORD0, "vTexCoord");
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
	samplerUniform = glGetUniformLocation(gShaderProgramObject, "u_sampler");
	//Here we have done all the preparations of data transfer from CPU to GPU

	const GLfloat CheckerboardVertices[] =
	{
		// Straight checkerboard
		-2.0f, -1.0f, 0.0f,		// left-bottom
		-2.0f, 1.0f, 0.0f,		// left-top
		0.0f, 1.0f, 0.0f,		// right-top
		0.0f, -1.0f, 0.0f,	// right-bottom
		// checkerboard at 45 degrees
			1.0f, -1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			2.41421f, 1.0f, -1.41421f,
			2.41421f, -1.0f, -1.41421f
	};
	
	const GLfloat checkerboardTexCoords[] =
	{										// Checkerboard texture
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		//Rotated
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};
	
	//Basically we are giving the vertices coordinates in the above array

	//Create vao - Vertex array objects
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//in the above statement we have accesed the GL-ARRAY_BUFFER using vbo. Without it wouldn't be possible to get GL_ARRAY_BUFFER
	//For the sake of understanding . GL_ARRAY_BUFFER is in the GPU side ad=nd we have bind our CPU side vbo with it like a Pipe to get the access.
	//glBufferData(GL_ARRAY_BUFFER, sizeof(CheckerboardVertices), CheckerboardVertices, GL_STATIC_DRAW);
	//The above statement states that , we are passing our vertices array to the GPU and GL_STATIC_DRAW means draw it now only. Don't draw it in Runtime. 
	//The below statement states that after storing the data in the GPU'S buffer . We are passing it to the vPosition now . 
	//Dynamic Draw of Checkerboard(Vertices)
	glBufferData(GL_ARRAY_BUFFER, 4 * 12 * sizeof(GL_FLOAT), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//GL_FALSE = We are not giving normalized coordinates as our coordinates are not converted in 0 - 1 range.
	//3 = This is the thing I was talking about in initialize. Here, we are telling GPU to break our array in 3 parts . 
	//0 and Null are for the Interleaved. 
	//GL_FLOAT- What is the type? .
	//AMC_ATTRIBUTE_POSITION. here we are passing data to vPosition.
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	///////////////////// T E X T U R E's VBO  /////////////////////////
	glGenBuffers(1, &vbo_texture);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(checkerboardTexCoords), checkerboardTexCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//unbind VAO
	glBindVertexArray(0);

	//Depth Lines
	glClearDepth(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//Texture calls
	glEnable(GL_TEXTURE_2D);
	Noise();
	//loadTexture(&smileyTexture, MAKEINTRESOURCE(IDBITMAP_SMILEY));
	loadTexture();

	
	//initializing Perspective ProjectionMatrix to identity.
	perspectiveProjectionMatrix = mat4::identity();

	resize(Win_Width, Win_Height);

	return(0);
}
void loadTexture(void)
{
	void MakeCheckImage(void);
	//generateNoise();
	//Noise();
	MakeCheckImage();
	//make3DNoiseTexture();


	//Now OpenGL part for texture starts from here
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,CheckImageWidth,CheckImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, noise1);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}
void generateNoise()
{
		double c;
		for (int y = 0; y < noiseHeight; y++)
		for (int x = 0; x < noiseWidth; x++)
		{
			//c = c * 255;
			noise[y][x]= ((rand() % 32768) / 32768.0);
		
			
		}
		for (int y = 0; y < noiseHeight; y++)
			for (int x = 0; x < noiseWidth; x++)
			{
				//noise[x][y] = 

			}
		fprintf(gpLogFile, "Finally exiting generateNoise\n");
}
void Noise()
{
	//Texturing and Modelling Code
	long i, j, k, ii, jj, kk;
	
	
	for(i = 0;i<SIZE;i++)
	for(j = 0;j<SIZE;j++)
	for (k = 0; k < SIZE; k++)
	{
		noise1[i][j][k] = (float)(rand() / (RAND_MAX  + 1.0));

	}


	for(i = 0;i<SIZE + 1;i++)
		for(j = 0;j<SIZE+1;j++)
			for (k = 0; k < SIZE + 1; k++)
			{
				ii = (i == SIZE) ? 0 : i;
				jj = (j == SIZE) ? 0 : j;
				kk = (k == SIZE) ? 0 : k;
				noise1[i][j][k] = noise1[ii][jj][kk];

				fprintf(gpLogFile, "value of noise is %f /t%f /t%f \n",noise1[i][j][k]);

			}

	fprintf(gpLogFile,"Finally exiting Noise\n");


}
float calc_noise(xyz_td pnt)
{
	float t1;
	float  p_l, p_l2,    /* value lerped down left side of face1 & face 2 */
		p_r, p_r2,    /* value lerped down left side of face1 & face 2 */
		p_face1,     /* value lerped across face 1 (x-y plane ceil of z) */
		p_face2,     /* value lerped across face 2 (x-y plane floor of z) */
		p_final;     /* value lerped through cube (in z)                  */

	float calcnoise[SIZE + -1][SIZE + -1][SIZE + -1];
	register int x, y, z, px, py, pz;


	px = (int)pnt.x;
	py = (int)pnt.y;
	pz = (int)pnt.z;
	x = px & (SIZE - 1);//make sure the values are in table
	y = py & (SIZE - 1);
	z = pz & (SIZE - 1);


	t1 = pnt.y - py;
	p_l = noise1[x][y][z + 1] + t1 * (noise1[x][y + 1][z + 1] - 
		noise1[x][y][z + 1]);
	p_r = noise1[x + 1][y][z + 1] + t1 * (noise1[x + 1][y + 1][z + 1] - 
		noise1[x + 1][y][z + 1]);
	p_l2 = noise1[x][y][z] + t1 * (noise1[x][y + 1][z] - 
		noise1[x][y][z]);
	p_r2 = noise1[x + 1][y][z] + t1 * (noise1[x + 1][y + 1][z] - noise1[x + 1][y][z]);

	t1 = pnt.x - px;
	p_face1 = p_l + t1 * (p_r - p_l);
	p_face2 = p_l2 + t1 * (p_r2 - p_l2);
	t1 = pnt.z - pz;
	p_final = p_face2 + t1 * (p_face1 - p_face2);
	return(p_final);
}
float turbulence(xyz_td pnt, float pixel_size)
{
	float t, scale;
	t = 0;

	for (scale = 1.0; scale > pixel_size; scale /= 2.0)
	{
		pnt.x = pnt.x / scale;
		pnt.y = pnt.y / scale;
		pnt.z = pnt.z / scale;

		t += calc_noise(pnt) * scale;
	}
	return(t);
}
void basic_gas(xyz_td pnt, float *density, float *parms)
{
	float turb;
	int i;
	static float pow_table[10000];
	static int calcd = 1;

	if (calcd)
	{
		calcd = 0;
		for (i = 10000 - 1; i >= 0; i--)
		{
			pow_table[i] = (float)pow(((double)(i)) / (10000 - 1)*parms[1] * 2.0, (double)parms[2]);

		}
		turb = turbulence(pnt,4);
		*density = pow_table[(int)(turb *(0.5*(10000 - 1)))];
	}
}
void MakeCheckImage(void)
{
	//code
	int i, j, c;

	for (i = 0; i < CheckImageHeight; i++)
	{
		for (j = 0; j < CheckImageWidth; j++)
		{
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;

			checkImage[i][j][0] = (GLubyte)c;	//R
			checkImage[i][j][1] = (GLubyte)c;	//G
			checkImage[i][j][2] = (GLubyte)c;	//B
			checkImage[i][j][3] = 255;		//A
		}
	}
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
	perspectiveProjectionMatrix = perspective(60.0f,
		(GLfloat)width / (GLfloat)height,
		1.0f,
		30.0f);

}



void display(void)
{
	GLfloat checkerBoardVertices[12];
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use shader program
	glUseProgram(gShaderProgramObject);

	//declaration of matrices
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;

	// intialize above matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	mat4 scaleMatrix = mat4::identity();

	// perform necessary transformations
	modelViewMatrix = translate(0.0f, 0.0f, -3.6f);
	modelViewMatrix = modelViewMatrix * scaleMatrix;

	// do necessary matrix multiplication
	// this was internally done by glOrtho() in FFP.	

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	// send necessary matrices to shader in respective uniforms
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	//GL_FALSE = Should we transpose the matrix?
	//DirectX is row major so there we will have to transpose but OpenGL is Colomn major so no need to transpose. 

	/*Texture Calls*/
	glActiveTexture(GL_TEXTURE0);
	//Here,GL_TEXTURE0 gets matched to AMC_TTRIBUTE_TEXCOORD0 
	//For one geometry we can give 80 textures.

	glBindTexture(GL_TEXTURE_2D, texName);
	//now give it to Fragment Shader
	glUniform1i(samplerUniform, 0);



	// bind with vao
	//this will avoid many binding to vbo
	glBindVertexArray(vao);
	checkerBoardVertices[0] = -2.0f;
	checkerBoardVertices[1] = -1.0f;
	checkerBoardVertices[2] = 0.0f;

	checkerBoardVertices[3] = -2.0f;
	checkerBoardVertices[4] = 1.0f;
	checkerBoardVertices[5] = 0.0f;

	checkerBoardVertices[6] = 0.0f;
	checkerBoardVertices[7] = 1.0f;
	checkerBoardVertices[8] = 0.0f;

	checkerBoardVertices[9] = 0.0f;
	checkerBoardVertices[10] = -1.0f;
	checkerBoardVertices[11] = 0.0f;

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(checkerBoardVertices), checkerBoardVertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	


	// bind with vao
	//this will avoid many binding to vbo
	
	// draw necessary scene
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);



	checkerBoardVertices[0] = 1.0f;
	checkerBoardVertices[1] = -1.0f;
	checkerBoardVertices[2] = 0.0f;

	checkerBoardVertices[3] = 1.0f;
	checkerBoardVertices[4] = 1.0f;
	checkerBoardVertices[5] = 0.0f;

	checkerBoardVertices[6] = 2.41421f;
	checkerBoardVertices[7] = 1.0f;
	checkerBoardVertices[8] = -1.41421f;

	checkerBoardVertices[9] = 2.41421f;
	checkerBoardVertices[10] = -1.0f;
	checkerBoardVertices[11] = -1.41421f;

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(checkerBoardVertices), checkerBoardVertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);




	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	//  0 =  From where to start in the array. 
	// We have to start from 0th element i.e 0. if the 0th element would be 50.0f then we would have given the 2nd parameter as 50.
	//4 = How many Vertices? 
	//GL_TRIANGLES is the thing between glBegin() and glEnd()


	// unbind vao
	glBindVertexArray(0);

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