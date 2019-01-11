#include "renderingmanager.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "volrenderbase.h"
#include "glutils/framebufferobject.h"
#include "glutils/arrayobject.h"

//#define GET_NVIDIA_MEMORY

#ifdef GET_NVIDIA_MEMORY
#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049
#endif

RenderingManager *RenderingManager::crr_instance = 0;

RenderingManager* RenderingManager::Instance ()
{
  if (!crr_instance)
    crr_instance = new RenderingManager();

  return crr_instance;
}

bool RenderingManager::Exists ()
{
  return (crr_instance != NULL);
}

void RenderingManager::DestroyInstance ()
{
  if (!RenderingManager::Exists())
    return;

  if (crr_instance)
  {
    delete crr_instance;
    crr_instance = NULL;
  }
}

// get current window width
int RenderingManager::GetWidth ()
{
  return window_width;
}

// get current window height
int RenderingManager::GetHeight ()
{
  return window_height;
}

void RenderingManager::AddVolumeRenderer (BaseVolumeRenderer* bvolrend)
{
  vtr_volrend_models.push_back(bvolrend);
}

// Init glew + camera + curr vol renderer
void RenderingManager::InitGL ()
{
  glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

  if (glewInit() != GLEW_OK)
  {
    printf("Glew didn't initialized!\n");
    exit(EXIT_FAILURE);
  }
  printf("Running OpenGL %s\n\n", glGetString(GL_VERSION));

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_ALPHA_TEST);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_CULL_FACE);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_TEXTURE_3D);
}

// Init glew + camera + curr vol renderer
void RenderingManager::Init ()
{
  // Create datamanager after initializing glew
  mgr_data.Init();
  
  sa_camera.SetInitialConfig(SimpleAroundCamera::CAM_CONFIGS::PY_Z);
  
  for (int i = 0; i < vtr_volrend_models.size(); i++)
    vtr_volrend_models[i]->SetRenderingParamsAndDataManager(&curr_rdr_parameters, &mgr_data);
  
  curr_rendering_mode = vtr_volrend_models.size() - 1;
  
  SetCurrentVolumeRenderer();
 
  Reshape(window_width, window_height);
}

// Call Main Loop
void RenderingManager::MainLoop ()
{
  glutMainLoop();
}

// Set Current Window Title
void RenderingManager::SetWindowTitle ()
{
  char buffer[512];
  sprintf_s(buffer, "(%.2lf) %s [%s | %s]\0"
    , window_fps
    , curr_vol_renderer->GetVRModeName()
    , mgr_data.GetCurrentVolumeName().c_str()
    , mgr_data.GetCurrentTransferFunctionName().c_str()
  );
  
  glutSetWindowTitle(buffer);
}

// Update the volume renderer with the current volume and transfer function
void RenderingManager::UpdateModelAndTransferFunction ()
{
  curr_vol_renderer->PrepareDataAndShaders(window_width, window_height);
  SetWindowTitle();
}

void RenderingManager::Display ()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  
  // Measure speed
  double currentTime = glutGet(GLUT_ELAPSED_TIME);
  nbFrames++;
  if ((currentTime - lastTime) > 1000.0)
  {
    window_fps = double(nbFrames) * 1000.0 / (currentTime - lastTime);
  
    lastTime = currentTime;
    nbFrames = 0;
    SetWindowTitle();
  }

  if (curr_vol_renderer->IsBuilt())
  {
    curr_vol_renderer->PrepareRender(&sa_camera);
    curr_vol_renderer->Render();
  }
  
  if (reprint_parameters)
    PrintRenderingParameters();

  glutSwapBuffers();
}

void RenderingManager::Reshape (int w, int h)
{
  glViewport(0, 0, w, h);
  window_width = w;
  window_height = h;
  curr_rdr_parameters.SetScreenSize(w, h);
  sa_camera.UpdateAspectRatio(float(w), float(h));

  if (curr_vol_renderer->IsBuilt())
  {
    curr_vol_renderer->Reshape(w,h);
    curr_vol_renderer->SetOutdated();
  }

  glutPostRedisplay();
}

void RenderingManager::Keyboard (unsigned char key, int x, int y)
{
  switch(key)
  {
  case ' ':
    input_to_renderer = !input_to_renderer;
    PrintRenderingParameters();
    break;
  default:
    break;
  }
  
  glutPostRedisplay();
}

