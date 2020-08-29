#include<Windows.h>
#include<glew.h>               // This SHOULD be above GL.h header.
#include<gl/GL.h>

#include<stdio.h>
#include"vmath.h"
#include"TexturedSmiley.h"
#pragma comment(lib,"opengl32.lib")//for linking
#pragma comment(lib,"glew32.lib")//for linking

//Variables for textures
GLuint vbo_texture;
GLuint smileyTexture;
GLuint samplerUniform;


//Normal variables
int Win_Width = 800;
int Win_Height = 600;

//
using namespace vmath;

static GLfloat angle = 0.0f;
float gAngle;
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
GLuint projectionUniform;
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
	BOOL loadTexture(GLuint *, TCHAR[]);



	GLuint gVertexShaderObject;
	GLuint gFragmentShaderObject;
	GLuint gGeometryShaderObject;

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



		"out VS_OUT{" \
		"out vec2 out_TexCoord;" \
		"}vs_out;" \

		
		"in vec4 vPosition;" \
		"uniform mat4 u_mvp_matrix;" \

		"void main(void)" \
		"{" \
		"gl_Position = u_mvp_matrix * vPosition;" \
		"vs_out.out_TexCoord =  vTexCoord;" \
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

	/////////////////////////////////// G E O M E T R Y ----- S H A D E R /////////////////////////

	gGeometryShaderObject = glCreateShader(GL_GEOMETRY_SHADER);

	// code for the shader
	const GLchar *geometryShaderSource =
		"#version 430" \
		"\n" \

		"layout(triangles,invocations = 32)in;" \
		"layout(triangle_strip,max_vertices = 3)out;" \
		"uniform mat4 u_mvp_matrix;" \
		"uniform mat4 u_proj_matrix;" \

		"in VS_OUT{" \
		"vec2 out_TexCoord;" \
		"}gs_in[];" \

		"out vec2 out_TexCoord;" \

		"float rand(vec2 co)" \
		"{" \
		"return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);" \
		"}" \

		"float randX =  100 * rand(vec2(gl_InvocationID,0.0));" \
		"float randY = -10.0 * rand(vec2(gl_InvocationID,0.0));" \
		"float randZ =  rand(vec2(gl_InvocationID,0.0));" \

		"void main(void)" \
		"{" \
		

		"out_TexCoord = gs_in[0].out_TexCoord;" \
		"gl_Position = u_mvp_matrix * (gl_in[0].gl_Position + vec4(randX * 5.0,randY,randZ,0.0));" \
		"EmitVertex();" \

		"out_TexCoord = gs_in[1].out_TexCoord;" \
		"gl_Position = u_mvp_matrix * (gl_in[1].gl_Position  + vec4(randX * 5.0,randY,randZ,0.0));" \
		"EmitVertex();" \

		"out_TexCoord = gs_in[2].out_TexCoord;" \
		"gl_Position = u_mvp_matrix * (gl_in[2].gl_Position + vec4(randX * 5.0,randY,randZ,0.0));" \
		"EmitVertex();" \

		

		"EndPrimitive();" \

		
		"}";


	glShaderSource(gGeometryShaderObject, 1, (const GLchar **)&geometryShaderSource, NULL);

	//Compile the vertex shader 
	glCompileShader(gGeometryShaderObject);

	//////////////// Error Checking//////////////////
	//Code for catching the errors 
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;


	glGetShaderiv(gGeometryShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gGeometryShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gGeometryShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpLogFile, "GS : %s\n", szInfoLog);
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
		"if(FragColor.r +FragColor.g + FragColor.b >= 2.0 )"\
		"discard;"\
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
	glAttachShader(gShaderProgramObject, gGeometryShaderObject);

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
	projectionUniform = glGetUniformLocation(gShaderProgramObject, "u_proj_matrix");
	//Here we have done all the preparations of data transfer from CPU to GPU

	const GLfloat smileyVertices[] =
	{
		-1.0f, -1.0f, 0.0f,		// left-bottom
		-1.0f, 1.0f, 0.0f,		// left-top
		1.0f, 1.0f, 0.0f,		// right-top
		1.0f, -1.0f, 0.0f		// right-bottom

	};
	const GLfloat smileyTexCoords[] =
	{									
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(smileyVertices), smileyVertices, GL_STATIC_DRAW);
	//The above statement states that , we are passing our vertices array to the GPU and GL_STATIC_DRAW means draw it now only. Don't draw it in Runtime. 
	//The below statement states that after storing the data in the GPU'S buffer . We are passing it to the vPosition now . 

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(smileyTexCoords), smileyTexCoords, GL_STATIC_DRAW);
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
	loadTexture(&smileyTexture, MAKEINTRESOURCE(IDBITMAP_SMILEY));



	//initializing Perspective ProjectionMatrix to identity.
	perspectiveProjectionMatrix = mat4::identity();

	resize(Win_Width, Win_Height);

	return(0);
}
BOOL loadTexture(GLuint *texture_Smiley, TCHAR imageResourceID[])
{
	HBITMAP hBitmap = NULL;
	BITMAP bmp;
	BOOL bStatus = FALSE;

	//Win32API
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResourceID, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	if (hBitmap != NULL)
	{
		bStatus = TRUE;
		GetObject(hBitmap, sizeof(BITMAP), &bmp);
		//Now OpenGL part for texture starts from here
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glGenTextures(1, texture_Smiley);
		glBindTexture(GL_TEXTURE_2D, *texture_Smiley);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);


		//gluBuild2DMipMaps = glTexImage2D + glGenerateMipMap
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,bmp.bmWidth,bmp.bmHeight,0,GL_BGR_EXT,GL_UNSIGNED_BYTE,bmp.bmBits);
		//2ndParam = MipMap Level. By passing zero we say that,I am not giving any level. You(OPENGL) decide by yourself.
		//Valid Numbers to be expected over there are - 2,4,6

		glGenerateMipmap(GL_TEXTURE_2D);
		
		DeleteObject(hBitmap);//As hBitmap posseses Global memory and it is always better to free it after the work has been done.
		//glBindTexture(GL_TEXTURE_2D, 0);
	}
	return bStatus;


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
	mat4 modelMatrix;
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;

	// intialize above matrices to identity
	modelMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	mat4 scaleMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	 viewMatrix = lookat(vec3(0.0f, 0.0f, 10.0f), vec3(0.0f, 0.0f ,- 20.0f), vec3(0.0f, 1.0f, 0.0f));
	// perform necessary transformations

	 modelMatrix = translate(0.0f, 0.0f, -5.0f);

	 modelMatrix *= rotate(gAngle, 1.0f, 0.0f, 0.0f);
	modelViewMatrix = viewMatrix * modelMatrix;
	//modelViewMatrix *= rotate(gAngle, 0.0f, 1.0f, 0.0f);
	//modelViewMatrix *= rotate(gAngle, 0.0f, 0.0f, 1.0f);
	//modelViewMatrix = modelViewMatrix * scaleMatrix;

	// do necessary matrix multiplication
	// this was internally done by glOrtho() in FFP.	

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	// send necessary matrices to shader in respective uniforms
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	//GL_FALSE = Should we transpose the matrix?
	//DirectX is row major so there we will have to transpose but OpenGL is Colomn major so no need to transpose. 

	/*Texture Calls*/
	glActiveTexture(GL_TEXTURE0) ;
	//Here,GL_TEXTURE0 gets matched to AMC_TTRIBUTE_TEXCOORD0 
	//For one geometry we can give 80 textures.

	glBindTexture(GL_TEXTURE_2D,smileyTexture);
	//now give it to Fragment Shader
	glUniform1i(samplerUniform, 0);



	// bind with vao
	//this will avoid many binding to vbo
	glBindVertexArray(vao);

	// bind with textures

	// draw necessary scene
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	//  0 =  From where to start in the array. 
	// We have to start from 0th element i.e 0. if the 0th element would be 50.0f then we would have given the 2nd parameter as 50.
	//3 = How many Vertices? 
	//GL_TRIANGLES is the thing between glBegin() and glEnd()


	// unbind vao
	glBindVertexArray(0);

	// unuse program
	glUseProgram(0);
	SwapBuffers(ghdc);
	gAngle += 0.05f;
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