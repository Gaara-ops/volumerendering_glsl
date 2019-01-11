#include "renderingparameters.h"

#include <glm/glm.hpp>

RenderingParameters::RenderingParameters()
{
  fov_y = 45;
  cam_near = 1.0f;
  cam_far = 1000.0f;
  step_size = 0.5f;

  phong_params.active = false;
  phong_params.Kambient = 0.4f;
  phong_params.Kdiffuse = 0.4f;
  phong_params.Kspecular = 0.2f;
  phong_params.Nshininess = 50.0f;
  phong_params.Ispecular = glm::vec3(1);

  phong_params.lightpos = glm::vec3(0, 0, 1000);

}

RenderingParameters::~RenderingParameters() {}

void RenderingParameters::SetStepSize(float step)
{
  if (step > 0 && step != step_size)
  {
    step_size = step;
  }
}

float RenderingParameters::GetStepSize()
{
  return step_size;
}

void RenderingParameters::ApplyBlinnPhongShading(bool apply_phong_shading)
{
  phong_params.active = apply_phong_shading;
}

bool RenderingParameters::IsUsingBlinnPhongShading()
{
  return phong_params.active;
}

void RenderingParameters::SetPhongParameters(float amb, float diff, float spec, float shini, glm::vec3 colorspec)
{
  phong_params.Kambient = amb;
  phong_params.Kdiffuse = diff;
  phong_params.Kspecular = spec;
  phong_params.Nshininess = shini;
  phong_params.Ispecular = colorspec;
}

float RenderingParameters::GetPhongKambient()
{
  return phong_params.Kambient;
}

float RenderingParameters::GetPhongKdiffuse()
{
  return phong_params.Kdiffuse;
}

float RenderingParameters::GetPhongKspecular()
{
  return phong_params.Kspecular;
}

float RenderingParameters::GetPhongNshininess()
{
  return phong_params.Nshininess;
}

glm::vec3 RenderingParameters::GetPhongIspecular()
{
  return phong_params.Ispecular;
}

void RenderingParameters::SetBlinnPhongLightingPosition (glm::vec3 lightpos)
{
  phong_params.lightpos = lightpos;
}

glm::vec3 RenderingParameters::GetBlinnPhongLightingPosition ()
{
  return phong_params.lightpos;
}

void RenderingParameters::SetScreenSize(int width, int height)
{
  screen_width = width;
  screen_height = height;
}

int RenderingParameters::GetScreenWidth()
{
  return screen_width;
}

int RenderingParameters::GetScreenHeight()
{
  return screen_height;
}

void RenderingParameters::SetMouseLastCoordinates(int mx, int my)
{
  mouse_last_x = mx;
  mouse_last_y = my;
}

int RenderingParameters::GetMouseLastX()
{
  return mouse_last_x;
}

int RenderingParameters::GetMouseLastY()
{
  return mouse_last_y;
}
