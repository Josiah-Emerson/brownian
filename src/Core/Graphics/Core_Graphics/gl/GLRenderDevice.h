#pragma once
#include "Core_Graphics/RenderDevice.h"
#include "Core_Window/Window.h"
#include "OpenGL.h"

namespace Core{
   struct GLShaderProgram{
      GLuint id;
   };

   enum class GLBufferTargetType{
   };

   struct GLBuffer{
      GLuint id;
      GLBufferTargetType targetType;
   };

   class GLRenderDevice : public RenderDevice{
      public:
         GLRenderDevice(Window& window);

         BufferHandle createBuffer(const BufferDesc& desc, const void* initialData = nullptr) override;
         ShaderPipelineHandle createShaderPipeline(const ShaderProgramCreateInfo& info) override;
         CommandList* beginCommandList() override;
         void submitCommandList(CommandList* cmd) override;

         // OpenGL specific functions
         GLuint getShaderProgramID(ShaderPipelineHandle handle);
         GLuint getBufferID(BufferHandle handle);


      private: 
         OpenGL m_openGL;
         std::vector<GLShaderProgram> m_shaderPrograms;
   };
} // namespace Core
