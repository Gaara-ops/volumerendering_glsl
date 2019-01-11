#ifndef VOLREND_UTILS_H
#define VOLREND_UTILS_H

#include "../glutils/texture3d.h"
#include "../glutils/texture2d.h"

#include "transferfunction.h"
#include "volume.h"
#include "summedareatable.h"

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

  gl::Texture3D* GenerateRGBATexture (vr::Volume* vol, vr::TransferFunction* tf,
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
  
  //https://stackoverflow.com/questions/1972172/interpolating-a-scalar-field-in-a-3d-space
  //https://www.ncbi.nlm.nih.gov/pmc/articles/PMC3719212/

  gl::Texture2D* GenerateNoiseTexture (float maxvalue, int w, int h);

  void GenerateSyntheticVolumetricModels (int d = 120, float s = 30.0f);
  
  gl::Texture3D* GenerateExtinctionSAT3DTex (Volume* vol, TransferFunction* tf);

  gl::Texture3D* GenerateScalarFieldSAT3DTex (Volume* vol);
}

#endif