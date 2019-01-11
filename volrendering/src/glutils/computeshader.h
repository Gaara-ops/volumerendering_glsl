/**
 * OpenGL Compute Shader Class Implementation
**/
#ifndef COMPUTE_SHADER_H
#define COMPUTE_SHADER_H

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <iostream>
#include <string>

#include "gutils.h"

namespace gl
{
  class ComputeShader
  {
  public:
    ComputeShader ();
    ~ComputeShader ();

    void SetShaderFile (std::string filename);
    void LoadAndLink ();

    void Bind ();
    static void Unbind ();

    GLuint GetProgramID ();
  
  protected:

  private:
    void CompileShader (GLuint shader_id, std::string filename);

    std::string m_shader_file;

    GLuint m_shader_program;
    GLuint m_compute_shader;
  };
}

#endif