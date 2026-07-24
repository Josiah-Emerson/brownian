#pragma once
#include "Core_Window/Window.h"
#include <GL/gl.h>

// Loads all needed functions for openGL. Is it fine to have them gloablly like this? 

namespace Core{
   // Typedefs 
   typedef void (APIENTRYP PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
   typedef void (APIENTRYP PFNGLBINDBUFFERPROC) (GLenum target, GLuint buffer);
   typedef void (APIENTRYP PFNGLBINDBUFFERBASEPROC) (GLenum target, GLuint index, GLuint buffer);
   typedef void (APIENTRYP PFNGLBINDVERTEXARRAYPROC) (GLuint array);
   typedef void (APIENTRYP PFNGLBUFFERDATAPROC) (GLenum target, GLsizeiptr size, const void* data, GLenum usage); 
   typedef void (APIENTRYP PFNGLBUFFERSUBDATAPROC) (GLenum target, GLintptr offset, GLsizeiptr size, const void * data);
   typedef void (APIENTRYP PFNGLCOMPILESHADERPROC) (GLuint shader);
   typedef void (APIENTRYP PFNGLCLEARPROC) (GLbitfield mask);
   typedef void (APIENTRYP PFNGLCLEARCOLORPROC) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
   typedef void (APIENTRYP PFNGLCLEARDEPTHFPROC) (GLfloat depth);
   typedef GLuint (APIENTRYP PFNGLCREATEPROGRAMPROC) (void);
   typedef GLuint (APIENTRYP PFNGLCREATESHADERPROC) (GLenum shaderType);
   typedef void (APIENTRYP PFNGLDELETEPROGRAMPROC) (GLuint program);
   typedef void (APIENTRYP PFNGLDELETESHADERPROC) (GLuint shader);
   typedef void (APIENTRYP PFNGLDEPTHFUNCPROC) (GLenum func);
   typedef void (APIENTRYP PFNGLDRAWARRAYSPROC) (GLenum mode, GLint first, GLsizei count);
   typedef void (APIENTRYP PFNGLDRAWELEMENTSPROC) (GLenum mode, GLsizei count, GLenum type, const void * indices);
   typedef void (APIENTRYP PFNGLENABLEPROC) (GLenum cap);
   typedef void (APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYPROC) (GLuint index);
   typedef void (APIENTRYP PFNGLGENBUFFERSPROC) (GLsizei n, GLuint *buffers);
   typedef void (APIENTRYP PFNGLGENVERTEXARRAYSPROC) (GLsizei n, GLuint *arrays);
   typedef void (APIENTRYP PFNGLGETACTIVEATTRIBPROC) (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
   typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMPROC) (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
   typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMNAMEPROC) (GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei *length, char *uniformName);
   typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMSIVPROC) (GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint* params);
   typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMBLOCKIVPROC) (GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params);
   typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC) (GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName);
   typedef void (APIENTRYP PFNGLGETATTACHEDSHADERSPROC) (GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders);
   typedef void (APIENTRYP PFNGLGETPROGRAMINFOLOGPROC) (GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
   typedef void (APIENTRYP PFNGLGETPROGRAMIVPROC) (GLuint program, GLenum pname, GLint *params);
   typedef void (APIENTRYP PFNGLGETSHADERINFOLOGPROC) (GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
   typedef void (APIENTRYP PFNGLGETSHADERIVPROC) (GLuint shader, GLenum pname, GLint *params);
   typedef const GLubyte* (APIENTRYP PFNGLGETSTRINGPROC) (GLenum name);
   typedef GLint (APIENTRYP PFNGLGETUNIFORMLOCATIONPROC) (GLuint program, const GLchar *name);
   typedef void (APIENTRYP PFNGLLINKPROGRAMPROC) (GLuint program);
   typedef void (APIENTRYP PFNGLSHADERSOURCEPROC) (GLuint shader, GLsizei count, const GLchar **string, const GLint *length);
   typedef void (APIENTRYP PFNGLUNIFORM1FPROC) (GLint location, GLfloat v0);
   typedef void (APIENTRYP PFNGLUNIFORM1FVPROC) (GLint location, GLsizei count, const GLfloat *value);
   typedef void (APIENTRYP PFNGLUNIFORM2FVPROC) (GLint location, GLsizei count, const GLfloat *value);
   typedef void (APIENTRYP PFNGLUNIFORM3FVPROC) (GLint location, GLsizei count, const GLfloat *value);
   typedef void (APIENTRYP PFNGLUNIFORM4FVPROC) (GLint location, GLsizei count, const GLfloat *value);
   typedef void (APIENTRYP PFNGLUNIFORM1IVPROC) (GLint location, GLsizei count, const GLint *value);
   typedef void (APIENTRYP PFNGLUNIFORM2IVPROC) (GLint location, GLsizei count, const GLint *value);
   typedef void (APIENTRYP PFNGLUNIFORM3IVPROC) (GLint location, GLsizei count, const GLint *value);
   typedef void (APIENTRYP PFNGLUNIFORM4IVPROC) (GLint location, GLsizei count, const GLint *value);
   typedef void (APIENTRYP PFNGLUNIFORMMATRIX3FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
   typedef void (APIENTRYP PFNGLUNIFORMMATRIX4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
   typedef void (APIENTRYP PFNGLUNIFORMBLOCKBINDINGPROC) (GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
   typedef void (APIENTRYP PFNGLUSEPROGRAMPROC) (GLuint program);
   typedef void (APIENTRYP PFNGLVALIDATEPROGRAMPROC) (GLuint program);
   typedef void (APIENTRYP PFNGLVERTEXATTRIBPOINTERPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
   
   struct OpenGL{
      OpenGL(Core::Window&); // loads functions
      bool operator==(const OpenGL&) const;
                             
      PFNGLATTACHSHADERPROC glAttachShader;
      PFNGLBINDBUFFERPROC glBindBuffer;
      PFNGLBINDBUFFERBASEPROC glBindBufferBase;
      PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
      PFNGLBUFFERDATAPROC glBufferData;
      PFNGLBUFFERSUBDATAPROC glBufferSubData;
      PFNGLCLEARPROC glClear;
      PFNGLCLEARCOLORPROC glClearColor;
      PFNGLCLEARDEPTHFPROC glClearDepthf;
      PFNGLCOMPILESHADERPROC glCompileShader;
      PFNGLCREATEPROGRAMPROC glCreateProgram;
      PFNGLCREATESHADERPROC glCreateShader;
      PFNGLDEPTHFUNCPROC glDepthFunc;
      PFNGLDELETEPROGRAMPROC glDeleteProgram;
      PFNGLDELETESHADERPROC  glDeleteShader;
      PFNGLDRAWARRAYSPROC glDrawArrays;
      PFNGLDRAWELEMENTSPROC glDrawElements;
      PFNGLENABLEPROC glEnable;
      PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
      PFNGLGENBUFFERSPROC glGenBuffers;
      PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
      PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib;
      PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform;
      PFNGLGETACTIVEUNIFORMNAMEPROC glGetActiveUniformName;
      PFNGLGETACTIVEUNIFORMSIVPROC glGetActiveUniformsiv;
      PFNGLGETACTIVEUNIFORMBLOCKIVPROC glGetActiveUniformBlockiv;
      PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glGetActiveUniformBlockName;
      PFNGLGETATTACHEDSHADERSPROC glGetAttachedShaders;
      PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
      PFNGLGETPROGRAMIVPROC glGetProgramiv;
      PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
      PFNGLGETSHADERIVPROC glGetShaderiv;
      PFNGLGETSTRINGPROC glGetString;
      PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
      PFNGLLINKPROGRAMPROC glLinkProgram;
      PFNGLSHADERSOURCEPROC glShaderSource;
      PFNGLUNIFORM1FPROC glUniform1f;
      PFNGLUNIFORM1FVPROC glUniform1fv;
      PFNGLUNIFORM2FVPROC glUniform2fv;
      PFNGLUNIFORM3FVPROC glUniform3fv;
      PFNGLUNIFORM4FVPROC glUniform4fv;
      PFNGLUNIFORM1IVPROC glUniform1iv;
      PFNGLUNIFORM2IVPROC glUniform2iv;
      PFNGLUNIFORM3IVPROC glUniform3iv;
      PFNGLUNIFORM4IVPROC glUniform4iv;
      PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix3fv;
      PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
      PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding;
      PFNGLUSEPROGRAMPROC glUseProgram;
      PFNGLVALIDATEPROGRAMPROC glValidateProgram;
      PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
   };

   // Functions
} // namespace Core