void RenderingManager::KeyboardUp (unsigned char key, int x, int y)
{
  if (input_to_renderer)
  {
    if (curr_vol_renderer->KeyboardUp(key, x, y))
    {
      reprint_parameters = true;
      curr_vol_renderer->SetOutdated();
    }
    glutPostRedisplay();
    return;
  }
  
  switch (key)
  {
  // Common
  case 'r':
  case 'R':
    curr_vol_renderer->ReloadShaders();
    break;
  
  case 'd':
  case 'D':
    float n_step;
    std::cout << "Set current step size: ";
    std::cin >> n_step;
    curr_rdr_parameters.SetStepSize(n_step);
    break;
  
  case'p':
  case'P':
    curr_rdr_parameters.ApplyBlinnPhongShading(!curr_rdr_parameters.IsUsingBlinnPhongShading());
    break;
  
  case 'L':
  case 'l':
    updating_world_light_pos = !updating_world_light_pos;
    curr_rdr_parameters.SetBlinnPhongLightingPosition(sa_camera.GetEye());
    break;
    
  // Set Current Volume
  case 'w':
  case 'W':
    if (mgr_data.NextVolume())
      UpdateModelAndTransferFunction();
    break;
  case 'q':
  case 'Q':
    if (mgr_data.PreviousVolume())
      UpdateModelAndTransferFunction();
    break;
  
  // Set Current Transfer Function
  case 's':
  case 'S':
    if (mgr_data.NextTransferFunction())
      UpdateModelAndTransferFunction();
    break;
  case 'a':
  case 'A':
    if (mgr_data.PreviousTransferFunction())
      UpdateModelAndTransferFunction();
    break;

  case 'z':
  case 'Z':
    sa_camera.SetPreviousInitialConfig();
    if (updating_world_light_pos)
    {
      curr_rdr_parameters.SetBlinnPhongLightingPosition(sa_camera.GetEye());
    }
    break;
  case 'x':
  case 'X':
    sa_camera.SetNextInitialConfig();
    if (updating_world_light_pos)
    {
      curr_rdr_parameters.SetBlinnPhongLightingPosition(sa_camera.GetEye());
    }
    break;
  
  // Set Current Renderer
  case '>':
    NextRenderer();
    break;
  case '<':
    PreviousRenderer();
    break;
  
  case 'g':
  case 'G':
    if (!mgr_data.UpdateGradientTexture())
      curr_rdr_parameters.ApplyBlinnPhongShading(false);
    curr_vol_renderer->UpdateGradientTexture();
    break;
  
  default:
    glutPostRedisplay();
    return;
    break;
  }
  reprint_parameters = true;
  curr_vol_renderer->SetOutdated();
  glutPostRedisplay();
}

void RenderingManager::OnMouse (int bt, int st, int x, int y)
{
  if (input_to_renderer)
  {
    if (curr_vol_renderer->OnMouse(bt, st, x, y))
    {
      glutPostRedisplay();
      return;
    }
  }
  
  curr_rdr_parameters.SetMouseLastCoordinates(x, y);
  sa_camera.MouseButton(bt, st, x, y);
  glutPostRedisplay();
}

void RenderingManager::OnMotion (int x, int y)
{
  sa_camera.MouseMotion(x, y);
  
  if (updating_world_light_pos)
  {
    curr_rdr_parameters.SetBlinnPhongLightingPosition(sa_camera.GetEye());
  }
  
  curr_vol_renderer->SetOutdated();
  glutPostRedisplay();
}

void RenderingManager::CloseFunc()
{
  gl::Shader::Unbind();
  gl::ArrayObject::Unbind();

  for (int i = vtr_volrend_models.size() - 1; i >= 0; i--)
    delete vtr_volrend_models[i];
  vtr_volrend_models.clear();
}

void RenderingManager::IdleFunc()
{
  glutPostRedisplay();
}

