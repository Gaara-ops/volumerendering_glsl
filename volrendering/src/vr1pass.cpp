#include "vr1pass.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

#include "math/simplearoundcamera.h"

#include "volrend/utils.h"

VR1Pass::VR1Pass ()
  : quad_vao(NULL), quad_vbo(NULL), quad_ibo(NULL)
  , shader_rendering(NULL)
{ }

VR1Pass::~VR1Pass ()
{
  Destroy();
}

void VR1Pass::Destroy ()
{
  DestroyTransferFunction();

  DestroyBufferData();
  DestroyRenderingShaders();
  BaseVolumeRenderer::Destroy();
}

void VR1Pass::ReloadShaders ()
{
  shader_rendering->Reload();
}

bool VR1Pass::PrepareDataAndShaders (int swidth, int sheight)
{
  if (vr_built)
    Destroy();

  glsl_transfer_function = data_manager->GetCurrentTransferFunction()->GenerateTexture_1D_RGBt();
  
  CreateRenderingPass();
  gl::ExitOnGLError("Error on Preparing Models and Shaders");
  
  Reshape(swidth, sheight);
  
  gl::ArrayObject::Unbind();
  gl::Shader::Unbind();
  
  SetBuilt(true);
  SetOutdated();
  return true;
}

bool VR1Pass::UpdateShaderParameters (SimpleAroundCamera* camera)
{
  shader_rendering->Bind();

  /////////////////////////////////////////////////////////////////
  // Camera settings
  shader_rendering->SetUniform("ViewMatrix", camera->LookAt());
  shader_rendering->BindUniform("ViewMatrix");

  shader_rendering->SetUniform("ProjectionMatrix", camera->Projection());
  shader_rendering->BindUniform("ProjectionMatrix");
  
  shader_rendering->SetUniform("fov_y_tangent", (float)tan((camera->GetFovY() / 2.0) * glm::pi<double>() / 180.0));
  shader_rendering->BindUniform("fov_y_tangent");

  shader_rendering->SetUniform("aspect_ratio", camera->GetAspectRatio());
  shader_rendering->BindUniform("aspect_ratio");

  //////////////////////////////////////////////////////////////////
  gl::Shader::Unbind();
  gl::ExitOnGLError("After UpdateShaderParameters on VR1Pass.");
  
  /////////////////////////////////////////////////////////////////
  return BindCommonUniforms(camera, shader_rendering);
}

bool VR1Pass::UpdateGradientTexture ()
{
  shader_rendering->Bind();

  if (data_manager->GetCurrentGradientTexture())
  {
    shader_rendering->SetUniformTexture3D("TexVolumeGradient", data_manager->GetCurrentGradientTexture()->GetTextureID(), 2);
    shader_rendering->BindUniform("TexVolumeGradient");
    shader_rendering->Unbind();
  }

  SetOutdated();

  return true;
}

bool VR1Pass::UpdateTransferFunction ()
{
  shader_rendering->Bind();
  DestroyTransferFunction();

  glsl_transfer_function = data_manager->GetCurrentTransferFunction()->GenerateTexture_1D_RGBt();

  shader_rendering->SetUniformTexture1D("TexTransferFunc", glsl_transfer_function->GetTextureID(), 1);
  shader_rendering->BindUniform("TexTransferFunc");
  shader_rendering->Unbind();

  SetOutdated();

  return true;
}

void VR1Pass::Render ()
{
  shader_rendering->Bind();
  
  quad_vao->Bind();
  quad_vbo->Bind();
  quad_ibo->Bind();

  // Render Front Face Quad Screen
  glCullFace(GL_BACK);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  quad_vao->DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT);

  gl::Shader::Unbind();

  gl::BufferObject::Unbind(GL_ARRAY_BUFFER);
  gl::BufferObject::Unbind(GL_ELEMENT_ARRAY_BUFFER);
  gl::ArrayObject::Unbind();
}

void VR1Pass::Reshape (int w, int h)
{
}

