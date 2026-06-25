#include "GLCommandList.h"

namespace Core{
   GLCommandList::GLCommandList(GLRenderDevice& glDevice, OpenGL& openGL)
      : m_glDevice(glDevice)
      , m_openGL(openGL)
   { }

   void GLCommandList::bindProgram(ShaderProgramHandle shaderProgramHandle){
      // TODO: More robust obviously
      m_openGL.glUseProgram(m_glDevice.getShaderProgramID(shaderProgramHandle));
   }

   void GLCommandList::bindVertexBuffer(BufferHandle vBufferHandle){
      m_openGL.glBindVertexArray(m_glDevice.getBufferID(vBufferHandle));
   }

} // namespace Core
