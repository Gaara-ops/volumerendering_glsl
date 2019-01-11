#include "simplearoundcamera.h"
#include <cstdio>
#include <iostream>

SimpleAroundCamera::SimpleAroundCamera (float _radius, float _min_rad, float _max_rad)
{
  radius = _radius;
  speed = 0.001f;
  speed_radius = 1.0f;

  min_radius = _min_rad;
  max_radius = _max_rad;

  m_current_config = SimpleAroundCamera::CAM_CONFIGS::CUSTOM;

  fovy = 45.0f;
  aspect_ratio = 1.0f;
  cam_near = 1.0f;
  cam_far = 1000.0f;

  perspective = true;
  change_center = false;
}

SimpleAroundCamera::~SimpleAroundCamera ()
{}

void SimpleAroundCamera::SetInitialConfig (glm::vec3 _center, glm::vec3 _up)
{
  m_current_config = SimpleAroundCamera::CAM_CONFIGS::CUSTOM;

  center = _center;
  eye = glm::vec3(_center.x, _center.y, _center.z + radius);
  up = _up;
}

void SimpleAroundCamera::SetInitialConfig (unsigned int cam_cfg)
{
  m_current_config = cam_cfg;

  switch (cam_cfg)
  {
  case PX_Y:
    center = glm::vec3(0, 0, 0);
    eye = center + glm::vec3(0, radius, 0);
    up = glm::vec3(1, 0, 0);
    break;
  case NX_Y:
    center = glm::vec3(0, 0, 0);
    eye = center + glm::vec3(0, radius, 0);
    up = glm::vec3(-1, 0, 0);
    break;
  case PY_Z:
    center = glm::vec3(0, 0, 0);
    eye = center + glm::vec3(0, 0, radius);
    up = glm::vec3(0, 1, 0);
    break;
  case NY_Z:
    center = glm::vec3(0, 0, 0);
    eye = center + glm::vec3(0, 0, radius);
    up = glm::vec3(0, -1, 0);
    break;
  case PZ_X:
    center = glm::vec3(0, 0, 0);
    eye = center + glm::vec3(radius, 0, 0);
    up = glm::vec3(0, 0, 1);
    break;
  case NZ_X:
    center = glm::vec3(0, 0, 0);
    eye = center + glm::vec3(radius, 0, 0);
    up = glm::vec3(0, 0, -1);
    break;
  default:
    break;
  };
}

int SimpleAroundCamera::MouseButton (int bt, int st, int x, int y)
{
  if (change_center)
  {
    if (st == 0 && bt == 0) {
      change_center_x = true;

      last_my = cur_my = y;
      last_mx = cur_mx = x;
    } else if (st == 0 && bt == 1) {
      change_center_y = true;

      last_my = cur_my = y;
      last_mx = cur_mx = x;
    } else if (st == 0 && bt == 2) {
      change_center_z = true;

      last_my = cur_my = y;
      last_mx = cur_mx = x;
    }
    else if (st == 1) {
      change_center_x = false;
      change_center_y = false;
      change_center_z = false;
    }
  }
  else
  {
    if (st == 0 && bt == 0) {
      arcball_on = true;
      changing_radius = false;

      last_my = cur_my = y;
      last_mx = cur_mx = x;
    } else if (st == 0 && (bt == 1 || bt == 2)) {
      speed_radius = bt == 1 ? 1.0f : 0.01f;
      changing_radius = true;
      arcball_on = false;

      last_my = cur_my = y;
      last_mx = cur_mx = x;
    } else if (st == 1 && bt == 0) {
      arcball_on = false;
    } else if (st == 1 && (bt == 1 || bt == 2)) {
      changing_radius = false;
    }
  }

  return 0;
}

