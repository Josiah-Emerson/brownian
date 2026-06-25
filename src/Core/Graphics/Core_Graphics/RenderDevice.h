#pragma once
#include <cstddef>
#include <memory>
#include "Core_Graphics/Buffer.h"
#include "Core_Graphics/CommandList.h"
#include "Core_Graphics/ShaderProgram.h"
#include "Core_Window/Window.h"

namespace Core{
   // Shaders -      Created by: Render Device
   //                Managed by: Shader Handle
   //
   // Buffers -      Created by: Render Device
   //                Managed by: Buffer Handle
   //
   // Renderer -     Created by: Layer
   //                Managed by: Renderer Object
   //
   // RenderDevice - Created by: Application 
   //                Managed by: std::unique_ptr<RenderDevice>
   //
   // Mesh -         Created by: AssetManager
   //                Managed by: MeshHandles
   //
   // Material -     Created by: AssetManager
   //                Managed by: MeshHandles

   struct BufferHandle{
      std::size_t idx;
   };

   struct ShaderHandle{
      std::size_t idx;
   };

   struct ShaderPipelineHandle{
      std::size_t idx;
   };

   class RenderDevice{
      public: 
         // RenderDevice();
         virtual ~RenderDevice() = default;

         static std::unique_ptr<RenderDevice> create(Window& window);

         // For now no initial data, but this was an option from the example RenderDevice I got 
         virtual BufferHandle createBuffer(const BufferDesc& desc, const void* initialData = nullptr) = 0;

         virtual ShaderPipelineHandle createShaderPipeline(const ShaderProgramCreateInfo& info) = 0;

         virtual CommandList* beginCommandList() = 0;
         virtual void submitCommandList(CommandList* cmd) = 0;

      protected: 
      private: 
   };

} // namespace Core
