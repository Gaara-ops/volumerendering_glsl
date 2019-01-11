#include "datamanager.h"

#include <fstream>

#include "volrend/utils.h"

DataManager::DataManager()
  : curr_volume_index(0)
  , curr_transferfunction_index(0)
  , curr_vr_volume(NULL)
  , curr_gl_volm_texture(NULL)
  , curr_gl_grad_texture(NULL)
  , curr_vr_transferfunction(NULL)
{
  use_sobel_to_compute_gradients = 2;
}

DataManager::~DataManager()
{
  DeleteVolumeData();

  if (curr_vr_transferfunction) delete curr_vr_transferfunction;
  curr_vr_transferfunction = NULL;
}

void DataManager::Init ()
{
  ///////////////////////////////////////////////////////
  // Read all models
  std::string line;
  std::string model_read_filename = PATH_TO_RESOURCES;
  model_read_filename.append("/volrend_datasets");
  std::ifstream f_openmodels(model_read_filename);
  if (!f_openmodels.is_open())
  {
    std::cout << "Error: Unable to read vol rendering datasets." << std::endl;
    exit(EXIT_FAILURE);
  }

  while (!f_openmodels.eof())
  {
    line.clear();
    std::getline(f_openmodels, line);

    int start_path = line.find_first_of("<") + 1;
    int end_path = line.find_first_of(">");

    int start_name = line.find_last_of("<") + 1;
    int end_name = line.find_last_of(">");

    stored_models.push_back(DataReference(
      line.substr(start_path, end_path - start_path),
      line.substr(start_name, end_name - start_name)
    ));
  }
  f_openmodels.close();
  ///////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////
  // Read all transfer functions
  std::string tfunc_read_filename = PATH_TO_RESOURCES;
  tfunc_read_filename.append("/volrend_transfun");
  std::ifstream f_opentransferfunctions(tfunc_read_filename);
  if (!f_opentransferfunctions.is_open())
  {
    std::cout << "Error: Unable to read vol rendering transfer functions." << std::endl;
    exit(EXIT_FAILURE);
  }

  while (!f_opentransferfunctions.eof())
  {
    line.clear();
    std::getline(f_opentransferfunctions, line);

    int start_path = line.find_first_of("<") + 1;
    int end_path = line.find_first_of(">");

    int start_name = line.find_last_of("<") + 1;
    int end_name = line.find_last_of(">");

    stored_transfer_functions.push_back(DataReference(
      line.substr(start_path, end_path - start_path),
      line.substr(start_name, end_name - start_name)
    ));
  }
  f_opentransferfunctions.close();
  ///////////////////////////////////////////////////////

  curr_vr_volume = vr_reader.ReadVolume(stored_models[curr_volume_index].path);
  curr_vr_volume->SetName(stored_models[curr_volume_index].name);

  std::cout << ". Generating Volume Texture..." << std::endl;
  curr_gl_volm_texture = vr::GenerateRTexture(curr_vr_volume, 0, 0, 0,
    curr_vr_volume->GetWidth(),
    curr_vr_volume->GetHeight(),
    curr_vr_volume->GetDepth());

  std::cout << ". Generating Gradient Texture..." << std::endl;
  GenerateGradientTex();

  curr_vr_transferfunction = vr::ReadTransferFunction(stored_transfer_functions[curr_transferfunction_index].path);
  curr_vr_transferfunction->SetName(stored_transfer_functions[curr_transferfunction_index].name);
}

vr::Volume* DataManager::GetCurrentVolume()
{
  return curr_vr_volume;
}

std::string DataManager::GetCurrentVolumeName()
{
  return stored_models[curr_volume_index].name;
}

unsigned int DataManager::GetCurrentVolumeIndex()
{
  return curr_volume_index;
}

gl::Texture3D* DataManager::GetCurrentVolumeTexture()
{
  return curr_gl_volm_texture;
}

gl::Texture3D* DataManager::GetCurrentGradientTexture()
{
  return curr_gl_grad_texture;
}