bool VR1Pass::KeyboardUp (unsigned char key, int x, int y)
{
  switch (key)
  {
  default:
    return false;
    break;
  }
  return true;
}

void VR1Pass::CreateRenderingPass ()
{
  glm::vec3 vol_aabb = glm::vec3(
    float(data_manager->GetCurrentVolume()->GetWidth())  * data_manager->GetCurrentVolume()->GetScaleX(),
    float(data_manager->GetCurrentVolume()->GetHeight()) * data_manager->GetCurrentVolume()->GetScaleY(),
    float(data_manager->GetCurrentVolume()->GetDepth())  * data_manager->GetCurrentVolume()->GetScaleZ()
  );

  // Quad Screen
  const vr::Vertex1p VERTICES[4] = {
    { { -1, -1, 0 } },
    { { -1, +1, 0 } },
    { { +1, +1, 0 } },
    { { +1, -1, 0 } } };
  const GLuint INDICES[6] = { 0, 2, 1, 0, 3, 2 };

  // Create Rendering Pass
  shader_rendering = new gl::Shader();
  shader_rendering->AddShaderFile(gl::SHADER_TYPE::VERTEX, "shader/vr1pass/singlepass.vert");
  shader_rendering->AddShaderFile(gl::SHADER_TYPE::FRAGMENT, "shader/raybboxintersection.frag");
  shader_rendering->AddShaderFile(gl::SHADER_TYPE::FRAGMENT, "shader/blinnphonggradient.frag");
  shader_rendering->AddShaderFile(gl::SHADER_TYPE::FRAGMENT, "shader/vr1pass/singlepass.frag");
  shader_rendering->LoadAndLink();
  
  shader_rendering->Bind();
  
  shader_rendering->SetUniform("VolumeScaledSizes", vol_aabb);
  
  // Bind textures and volume config uniforms
  if (data_manager->GetCurrentVolumeTexture())
    shader_rendering->SetUniformTexture3D("TexVolume", data_manager->GetCurrentVolumeTexture()->GetTextureID(), 0);
  if (glsl_transfer_function)
    shader_rendering->SetUniformTexture1D("TexTransferFunc", glsl_transfer_function->GetTextureID(), 1);
  if (data_manager->GetCurrentGradientTexture())
    shader_rendering->SetUniformTexture3D("TexVolumeGradient", data_manager->GetCurrentGradientTexture()->GetTextureID(), 2);
  
  shader_rendering->BindUniforms();
  
  gl::ExitOnGLError("ERROR: Could not get shader uniform locations");
  gl::Shader::Unbind();
  
  // VBO, VAO, IBO
  quad_vao = new gl::ArrayObject(1);
  quad_vao->Bind();
  
  quad_vbo = new gl::BufferObject(GL_ARRAY_BUFFER);
  quad_ibo = new gl::BufferObject(GL_ELEMENT_ARRAY_BUFFER);
  
  // bind the VBO to the VAO
  quad_vbo->SetBufferData(sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);
  
  quad_vao->SetVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VERTICES[0]), (GLvoid*)0);
  
  // bind the IBO to the VAO
  quad_ibo->SetBufferData(sizeof(INDICES), INDICES, GL_STATIC_DRAW);

  gl::Shader::Unbind();

  gl::BufferObject::Unbind(GL_ARRAY_BUFFER);
  gl::BufferObject::Unbind(GL_ELEMENT_ARRAY_BUFFER);
  gl::ArrayObject::Unbind();
}

void VR1Pass::DestroyRenderingShaders ()
{
  if (shader_rendering)
    delete shader_rendering;
  shader_rendering = NULL;

  gl::ExitOnGLError("Could not destroy the shaders!");
}

void VR1Pass::DestroyBufferData ()
{
  if (quad_vbo)
    delete quad_vbo;
  quad_vbo = NULL;

  if (quad_ibo)
    delete quad_ibo;
  quad_ibo = NULL;

  if (quad_vao)
    delete quad_vao;
  quad_vao = NULL;

  gl::ExitOnGLError("Could not destroy buffers");
}
