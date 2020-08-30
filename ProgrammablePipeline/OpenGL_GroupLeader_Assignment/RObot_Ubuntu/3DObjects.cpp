#include<stdio.h>
#include<iostream>//this is a cache of namespace
#include<stdlib.h>
#include<memory.h>
#include<math.h>
#include "vmath.h"
#include"Cylinder.h"
#include"Sphere.h"
#include"OGL_STACK.h"
#define PI 3.14159
//Headers for OpenGL
#include<GL/glew.h>
#include<GL/gl.h>
#include<GL/glx.h>

// headers for XLib

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

//namespaces. 

using namespace std;
using namespace vmath;
bool perVertexLighting = false;
bool perFragmentLighting = false;

GLuint vao_SpherePerFragment;
GLuint vbo_position_SpherePerFragment;
GLuint vbo_Normals_SpherePerFragment;
GLuint vbo_Element_SpherePerFragment;
GLuint gShaderProgramObjectPerFragment;

GLuint gbLighting = 1;
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
GLuint LKeyPressedPerFragment = 0;

//	Per fragment
GLuint projectionUniformPerFragment;
GLuint modelUniformPerFragment;
GLuint ViewUniformPerFragment;
GLuint LDUniformPerFragment;
GLuint LAUniformPerFragment;
GLuint LSUniformPerFragment;
GLuint KDUniformPerFragment;
GLuint KAUniformPerFragment;
GLuint KSUniformPerFragment;
float MaterialShininessUniformPerFragment;
GLuint LightPositionUniformPerFragment;


GLfloat lightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightPosition[] = { 0.0f,0.0f,2.0f,1.0f };

GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialShininess = 50.0f;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

int gNumVertices;
int gNumElements;



GLuint vao_Sphere;
GLuint vbo_position_Sphere;
GLuint vbo_Normals_Sphere;
GLuint vbo_Element_Sphere;



GLfloat angle = 0.0f;
//global variable declarations


char keys[26];
bool bFullscreen = false;
Display *gpDisplay = NULL;
XVisualInfo *gpXVisualInfo = NULL;
Colormap gColormap;
Window gWindow;
FILE *gpLogFile = NULL;

int giWindowWidth = 800;
int giWindowHeight = 600;


GLfloat angleTriangle = 0.0f;
GLfloat angleRectangle = 0.0f;


//global variables required fir FBConfig
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display *,GLXFBConfig ,GLXContext ,Bool ,const int *);
glXCreateContextAttribsARBProc glXCreateContextAttribsARB = NULL;
GLXFBConfig gGLXFBConfig;
static GLXContext gGLXContext;//OPenGL Renderer. 



GLuint vao_triangle;
GLuint vao_rectangle;
GLuint vbo_position_triangle;
GLuint vbo_position_rectangle;
GLuint vbo_color_rectangle;
GLuint vbo_color_triangle;
GLuint mvpUniform;
mat4 perspectiveProjectionMatrix;

GLuint gShaderProgramObject;

/////////// NEW VARIABLES ///////
//Hand Variables

// //Variables to Turn the body
int Turn = 0;

GLfloat turnAngleRightHand = 0;
GLfloat turnAngleLeftHand = 0;

//Sphere
int Spherevertices = 0;

GLfloat shoulder = -90;
GLfloat elbow = -30;


GLfloat leftLegUpper = -90.0f;
GLfloat leftLegLower = 30.0f;
GLfloat  RightLegUpper = -90.0f;
GLfloat  RightLegLower = 30.0f;


GLfloat LeftHandShoulder = -90.0f;
GLfloat LeftHandElbow = -0.0f;


GLfloat RightHandShoulder = -90.0f;
GLfloat RightHandElbow = 0.0f;



///////// COUNTS for Leg ////////
int RightLeg = 1;



GLfloat walkingTranslate = -8.0f;

int Walking = 0;



////////SITTING ////
int Sitting = 0;
int SittingUP = 0;
GLfloat UpperChestSitting = 0.0f;
GLfloat FaceSitting = 2.0f;

GLfloat LeftHandShoulderSitting = 1.0f;
GLfloat RightHandShoulderSitting = 1.0f;


//////////////////////////////////
/////////////Skipping

int Skipping = 0;
GLfloat UpperChestSkippingY = 0.0f;
GLfloat FaceSkippingY = 2.0f;
GLfloat LegSkippingY = -1.6f;
GLfloat HandSkippingY = 1.0f;

int Jump = 1;
//// Swimming////
int swimming = 0;
int smimmingLegsToggle = 0;


//Running
int Running = 0;



//Function Prototypes
void initialize(void);
void resize(int ,int );
void display(void);
void uninitialize(void);
	
void PerVertexLighting();
void PerFragmentLighting();
void UpdateWalking();
void UpdateSitting();
void UpdateSkipping();
void UpdateRunning();
void UpdateSwimming();
int Sphere(GLfloat, int, GLfloat **, GLfloat **, GLfloat **);
//variables


bool bDone = false;
//Entry Point Function 

int main(void)
{

	//function prototypes

	void CreateWindow(void);
	void ToggleFullscreen(void);
	void uninitialize();

	//variable declarations
	int winWidth = giWindowWidth;
	int winHeight = giWindowHeight;
	
	//code
	CreateWindow();
	initialize();
	//MessageLoop 

	XEvent event;
	KeySym keysym;
	
	while(bDone == false)
	{
		while(XPending(gpDisplay))
		{
			XNextEvent(gpDisplay, &event);		//Get next message. Fill 'event' with the address of next event.
			switch(event.type)			//Switch on event type
			{
				case MapNotify:
					break;
				case KeyPress:
					keysym = XkbKeycodeToKeysym(gpDisplay, event.xkey.keycode, 0, 0);
					switch(keysym)
					{
						case XK_Escape:			//Exit on Esc key press
							bDone = true;
						default:
							break;
					}
					XLookupString(&event.xkey, keys, sizeof(keys), NULL, NULL);
					switch(keys[0])
					{
						
		case 'L':
		case 'l':
			if (gbLighting == 0)
			{
				gbLighting = 1;
			}
			else
			{
				gbLighting = 0;
			}


			break;
		case 'V':
		case 'v':
			perFragmentLighting = false;
			break;
		case 'F':
		case 'f':
			perFragmentLighting = true;
			break;
		case 'E':
		case 'e':
		if(bFullscreen == false)
							{
								ToggleFullscreen();
								bFullscreen = true;
							}
							else
							{
								ToggleFullscreen();
								bFullscreen = false;
							}
		break;
		case 'R':
		case 'r':
			Sitting = 0;
			Skipping = 0;
			swimming = 0;
			Walking = 0;
			Turn = 0;
			if (Running == 0)
			{

				RightLeg = 1;
				walkingTranslate = -8.0f;
				leftLegUpper = -90.0f;
				leftLegLower = 30.0f;
				RightLegUpper = -90.0f;
				RightLegLower = 30.0f;


				LeftHandShoulder = -90.0f;
				LeftHandElbow = -30.0f;


				RightHandShoulder = -90.0f;
				RightHandElbow = -30.0f;
				Running  = 1;
			}
			else
				Running = 0;

			break;
		case 'M':
		case 'm':
			if (swimming == 0)
			{
				swimming = 1;
				turnAngleLeftHand = 94.044;
				leftLegLower = 30.0f;
				RightLegLower = 30.0f;
				//LeftHandElbow = 30.0f;
				//RightHandElbow = -30.0f;
			}
			else
			swimming = 0;

			break;
		case 'W':
		case 'w':
			Sitting = 0;
			Skipping = 0;
			swimming = 0;
			Running = 0;
			Turn = 0;
			if (Walking == 0)
			{
				RightLeg = 1;
				walkingTranslate = -8.0f;
				leftLegUpper = -90.0f;
				 leftLegLower = 30.0f;
				  RightLegUpper = -90.0f;
				  RightLegLower = 30.0f;


				 LeftHandShoulder = -90.0f;
				 LeftHandElbow = -30.0f;


				 RightHandShoulder = -90.0f;
				 RightHandElbow = -30.0f;
				Walking = 1;
			}
			else
				Walking = 0;
			break;
		case 'S':
		case 's':
			//Sitting = 0;
			Skipping = 0;
			swimming = 0;
			Running = 0;
			Turn = 0;
			Walking = 0;
			if (Sitting == 0)
			{
				/* UpperChestSitting = 0.3f;
				 FaceSitting = 2.0f;

				 LeftHandShoulderSitting = 1.0f;
				 RightHandShoulderSitting = 1.0f;




				 leftLegUpper = -90.0f;
				 leftLegLower = 30.0f;
				  RightLegUpper = -90.0f;
				  RightLegLower = 30.0f;


				 LeftHandShoulder = -90.0f;
				 LeftHandElbow = -30.0f;


				 RightHandShoulder = -90.0f;
				 RightHandElbow = -30.0f;*/

				 leftLegUpper = -90.0f;
				 leftLegLower = -90.0f;
				  RightLegUpper = -90.0f;
				  RightLegLower = 30.0f;


				 LeftHandShoulder = -90.0f;
				 LeftHandElbow = -0.0f;


				 RightHandShoulder = -90.0f;
				 RightHandElbow = 0.0f;


				walkingTranslate = -8.0f;
				Sitting = 1;
			}
			else if(Sitting == 1)
			{
			
				Sitting = 2;
			}
			else if (Sitting == 2)
			{
				Sitting = 1;
			}
			/*shoulder = (int)(shoulder + 3) % 360;
			leftLegUpper = (int)(leftLegUpper + 3) % 360;
			leftLegLower = (int)(leftLegLower + 1.5) % 360;


			RightLegUpper = (int)(RightLegUpper - 3) % 360;
			RightLegLower = (int)(RightLegLower - 1.5) % 360;*/

			break;
		case 'K':
		case 'k':
			Running = 0;
			Sitting = 0;
			Walking = 0;
			swimming = 0;
			Turn = 0;
			walkingTranslate = -8.0f;
			if (Skipping == 0)
			{
				LeftHandShoulder = -110.0f;
				RightLegUpper = -90.0F;
				RightLegLower = 30.0;
				leftLegUpper = -90.0F;
				leftLegLower = 30.0f;
				LeftHandElbow = -30.0f;

				LeftHandElbow = 150.0f;
				RightHandElbow = -150.0f;
				Skipping = 1;

			}
			else
				Skipping = 0;

			break;
		case 'A':
			
			RightHandShoulder = RightHandShoulder + 0.4f;
			RightHandElbow = RightHandElbow + 0.5f;

		

			break;
		case 'a':

			RightHandShoulder = RightHandShoulder- 0.85f;
			LeftHandShoulder = LeftHandShoulder + 0.85f;

			RightLegUpper = RightLegUpper - 0.95f;
			leftLegUpper += 0.95f;
			/*RightHandShoulder = RightHandShoulder - 0.4f;
				RightHandElbow = RightHandElbow - 0.5f;
			
				fprintf(gpLogFile, "The value of Right Hand Shoulder is %f\n", RightHandShoulder);
				fprintf(gpLogFile, "The value of Right Hand Elbow is %f\n", RightHandElbow);*/
			//leftLegUpper = leftLegUpper - 0.35f;

		//	fprintf(gpLogFile, "The value of leftLegUpper is %f\n", leftLegUpper);
			//LeftHandShoulder = (int)(LeftHandShoulder - 1.0) % 360;
			break;
		case 'Q':
		case 'q':
			if (Turn == 0)
				Turn = 1;
			else
				Turn = 0;

			break;
		case 'P':
		case 'p':
			turnAngleLeftHand = turnAngleLeftHand + 0.95f;
		//	fprintf(gpLogFile, "LeftHand is %f\n", turnAngleLeftHand);
			turnAngleRightHand = turnAngleRightHand + 0.95f;
		
			//fprintf(gpLogFile, "RightHand is %f\n", turnAngleRightHand);
			break;
						default:
							break;
					}		
					break;
				case ButtonPress:
					switch(event.xbutton.button)
					{
						case 1:			//Left mouse button
							break;
						case 2:			//Middle mouse button
							break;	
						case 3:			//Right mouse button
							break;
						default:
							break;
					}
					break;
					
				case MotionNotify:			//Mouse move
					break;
				case ConfigureNotify:
					winWidth = event.xconfigure.width;
					winHeight = event.xconfigure.height;
					resize(winWidth, winHeight);
					break;
				case Expose:				//Painting of window
					break;
				case DestroyNotify:			
					break;
				case 33:				//Close press
					bDone = true;
					break;
				default:
					break;
			}
		}
		
		display();
		//Spin call here
}
	uninitialize();
	return(0);
}