bool DataManager::PreviousVolume()
{
  if (curr_volume_index > 0)
  {
    curr_volume_index -= 1;

    DeleteVolumeData();

    curr_vr_volume = vr_reader.ReadVolume(stored_models[curr_volume_index].path);
    curr_vr_volume->SetName(stored_models[curr_volume_index].name);

    curr_gl_volm_texture = vr::GenerateRTexture(curr_vr_volume, 0, 0, 0,
      curr_vr_volume->GetWidth(),
      curr_vr_volume->GetHeight(),
      curr_vr_volume->GetDepth());
    GenerateGradientTex();

    return true;
  }
  return false;
}

bool DataManager::NextVolume()
{
  if (curr_volume_index + 1 < stored_models.size())
  {
    curr_volume_index += 1;

    DeleteVolumeData();

    curr_vr_volume = vr_reader.ReadVolume(stored_models[curr_volume_index].path);
    curr_vr_volume->SetName(stored_models[curr_volume_index].name);

    curr_gl_volm_texture = vr::GenerateRTexture(curr_vr_volume, 0, 0, 0,
      curr_vr_volume->GetWidth(),
      curr_vr_volume->GetHeight(),
      curr_vr_volume->GetDepth());
    GenerateGradientTex();

    return true;
  }
  return false;
}

vr::TransferFunction* DataManager::GetCurrentTransferFunction()
{
  return curr_vr_transferfunction;
}

std::string DataManager::GetCurrentTransferFunctionName()
{
  return stored_transfer_functions[curr_transferfunction_index].name;
}

unsigned int DataManager::GetCurrentTransferFunctionIndex()
{
  return curr_transferfunction_index;
}

bool DataManager::PreviousTransferFunction()
{
  if (curr_transferfunction_index > 0)
  {
    curr_transferfunction_index -= 1;
    curr_vr_transferfunction = vr::ReadTransferFunction(stored_transfer_functions[curr_transferfunction_index].path);
    curr_vr_transferfunction->SetName(stored_transfer_functions[curr_transferfunction_index].name);
    return true;
  }
  return false;
}

bool DataManager::NextTransferFunction()
{
  std::cout << curr_transferfunction_index << " " << stored_transfer_functions.size() << std::endl;
  if (curr_transferfunction_index + 1 < stored_transfer_functions.size())
  {
    curr_transferfunction_index += 1;
    curr_vr_transferfunction = vr::ReadTransferFunction(stored_transfer_functions[curr_transferfunction_index].path);
    curr_vr_transferfunction->SetName(stored_transfer_functions[curr_transferfunction_index].name);
    return true;
  }
  return false;
}

bool DataManager::UpdateGradientTexture()
{
  DeleteGradientTexture();

  use_sobel_to_compute_gradients = (use_sobel_to_compute_gradients + 1) % 3;

  return GenerateGradientTex();
}

std::string DataManager::CurrentGradientName()
{
  if (use_sobel_to_compute_gradients == 0)
    return "Sobel Feldman";
  else if (use_sobel_to_compute_gradients == 1)
    return "First Derivate";
  return "NULL";
}

bool DataManager::GenerateGradientTex()
{
  if (use_sobel_to_compute_gradients == 0)
  {
    std::cout << "Gradients: Sobel Feldman." << std::endl;
    curr_gl_grad_texture = vr::GenerateSobelFeldmanGradientTexture(curr_vr_volume);
  }
  else if (use_sobel_to_compute_gradients == 1)
  {
    std::cout << "Gradients: First Derivate." << std::endl;
    curr_gl_grad_texture = vr::GenerateGradientTexture(curr_vr_volume);
  }
  else
  {
    std::cout << "Gradients: NULL." << std::endl;
    curr_gl_grad_texture = NULL;
    return false;
  }
  return true;
}

void DataManager::DeleteVolumeData()
{
  if (curr_vr_volume) delete curr_vr_volume;
  curr_vr_volume = NULL;

  if (curr_gl_volm_texture) delete curr_gl_volm_texture;
  curr_gl_volm_texture = NULL;

  DeleteGradientTexture();
}

void DataManager::DeleteGradientTexture()
{
  if (curr_gl_grad_texture) delete curr_gl_grad_texture;
  curr_gl_grad_texture = NULL;
}