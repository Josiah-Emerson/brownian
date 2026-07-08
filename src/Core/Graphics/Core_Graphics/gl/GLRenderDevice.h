#pragma once
#include "Core_Graphics/RenderDevice.h"
// #include "Core_Graphics/gl/GLCommandList.h"
#include "Core_Window/Window.h"
#include "OpenGL.h"
#include <GL/glext.h>
#include <array>
#include "Core_Graphics/ShaderPipeline.h"

// TODO: Do we want to have the functionality for setting data on a buffer here instead of like 
// in the GLCommandList?

namespace Core{
   // Piplines to more accomodating to modern APIs ?
   struct GLShaderPipeline{
      GLuint programID;
      VertexLayout vLayout;
      UniformLayout uLayout; 
   };

   enum class GLBufferTargetType{
      NONE,
      VERTEX_BUFFER = GL_ARRAY_BUFFER,
      UNIFORM_BUFFER = GL_UNIFORM_BUFFER,
      INDEX_BUFFER = GL_ELEMENT_ARRAY_BUFFER,
   };

   enum class GLMemoryUseHint{
      NONE,
      DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
      STATIC_DRAW = GL_STATIC_DRAW,
   };

   struct GLBuffer{
      GLuint id;
      GLBufferTargetType targetType;
      GLMemoryUseHint memUseHint;
      bool hasData;
      std::size_t size; // Size as described by the BufferDesc unless updated
   };

   class GLRenderDevice : public RenderDevice{
      public:
         GLRenderDevice(Window& window);

         BufferHandle createBuffer(const BufferDesc& desc, const void* initialData = nullptr) override;
         ShaderPipelineHandle createShaderPipeline(const ShaderPipelineDesc& info) override;
         CommandList* beginCommandList() override;
         void submitCommandList(CommandList* cmd) override;

         // GL Specific
         const GLShaderPipeline& getShaderPipeline(ShaderPipelineHandle) const;
         const GLBuffer& getBuffer(BufferHandle) const;
         GLBuffer& getBuffer(BufferHandle);
         const GLBuffer& getBuffer(StandardUniformBlock block) const;
         GLBuffer& getBuffer(StandardUniformBlock block);
         GLuint getVAO(ShaderPipelineHandle pipelineHandle, BufferHandle bufferHandle);

      private: 
         // TODO: How to guarantee concurrency? 
         // OpenGL (I think) will give an ID when creating a new shader/buffer 
         // that is guaranteed to not have been in use prior to the call? Is it possible 
         // for a previously used value to become unused and recycled which would invalidate ?
         struct VAOKey{
            GLuint shaderProgramID;
            GLuint bufferID;

            bool operator==(const VAOKey& other) const;
         };

         struct VAOKeyHash{
            std::size_t operator()(const VAOKey& key) const noexcept;
         };

         GLuint tmpCompileShader(const char*, GLenum);

      private: 
         OpenGL m_openGL;
         std::vector<GLShaderPipeline> m_shaderPipelines;
         std::vector<GLBuffer> m_buffers;
         std::array<BufferHandle, static_cast<std::size_t>(StandardUniformBlock::NUM)> m_stdUniformBufferHandles;
         std::unordered_map<VAOKey, GLuint, VAOKeyHash> m_VAOCache;
   };
} // namespace Core
