#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
  #include <windows.h>
  #include "GL/gl.h"
  #include "GL/glu.h"
#endif

#include "GL/gl.h"
#include "GL/glut.h"


// The Width & Height have been reduced to help you debug
//	more quickly by reducing the resolution 
//  Your code should work for any dimension, and should be set back
//	to 640x480 for submission.
//#define WINDOW_WIDTH 320
//#define WINDOW_HEIGHT 240
//#define WINDOW_WIDTH 20 
//#define WINDOW_HEIGHT 20
//#define WINDOW_WIDTH 1600
//#define WINDOW_HEIGHT 900
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
//#define WINDOW_WIDTH 1920 
//#define WINDOW_HEIGHT 1080

#include "Scene.h"
#include "RayTrace.h"
#include "NormalRenderer.h"
//#include <windows.h>
//#include <ppl.h>
//#include <omp.h>

// --- Global State Variables --- 

// - Menu State Identifier - 
int g_iMenuId;

// - Mouse State Variables - 
int g_vMousePos[2] = {0, 0};
int g_iLeftMouseButton = 0;    // 1 if pressed, 0 if not 
int g_iMiddleMouseButton = 0;
int g_iRightMouseButton = 0;

// - RayTrace Variable for the RayTrace Image Generation - 
RayTrace g_RayTrace;

// - NormalRenderer Variable for drawing with OpenGL calls instead of the RayTracer - 
NormalRenderer g_NormalRenderer;

// - RayTrace Buffer - 
Vector g_ScreenBuffer[WINDOW_HEIGHT][WINDOW_WIDTH];

//unsigned int g_X = 0, g_Y = 0;
size_t g_X = 0, g_Y = 0;
bool g_bRayTrace = false;
bool g_bRenderNormal = true;

int g_iMode = 1; // mode 1 = normal mode 0 = raytrace

void myinit()
{
  // Default to these camera settings
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity ();
  glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, 1, -1);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Set the Scene Variable for the NormalRenderer
  g_NormalRenderer.SetScene (g_RayTrace.GetCurScene());

  glClearColor(0, 0, 0, 0);
}

void display()
{
  if (g_bRenderNormal)
  {
    g_NormalRenderer.RenderScene ();
  }
  else
  {
    // Set up the camera to render pixel-by-pixel
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity ();
    glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, 1, -1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBegin(GL_POINTS);
    {
      for (int y = 0; y < WINDOW_HEIGHT; y++)
      {
        for (int x = 0; x < WINDOW_WIDTH; x++)
        {
	  // Write to screen
//          g_ScreenBuffer[y][x] = g_RayTrace.CalculatePixel (x, y);

          glColor3f(g_ScreenBuffer[y][x].x, g_ScreenBuffer[y][x].y, g_ScreenBuffer[y][x].z);
//          glColor3f(0.4,0.4,0.4);
          glVertex2i(x, y);
        }
      }
    }

    glEnd();
  }

  glFlush();

  glutSwapBuffers ();
}

void menufunc(int value)
{
  switch (value)
  {
  case 0:
    // Start the Ray Tracing
    g_bRayTrace = true;
    g_bRenderNormal = false;
    g_iMode = 0;
    break;
  case 1:
    // Render Normal
    g_bRayTrace = false;
    g_X = 0;
    g_Y = 0;
    g_bRenderNormal = true;
    glutPostRedisplay ();
    g_iMode = 1;
    break;
  case 2:
    g_RayTrace.incrementScene();
    if(g_iMode == 1)//Render normal
    {
      myinit();
      glutPostRedisplay();
    }
    else if( g_iMode ==0)
    {
      g_bRayTrace = true;
    }
    break;
  case 3:
    // Quit Program
    exit(0);
    break;
  }
}

void doIdle()
{
  if (g_bRayTrace)
  {
// To run single threaded comment out the next 2 lines
//    concurrency::parallel_for(size_t(0), size_t(WINDOW_HEIGHT), [&](size_t g_Y)
//    {
// To run single threaded uncomment out the next 2 lines
    for(g_Y =0.0;g_Y<WINDOW_HEIGHT;g_Y++)
    {
      for(size_t X=0.0;X<WINDOW_WIDTH;X++)
      {
        g_ScreenBuffer[g_Y][X] = g_RayTrace.CalculatePixel (X, g_Y);
      }
    }
// To run single threaded comment out the next line
//    );
    g_bRayTrace = false;
    glutPostRedisplay ();
  }
  else
  {
    glutPostRedisplay ();
  }
}

void mousebutton(int button, int state, int x, int y)
{
  switch (button)
  {
  case GLUT_LEFT_BUTTON:
    g_iLeftMouseButton = (state==GLUT_DOWN);
    break;
  case GLUT_MIDDLE_BUTTON:
    g_iMiddleMouseButton = (state==GLUT_DOWN);
    break;
  case GLUT_RIGHT_BUTTON:
    g_iRightMouseButton = (state==GLUT_DOWN);
    break;
  }

  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}

int main (int argc, char ** argv)
{

  //You will be creating a menu to load in scenes
  //The test.xml is the default scene and you will modify this code
//  if (!g_RayTrace.m_Scene.Load ("test.xml"))
//  if (!g_RayTrace.m_Scene.Load ("snowman.xml"))
  if (!g_RayTrace.LoadScene("Scene1.xml"))
  {
    printf ("failed to load scene\n");
    exit(1);
  }

  if (!g_RayTrace.LoadScene("Scene2.xml"))
  {
    printf ("failed to load scene\n");
    exit(1);
  }
  glutInit(&argc,argv);
  g_iMode = 1;

  // create a window */
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

  glutCreateWindow("Assignment 5 - Ray Tracer");

  // tells glut to use a particular display function to redraw */
  glutDisplayFunc(display);

  // create a right mouse button menu */
  g_iMenuId = glutCreateMenu(menufunc);
  glutSetMenu(g_iMenuId);
  glutAddMenuEntry("Render RayTrace",0);
  glutAddMenuEntry("Render Normal",1);
  glutAddMenuEntry("Change Scene",2);
  glutAddMenuEntry("Quit",3);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  // callback for mouse button changes */
  glutMouseFunc(mousebutton);

  // callback for idle function */
  glutIdleFunc(doIdle);

  // do initialization */
  myinit();

  glutMainLoop();
  return 0;
}