void CreateWindow(void)
{
	//function prototypes 
	//void uninitialize(void);
	//variable declarations
	
	XSetWindowAttributes winAttribs;	
	int defaultScreen;
	int defaultDepth;
	int styleMask;
	
        //variables for FBCOnfig
	GLXFBConfig *pGLXFBConfigs = NULL; //This will hold all our fb configs
	GLXFBConfig bestGlXFBConfig;
	XVisualInfo *pTempXVisualInfo = NULL;
	int iNumOfFBConfigs = 0;

//STEP 1:- Initialize frameBufferAttributes[]
	static int frameBufferAttributes[] = 
	{
		GLX_X_RENDERABLE, True,			//We want video rendering and not image rendering
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,	//Window is drawable
		GLX_RENDER_TYPE, GLX_RGBA_BIT,		//Frame is of RGBA type
		GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,	//Equivalent to giving TrueColor in XMatchVisualInfo
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		GLX_DEPTH_SIZE, 24,
		GLX_STENCIL_SIZE, 8,
		GLX_DOUBLEBUFFER, True,
		None			//As a stopper for array
       };	

	//code
	gpDisplay = XOpenDisplay(NULL);
	if(gpDisplay == NULL)
	{
		printf("ERROR:Unable to Open X Display . \nExitting Now ...\n");
		uninitialize();
		exit(1);
	}
	defaultScreen = XDefaultScreen(gpDisplay);
	defaultDepth = DefaultDepth(gpDisplay,defaultScreen);
        //STEP 2:- Retrieve all the FBConfigs from the driver
	pGLXFBConfigs = glXChooseFBConfig(gpDisplay,defaultScreen,frameBufferAttributes,&iNumOfFBConfigs);
	if(pGLXFBConfigs == NULL)
	{
		printf("Failed to get valid FrameBuffer Config.\nExiting now...\n");
		uninitialize();
		exit(1);
	}
	 //As we are not asking the system to choose or match a visual, we get multiple visuals to choose from	
		printf("%d Matching FB Configs found.\n", iNumOfFBConfigs);
	
	//STEP 3:- Pick that config which has most number if samples
	int bestFrameBufferConfig = -1;
	int bestNoOfSamples = -1;
	int worstFrameBufferConfig = -1;
	int worstNumberOfSamples = 999;
	
	for(int i = 0;i<iNumOfFBConfigs;i++)
	{
		//for each obtained fbconfig, get temporary visual info(itterate each) and we can know each's properties(strength)
		pTempXVisualInfo = glXGetVisualFromFBConfig(gpDisplay,pGLXFBConfigs[i]);
		if(pTempXVisualInfo)
		{
			//get number of sample buffers from respective FBConfigs
			int sampleBuffers;
			int samples;
			glXGetFBConfigAttrib(gpDisplay,pGLXFBConfigs[i],GLX_SAMPLE_BUFFERS,&sampleBuffers);
			//STEP 4:- get no of samples from each
			glXGetFBConfigAttrib(gpDisplay,pGLXFBConfigs[i],GLX_SAMPLES,&samples);
			printf("Matching Framebuffer Config = %d : Visual ID = 0x%lu : Sample_BUFFERS = %d : SAMPLES = %d\n", i,pTempXVisualInfo->visualid, sampleBuffers, samples);

			//STEP 5:- More the no of samples and sampleBuffers more Powerful eligible FBConfig is
			//So,do the comparison
			if(bestFrameBufferConfig < 0 || sampleBuffers && samples > bestNoOfSamples)
			{
				bestFrameBufferConfig = i;
				bestNoOfSamples = samples;
			}
				if(worstFrameBufferConfig < 0 || !sampleBuffers || samples < worstNumberOfSamples)
			{
				worstFrameBufferConfig = i;
				worstNumberOfSamples = samples;
			}
		}
		XFree(pTempXVisualInfo);
	}
		
		//Step 6:-	
		//Now assign the found best one
		bestGlXFBConfig = pGLXFBConfigs[bestFrameBufferConfig];
		
		//assign the best one to global one
		gGLXFBConfig = bestGlXFBConfig;
		
		//FREE the obtained 
				
		XFree(pGLXFBConfigs);
		
		//get the best visual
		gpXVisualInfo = glXGetVisualFromFBConfig(gpDisplay, bestGlXFBConfig);





		


	winAttribs.border_pixel = 0;
	winAttribs.background_pixmap = 0;
	winAttribs.colormap = XCreateColormap(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),gpXVisualInfo->visual,AllocNone);
	gColormap = winAttribs.colormap;

	winAttribs.background_pixel = BlackPixel(gpDisplay,defaultScreen);
	winAttribs.event_mask = ExposureMask |  VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;
	styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
	gWindow = XCreateWindow(gpDisplay,
				RootWindow(gpDisplay,gpXVisualInfo->screen),
				0,
				0,
				giWindowWidth,
				giWindowHeight,
				0,
				gpXVisualInfo->depth,
				InputOutput,
				gpXVisualInfo->visual,
				styleMask,
				&winAttribs);
	if(!gWindow)
	{
		printf("Error : Failed to create main window.\nExitting now \n");
		uninitialize();
		exit(1);
	}
	XStoreName(gpDisplay,gWindow,"Aditya's First XWindow");

	Atom windowManagerDelete = XInternAtom(gpDisplay,"WM_DELETE_WINDOW",True);
	XSetWMProtocols(gpDisplay,gWindow,&windowManagerDelete,1);

	XMapWindow(gpDisplay,gWindow);
}
void initialize(void)
{
	
	GLuint gVertexShaderObject;
	GLuint gFragmentShaderObject;
	GLuint gVertexShaderObjectPerFragment;
	GLuint gFragmentShaderObjectPerFragment;


	GLenum result;
	//Function protoypes:
	void resize(int, int);
	void uninitialize(void);
	
	//Code:
	//6. Create a new GL context 4.5 for rendering:
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((GLubyte*)"glXCreateContextAttribsARB");
	GLint attribs[] = 
	{
		GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
		GLX_CONTEXT_MINOR_VERSION_ARB, 5,
		GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};
	gGLXContext = glXCreateContextAttribsARB(gpDisplay, gGLXFBConfig, 0, True, attribs);
	if(!gGLXContext)	//On failure, fall back to old style 2.x context
	{
		//When a context version below 3.0 is requested, implementations will return the newest context version
		//compatible with openGL versions less than version 3.0
		GLint attribs[] = 
		{
			GLX_CONTEXT_MAJOR_VERSION_ARB, 1,
			GLX_CONTEXT_MINOR_VERSION_ARB, 0,
			0
		};
		printf("Failed to create GLX 4.5 context. Hence, using old style GLX Context. \n");
		gGLXContext = glXCreateContextAttribsARB(gpDisplay, gGLXFBConfig, 0, True, attribs);
	}
	else	//Successfully created 4.x context
	{
		printf("OpenGL Context 4.5 is created.\n");
	}
	
	//Verifying that context is a direct context
	if(!glXIsDirect(gpDisplay, gGLXContext))
	{
		printf("Indirect GLX rendering Context Obtained\n");
	}
	else
	{
		printf("Direct GLX Rendering Context Obtained\n");
	}
	glXMakeCurrent(gpDisplay, gWindow, gGLXContext);

	result = glewInit();
	if(result!= GLEW_OK)
	{
		uninitialize();
		bDone = true;
	}

	//////////////////////////////// V E R T E X - S H A D E R //////////////////////////


	//Define Vertex Shader Object
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar **p;
	const GLchar *vertexShaderSourceCode =
		
		"#version 430 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform int u_LKeyPressed;" \
		"uniform vec3 u_LDUniform;" \
		"uniform vec3 u_LAUniform;" \
		"uniform vec3 u_LSUniform;" \
		"uniform vec3 u_KDUniform;" \
		"uniform vec3 u_KAUniform;" \
		"uniform vec3 u_KSUniform;" \
		"uniform float u_MaterialShininess;" \
		"uniform vec4 u_LightPosition;" \
		"out vec3  phongADSLight;" \
		"void main(void)" \
		"{" \
		"if(u_LKeyPressed == 1)" \
		"{" \
		"vec4 eye_coordinates = u_viewMatrix * u_modelMatrix *  vPosition;" \
		"vec3 tNorm = normalize(mat3(u_viewMatrix * u_modelMatrix) * vNormal);" \
		"vec3 light_direction = normalize(vec3(u_LightPosition - eye_coordinates));" \
		"float tNormal_dot_lightDirection = max(dot(light_direction,tNorm),0.0);" \
		"vec3 reflectionVector = reflect(-light_direction,tNorm);" \
		"vec3 viewVector = normalize(vec3(-eye_coordinates.xyz));" \
		"vec3 ambient = u_LAUniform * u_KAUniform;" \
		"vec3 diffuse = u_LDUniform * u_KDUniform * tNormal_dot_lightDirection;" \
		"vec3 specular = u_LSUniform * u_KSUniform * pow(max(dot(reflectionVector,viewVector),0.0),u_MaterialShininess);" \
		"phongADSLight = ambient + diffuse + specular;" \
		"}" \
		"else" \
		"{" \
		"phongADSLight = vec3(1.0,1.0,1.0);" \
		"}" \
		"gl_Position = u_projectionMatrix * u_viewMatrix *u_modelMatrix * vPosition;" \
		"}";

	// GPU will run the above code. And GPU WILL RUN FOR PER VERTEX. of there are 1000 vertex. Then GPU will run this shader for
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
				//fprintf(gpLogFile, "%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				
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
		"in vec3 phongADSLight;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = vec4(phongADSLight ,1.0);" \
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
				//fprintf(gpLogFile, "%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				
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
	glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_NORMAL, "vNormal");
	//Here the above line means that we are linking the GPU's variable vPosition with the CPU's  enum member  i.e AMC_ATTRIBUTE_POSITION .
	//So whatever changes will be done in AMC_ATTRIBUTE_POSITION , those will also reflect in vPosition
	glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_COLOR, "vColor");
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
				//fprintf(gpLogFile, "%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				
				exit(0);


			}
		}
	}


	//POST Linking
	//Retrieving uniform locations 
	modelUniform = glGetUniformLocation(gShaderProgramObject, "u_modelMatrix");
	ViewUniform = glGetUniformLocation(gShaderProgramObject, "u_viewMatrix");
	projectionUniform = glGetUniformLocation(gShaderProgramObject, "u_projectionMatrix");

	LKeyPressed = glGetUniformLocation(gShaderProgramObject, "u_LKeyPressed");
	LDUniform = glGetUniformLocation(gShaderProgramObject, "u_LDUniform");
	LAUniform = glGetUniformLocation(gShaderProgramObject, "u_LAUniform");
	LSUniform = glGetUniformLocation(gShaderProgramObject, "u_LSUniform");

	KDUniform = glGetUniformLocation(gShaderProgramObject, "u_KDUniform");
	KAUniform = glGetUniformLocation(gShaderProgramObject, "u_KAUniform");
	KSUniform = glGetUniformLocation(gShaderProgramObject, "u_KSUniform");
	LightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_LightPosition");
	MaterialShininessUniform = glGetUniformLocation(gShaderProgramObject, "u_MaterialShininess");
	//Here we have done all the preparations of data transfer from CPU to GPU
	

	//////////////// PER - FRAGMENT ////////////////////

	//////////////////////////////// V E R T E X - S H A D E R //////////////////////////


	//Define Vertex Shader Object
	gVertexShaderObjectPerFragment = glCreateShader(GL_VERTEX_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	
	const GLchar *vertexShaderSourceCodePerFragment =

		"#version 430 core" \
		"\n"
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform int u_LKeyPressed;" \
		"out vec3 tNorm;" \
		"out vec3 light_direction;" \
		"out vec3 viewVector;" \
		"uniform vec4 u_LightPosition;" \

		"void main(void)" \
		"{" \
		"if(u_LKeyPressed == 1)" \
		"{" \
		"vec4 eye_coordinates = u_viewMatrix * u_modelMatrix *  vPosition;" \
		"tNorm = mat3(u_viewMatrix * u_modelMatrix) * vNormal;" \
		"light_direction = vec3(u_LightPosition - eye_coordinates);" \
		"viewVector = vec3(-eye_coordinates.xyz);" \
		"}" \
		"gl_Position = u_projectionMatrix * u_viewMatrix *u_modelMatrix * vPosition;" \
		"}";

	// GPU will run the above code. And GPU WILL RUN FOR PER VERTEX. If there are 1000 vertex. Then GPU will run this shader for
	//1000 times. We are Multiplying each vertex with the Model View Matrix.
	//And how does the GPU gets to know about at what offset the array has to be taken . Go to glVertexAttribPointer() in Display.
	// in = Input. 

	//p = &vertexShaderSourceCode;
		//Specify above source code to the vertex shader object
	glShaderSource(gVertexShaderObjectPerFragment, 1, (const GLchar **)&vertexShaderSourceCodePerFragment, NULL);

	//Compile the vertex shader 
	glCompileShader(gVertexShaderObjectPerFragment);

	//////////////// Error Checking//////////////////
	//Code for catching the errors 
	GLint iShaderCompileStatusPerFragment = 0;
	GLint iInfoLogLengthPerFragment = 0;
	GLchar *szInfoLogPerFragment = NULL;


	glGetShaderiv(gVertexShaderObjectPerFragment, GL_COMPILE_STATUS, &iShaderCompileStatusPerFragment);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObjectPerFragment, GL_INFO_LOG_LENGTH, &iInfoLogLengthPerFragment);
		if (iInfoLogLengthPerFragment > 0)
		{
			szInfoLogPerFragment = (GLchar *)malloc(iInfoLogLengthPerFragment);
			if (szInfoLogPerFragment != NULL)
			{
				GLsizei writtenPerFragment;
				glGetShaderInfoLog(gVertexShaderObjectPerFragment, iInfoLogLengthPerFragment, &writtenPerFragment, szInfoLogPerFragment);
				//fprintf(gpLogFile, "VertexShader :\n");
				//fprintf(gpLogFile, "%s\n", szInfoLogPerFragment);
				free(szInfoLogPerFragment);
				uninitialize();
				//DestroyWindow(hwnd);
				exit(0);


			}
		}
	}
	/////////////////    F R A G M E N T S H A D E R            //////////////////////////
	//Define Vertex Shader Object
	gFragmentShaderObjectPerFragment = glCreateShader(GL_FRAGMENT_SHADER); //This command will create the Shader Object
	//Now Write vertex shader code
	const GLchar *fragmentShaderSourceCodePerFragment =
		"#version 430 core" \
		"\n" \
		"in vec3 tNorm;" \
		"in vec3 light_direction;" \
		"in vec3 viewVector;" \

		"vec3 phongADSLight;" \
		"uniform vec3 u_LDUniform;" \
		"uniform vec3 u_LAUniform;" \
		"uniform vec3 u_LSUniform;" \
		"uniform vec3 u_KDUniform;" \
		"uniform vec3 u_KAUniform;" \
		"uniform vec3 u_KSUniform;" \
		"uniform float u_MaterialShininess;" \
		"out vec4 FragColor;" \
		"uniform int u_LKeyPressed;" \
		"void main(void)" \
		"{" \
		"if(u_LKeyPressed == 1)" \
		"{" \
		"vec3 normalizedTNorm = normalize(tNorm);" \
		"vec3 normalizedLightDirection = normalize(light_direction);" \
		"vec3 normalizedViewVector = normalize(viewVector);" \
		"float tNormal_dot_lightDirection = max(dot(normalizedLightDirection,normalizedTNorm),0.0);" \
		"vec3 reflectionVector = reflect(-normalizedLightDirection,normalizedTNorm);" \

		"vec3 ambient = u_LAUniform * u_KAUniform;" \
		"vec3 diffuse = u_LDUniform * u_KDUniform * tNormal_dot_lightDirection;" \
		"vec3 specular = u_LSUniform * u_KSUniform * pow(max(dot(reflectionVector,normalizedViewVector),0.0),u_MaterialShininess);" \
		"phongADSLight = ambient + diffuse + specular;" \
		"}" \
		"else" \
		"{" \
		"phongADSLight = vec3(1.0,1.0,1.0);" \
		"}" \
		"FragColor = vec4(phongADSLight ,1.0);" \
		"}";

	//FragColor = vec4(1,1,1,1) = White Color
	//this means here we are giving color to the Triangle.




	//Specify above source code to the vertex shader object
	glShaderSource(gFragmentShaderObjectPerFragment, 1, (const GLchar **)&fragmentShaderSourceCodePerFragment, NULL);

	//Compile the vertex shader 
	glCompileShader(gFragmentShaderObjectPerFragment);
	//Code for catching the errors 
		   /*iShaderCompileStatus = 0;
		   iInfoLogLength = 0;*/
	szInfoLogPerFragment = NULL;


	glGetShaderiv(gFragmentShaderObjectPerFragment, GL_COMPILE_STATUS, &iShaderCompileStatusPerFragment);
	if (iShaderCompileStatusPerFragment == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObjectPerFragment, GL_INFO_LOG_LENGTH, &iInfoLogLengthPerFragment);
		if (iInfoLogLengthPerFragment > 0)
		{
			szInfoLogPerFragment = (GLchar *)malloc(iInfoLogLengthPerFragment);
			if (szInfoLogPerFragment != NULL)
			{
				GLsizei written1PerFragment;
				glGetShaderInfoLog(gFragmentShaderObjectPerFragment, iInfoLogLengthPerFragment, &written1PerFragment, szInfoLogPerFragment);
				//fprintf(gpLogFile, "%sFragment Shader : \n", szInfoLogPerFragment);
				free(szInfoLogPerFragment);
				uninitialize();
				//DestroyWindow(hwnd);
				exit(0);


			}
		}
	}
	// CREATE SHADER PROGRAM OBJECT
	gShaderProgramObjectPerFragment = glCreateProgram();
	//attach vertex shader to the gShaderProgramObject
	glAttachShader(gShaderProgramObjectPerFragment, gVertexShaderObjectPerFragment);

	//attach fragment shader to the gShaderProgramObject
	glAttachShader(gShaderProgramObjectPerFragment, gFragmentShaderObjectPerFragment);


	//Pre-Linking  binding to vertexAttributes
	glBindAttribLocation(gShaderProgramObjectPerFragment, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObjectPerFragment, AMC_ATTRIBUTE_NORMAL, "vNormal");
	//Here the above line means that we are linking the GPU's variable vPosition with the CPU's  enum member  i.e AMC_ATTRIBUTE_POSITION .
	//So whatever changes will be done in AMC_ATTRIBUTE_POSITION , those will also reflect in vPosition

	//RULE : ALWAYS BIND THE ATTRIBUTES BEFORE LINKING AND BIND THE UNIFORM AFTER LINKING.

	//Link the shader program 
	glLinkProgram(gShaderProgramObjectPerFragment);

	//Code for catching the errors 
	GLint iProgramLinkStatusPerFragment = 0;



	glGetProgramiv(gShaderProgramObjectPerFragment, GL_LINK_STATUS, &iProgramLinkStatusPerFragment);
	if (iProgramLinkStatusPerFragment == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObjectPerFragment, GL_INFO_LOG_LENGTH, &iInfoLogLengthPerFragment);
		if (iInfoLogLength > 0)
		{
			szInfoLogPerFragment = (GLchar *)malloc(iInfoLogLengthPerFragment);
			if (szInfoLogPerFragment != NULL)
			{
				GLsizei written3PerFragment;
				glGetProgramInfoLog(gShaderProgramObjectPerFragment, iInfoLogLengthPerFragment, &written3PerFragment, szInfoLogPerFragment);
				//fprintf(gpLogFile, "%s\n", szInfoLogPerFragment);
				free(szInfoLogPerFragment);
				uninitialize();
				//DestroyWindow(hwnd);
				exit(0);


			}
		}
	}


	//POST Linking
	//Retrieving uniform locations 
	modelUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_modelMatrix");
	ViewUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_viewMatrix");
	projectionUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_projectionMatrix");

	LKeyPressedPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_LKeyPressed");
	LDUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_LDUniform");
	LAUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_LAUniform");
	LSUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_LSUniform");

	KDUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_KDUniform");
	KAUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_KAUniform");
	KSUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_KSUniform");
	LightPositionUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_LightPosition");
	MaterialShininessUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_MaterialShininess");




















