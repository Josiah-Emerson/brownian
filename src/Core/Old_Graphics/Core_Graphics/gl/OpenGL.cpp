#include "Core_Graphics/gl/OpenGL.h"
#include <GL/gl.h>
#include <stdexcept>
// MACROS
// Load function
#define FETCH_GL_FUNC(FTYPE, FNAME) do{ \
   (FNAME) = (FTYPE) window.getProcAddress((char*)#FNAME); \
   if(!(FNAME)) \
      throw std::runtime_error("Could not load OpenGL function "#FNAME); \
}while(0)

namespace Core{
   OpenGL::OpenGL(Core::Window& window){
      FETCH_GL_FUNC(PFNGLATTACHSHADERPROC, glAttachShader);
      FETCH_GL_FUNC(PFNGLBINDBUFFERPROC, glBindBuffer);
      FETCH_GL_FUNC(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray);
      FETCH_GL_FUNC(PFNGLBUFFERDATAPROC, glBufferData);
      FETCH_GL_FUNC(PFNGLCLEARPROC, glClear);
      FETCH_GL_FUNC(PFNGLCLEARCOLORPROC, glClearColor);
      FETCH_GL_FUNC(PFNGLCOMPILESHADERPROC, glCompileShader);
      FETCH_GL_FUNC(PFNGLCREATEPROGRAMPROC, glCreateProgram);
      FETCH_GL_FUNC(PFNGLCREATESHADERPROC, glCreateShader);
      FETCH_GL_FUNC(PFNGLDEPTHFUNCPROC, glDepthFunc);
      FETCH_GL_FUNC(PFNGLDELETEPROGRAMPROC, glDeleteProgram);
      FETCH_GL_FUNC(PFNGLDELETESHADERPROC,  glDeleteShader);
      FETCH_GL_FUNC(PFNGLDRAWARRAYSPROC, glDrawArrays);
      FETCH_GL_FUNC(PFNGLENABLEPROC, glEnable);
      FETCH_GL_FUNC(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);
      FETCH_GL_FUNC(PFNGLGENBUFFERSPROC, glGenBuffers);
      FETCH_GL_FUNC(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays);
      FETCH_GL_FUNC(PFNGLGETACTIVEATTRIBPROC, glGetActiveAttrib);
      FETCH_GL_FUNC(PFNGLGETACTIVEUNIFORMPROC, glGetActiveUniform);
      FETCH_GL_FUNC(PFNGLGETATTACHEDSHADERSPROC, glGetAttachedShaders);
      FETCH_GL_FUNC(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog);
      FETCH_GL_FUNC(PFNGLGETPROGRAMIVPROC, glGetProgramiv);
      FETCH_GL_FUNC(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog);
      FETCH_GL_FUNC(PFNGLGETSHADERIVPROC, glGetShaderiv);
      FETCH_GL_FUNC(PFNGLGETSTRINGPROC, glGetString);
      FETCH_GL_FUNC(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation);
      FETCH_GL_FUNC(PFNGLLINKPROGRAMPROC, glLinkProgram);
      FETCH_GL_FUNC(PFNGLSHADERSOURCEPROC, glShaderSource);
      FETCH_GL_FUNC(PFNGLUNIFORM3FVPROC, glUniform3fv);
      FETCH_GL_FUNC(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv);
      FETCH_GL_FUNC(PFNGLUSEPROGRAMPROC, glUseProgram);
      FETCH_GL_FUNC(PFNGLVALIDATEPROGRAMPROC, glValidateProgram);
      FETCH_GL_FUNC(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);

      // General setup for openGL
      // These two ensure that vertexes are drawn via depth, and thus weird things like incorrect ordering of verticies happen
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LESS);
   }

   bool OpenGL::operator==(const OpenGL& other) const{
      return this == &other;
   }

} // namespace Core
