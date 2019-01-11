#ifndef DATA_MANAGER_VOLUME_RENDERING_H
#define DATA_MANAGER_VOLUME_RENDERING_H

#include <iostream>
#define PATH_TO_RESOURCES "../resources/"

#include "volrend/utils.h"
#include "volrend/reader.h"
#include "glutils/texture3d.h"
#include "glutils/texture1d.h"

class DataReference
{
public:
  DataReference() {}
  DataReference(std::string _path, std::string _name = "")
  {
    path = std::string(PATH_TO_RESOURCES);
    path.append(_path);
    name = _name;
    if (name == "")
      name = path.substr(path.find_last_of('/') + 1);
  }

  std::string path;
  std::string name;
};

// A class that encapsulates all datasets/transfer functions
// TODO: Get it from a txt/input file
class DataManager
{
public:
  DataManager();
  ~DataManager();

  void Init ();

  vr::Volume* GetCurrentVolume();
  std::string GetCurrentVolumeName();
  unsigned int GetCurrentVolumeIndex();

  gl::Texture3D* GetCurrentVolumeTexture();
  gl::Texture3D* GetCurrentGradientTexture();

  bool PreviousVolume();
  bool NextVolume();

  vr::TransferFunction* GetCurrentTransferFunction();
  std::string GetCurrentTransferFunctionName();
  unsigned int GetCurrentTransferFunctionIndex();

  bool PreviousTransferFunction();
  bool NextTransferFunction();

  bool UpdateGradientTexture();
  std::string CurrentGradientName();

protected:
  bool GenerateGradientTex();
private:
  vr::VolumeRenderingReader vr_reader;

  unsigned int curr_volume_index;
  unsigned int curr_transferfunction_index;

  vr::Volume* curr_vr_volume;
  gl::Texture3D* curr_gl_volm_texture;
  gl::Texture3D* curr_gl_grad_texture;
  int use_sobel_to_compute_gradients;

  void DeleteVolumeData();
  void DeleteGradientTexture();

  vr::TransferFunction* curr_vr_transferfunction;

  std::vector<DataReference> stored_models;
  std::vector<DataReference> stored_transfer_functions;
};

#endif