int SimpleAroundCamera::MouseMotion (int x, int y)
{
  if (change_center)
  {
    float yrot = -(x - last_mx);
    if (change_center_x) {
      center.x = center.x + yrot;
    } 

    if (change_center_y) {
      center.y = center.y + yrot;
    }

    if (change_center_z) {
      center.z = center.z + yrot;
    }

    last_mx = cur_mx;
    cur_mx = x;
  }
  else
  {
    if (arcball_on) {
      float xrot = -(y - last_my) * speed;
      float yrot = -(x - last_mx) * speed;

      glm::quat p = glm::quat(0, eye.x, eye.y, eye.z);

      glm::quat qy = glm::quat(cos(yrot), sin(yrot)*up);

      glm::vec3 loc_up = up;

      float max = 0.99f;
      float dt = glm::dot(glm::normalize(glm::vec3(center - eye)), loc_up);
      if ((dt > max && xrot > 0.0f) || (dt < -max && xrot < 0.0f))
        xrot = 0.0f;

      glm::vec3 vr = glm::normalize(glm::cross(glm::normalize(glm::vec3(center - eye)), loc_up));
      glm::quat qx = glm::quat(cos(xrot), sin(xrot)*vr);

      glm::quat rq =
        glm::cross(glm::cross(glm::cross(glm::cross(qx, qy), p),
        glm::inverse(qy)), glm::inverse(qx));

      eye = glm::vec3(rq.x, rq.y, rq.z);


      last_mx = cur_mx;
      last_my = cur_my;
      cur_mx = x;
      cur_my = y;
    }
    //////////////////////////////////////////
    //////////////////////////////////////////
    if (changing_radius) {
      float ydiff = (y - last_my) * speed_radius;

      radius += ydiff;
      if (radius < min_radius)
        radius = min_radius;
      if (radius > max_radius)
        radius = max_radius;


      glm::vec3 c_e = glm::normalize(glm::vec3(eye - center));

      eye = c_e * radius;

      last_my = cur_my;
      cur_my = y;
    }
  }

  return 0;
}

void SimpleAroundCamera::SetSpeedRadius (float spd)
{
  speed_radius = spd;
}

void SimpleAroundCamera::SetNextInitialConfig ()
{
  if (m_current_config < SimpleAroundCamera::CAM_CONFIGS::NZ_X)
  {
    m_current_config = m_current_config + 1;
  }
  SetInitialConfig(m_current_config);
}

void SimpleAroundCamera::SetPreviousInitialConfig ()
{
  if (m_current_config > SimpleAroundCamera::CAM_CONFIGS::PX_Y)
  {
    m_current_config = m_current_config - 1;
  }
  SetInitialConfig(m_current_config);
}

glm::mat4 SimpleAroundCamera::LookAt ()
{
  return glm::lookAt(eye, center, up);
}

glm::mat4 SimpleAroundCamera::Projection ()
{
  if (perspective)
  {
    glm::mat4 perspection = glm::perspective(fovy, aspect_ratio, cam_near, cam_far);
    //std::cout << fovy << " " << aspect_ratio << " " << cam_near << " " << cam_far << std::endl;
    //std::cout << rola[0][0] << " " << rola[0][1] << " " << rola[0][2] << " " << rola[0][3] << std::endl;
    //std::cout << rola[1][0] << " " << rola[1][1] << " " << rola[1][2] << " " << rola[1][3] << std::endl;
    //std::cout << rola[2][0] << " " << rola[2][1] << " " << rola[2][2] << " " << rola[2][3] << std::endl;
    //std::cout << rola[3][0] << " " << rola[3][1] << " " << rola[3][2] << " " << rola[3][3] << std::endl;
    return perspection;
  }
  else
  {
    printf("TODO: Ortographic\n");
    return glm::mat4();
  }
}

glm::vec3 SimpleAroundCamera::GetEye ()
{
  return eye;
}

glm::vec3 SimpleAroundCamera::GetDir ()
{
  return glm::normalize(center - eye);
}

void SimpleAroundCamera::UpdateAspectRatio (float w, float h)
{
  aspect_ratio = w / h;
}

float SimpleAroundCamera::GetAspectRatio ()
{
  return aspect_ratio;
}

float SimpleAroundCamera::GetFovY ()
{
  return fovy;
}

float SimpleAroundCamera::GetTanFovY ()
{
  return (float)tan(DEGREE_TO_RADIANS(GetFovY()) / 2.0);
}

void SimpleAroundCamera::GetCameraVectors  (glm::vec3* cforward, glm::vec3* cup, glm::vec3* cright)
{
  (*cforward) = -GetDir();
  (*cright) = glm::normalize(glm::cross(up, (*cforward)));
  (*cup) = glm::normalize(glm::cross((*cforward), (*cright)));
}

bool SimpleAroundCamera::IsChangingCenterVectors ()
{
  return change_center;
}
void SimpleAroundCamera::ChangeCenterVectors (bool chgvec)
{
  change_center = chgvec;
  if (change_center)
  {

  }
  else
  {
    change_center_x = false;
    change_center_y = false;
    change_center_z = false;
  }
}
