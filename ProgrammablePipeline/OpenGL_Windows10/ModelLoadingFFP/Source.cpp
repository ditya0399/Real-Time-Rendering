#include<Windows.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#include<stdio.h>
#include<stdlib.h>
#include<vector>

#pragma comment(lib,"opengl32.lib")//for linking
#pragma comment(lib,"glu32.lib")//for linking

#define ERRORBOX(lpszErrorMessage,lpszCaption){                                                                                 \
																	MessageBox((HWND)NULL,TEXT(lpszErrorMessage),                \
																	 TEXT(lpszCaption),MB_ICONERROR);		                      \
											                         ExitProcess(EXIT_FAILURE);                                    \
											  }										   				 


#define ERRORBOX2(lpszErrorMessage,lpszCaption){                                                                                 \
																    MessageBox((HWND)NULL,TEXT(lpszErrorMessage),                 \
																    TEXT(lpszCaption),MB_ICONERROR);		                       \
											                        DestroyWindow(hwnd);                                            \
											 }							
int Win_Width = 800;
int Win_Height = 600;

//Global variables for ModelLoading
#define BUFFERSIZE 256
#define S_EQUAL 0
#define NR_NORMAL_COORDS 3
#define NR_TEXTUER_COORDS 2
#define NR_POINT_QUADS 3
#define MIN_NR_FACE_TOKENS 4
#define MAX_NR_FACE_TOKENS 4

std::vector<std::vector<float>> g_Vertices;
std::vector<std::vector<float>> g_Textures;
std::vector<std::vector<float>> g_Normals;

std::vector<std::vector<int>> g_Face_Tri;
std::vector<std::vector<int>> g_Face_Texture;
std::vector<std::vector<int>> g_Face_Normals;





FILE *g_fp_MeshFile = NULL;
FILE *g_fp_LogFile = NULL;
char line[BUFFERSIZE];

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
		TEXT("Perspective"),
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

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (gbIsActiveWindow == true)
			{
				//if
			}
			display(); //callPisplay
		}
	}
	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	void resize(int, int);
	void uninitialize(void);

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
	void LoadMeshData();
	void resize(int, int);

	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	errno_t error;
	error = fopen_s(&g_fp_LogFile, "MeshLoadingFile.TXT", "w");

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
	pfd.cDepthBits = 24;

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
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);

	glShadeModel(GL_SMOOTH);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	LoadMeshData();
	resize(Win_Width, Win_Height);

	return(0);
}

void resize(int iWidth, int iHeight)
{
	if (iHeight == 0)
	{
		iHeight = 1;
	}

	glViewport(0, 0, (GLsizei)iWidth, (GLsizei)iHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f,
		(GLfloat)iWidth / (GLfloat)iHeight,
		0.1f,
		100.0f
	);
}


