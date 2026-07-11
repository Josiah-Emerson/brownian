#pragma once
#include <cstddef>

// TODO: RenderPass
namespace Core{
   class ShaderPipelineHandle;
   class BufferHandle;
   class RenderPassDesc;
   enum class StandardUniformBlock;

   class CommandList{
      public:
         virtual ~CommandList() = default;

         // TODO: Set viewport
         virtual void beginRenderPass(const RenderPassDesc& desc) = 0;
         virtual void endRenderPass() = 0;
         virtual void bindShaderPipeline(ShaderPipelineHandle pipelineHandle) = 0;
         virtual void bindVertexBuffer(BufferHandle vBufferHandle) = 0;
         virtual void bindIndexBuffer(BufferHandle idxBufHandle) = 0;
         virtual void setUniformBufferData(BufferHandle uBufferHandle, void* data) = 0;
         virtual void setUniformBufferData(StandardUniformBlock block, void* data) = 0;
         virtual void drawElement(std::size_t indexCount) = 0;
   };
} // namespace Core
