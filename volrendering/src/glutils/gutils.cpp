#include "gutils.h"

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cerrno>

namespace gl
{
  void ExitOnGLError(const char* error_message)
  {
    const GLenum ErrorValue = glGetError();

    if (ErrorValue != GL_NO_ERROR)
    {
      const char* APPEND_DETAIL_STRING = ": %s\n";
      const size_t APPEND_LENGTH = strlen(APPEND_DETAIL_STRING) + 1;
      const size_t message_length = strlen(error_message);
      char* display_message = (char*)malloc(message_length + APPEND_LENGTH);

      memcpy(display_message, error_message, message_length);
      memcpy(&display_message[message_length], APPEND_DETAIL_STRING, APPEND_LENGTH);

      fprintf(stderr, display_message, gluErrorString(ErrorValue));

      free(display_message);
      exit(EXIT_FAILURE);
    }
  }

  char* TextFileRead (const char* file_name)
  {
#ifdef _DEBUG
    printf ("File Name TextFileRead \"%s\"\n", file_name);
#endif
    FILE *file_source;
    errno_t err;

    char *content = NULL;
    int count = 0;
    if (file_name != NULL)
    {
      err = fopen_s(&file_source, file_name, "rt");

      if (file_source != NULL)
      {
        fseek (file_source, 0, SEEK_END);
        count = ftell (file_source);
        rewind (file_source);

        if (count > 0) {
          content = (char *)malloc (sizeof (char)* (count + 1));
          count = (int)fread (content, sizeof (char), count, file_source);
          content[count] = '\0';
        }
        fclose (file_source);
      }
      else
      {
        printf ("\nFile \"%s\" not found", file_name);
        getchar ();
        exit (1);
      }
    }
    return content;
  }
}