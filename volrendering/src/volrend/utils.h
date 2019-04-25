#ifndef VOLREND_UTILS_H
#define VOLREND_UTILS_H

#include "../glutils/texture3d.h"
#include "../glutils/texture2d.h"

#include "transferfunction.h"
#include "volume.h"

#include <glm/glm.hpp>

#define USE_16F_INTERNAL_FORMAT

namespace vr
{
  typedef struct Vertex {
    float Position[3];
    float Color[3];
  } Vertex;

  typedef struct Vertex1p {
    float Position[3];
  } Vertex1p;

  typedef struct Vertex2p {
    float Position[3];
    float Color[3];
  } Vertex2p;

  gl::Texture3D* GenerateRTexture (vr::Volume* vol,
                                     int init_x = 0,
                                     int init_y = 0,
                                     int init_z = 0,
                                     int last_x = 0,
                                     int last_y = 0,
                                     int last_z = 0);

  gl::Texture3D* GenerateGradientTexture (vr::Volume* vol,
                                            int gradient_sample_size = 1,
                                            int filter_nxnxn = 0,
                                            bool normalized_gradient = true,
                                            int init_x = -1,
                                            int init_y = -1,
                                            int init_z = -1,
                                            int last_x = -1,
                                            int last_y = -1,
                                            int last_z = -1);

  // https://en.wikipedia.org/wiki/Sobel_operator  
  gl::Texture3D* GenerateSobelFeldmanGradientTexture (Volume* vol);
  
  void GenerateSyntheticVolumetricModels (int d = 120, float s = 30.0f);
}

#endif