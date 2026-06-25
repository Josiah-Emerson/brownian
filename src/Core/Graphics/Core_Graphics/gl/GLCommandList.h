#pragma once
#include "Core_Graphics/CommandList.h"
#include "Core_Graphics/gl/GLRenderDevice.h"
#include "Core_Graphics/gl/OpenGL.h"

namespace Core{
   class GLCommandList : public CommandList{
      public:
         GLCommandList(GLRenderDevice& glDevice, OpenGL& opengl);

         void beginRenderPass(RenderPassDesc passDesc) override;
         void endRenderPass() override;
         void bindShaderPipeline(ShaderPipelineHandle pipelineHandle) override;
         void bindVertexBuffer(BufferHandle vBufferHandle) override;
         void setUniformBuffer(BufferHandle uBufferHandle, void* data) override;
         void drawElement() override;

      private:
         OpenGL& m_openGL;
         GLRenderDevice& m_glDevice;

   };
} // namespace Core