GLfloat *SphereVertices = NULL;
	GLfloat *SphereTextures = NULL;
	GLfloat *SphereNormals = NULL;
	//Basically we are giving the vertices coordinates in the above array
	Spherevertices = Sphere(0.6f, 400, &SphereVertices, &SphereTextures, &SphereNormals);



	// getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	// gNumVertices = getNumberOfSphereVertices();
	// gNumElements = getNumberOfSphereElements();


		//Basically we are giving the vertices coordinates in the above array
	

	//////////////////////// FOR SPHERE//////////////// 

	glGenVertexArrays(1, &vao_Sphere);
	glBindVertexArray(vao_Sphere);
	glGenBuffers(1, &vbo_position_Sphere);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_Sphere);
	glBufferData(GL_ARRAY_BUFFER,  3 * Spherevertices * sizeof(GLfloat), SphereVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	//NORMALS 
	glGenBuffers(1, &vbo_Normals_Sphere);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Normals_Sphere);
	glBufferData(GL_ARRAY_BUFFER,  3 * Spherevertices * sizeof(GLfloat), SphereNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);



	// //Elements
	// glGenBuffers(1, &vbo_Element_Sphere);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);


	// glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

///////////////////////// PER FRAGMENT /////////////////////////////////////////////
glGenVertexArrays(1, &vao_SpherePerFragment);
	glBindVertexArray(vao_SpherePerFragment);
	glGenBuffers(1, &vbo_position_SpherePerFragment);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_SpherePerFragment);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	//NORMALS 
	glGenBuffers(1, &vbo_Normals_SpherePerFragment);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Normals_SpherePerFragment);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);



	//Elements
	glGenBuffers(1, &vbo_Element_SpherePerFragment);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_SpherePerFragment);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);



	make_cylinder(0.4, 8.2f, 30, 30);


	
	glClearDepth(1.0f);		//clear depth buffer
	glEnable(GL_DEPTH_TEST);	//enable the depth
	glDepthFunc(GL_LEQUAL);		//less than or equal to 1.0f in far. Uses ray tracing algorithm

	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	// Blue background

	// Set orthographicMatrix to identity matrix
	perspectiveProjectionMatrix = mat4::identity();
		
	
	resize(giWindowWidth, giWindowHeight);		//Stray resize
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
	//fprintf(gpLogFile, "Value of sitting is %d\n", Sitting);