void RenderingManager::PrintRenderingParameters()
{
  // Clear Console
  system("CLS");

  std::cout << "///////////////////////////////////////////" << std::endl;
  std::cout << "// General Parameters                    //" << std::endl;
  std::cout << "///////////////////////////////////////////" << std::endl;
#ifdef GET_NVIDIA_MEMORY
  GLint nTotalMemoryInKB = 0, nCurAvailMemoryInKB = 0;
  glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &nTotalMemoryInKB);
  glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &nCurAvailMemoryInKB);
  std::cout << "Total   Available Memory in KB: " << nTotalMemoryInKB << std::endl;
  std::cout << "Current Available Memory in KB: " << nCurAvailMemoryInKB << std::endl;
#endif
  std::cout << "Running OpenGL " << glGetString(GL_VERSION) << std::endl;
  std::cout << std::endl;

  std::cout << "///////////////////////////////////////////" << std::endl;
  std::cout << "// Volume Rendering Parameters           //" << std::endl;
  std::cout << "///////////////////////////////////////////" << std::endl;
  std::cout << "[q/w] Volume: " << mgr_data.GetCurrentVolumeName() << std::endl;
  std::cout << "[   ] Sizes: [" << mgr_data.GetCurrentVolume()->GetWidth() << ", " <<
    mgr_data.GetCurrentVolume()->GetHeight() << ", " <<
    mgr_data.GetCurrentVolume()->GetDepth() << "]" << std::endl;
  std::cout << "[   ] Scale: [" << mgr_data.GetCurrentVolume()->GetScaleX() << ", " <<
    mgr_data.GetCurrentVolume()->GetScaleY() << ", " <<
    mgr_data.GetCurrentVolume()->GetScaleZ() << "]" << std::endl;
  std::cout << "[a/s] Transfer Function: " << mgr_data.GetCurrentTransferFunctionName() << std::endl;
  std::cout << "[ v ] Change Viewport: " << window_width << " " << window_height << std::endl;
  std::cout << std::endl;
  std::cout << "[</>] Renderer: " << curr_vol_renderer->GetVRModeName() << std::endl;
  std::cout << "[ r ] Reload Shaders." << std::endl;
  std::cout << "[ d ] Step Size: " << curr_rdr_parameters.GetStepSize() << std::endl;
  std::cout << "[ p ] " << (curr_rdr_parameters.IsUsingBlinnPhongShading() ? "Using Phong." : "Not using Phong.") << std::endl;
  std::cout << "[ l ] " << (updating_world_light_pos ? "Updating light position aligned with the camera." : "Light position set.") << std::endl;
  std::cout << "[z/x] Change initial position of the camera." << std::endl;
  std::cout << "[ g ] Update Gradient." << " [" << mgr_data.CurrentGradientName() << "]" << std::endl;
  std::cout << std::endl;
  std::cout << "[' '] " << (input_to_renderer ? "Interacting with renderer" : "Interacting with manager") << std::endl;
  std::cout << std::endl;
  
  std::cout << "///////////////////////////////////////////" << std::endl;
  std::cout << "// Renderer Parameters                   //" << std::endl;
  std::cout << "///////////////////////////////////////////" << std::endl;
  curr_vol_renderer->PrintRendererParameters();
  std::cout << std::endl;

  reprint_parameters = false;
}

// Go to previous renderer
bool RenderingManager::PreviousRenderer ()
{
  if (curr_rendering_mode > 0)
  {
    curr_rendering_mode -= 1;
    SetCurrentVolumeRenderer();
    return true;
  }
  return false;
}

// Go to next renderer
bool RenderingManager::NextRenderer ()
{
  if (curr_rendering_mode + 1 < vtr_volrend_models.size())
  {
    curr_rendering_mode += 1;
    SetCurrentVolumeRenderer();
    return true;
  }
  return false;
}

// Set current volume renderer
void RenderingManager::SetCurrentVolumeRenderer ()
{
  if (curr_vol_renderer)
    curr_vol_renderer->Destroy();
  
  curr_vol_renderer = vtr_volrend_models[curr_rendering_mode];
  UpdateModelAndTransferFunction();
}

RenderingManager::RenderingManager ()
  : window_width(512)
  , window_height(512)
  , sa_camera(256.0)
  , curr_rendering_mode(0)
  , input_to_renderer(false)
  , curr_vol_renderer(NULL)
{
  updating_world_light_pos = true;
  reprint_parameters = true;

  lastTime = glutGet(GLUT_ELAPSED_TIME);
  nbFrames = 0;
  window_fps = 0.0;
}

RenderingManager::~RenderingManager ()
{
  CloseFunc();
}