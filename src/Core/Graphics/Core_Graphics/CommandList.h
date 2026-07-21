#pragma once
#include <cstddef>
#include <string>
#include "ShaderData.h"

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
         virtual void setUniformBufferData(BufferHandle uBufferHandle, const void* data) = 0;
         virtual void setUniformBufferData(StandardUniformBlock block, const void* data) = 0;
         virtual void setUniformVariable(const std::string& variableName, const void* data) = 0; 
         virtual void setUniformVariable(const std::string& variableName, const ShaderData& data) = 0; // NOTE: There is no setUniformBufferData which accepts a ShaderData object because 
                                                                                                       // the data for uniform buffers are in blocks (and thus can be whatever combination 
                                                                                                       // of data it wants, and introducting a block of ShaderData vars is odd)
                                                                                                       // while the uniform variables are standalone variables and thus one ShaderData 
                                                                                                       // can correspond to one
         virtual void drawElement(std::size_t indexCount) = 0;
   };
} // namespace Core