//	fprintf(gpLogFile, "Vlue if walking is %d\n", Walking);

	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 RotationMatrix;
	mat4 TranslateMatrix;
	mat4 ScaleMatrix;
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	RotationMatrix = mat4::identity();
	TranslateMatrix = mat4::identity();
	ScaleMatrix = mat4::identity();


	// use shader program
	if (perFragmentLighting == true)
	{
	
		PerFragmentLighting();
	}
	else
	{
		PerVertexLighting();
	}
	
	
	
		TranslateMatrix = translate(0.0f, 2.0f, walkingTranslate);
		TranslateMatrix *= rotate(0.0f, turnAngleLeftHand, 0.0f);
		modelMatrix = TranslateMatrix;
		push(modelMatrix);

		modelMatrix *= translate(0.0f, 0.2f, 0.0f);

		if (Sitting == 1 || Sitting == 2)
		{
			TranslateMatrix *= translate(0.0f, UpperChestSitting, 0.0f);
			modelMatrix = TranslateMatrix;
		}
		/*else if (Sitting == 2)
		{
			TranslateMatrix *= translate(0.0f, UpperChestSitting, 0.0f);
			modelMatrix = TranslateMatrix;
		}*/
		else if (Skipping == 1)
		{
			TranslateMatrix *= translate(0.0f, UpperChestSkippingY, 0.0f);
			modelMatrix = TranslateMatrix;
			push(modelMatrix);
		}
		else if (swimming == 1)
		{
			TranslateMatrix = translate(-1.0f, 1.0f, walkingTranslate);
			TranslateMatrix *= rotate(turnAngleLeftHand, turnAngleLeftHand, 0.0f);
			modelMatrix = TranslateMatrix;
			push(modelMatrix);
		}
	
	if (perFragmentLighting == true)
	{

		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);
	}
	else
	{
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	}
	glBindVertexArray(vao_Sphere);
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3 * Spherevertices);
	glBindVertexArray(0);

	/////////////////////////////// UPPER BODY ---- CHEST /////////////////////


	if (Sitting == 1)
	{
		TranslateMatrix = peek();
		TranslateMatrix *= translate(0.0f, -1.6f, 0.0f);
		push(TranslateMatrix);
		TranslateMatrix *= translate(0.0f, UpperChestSitting, 0.0f);
		if (UpperChestSitting > -0.2f)
			UpperChestSitting -= 0.002f;
		ScaleMatrix = scale(1.3f, 2.2f, 1.1f);
		//TranslateMatrix *= rotate(0.0f, turnAngle, 0.0f);
		TranslateMatrix = TranslateMatrix * ScaleMatrix;
		//	RotationMatrix = rotate(angleRectangle, 1.0f, 0.0f, 0.0f);
		RotationMatrix = rotate(angleRectangle, angleRectangle, angleRectangle);

		// perform necessary transformations
		modelMatrix = TranslateMatrix;
	}
	else if (Sitting == 2)
	{
		
			TranslateMatrix = peek();
			TranslateMatrix *= translate(0.0f, -1.6f, 0.0f);
			push(TranslateMatrix);
			TranslateMatrix *= translate(0.0f, UpperChestSitting, 0.0f);
			if (UpperChestSitting < 0.0f)
				UpperChestSitting += 0.002f;
			ScaleMatrix = scale(1.3f, 2.2f, 1.1f);
			//TranslateMatrix *= rotate(0.0f, turnAngle, 0.0f);
			TranslateMatrix = TranslateMatrix * ScaleMatrix;
			//	RotationMatrix = rotate(angleRectangle, 1.0f, 0.0f, 0.0f);
			RotationMatrix = rotate(angleRectangle, angleRectangle, angleRectangle);

			// perform necessary transformations
			modelMatrix = TranslateMatrix;
		
	}
	else
	{
		TranslateMatrix = peek();
		TranslateMatrix *= translate(0.0f, -1.6f, 0.0f);
		push(TranslateMatrix);
		if (Skipping == 1)
		{
			TranslateMatrix *= translate(0.0f, UpperChestSkippingY, 0.0f);
			push(TranslateMatrix);
		}
		ScaleMatrix = scale(1.3f, 2.2f, 1.1f);
		//TranslateMatrix *= rotate(0.0f, turnAngle, 0.0f);
		TranslateMatrix = TranslateMatrix * ScaleMatrix;
		//	RotationMatrix = rotate(angleRectangle, 1.0f, 0.0f, 0.0f);
		RotationMatrix = rotate(angleRectangle, angleRectangle, angleRectangle);

		// perform necessary transformations
		modelMatrix = TranslateMatrix;
	}

	if (perFragmentLighting == true)
	{

		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);
	}
	else
	{
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	}
	glBindVertexArray(vao_Sphere);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3 * Spherevertices);
	glBindVertexArray(0);


