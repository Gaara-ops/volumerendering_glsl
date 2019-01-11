#ifndef VOLUME_RENDERING_1_PASS
#define VOLUME_RENDERING_1_PASS

#include "glutils/texture1d.h"
#include "glutils/texture3d.h"

#include "glutils/arrayobject.h"
#include "glutils/bufferobject.h"

#include "glutils/shader.h"

#include "volrenderbase.h"

class SimpleAroundCamera;

class VR1Pass : public BaseVolumeRenderer
{
public:
  VR1Pass ();
  virtual ~VR1Pass ();

  //////////////////////////////////////////////////////////////////////////////
  // BaseVolumeRenderer
  virtual const char* GetVRModeName () { return "1-Pass"; }
  virtual const char* GetVRAbreviationName () { return "v1p"; }

  virtual void Destroy ();
  virtual void ReloadShaders ();

  virtual bool PrepareDataAndShaders (int shader_width, int shader_height);

  virtual bool UpdateShaderParameters (SimpleAroundCamera* camera);

  virtual bool UpdateGradientTexture ();
  virtual bool UpdateTransferFunction ();

  virtual void Render ();
  virtual void Reshape (int w, int h);
  virtual bool KeyboardUp (unsigned char key, int x, int y);
  //////////////////////////////////////////////////////////////////////////////

protected:
  gl::ArrayObject*  quad_vao;
  gl::BufferObject* quad_vbo;
  gl::BufferObject* quad_ibo;

  gl::Shader* shader_rendering;

private:
  void CreateRenderingPass ();
  void DestroyRenderingShaders ();
  void DestroyBufferData ();
};

#endif