void LoadMeshData()
{
	void uninitialize();
	errno_t err;
    char *sep_Space =(char *)" "; //Dynamic Buffer to hold Space.
	char *sep_fSlash = (char *)"/";//Dynamic Buffer to hold backsplash.
	char *first_token = NULL;//Character pointer for holding the first word in a line.
	char *token = NULL;// Character pointer for holding next word separated by the specified separator to strtok
	
	
	char *next_token = NULL;



	// Array of character pointers to hold strings of face entries
	// Face entries can be variable. In some files they are 3 and in some they are 4
	char *face_tokens[MAX_NR_FACE_TOKENS];
	// Number of non-null tokens in the above vector
	int nr_tokens;
	// Character pointer for holding string associated with vertex index
	char *token_vertex_index = NULL;
	// Character pointer for holding string associated with texture index
	char *token_texture_INDEX = NULL;

	//Open the OBJ File
	err = fopen_s(&g_fp_MeshFile, "Skull.obj", "r");
	if (!g_fp_MeshFile)
	{
		uninitialize();
	}
	//Loop will run untill there is line in a file.
	while (fgets(line, BUFFERSIZE, g_fp_MeshFile) != NULL && !feof(g_fp_MeshFile))
	{
		// Bind the line to a separator and get first token
		first_token = strtok_s(line, sep_Space, &next_token);

		//if the first token is actually VertexData.
		if (strcmp(first_token, "v") == S_EQUAL)
		{
			//Create a vector of NUMBER_POINT_QUADS(Actually this should be COORDS)
			//Step 1: Get Next Token
			//Step 2: give the data to atof to get the Floating point Number
			//Step 3: Add the floating point number generated to the vector.
			//After the end, a vector is being constructed
			std::vector<float>  vec_point_quad(NR_POINT_QUADS);
			for (int i = 0; i != NR_POINT_QUADS; i++)			
				vec_point_quad[i] = atof(strtok_s(NULL,sep_Space,&next_token));
			g_Vertices.push_back(vec_point_quad);

		}

		//if first token is Texture Data.
		else if (strcmp(first_token, "vt") == S_EQUAL)
		{
			//Create a vector of NUMBER_TEXTURE_COORDS
			//Step 1: Get Next Token
			//Step 2: give the data to atof to get the Floating point Number
			//Step 3: Add the floating point number generated to the vector.
			//After the end, a vector is being constructed
			std::vector<float>  vec_texture_coords(NR_TEXTUER_COORDS);
			for (int i = 0; i != NR_TEXTUER_COORDS; i++)
				vec_texture_coords[i] = atof(strtok_s(NULL, sep_Space, &next_token));
			    g_Textures.push_back(vec_texture_coords);
		}
		//if normal data
		else if (strcmp(first_token, "vn") == S_EQUAL)
		{
			//Create a vector of NUMBER_POINT_COORDS
			//Step 1: Get Next Token
			//Step 2: give the data to atof to get the Floating point Number
			//Step 3: Add the floating point number generated to the vector.
			//After the end, a vector is being constructed
			std::vector<float>  vec_normal_coords(NR_NORMAL_COORDS);
			for (int i = 0; i != NR_NORMAL_COORDS; i++)
				vec_normal_coords[i] = atof(strtok_s(NULL, sep_Space, &next_token));
			g_Normals.push_back(vec_normal_coords);
		}
		//if face data
		else if (strcmp(first_token, "f") == S_EQUAL)
		{
			//define 3 vectors of integers with length 3
			//this will hold all the triangle positional coordinates and texture coordinates
			std::vector<int>triangle_vertex_indices(3),texture_vertex_indices(3);
			memset((void *)face_tokens, 0, MAX_NR_FACE_TOKENS);
			nr_tokens = 0;
			while (token = strtok_s(NULL, sep_Space, &next_token))
			{
				if (strlen(token) < 4)
					break;
				face_tokens[nr_tokens] = token;
				nr_tokens++;

			}
			// Every face data entry is going to have minimum 3 fields, hence construct a triangle out of it
			// Step1 : triangle coordinate index data
			// Step2 : texture coordinate index data
			// Step3 : normal coordinate index data
			// Step4 : Put the data in triangle_vertex_indices, texture_vertex_indices and normal_vertex_indices
			// Vectors will be constructed at the end of the loop
			for (int i = 0; i != MIN_NR_FACE_TOKENS; ++i)
			{
				token_vertex_index = strtok_s(face_tokens[i], sep_fSlash, &next_token);
				token_texture_INDEX = strtok_s(NULL, sep_fSlash, &next_token);

				//Normal index in same way 
				
				triangle_vertex_indices[i] = atoi(token_vertex_index);
				texture_vertex_indices[i] = atoi(token_texture_INDEX);
			}
				g_Face_Tri.push_back(triangle_vertex_indices);
				g_Face_Texture.push_back(texture_vertex_indices);
			//Normals faces in the same way
			if (nr_tokens == MAX_NR_FACE_TOKENS)
			{
				triangle_vertex_indices[1] = triangle_vertex_indices[2];
				triangle_vertex_indices[2] = atoi(token_vertex_index);
				
				texture_vertex_indices[1] = texture_vertex_indices[2];
				texture_vertex_indices[2] = atoi(token_texture_INDEX);

				//same for normals
				
				token_vertex_index = strtok_s(face_tokens[MAX_NR_FACE_TOKENS - 1], sep_fSlash, &next_token);
				token_texture_INDEX = strtok_s(NULL, sep_fSlash, &next_token);

				//SAME for normals


				g_Face_Tri.push_back(triangle_vertex_indices);
				g_Face_Texture.push_back(texture_vertex_indices);

				//same for normals
			}
		}
		memset((void *)line, (int)'\0',BUFFERSIZE);
	}
	fclose(g_fp_MeshFile);
	g_fp_MeshFile = NULL;

	fprintf(g_fp_LogFile,"g vertices %llu gTexture- %llu gNormal - %llu GfaceTri - %llu\n", g_Vertices.size(), g_Textures.size(), g_Normals.size(), g_Face_Tri.size());

}
void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -7.0f);
	//glRotatef(Rotate, 0.0f, 1.0f, 0.0f);
	//glScalef(1.5f,1.5f,1.5f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (int i = 0; i!= g_Face_Tri.size(); ++i)
	{
		glBegin(GL_TRIANGLES);
		for (int j = 0; j != g_Face_Tri[i].size(); j++)
		{
			int vi = g_Face_Tri[i][j] - 1;
			glVertex3f(g_Vertices[vi][0],g_Vertices[vi][1],g_Vertices[vi][2]);

		}
		glEnd();

	}


	SwapBuffers(ghdc);

}

void uninitialize(void)
{
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
