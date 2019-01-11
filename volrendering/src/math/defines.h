#ifndef MATH_DEFINES_H
#define MATH_DEFINES_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#ifndef DEGREE_TO_RADIANS
  #define DEGREE_TO_RADIANS(s) (s * (glm::pi<double>() / 180.0))
#endif

#endif
