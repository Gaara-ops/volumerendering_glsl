#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <iostream>

#include "defines.h"

class SimpleAroundCamera
{
public:
  enum CAM_CONFIGS : unsigned int {
    PX_Y = 0,
    NX_Y = 1,
    PY_Z = 2,
    NY_Z = 3,
    PZ_X = 4,
    NZ_X = 5,
    CUSTOM = 6,
  };
  unsigned int m_current_config;

public:
  SimpleAroundCamera (float _radius, float _min_rad = 1.0f, float _max_rad = 25000.0f);
  ~SimpleAroundCamera ();

  void SetInitialConfig (glm::vec3 _center, glm::vec3 _up);
  void SetInitialConfig (unsigned int cam_cfg);

  int MouseButton (int bt, int st, int x, int y);
  int MouseMotion (int x, int y);

  void SetSpeedRadius (float spd);

  void SetNextInitialConfig ();
  void SetPreviousInitialConfig ();

  glm::mat4 LookAt ();
  glm::mat4 Projection ();

  glm::vec3 GetEye ();
  glm::vec3 GetDir ();
  glm::vec3 GetUp()
  {
    return up;
  }
  
  void UpdateAspectRatio (float w, float h);
  float GetAspectRatio ();
  float GetFovY ();
  float GetTanFovY ();

  void GetCameraVectors (glm::vec3* forward, glm::vec3* up, glm::vec3* right);

  bool IsChangingCenterVectors ();
  void ChangeCenterVectors (bool chgvec);

  float cam_near, cam_far;
protected:
  glm::vec3 eye,center,up;
  float radius;
  float speed;
  float speed_radius;

  float min_radius;
  float max_radius;

  float fovy;
  float aspect_ratio;
  bool perspective;

  bool change_center;
  bool change_center_x;
  bool change_center_y;
  bool change_center_z;

private:
  int last_mx, last_my;
  int cur_mx, cur_my;
  bool arcball_on;
  bool changing_radius;
};