//	/////////////////////////////////////////////////////////////////
//
//	//////// Right HAND /////////////// 

	RotationMatrix = rotate((GLfloat)RightHandShoulder, 1.0f, 0.0f, 0.0f);
	ScaleMatrix = scale(0.3f, 0.3f, 0.09f);

	if (swimming == 1)
	{
		RotationMatrix = rotate((GLfloat)RightHandShoulder, 1.0f, 0.0f, 0.0f);
		modelMatrix = peek();
		modelMatrix *= translate(0.83f, 0.85f, 0.0f);
		modelMatrix = modelMatrix * RotationMatrix;
		modelMatrix *= translate(0.0f, 0.0f, -1.0f);
		push(modelMatrix);
		modelMatrix *= ScaleMatrix;
		push(modelMatrix);
	}
	else
	{
		RotationMatrix = rotate((GLfloat)RightHandShoulder, 1.0f, 0.0f, 0.0f);
		modelMatrix = peek();
		modelMatrix *= translate(0.83f, 0.85f, 0.0f);
		modelMatrix = modelMatrix * RotationMatrix;
		modelMatrix *= translate(0.0f, 0.0f, -1.0f);
		push(modelMatrix);
		modelMatrix *= ScaleMatrix;
		push(modelMatrix);
	}

	if (perFragmentLighting == true)
	{

		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);
	}
	else
	{
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	}

	draw_cylinder();
//
	pop();
	if (Skipping == 1)
	{
		//pop();
		modelMatrix = peek();
		modelMatrix *= translate(0.0f, -0.1f, -0.1f);
		modelMatrix *= rotate(60.00f, (GLfloat)RightHandElbow, 260.0f);

		//modelMatrix *= rotate((GLfloat)LeftHandElbow, 0.0f, 1.0f, 0.0f);
		modelMatrix *= translate(0.0f, 0.0f, -0.7f);
		modelMatrix *= scale(0.3f, 0.3f, 0.09f);
	}
	//else if (Running == 1)
	//{
	//	modelMatrix = peek();
	//	modelMatrix *= translate(0.0f, 0.0f, -0.1f);
	//	modelMatrix *= rotate((GLfloat)RightHandElbow, 1.0f, 0.0f, 0.0f);
	//	modelMatrix *= translate(0.0f, 0.0f, -0.75f);
	//	modelMatrix *= scale(0.3f, 0.3f, 0.09f);
	//}
	//else if (swimming == 1)
	//{
	//	modelMatrix = peek();
	//	modelMatrix *= translate(0.0f, 0.0f, -0.1f);
	//	modelMatrix *= rotate((GLfloat)RightHandElbow, 1.0f, 0.0f, 0.0f);
	//	modelMatrix *= translate(0.0f, 0.0f, -0.75f);
	//	modelMatrix *= scale(0.3f, 0.3f, 0.09f);
	//}
	else
	{
		modelMatrix = peek();
		modelMatrix *= translate(0.0f, 0.0f, -0.1f);
		modelMatrix *= rotate((GLfloat)RightHandElbow, 1.0f, 0.0f, 0.0f);
		modelMatrix *= translate(0.0f, 0.0f, -0.75f);
		modelMatrix *= scale(0.3f, 0.3f, 0.09f);
	}
	if (perFragmentLighting == true)
	{

		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);
	}
	else
	{
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	}
	//
	draw_cylinder();
//
//	
//	////////////////////////////////// WRIST Right hand ///////////////////////
//	if (Skipping == 1)
//	{
//		//pop();
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, -0.1f, -0.1f);
//		modelMatrix *= rotate(60.00f, (GLfloat)RightHandElbow, 260.0f);
//
//		//modelMatrix *= rotate((GLfloat)LeftHandElbow, 0.0f, 1.0f, 0.0f);
//		modelMatrix *= translate(0.0f, 0.0f, -0.7f);
//		modelMatrix *= scale(0.32f, 0.32f, 0.32f);
//	}
//	else if (Running == 1)
//	{
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, 0.0f, -0.75f);
//		modelMatrix *= rotate((GLfloat)RightHandElbow, 1.0f, 0.0f, 0.0f);
//		modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	else if (Sitting == 1)
//	{
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, -0.5f, -0.75f);
//		modelMatrix *= rotate((GLfloat)RightHandElbow, 1.0f, 0.0f, 0.0f);
//		modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	else if (Sitting == 2)
//	{
//
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, -0.5f, -0.75f);
//		modelMatrix *= rotate((GLfloat)RightHandElbow, 1.0f, 0.0f, 0.0f);
//		modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//
//	}
//	else
//	{
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, 0.0f, -0.75f);
//		modelMatrix *= rotate((GLfloat)RightHandElbow, 1.0f, 0.0f, 0.0f);
//		modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	if (perFragmentLighting == true)
//	{
//
//		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//	else
//	{
//		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//	glBindVertexArray(vao_Sphere);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
//	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
//	glBindVertexArray(0);
//
//
//
//
//	//////////////////////////////
//	if (Skipping == 1)
//	{
//		RotationMatrix = rotate((GLfloat)LeftHandShoulder, 1.0f, 0.0f, 0.0f);
//		ScaleMatrix = scale(0.3f, 0.3f, 0.3f);
//		modelMatrix = translate(0.9f, HandSkippingY - 0.12f, walkingTranslate -0.2f );
//		modelMatrix = modelMatrix * RotationMatrix;
//		//modelMatrix *= rotate(0.0f, 0.0f, turnAngleLeftHand);
//		modelMatrix *= translate(0.0f, 0.0f, -1.0f);
//		push(modelMatrix);
//		modelMatrix *= ScaleMatrix;
//		push(modelMatrix);
//
//	}
//	else if (Running == 1)
//	{
//		pop();
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, 0.0f, -0.1f);
//		modelMatrix *= rotate((GLfloat)LeftHandElbow, 1.0f, 0.0f, 0.0f);
//		modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	else
//	{
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, 0.0f, -0.1f);
//		modelMatrix *= rotate((GLfloat)LeftHandElbow, 1.0f, 0.0f, 0.0f);
//		modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	if (perFragmentLighting == true)
//	{
//
//		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//	else
//	{
//		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//	glBindVertexArray(vao_Sphere);
//	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 0.5f, 0.35f, 0.005f);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
//	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
//	glBindVertexArray(0);
//
//	resetStack();
//	//resetStack();
//
//
//	//////////////////// LEFT HAND /////////////////////
//
//

pop();

	RotationMatrix = rotate((GLfloat)LeftHandShoulder, 1.0f, 0.0f, 0.0f);
	ScaleMatrix = scale(0.3f, 0.3f, 0.09f);

		RotationMatrix = rotate((GLfloat)LeftHandShoulder, 0.0f,0.0f);
		
		modelMatrix = peek();
		modelMatrix *= translate(-0.85f, 0.85f, 0.0f);
		modelMatrix = modelMatrix * RotationMatrix;
		modelMatrix *= translate(0.0f, 0.0f, -1.0f);
		push(modelMatrix);
		modelMatrix *= ScaleMatrix;
		push(modelMatrix);


	
	if (perFragmentLighting == true)
	{

		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);
	}
	else
	{
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	}

	draw_cylinder();
//
	pop();

		if (Skipping == 1)
	{
		//pop();
		modelMatrix = peek();
		modelMatrix *= translate(0.00f, -0.1f, -0.1f);
		modelMatrix *= rotate(60.00f, (GLfloat)LeftHandElbow, -260.0f);

		//modelMatrix *= rotate((GLfloat)LeftHandElbow, 0.0f, 1.0f, 0.0f);
		modelMatrix *= translate(0.0f, 0.0f, -0.7f);
		modelMatrix *= scale(0.3f, 0.3f, 0.09f);
	}
		else
		{

			modelMatrix = peek();
			modelMatrix *= translate(0.0f, 0.0f, -0.1f);
			modelMatrix *= rotate((GLfloat)LeftHandElbow, 1.0f, 0.0f, 0.0f);
			modelMatrix *= translate(0.0f, 0.0f, -0.7f);
			modelMatrix *= scale(0.3f, 0.3f, 0.09f);
		}

	if (perFragmentLighting == true)
	{

		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);
	}
	else
	{
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	}
	draw_cylinder();
