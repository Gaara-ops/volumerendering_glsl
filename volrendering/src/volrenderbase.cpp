#include "volrenderbase.h"

BaseVolumeRenderer::BaseVolumeRenderer ()
{
  glsl_transfer_function = NULL;

  SetBuilt(false);

  SetOutdated();
}

BaseVolumeRenderer::~BaseVolumeRenderer ()
{
  Destroy();
}

void BaseVolumeRenderer::SetRenderingParamsAndDataManager (RenderingParameters* rendering_params, DataManager* data_mgr)
{
  rdr_params = rendering_params;
  data_manager = data_mgr;
}

void BaseVolumeRenderer::Destroy ()
{
  DestroyTransferFunction();
  gl::ExitOnGLError("ERROR: Could not destroy data");

  SetBuilt(false);
}

void BaseVolumeRenderer::ReloadShaders ()
{
}

bool BaseVolumeRenderer::UpdateGradientTexture ()
{
  SetOutdated();
  return true;
}

bool BaseVolumeRenderer::UpdateTransferFunction ()
{
  SetOutdated();
  return true;
}

void BaseVolumeRenderer::DestroyTransferFunction ()
{
  if (glsl_transfer_function)
    delete glsl_transfer_function;
  glsl_transfer_function = NULL;
}

void BaseVolumeRenderer::Render ()
{

}

void BaseVolumeRenderer::Reshape (int w, int h)
{
}

bool BaseVolumeRenderer::KeyboardUp (unsigned char key, int x, int y)
{
  return false;
}

bool BaseVolumeRenderer::OnMouse (int bt, int st, int x, int y)
{
  return false;
}

bool BaseVolumeRenderer::BindCommonUniforms (SimpleAroundCamera* camera, gl::Shader* ref_shader)
{
  ref_shader->Bind();
  ref_shader->BindUniforms();

  ref_shader->SetUniform("VolumeScales", data_manager->GetCurrentVolume()->GetScale());
  ref_shader->BindUniform("VolumeScales");

  //////////////////////////////////////////////////////////////////
  // General
  ref_shader->SetUniform("StepSize", rdr_params->GetStepSize());
  ref_shader->BindUniform("StepSize");

  ref_shader->SetUniform("WorldEyePos", camera->GetEye());
  ref_shader->BindUniform("WorldEyePos");

  ref_shader->SetUniform("WorldLightingPos", rdr_params->GetBlinnPhongLightingPosition());
  ref_shader->BindUniform("WorldLightingPos");

  //////////////////////////////////////////////////////////////////
  // Blinn-Phong Parameters
  ref_shader->SetUniform("ApplyPhongShading", rdr_params->IsUsingBlinnPhongShading() ? 1 : 0);
  ref_shader->BindUniform("ApplyPhongShading");

  ref_shader->SetUniform("Kambient", rdr_params->GetPhongKambient());
  ref_shader->BindUniform("Kambient");
  ref_shader->SetUniform("Kdiffuse", rdr_params->GetPhongKdiffuse());
  ref_shader->BindUniform("Kdiffuse");
  ref_shader->SetUniform("Kspecular", rdr_params->GetPhongKspecular());
  ref_shader->BindUniform("Kspecular");
  ref_shader->SetUniform("Nshininess", rdr_params->GetPhongNshininess());
  ref_shader->BindUniform("Nshininess");

  ref_shader->SetUniform("Ispecular", rdr_params->GetPhongIspecular());
  ref_shader->BindUniform("Ispecular");
  
  /////////////////////////////////////////////////////////////////
  gl::Shader::Unbind();
  gl::ExitOnGLError("Bind Common Shader Uniforms.");
  
  /////////////////////////////////////////////////////////////////
  return true;
}

void BaseVolumeRenderer::PrintRendererParameters ()
{

}

void BaseVolumeRenderer::PrepareRender (SimpleAroundCamera* camera)
{
  if (IsOutdated())
  {
    UpdateShaderParameters(camera);
    vr_outdated = false;
  }
}

void BaseVolumeRenderer::SetOutdated ()
{
  vr_outdated = true;
}

bool BaseVolumeRenderer::IsOutdated ()
{
  return vr_outdated;
}

void BaseVolumeRenderer::SetBuilt (bool b_built)
{
  vr_built = b_built;
}

bool BaseVolumeRenderer::IsBuilt ()
{
  return vr_built;
}