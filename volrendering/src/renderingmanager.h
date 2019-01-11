#ifndef RENDERING_MANAGER_H
#define RENDERING_MANAGER_H

#include <glm/glm.hpp>
#include <vector>

#include "math/simplearoundcamera.h"

#include "datamanager.h"
#include "renderingparameters.h"

class BaseVolumeRenderer;

class RenderingManager
{
public:
  /*! Returns the current instance of Viewer (lazy instantiation).
  */
  static RenderingManager *Instance ();

  /*! Verify if already exists an instance of the Viewer.
  \return exist or not exist (true or false)
  */
  static bool Exists ();

  /*! Just Destroy the instance of the singleton.
  */
  static void DestroyInstance ();

  // get current window width
  int GetWidth ();
  // get current window height
  int GetHeight ();

  void AddVolumeRenderer (BaseVolumeRenderer* volrend);

  void InitGL ();

  // Init glew + camera + curr vol renderer
  void Init ();
  // Call Main Loop
  void MainLoop ();
  
  // Set Current Window Title
  void SetWindowTitle ();

  // Update the volume renderer with the current volume and transfer function
  void UpdateModelAndTransferFunction ();

  void Display ();
  void Reshape (int w, int h);
  void Keyboard (unsigned char key, int x, int y);
  void KeyboardUp (unsigned char key, int x, int y);
  void OnMouse (int bt, int st, int x, int y);
  void OnMotion (int x, int y);
  void CloseFunc ();
  void IdleFunc ();

protected:
  DataManager mgr_data;

private:
  void PrintRenderingParameters ();

  // Go to previous renderer
  bool PreviousRenderer ();
  // Go to next renderer
  bool NextRenderer ();
  // Set current volume renderer
  void SetCurrentVolumeRenderer ();

  bool input_to_renderer;
  bool updating_world_light_pos;

  unsigned int window_width;
  unsigned int window_height;
  
  SimpleAroundCamera sa_camera;

  int curr_rendering_mode;
  bool reprint_parameters;

  BaseVolumeRenderer* curr_vol_renderer;
  std::vector<BaseVolumeRenderer*> vtr_volrend_models;
  RenderingParameters curr_rdr_parameters;
  
protected:
  /*! Constructor.*/
  RenderingManager ();
  /*! Destructor.*/
  virtual ~RenderingManager ();

private:
  /*! the pointer to the singleton instance.*/
  static RenderingManager* crr_instance;

  double lastTime;
  int nbFrames;
  double window_fps;
};

#endif