//
//	//////////////////////////////////// WRIST left hand
//	 if (Skipping == 1)
//	{
//		//pop();
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, -0.1f, -0.1f);
//		modelMatrix *= rotate(60.00f, (GLfloat)LeftHandElbow, -260.0f);
//
//		//modelMatrix *= rotate((GLfloat)LeftHandElbow, 0.0f, 1.0f, 0.0f);
//		modelMatrix *= translate(0.0f, 0.0f, -0.7f);
//		modelMatrix *= scale(0.32f, 0.32f, 0.32f);
//	}
//	 else if (Sitting == 1)
//	 {
//		 modelMatrix = peek();
//		 modelMatrix *= translate(0.0f, -0.5f, -0.75f);
//		 modelMatrix *= rotate((GLfloat)LeftHandElbow, 1.0f, 0.0f, 0.0f);
//		 modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		 modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	 }
//	 else if (Sitting == 2)
//	 {
//
//		 modelMatrix = peek();
//		 modelMatrix *= translate(0.0f, -0.5f, -0.75f);
//		 modelMatrix *= rotate((GLfloat)LeftHandElbow, 1.0f, 0.0f, 0.0f);
//		 modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		 modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//
//	 }
//	 else if (Running == 1)
//	 {
//		 modelMatrix = peek();
//		 modelMatrix *= translate(0.0f, 0.0f, -0.75f);
//		 modelMatrix *= rotate((GLfloat)LeftHandElbow * 0.8f, 1.0f, 0.0f, 0.0f);
//		 modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		 modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	 }
//	else
//	{
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, 0.0f, -0.75f);
//		modelMatrix *= rotate((GLfloat)LeftHandElbow, 1.0f, 0.0f, 0.0f);
//		modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	if (perFragmentLighting == true)
//	{
//
//		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//	else
//	{
//		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//	glBindVertexArray(vao_Sphere);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
//	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
//	glBindVertexArray(0);
//
//
//
//
//	//////////////////////////////
//	if (Skipping == 1)
//	{
//		RotationMatrix = rotate((GLfloat)LeftHandShoulder, 1.0f, 0.0f, 0.0f);
//		ScaleMatrix = scale(0.3f, 0.3f, 0.3f);
//		modelMatrix = translate(-0.9f, HandSkippingY , walkingTranslate + 0.12f);
//		modelMatrix = modelMatrix * RotationMatrix;
//		//modelMatrix *= rotate(0.0f, 0.0f, turnAngleLeftHand);
//		modelMatrix *= translate(0.0f, 0.0f, -1.0f);
//		push(modelMatrix);
//		modelMatrix *= ScaleMatrix;
//		push(modelMatrix);
//
//	}
//	else if (Running == 1)
//	{
//		pop();
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, 0.0f, -0.1f);
//		modelMatrix *= rotate((GLfloat)LeftHandElbow, 1.0f, 0.0f, 0.0f);
//		modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	else 
//	{
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, 0.0f, -0.1f);
//		modelMatrix *= rotate((GLfloat)LeftHandElbow, 1.0f, 0.0f, 0.0f);
//		modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	if (perFragmentLighting == true)
//	{
//
//		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//	else
//	{
//		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//	glBindVertexArray(vao_Sphere);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
//	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
//	glBindVertexArray(0);
//
//	resetStack();
//
//
//
//	//////////////////// Left Leg ////////////////////
//
	pop();
	RotationMatrix = rotate((GLfloat)leftLegUpper, 1.0f, 0.0f, 0.0f);

	
 if(Sitting == 1)
	{
		modelMatrix = peek();
		modelMatrix *= translate(0.0f, -1.85f, 0.0f);
		modelMatrix *= translate(-0.4f, 0.0f, 0.3f);
		push(modelMatrix);
		if (Walking == 1)
			RotationMatrix = rotate((GLfloat)leftLegUpper, 1.0f, 0.0f, 0.0f);
		else if (Sitting == 1)
			RotationMatrix = rotate((GLfloat)leftLegUpper, 1.0f, 0.0f, 0.0f);
		ScaleMatrix = scale(0.3f, 0.3f, 0.1f);


		//	push(modelMatrix);
		modelMatrix = modelMatrix * RotationMatrix;
		modelMatrix *= translate(0.0f, 0.0f, 0.0f);
	//	push(modelMatrix);
		modelMatrix *= ScaleMatrix;
		push(modelMatrix);
	}
	else if (Sitting == 2)
	{
		modelMatrix = peek();
		modelMatrix *= translate(0.0f, -1.85f, 0.0f);
		modelMatrix *= translate(-0.4f, 0.0f, 0.3f);
		push(modelMatrix);
		if (Walking == 1)
			RotationMatrix = rotate((GLfloat)leftLegUpper, 1.0f, 0.0f, 0.0f);
		else if (Sitting == 1)
			RotationMatrix = rotate((GLfloat)leftLegUpper, 1.0f, 0.0f, 0.0f);
		ScaleMatrix = scale(0.3f, 0.3f, 0.1f);


		//	push(modelMatrix);
		modelMatrix = modelMatrix * RotationMatrix;
		modelMatrix *= translate(0.0f, 0.0f, 0.0f);
		//	push(modelMatrix);
		modelMatrix *= ScaleMatrix;
		push(modelMatrix);
	}
	
	

	else
	{
		modelMatrix = peek();
		modelMatrix *= translate(0.0f, -0.85f, 0.0f);
		modelMatrix *= translate(-0.4f, 0.0f, 0.3f);
		
			RotationMatrix = rotate((GLfloat)leftLegUpper, 1.0f, 0.0f, 0.0f);
	
		ScaleMatrix = scale(0.3f, 0.3f, 0.1f);


		//	push(modelMatrix);
		modelMatrix = modelMatrix * RotationMatrix;
		modelMatrix *= translate(0.0f, 0.0f, -1.0f);
		push(modelMatrix);
		modelMatrix *= ScaleMatrix;
		push(modelMatrix);
	}

	
	if (perFragmentLighting == true)
	{

		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);
	}
	else
	{
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	}

	draw_cylinder();
//
	pop();

	
	 if(Sitting == 1 || Sitting == 2)
	{
		
		modelMatrix = peek();
		modelMatrix *= translate(0.0f, 0.0f, 0.0f);
		modelMatrix *= rotate((GLfloat)leftLegLower, 1.0f, 0.0f, 0.0f);
		modelMatrix *= translate(0.0f, 0.0f, -0.8f);
		modelMatrix *= scale(0.3f, 0.3f, 0.09f);
	}
	
	else
	{
		modelMatrix = peek();
		modelMatrix *= translate(0.0f, 0.0f, -0.1f);
		modelMatrix *= rotate((GLfloat)leftLegLower, 1.0f, 0.0f, 0.0f);
		modelMatrix *= translate(0.0f, 0.0f, -0.7f);
		modelMatrix *= scale(0.3f, 0.3f, 0.09f);
	}
	if (perFragmentLighting == true)
	{

		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);
	}
	else
	{
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	}
	draw_cylinder();
//	if (Walking == 1)
//	{
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, -0.0f, -0.1f);
//		modelMatrix *= rotate((GLfloat)leftLegLower, 1.0f, 0.0f, 0.0f);
//		modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	else if (Sitting == 1)
//	{
//		//pop();
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, -0.02f, 0.025f);
//		modelMatrix *= rotate((GLfloat)leftLegLower, 1.0f, 0.0f, 0.0f);
//		//modelMatrix *= translate(0.0f, 0.0f, 0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	else if (Sitting == 2)
//	{
//		//pop();
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, -0.02f, 0.025f);
//		modelMatrix *= rotate((GLfloat)leftLegLower, 1.0f, 0.0f, 0.0f);
//		//modelMatrix *= translate(0.0f, 0.0f, 0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	else if (Skipping == 1)
//	{
//	//	pop();
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, -0.1f, -0.0f);
//		modelMatrix *= rotate((GLfloat)leftLegLower, 1.0f, 0.0f, 0.0f);
//		//modelMatrix *= translate(0.0f, 0.0f, 0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	else
//	{
//
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, -0.0f, -0.1f);
//		modelMatrix *= rotate((GLfloat)leftLegLower, 1.0f, 0.0f, 0.0f);
//		push(modelMatrix);
//		modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//
//	}
//	if (perFragmentLighting == true)
//	{
//
//		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//	else
//	{
//		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//
//
//	glBindVertexArray(vao_Sphere);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
//	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
//	glBindVertexArray(0);
//
//
//
//	//////////////////////// SHOES /////////////////////////
//
	
//	modelMatrix = peek();
//	modelMatrix *= translate(0.0f, -0.0f, -0.8f);
//	modelMatrix *= rotate((GLfloat)leftLegLower, 1.0f, 0.0f, 0.0f);
//	modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//	modelMatrix *= scale(0.3f, 0.7f, 0.3f);
//
//	
//	if (perFragmentLighting == true)
//	{
//
//		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//	else
//	{
//		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//
//
//	glBindVertexArray(vao_Sphere);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
//	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
//	glBindVertexArray(0);
//
//
//
//
//
//
//
//
//	resetStack();
//
//
//	////////////////////////// RIGHT LEG ////////////////////////////////
//
//	
//
pop();


