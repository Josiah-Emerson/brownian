#pragma once
#include <cstddef>

// To draw a cube I need to: 
// glUseProgram
// bindVertexArray
// setUniforms
// FOR NOW: Just do glDrawArrays as this doesn't require an element buffer and just 
// does the drawing sequentially, and then once we get that down move into an ability 
// to use an EBO. this would likely be something like cmd->drawIndexed(mesh.indexCount)

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
         virtual void setUniformBufferData(BufferHandle uBufferHandle, void* data) = 0;
         virtual void setUniformBufferData(StandardUniformBlock block, void* data) = 0;
         virtual void drawElement(std::size_t indexCount) = 0;
   };
} // namespace Core
