#ifndef RENDERING_PARAMETERS_H
#define RENDERING_PARAMETERS_H

#include <glm/glm.hpp>

#include <cstring>
#include <sstream>

class RenderingParameters
{
public:
  RenderingParameters();
  ~RenderingParameters();

  void SetStepSize(float step);
  float GetStepSize();

  ///////////////////////////////////////
  // Phong
  void ApplyBlinnPhongShading(bool apply_phong_shading);
  bool IsUsingBlinnPhongShading();

  void SetPhongParameters(float amb, float diff, float spec, float shini, glm::vec3 colorspec);
  float GetPhongKambient();
  float GetPhongKdiffuse();
  float GetPhongKspecular();
  float GetPhongNshininess();
  glm::vec3 GetPhongIspecular();

  void SetBlinnPhongLightingPosition (glm::vec3 lightpos);
  glm::vec3 GetBlinnPhongLightingPosition ();

  ///////////////////////////////////////
  // Screen/Window/Callback properties
  void SetScreenSize(int width, int height);
  int GetScreenWidth();
  int GetScreenHeight();
  void SetMouseLastCoordinates(int mx, int my);
  int GetMouseLastX();
  int GetMouseLastY();

  int uchartoint(unsigned char c)
  {
    std::stringstream str;
    str << c;

    int x;
    str >> x;

    return x;
  }

protected:
  float fov_y;
  float cam_near;
  float cam_far;
  float step_size;

  struct Phong {
    bool active;

    float Kambient;
    float Kdiffuse;
    float Kspecular;
    float Nshininess;
    glm::vec3 Ispecular;

    glm::vec3 lightpos;
  };
  Phong phong_params;

  int screen_width;
  int screen_height;

  int mouse_last_x;
  int mouse_last_y;

private:

};

#endif