if(Sitting == 1 || Sitting == 2)
{
	modelMatrix = peek();
	modelMatrix *= translate(0.0f, -1.85f, 0.0f);
	modelMatrix *= translate(0.4f, 0.0f, 0.3f);
	push(modelMatrix);
	if (Walking == 1)
		RotationMatrix = rotate((GLfloat)leftLegUpper, 1.0f, 0.0f, 0.0f);
	else if (Sitting == 1)
		RotationMatrix = rotate((GLfloat)leftLegUpper, 1.0f, 0.0f, 0.0f);
	ScaleMatrix = scale(0.3f, 0.3f, 0.1f);


	//	push(modelMatrix);
	modelMatrix = modelMatrix * RotationMatrix;
	modelMatrix *= translate(0.0f, 0.0f, 0.0f);
	//	push(modelMatrix);
	modelMatrix *= ScaleMatrix;
	push(modelMatrix);
}
else
{
	modelMatrix = peek();
	modelMatrix *= translate(0.0f, -0.85f, 0.0f);
	modelMatrix *= translate(0.4f, 0.0f, 0.3f);

	RotationMatrix = rotate((GLfloat)RightLegUpper, 1.0f, 0.0f, 0.0f);

	ScaleMatrix = scale(0.3f, 0.3f, 0.1f);


	//	push(modelMatrix);
	modelMatrix = modelMatrix * RotationMatrix;
	modelMatrix *= translate(0.0f, 0.0f, -1.0f);
	push(modelMatrix);
	modelMatrix *= ScaleMatrix;
	push(modelMatrix);
}
	if (perFragmentLighting == true)
	{

		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);
	}
	else
	{
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	}

	draw_cylinder();
//
	pop();
//


 if(Sitting == 1 || Sitting == 2)
	{

		modelMatrix = peek();
		modelMatrix *= translate(0.0f, 0.0f, 0.0f);
		modelMatrix *= rotate((GLfloat)leftLegLower, 1.0f, 0.0f, 0.0f);
		modelMatrix *= translate(0.0f, 0.0f, -0.8f);
		modelMatrix *= scale(0.3f, 0.3f, 0.09f);
	}
	else
	{
		modelMatrix = peek();
		modelMatrix *= translate(0.0f, 0.0f, -0.1f);
		modelMatrix *= rotate((GLfloat)RightLegLower, 1.0f, 0.0f, 0.0f);
		modelMatrix *= translate(0.0f, 0.0f, -0.7f);
		modelMatrix *= scale(0.3f, 0.3f, 0.09f);
	}
	if (perFragmentLighting == true)
	{

		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);
	}
	else
	{
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	}
	draw_cylinder();
//	if (Walking == 1)
//	{
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, -0.0f, -0.1f);
//		modelMatrix *= rotate((GLfloat)RightLegLower, 1.0f, 0.0f, 0.0f);
//		modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	else if (Sitting == 1)
//	{
//		//pop();
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, -0.02f, 0.025f);
//		modelMatrix *= rotate((GLfloat)RightLegLower, 1.0f, 0.0f, 0.0f);
//		//modelMatrix *= translate(0.0f, 0.0f, 0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	else if (Sitting == 2)
//	{
//		//pop();
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, -0.02f, 0.025f);
//		modelMatrix *= rotate((GLfloat)RightLegLower, 1.0f, 0.0f, 0.0f);
//		//modelMatrix *= translate(0.0f, 0.0f, 0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	else if (Skipping == 1)
//	{
//		//	pop();
//		modelMatrix = peek();
//		modelMatrix *= translate(0.0f, -0.1f, -0.0f);
//		modelMatrix *= rotate((GLfloat)RightLegLower, 1.0f, 0.0f, 0.0f);
//		//modelMatrix *= translate(0.0f, 0.0f, 0.0f);
//		modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//	}
//	else
//	{
//		
//			modelMatrix = peek();
//			modelMatrix *= translate(0.0f, -0.0f, -0.1f);
//			modelMatrix *= rotate((GLfloat)RightLegLower, 1.0f, 0.0f, 0.0f);
//			push(modelMatrix);
//			modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//			modelMatrix *= scale(0.3f, 0.3f, 0.3f);
//		
//	}
//	if (perFragmentLighting == true)
//	{
//
//		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//	else
//	{
//		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//
//
//	glBindVertexArray(vao_Sphere);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
//	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
//	glBindVertexArray(0);
//
//
//
//	///////////// SHOES RIGHT LEG /////////
//	modelMatrix = peek();
//	modelMatrix *= translate(0.0f, -0.0f, -0.8f);
//	modelMatrix *= rotate((GLfloat)RightLegLower, 1.0f, 0.0f, 0.0f);
//	modelMatrix *= translate(0.0f, 0.0f, -0.0f);
//	modelMatrix *= scale(0.3f, 0.7f, 0.3f);
//
//
//	if (perFragmentLighting == true)
//	{
//
//		glUniformMatrix4fv(modelUniformPerFragment, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniformPerFragment, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//	else
//	{
//		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, modelMatrix);
//		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, viewMatrix);
//		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
//	}
//
//
//	glBindVertexArray(vao_Sphere);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element_Sphere);
//	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
//	glBindVertexArray(0);
//
//

	

	resetStack();









glUseProgram(0);


	

	UpdateSitting();
	UpdateSkipping();
	UpdateWalking();
	UpdateRunning();
	UpdateSwimming();
	



	glXSwapBuffers(gpDisplay,gWindow);
     angleTriangle = angleTriangle + 0.2f;
	angleRectangle = angleRectangle+0.2f;
}


	void UpdateSkipping()
{
	if (Skipping == 1)
	{
		if (Jump == 1)
		{
			if (leftLegLower <= 70.000153)
			{
				RightLegLower = RightLegLower + 0.14;
				leftLegLower = leftLegLower + 0.14;


				LeftHandElbow = LeftHandElbow - 0.45;
				RightHandElbow = RightHandElbow + 0.45;
				//fprintf(gpLogFile, "The value of leftLegLower is %f\n", leftLegLower);

				if (UpperChestSkippingY <= 0.2)
					UpperChestSkippingY += 0.0006f;
				if (FaceSkippingY <= 2.2)
					FaceSkippingY+= 0.0006f;
				if (LegSkippingY <= -1.4)
					LegSkippingY += 0.0006f;
				if (HandSkippingY <= 1.2)
					HandSkippingY += 0.0006f;
				//fprintf(gpLogFile, "UpperChestSkippingY is %f\n", UpperChestSkippingY);
			}
		}
		if (leftLegLower >= 70.0001253)
		{
			Jump = 0;
			//fprintf(gpLogFile, "Here is JUMP is 0");
		}
		if (Jump == 0)
		{
			if (leftLegLower >= 30.0000)
			{
				RightLegLower = RightLegLower - 0.14;
				leftLegLower = leftLegLower - 0.14;
				//fprintf(gpLogFile, "The value of leftLegLower is %f\n", leftLegLower);
				LeftHandElbow = LeftHandElbow - 0.45;
				RightHandElbow = RightHandElbow + 0.45;
				if(UpperChestSkippingY >= 0.0)
					UpperChestSkippingY -= 0.0006f;
				if (FaceSkippingY >= 2.0)
					FaceSkippingY -= 0.0006f;
				if (LegSkippingY >= -1.6)
					LegSkippingY -= 0.0006f;
				if (HandSkippingY >= 1.0)
					HandSkippingY -= 0.0006f;
				if (leftLegLower <= 30.0000)
				{
					Jump = 1;
				}
				
			}
		}


	}
}
void UpdateSitting()
{
	if (Sitting == 1)
	{
		
		
			if (leftLegUpper >= -161.0)
			{
				

				/*	LeftHandShoulder = (int)(LeftHandShoulder - 1.0) % 360;
					LeftHandElbow = (int)(LeftHandElbow - 0.5) % 360;

					RightHandShoulder = (int)(RightHandShoulder + 1.0) % 360;
					RightHandElbow = (int)(RightHandElbow + 0.5) % 360;
		*/

				leftLegUpper = (int)(leftLegUpper - 1.0) % 360;
				//leftLegLower = -90.0;


				//RightLegUpper = (int)(RightLegUpper - 1.0) % 360;
				//RightLegLower = -90.0;
			}
	
				
		
	}
	else if (Sitting == 2)
	{
		if (leftLegUpper <= -90.0)
		{
			//fprintf(gpLogFile, "Value of RightLegUpper is %f\n", RightLegUpper);
			leftLegUpper = (int)(leftLegUpper + 1.0) % 360;
			/*leftLegLower = -100.0;


			RightLegUpper = (int)(RightLegUpper + 1.0) % 360;
			RightLegLower = -100.0;*/

		}
		//if()
	}
	
}
void UpdateWalking()
{



	if (Walking == 1)
	{
		//Sitting = 0;

		/*if (walkingTranslate <= -5.0f)
			walkingTranslate += 0.02f;
		else
			Walking = 0;
*/
		if (RightLeg == 1)
		{
			if (RightLegUpper >= -141.0)
			{


				LeftHandShoulder = (int)(LeftHandShoulder - 1.0) % 360;
				LeftHandElbow = (int)(LeftHandElbow - 0.5) % 360;

				RightHandShoulder = (int)(RightHandShoulder + 1.0) % 360;
				RightHandElbow = (int)(RightHandElbow + 0.5) % 360;


				leftLegUpper = (int)(leftLegUpper + 1.0) % 360;
				leftLegLower = (int)(leftLegLower + 0.5) % 360;


				RightLegUpper = (int)(RightLegUpper - 1.0) % 360;
				RightLegLower = (int)(RightLegLower - 0.5) % 360;
			}
		}

		if (RightLegUpper <= -141.0)
			RightLeg = 0;


		if (RightLeg == 0)
		{
			if (RightLegUpper <= -50.0)
			{

				LeftHandShoulder = (int)(LeftHandShoulder + 1.0) % 360;
				LeftHandElbow = (int)(LeftHandElbow + 0.5) % 360;

				RightHandShoulder = (int)(RightHandShoulder - 1.0) % 360;
				RightHandElbow = (int)(RightHandElbow - 0.5) % 360;



				leftLegUpper = (int)(leftLegUpper - 1.0) % 360;
				leftLegLower = (int)(leftLegLower - 0.5) % 360;


				RightLegUpper = (int)(RightLegUpper + 1.0) % 360;
				RightLegLower = (int)(RightLegLower + 0.5) % 360;
			}
			if (RightLegUpper >= -50.0)
			{
				RightLeg = 1;
			}
		}


	}


}
void UpdateRunning()
{
	if (Running == 1)
	{
		if (RightLeg == 1)
		{
			if (RightHandShoulder >= -132.000076)
			{


				RightHandShoulder = RightHandShoulder - 0.6f;
				RightHandElbow = RightHandElbow - 0.7f;


				LeftHandShoulder = LeftHandShoulder + 0.6f;
				LeftHandElbow = LeftHandElbow + 0.7f;

				leftLegUpper = leftLegUpper - 0.7;
				leftLegLower = leftLegLower - 0.6;


				RightLegUpper = RightLegUpper + 0.7;
				RightLegLower = RightLegLower + 0.6;

			}
			if (RightHandShoulder <= -132.000076)
				RightLeg = 0;
		}
		
		if (RightLeg == 0)
		{
			if (RightHandShoulder <= -50.0)
			{
				RightHandShoulder = RightHandShoulder + 0.6f;
				RightHandElbow = RightHandElbow + 0.7f;


				LeftHandShoulder = LeftHandShoulder - 0.6f;
				LeftHandElbow = LeftHandElbow - 0.7f;

				leftLegUpper = leftLegUpper + 0.7;
				leftLegLower = leftLegLower + 0.6;


				RightLegUpper = RightLegUpper - 0.7;
				RightLegLower = RightLegLower - 0.6;
			}
			if (RightHandShoulder >= -50.0)
				RightLeg = 1;
		}

	}
}
void UpdateSwimming()
{
	if (swimming == 1)
	{
		if (smimmingLegsToggle == 0)
		{
			if (leftLegUpper >= -103.9)
			{
				RightLegUpper = RightLegUpper + 0.25f;
				leftLegUpper -= 0.25f;
				//fprintf(gpLogFile, "I am here\n");
			}
			if (leftLegUpper <= -103.9)
			{
				smimmingLegsToggle = 1;
				//fprintf(gpLogFile, "Value of leftLegUpper is %f\n", leftLegUpper);
			}
		}
		if(smimmingLegsToggle == 1)
		{

			if (leftLegUpper <= -70.0)
			{

				RightLegUpper = RightLegUpper - 0.25f;
				leftLegUpper += 0.25f;
			}
			if (leftLegUpper >= -70.0)
			{
			//	fprintf(gpLogFile, "I am here\n");
				smimmingLegsToggle = 0;
			}

		}
		RightHandShoulder = RightHandShoulder - 0.45f;
		LeftHandShoulder = LeftHandShoulder + 0.45f;
	}
}

