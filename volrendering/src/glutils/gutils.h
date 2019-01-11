#ifndef GL_UTILS_H
#define GL_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <ctime>
#include <vector>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>

namespace gl
{
  void ExitOnGLError (const char* error_message);
  char* TextFileRead (const char* file_name);
}

#endif