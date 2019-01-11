#ifndef BASE_VOLUME_RENDERER
#define BASE_VOLUME_RENDERER

#include "volrend/volume.h"
#include "volrend/transferfunction.h"

#include "glutils/shader.h"

#include "math/simplearoundcamera.h"

#include "datamanager.h"
#include "renderingparameters.h"

class SimpleAroundCamera;

class BaseVolumeRenderer
{
public:
  BaseVolumeRenderer ();
  ~BaseVolumeRenderer ();

  void SetRenderingParamsAndDataManager (RenderingParameters* rendering_params, DataManager* data_mgr);

  //////////////////////////////////////////////////////////////////////////////
  // Vol Render Base
  //////////////////////////////////////////////////////////////////////////////
  virtual const char* GetVRModeName () = 0;
  virtual const char* GetVRAbreviationName () { return ""; }

  virtual void Destroy ();

  virtual void ReloadShaders ();
  
  virtual bool PrepareDataAndShaders (int shader_width, int shader_height) = 0;

  virtual bool UpdateShaderParameters (SimpleAroundCamera* camera) = 0;
  
  virtual bool UpdateGradientTexture ();

  virtual bool UpdateTransferFunction ();
  virtual void DestroyTransferFunction ();

  virtual void Render ();
  virtual void Reshape (int w, int h);
  virtual bool KeyboardUp (unsigned char key, int x, int y);
  virtual bool OnMouse (int bt, int st, int x, int y);
  
  virtual bool BindCommonUniforms (SimpleAroundCamera* camera, gl::Shader* ref_shader);
  
  virtual void PrintRendererParameters ();
  //////////////////////////////////////////////////////////////////////////////

  void PrepareRender (SimpleAroundCamera* camera);
  
  void SetOutdated ();
  bool IsOutdated ();
  
  void SetBuilt (bool b_built);
  bool IsBuilt ();
  
protected:
  bool vr_built;
  bool vr_outdated;

  gl::Texture1D* glsl_transfer_function;

  DataManager* data_manager;
  RenderingParameters* rdr_params;

private:
};

#endif