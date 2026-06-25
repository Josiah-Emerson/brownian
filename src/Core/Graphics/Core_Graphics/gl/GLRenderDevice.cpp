#include "GLRenderDevice.h"
#include "Core_Graphics/gl/GLCommandList.h"
#include "Core_Utils/Log.h"

namespace Core{
   GLRenderDevice::GLRenderDevice(Window& window)
      : m_openGL { window }
   {
      FIG_LOG_MESSAGE("GLRenderDevice Created")
   }

   BufferHandle GLRenderDevice::createBuffer(const BufferDesc& desc, const void* initialData){
   }

   ShaderPipelineHandle GLRenderDevice::createShaderPipeline(const ShaderProgramCreateInfo& info){
   }

   CommandList* GLRenderDevice::beginCommandList(){
      return new GLCommandList {*this, m_openGL};
   }

   void GLRenderDevice::submitCommandList(CommandList* cmd){
      FIG_ASSERT(cmd, "CommandList* is null")
      // Do nothing for OpenGL implementation since the CommandList already does it

      delete cmd;
   }

} // namespace Core
