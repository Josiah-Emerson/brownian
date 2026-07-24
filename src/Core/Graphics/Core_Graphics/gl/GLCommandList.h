#pragma once
#include "Core_Graphics/CommandList.h"
#include "Core_Graphics/gl/GLRenderDevice.h"
#include "Core_Graphics/gl/OpenGL.h"

namespace Core{
   struct GLRenderPass{
      bool active = false;
      bool m_storeColor = true;
      bool m_storeDepth = true;
   };

   class GLCommandList : public CommandList{
      public:
         GLCommandList(GLRenderDevice& glDevice, OpenGL& opengl);

         void beginRenderPass(const RenderPassDesc& desc) override;
         void endRenderPass() override;
         void bindShaderPipeline(ShaderPipelineHandle pipelineHandle) override;
         void bindVertexBuffer(BufferHandle vBufferHandle) override;
         void bindIndexBuffer(BufferHandle idxBufHandle) override;
         void setUniformBufferData(BufferHandle uBufferHandle, const void* data) override;
         void setUniformBufferData(StandardUniformBlock block, const void* data) override;
         void setUniformVariable(const std::string& variableName, const void* data) override;
         void setUniformVariable(const std::string& variableName, const ShaderData& data) override;
         void drawElement(std::size_t indexCount) override;

      private:
      private: 
         // TODO: Perhaps refactor these into its own 'state' struct with things like 
         // render pass, current pipeline handle, perhaps pointers to the actual pipeline 
         // objects so we don't have to continuously ask the render device for it (unless it 
         // doesn't take that much extra time to query this)
         OpenGL& m_openGL;
         GLRenderDevice& m_glDevice;
         GLRenderPass m_renderPass {};
         // TODO: Should this be in the the render pass data? 
         // TODO: Make this so that its not jsut two random vars to duct tape stuff together
         ShaderPipelineHandle m_currentPipelineHandle;
         bool m_pipelineIsActive { false };
   };
} // namespace Core
