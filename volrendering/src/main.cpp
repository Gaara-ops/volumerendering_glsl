#include <GL/glew.h>
#include <GL/freeglut.h>

#include "renderingmanager.h"

static void Display(void)
{
  RenderingManager::Instance()->Display();
}

static void Reshape(int w, int h)
{
  RenderingManager::Instance()->Reshape(w, h);
}

static void Keyboard(unsigned char key, int x, int y)
{
  switch (key)
  {
  case 27:
    RenderingManager::Instance()->DestroyInstance();
    exit(EXIT_FAILURE);
    return;
  default:
    break;
  }

  RenderingManager::Instance()->Keyboard(key, x, y);
}

static void KeyboardUp(unsigned char key, int x, int y)
{
  RenderingManager::Instance()->KeyboardUp(key, x, y);
}

static void OnMouse(int bt, int st, int x, int y)
{
  RenderingManager::Instance()->OnMouse(bt, st, x, y);
}

static void OnMotion(int x, int y)
{
  RenderingManager::Instance()->OnMotion(x, y);
}

static void CloseFunc()
{
  RenderingManager::Instance()->DestroyInstance();
}

static void IdleFunc()
{
  RenderingManager::Instance()->IdleFunc();
}

#include "vr1pass.h"
void AddVolumeRendereringMethods ()
{
  RenderingManager::Instance()->AddVolumeRenderer(new VR1Pass());
}

int main(int argc, char* argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL | GLUT_ALPHA);
  glutInitWindowSize(RenderingManager::Instance()->GetWidth(),
                     RenderingManager::Instance()->GetHeight());
  glutCreateWindow("");

  glutDisplayFunc(Display);
  glutReshapeFunc(Reshape);
  glutKeyboardFunc(Keyboard);
  glutKeyboardUpFunc(KeyboardUp);
  glutMouseFunc(OnMouse);
  glutMotionFunc(OnMotion);
  glutCloseFunc(CloseFunc);
  glutIdleFunc(IdleFunc);
  
  RenderingManager::Instance()->InitGL();
  
  AddVolumeRendereringMethods();
  
  RenderingManager::Instance()->Init();
  RenderingManager::Instance()->MainLoop();
  RenderingManager::Instance()->DestroyInstance();

  return 0;
}