void ToggleFullscreen(void)
{
	//variable declarations 
	Atom wm_state;
	Atom fullscreen;
	XEvent xev = {0};
	
	//code
	
	wm_state = XInternAtom(gpDisplay,"_NET_WM_STATE",False);
	
	memset(&xev,0,sizeof(xev));

	xev.type = ClientMessage;
	xev.xclient.window = gWindow;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0]=bFullscreen ? 0 : 1;
	
	fullscreen = XInternAtom(gpDisplay,"_NET_WM_STATE_FULLSCREEN",False);
	xev.xclient.data.l[1] = fullscreen;
	XSendEvent(gpDisplay,
		  RootWindow(gpDisplay,gpXVisualInfo->screen),
		False,
		StructureNotifyMask,
		&xev);
}
void PerVertexLighting()
{
	glUseProgram(gShaderProgramObject);

	//fprintf(gpLogFile, "gbLighting is TRUE!\n");

	if (gbLighting == 1)
	{
		glUniform1i(LKeyPressed, 1);
		glUniform3fv(LDUniform, 1, lightDiffuse);
		glUniform3fv(LAUniform, 1, lightAmbient);
		glUniform3fv(LSUniform, 1, lightSpecular);

		glUniform3fv(KDUniform, 1, materialDiffuse);
		glUniform3fv(KAUniform, 1, materialAmbient);
		glUniform3fv(KSUniform, 1, materialSpecular);
		glUniform1f(MaterialShininessUniform, materialShininess);
		glUniform4fv(LightPositionUniform, 1, lightPosition);
	}
	else
	{
		glUniform1i(LKeyPressed, 0);
	}
	



}
void PerFragmentLighting()
{
	glUseProgram(gShaderProgramObjectPerFragment);

	//fprintf(gpLogFile, "gbLighting is TRUE!\n");
	if (gbLighting == 1)
	{
		glUniform1i(LKeyPressedPerFragment, 1);
		glUniform3fv(LDUniformPerFragment, 1, lightDiffuse);
		glUniform3fv(LAUniformPerFragment, 1, lightAmbient);
		glUniform3fv(LSUniformPerFragment, 1, lightSpecular);

		glUniform3fv(KDUniformPerFragment, 1, materialDiffuse);
		glUniform3fv(KAUniformPerFragment, 1, materialAmbient);
		glUniform3fv(KSUniformPerFragment, 1, materialSpecular);
		glUniform1f(MaterialShininessUniformPerFragment, materialShininess);
		glUniform4fv(LightPositionUniformPerFragment, 1, lightPosition);
	}
	else
	{
		glUniform1i(LKeyPressedPerFragment, 0);

	}


}
void update(void)
{


}
int Sphere(GLfloat r, int n, GLfloat **Vertices, GLfloat **Textures, GLfloat **Normals)
{
	int i, j;
	int NoOfVertices = 0;
	GLdouble phi1, phi2, theta, s, t;
	GLfloat ex, ey, ez, px, py, pz;
	GLfloat x0 = 0.0f;
	GLfloat y0 = 0.0f;
	GLfloat z0 = 0.0f;
	*Vertices = (GLfloat *)malloc(3 * sizeof(GLfloat) * n * (n + 1) * 2);
	*Normals = (GLfloat *)malloc(3 * sizeof(GLfloat) * n * (n + 1) * 2);
	*Textures = (GLfloat *)malloc(2 * sizeof(GLfloat) * n * (n + 1) * 2);
	GLfloat *Vert = *Vertices;
	GLfloat *TexCoords = *Textures;
	GLfloat *NormalsSphere = *Normals;
	if (Vertices)
	{
		for (j = 0; j < n; j++)
		{

			phi1 = j * M_PI * 2 / n;
			phi2 = (j + 1) * M_PI * 2 / n;
			for (i = 0; i <= n; i++)
			{
				theta = i * M_PI / n;
				ex = sin(theta) * cos(phi2);
				ey = sin(theta) * sin(phi2);
				ez = cos(theta);
				px = r * ex;
				py = r * ey;
				pz = r * ez;

				NormalsSphere[(NoOfVertices * 3) + 0] = ex;
				NormalsSphere[(NoOfVertices * 3) + 1] = ey;
				NormalsSphere[(NoOfVertices * 3) + 2] = ez;

				s = phi2 / (M_PI * 2);
				t = 1 - (theta / M_PI);
				TexCoords[(NoOfVertices * 2) + 0] = s;
				TexCoords[(NoOfVertices * 2) + 1] = t;

				Vert[(NoOfVertices * 3) + 0] = px;
				Vert[(NoOfVertices * 3) + 1] = py;
				Vert[(NoOfVertices * 3) + 2] = pz;

				///////////////////////





				ex = sin(theta) * cos(phi1);
				ey = sin(theta) * sin(phi1);
				ez = cos(theta);
				px = r * ex;
				py = r * ey;
				pz = r * ez;
				NormalsSphere[(NoOfVertices * 3) + 3] = ex;
				NormalsSphere[(NoOfVertices * 3) + 4] = ey;
				NormalsSphere[(NoOfVertices * 3) + 5] = ez;


				//
				s = phi1 / (M_PI * 2);
				t = 1 - (theta / M_PI);
				TexCoords[(NoOfVertices * 2) + 2] = s;
				TexCoords[(NoOfVertices * 2) + 3] = t;

				//
				Vert[(NoOfVertices * 3) + 3] = px;
				Vert[(NoOfVertices * 3) + 4] = py;
				Vert[(NoOfVertices * 3) + 5] = pz;
				NoOfVertices = NoOfVertices + 2;
			}
		}
	}
	//fprintf(gpLogFile, "No of vertices %d\n", NoOfVertices);
	return NoOfVertices;

}

void uninitialize(void)
{
	GLXContext currentGLXContext;
	currentGLXContext = glXGetCurrentContext();

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
	if(currentGLXContext != NULL && currentGLXContext == gGLXContext)
	{
		glXMakeCurrent(gpDisplay, 0, 0);
	}
	
	if(gGLXContext)
	{
		glXDestroyContext(gpDisplay, gGLXContext);
	}

	if(gWindow)
	{
		XDestroyWindow(gpDisplay,gWindow);
	}
	if(gColormap)
	{
		XFreeColormap(gpDisplay,gColormap);
	}
	if(gpXVisualInfo)
	{
		free(gpXVisualInfo);
		gpXVisualInfo = NULL;
	}
	if(gpDisplay)
	{
		XCloseDisplay(gpDisplay);
		gpDisplay = NULL;
	}
}



