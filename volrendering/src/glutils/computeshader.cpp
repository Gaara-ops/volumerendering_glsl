#include "computeshader.h"
#include "gutils.h"

namespace gl
{
  ComputeShader::ComputeShader ()
  {

  }

  ComputeShader::~ComputeShader ()
  {
    Unbind();

    glDetachShader(m_shader_program, m_compute_shader);
    glDeleteShader(m_compute_shader);
   
    glDeleteProgram(m_shader_program);
  }

  void ComputeShader::SetShaderFile (std::string filename)
  {
    m_shader_file = filename;
  }

  void ComputeShader::LoadAndLink ()
  {
    // Creates a compute shader and the respective program that
    //  contains the shader.
    m_shader_program = glCreateProgram();
    m_compute_shader = glCreateShader(GL_COMPUTE_SHADER);

    CompileShader(m_compute_shader, m_shader_file);
    
    glAttachShader(m_shader_program, m_compute_shader);
    glLinkProgram(m_shader_program);
    
    // Check link status
    int rvalue;
    glGetProgramiv(m_shader_program, GL_LINK_STATUS, &rvalue);
    if (!rvalue) {
      fprintf(stderr, "Error in linking compute shader program\n");
      GLchar log[10240];
      GLsizei length;
      glGetProgramInfoLog(m_shader_program, 10239, &length, log);
      fprintf(stderr, "Linker log:\n%s\n", log);
      exit(41);
    }

    gl::ExitOnGLError("gl::ComputeShader >> Unable to load and link shaders.");
  }

  void ComputeShader::Bind ()
  {
    glUseProgram(m_shader_program);
    gl::ExitOnGLError ("gl::ComputeShader >> Could not bind the shader program!");
  }

  void ComputeShader::Unbind ()
  {
    glUseProgram(0);
  }

  GLuint ComputeShader::GetProgramID ()
  {
    return m_shader_program;
  }

  void ComputeShader::CompileShader (GLuint shader_id, std::string filename)
  {
    char* shader_source = gl::TextFileRead(filename.c_str());
    const char* const_shader_source = shader_source;

    // Second parameters can be > 1 if const_shader_source is an array.
    glShaderSource(shader_id, 1, &const_shader_source, NULL);
    free(shader_source);

    glCompileShader(shader_id);
    
    // Check compilation status
    int rvalue;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &rvalue);
    if (!rvalue) {
      fprintf(stderr, "Error in compiling the compute shader\n");
      GLchar log[10240];
      GLsizei length;
      glGetShaderInfoLog(shader_id, 10239, &length, log);
      fprintf(stderr, "Compiler log:\n%s\n", log);
      exit(40);
    }

    gl::ExitOnGLError ("gl::ComputeShader >> Could not compile the shader file!");
  }
}