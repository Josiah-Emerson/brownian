#pragma once
#include "Core_Graphics/RenderDevice.h"
#include "Core_Window/Window.h"
#include "OpenGL.h"
#include <GL/glext.h>
#include <array>
#include "Core_Graphics/ShaderPipeline.h"

// TODO: Do we want to have the functionality for setting data on a buffer here instead of like 
// in the GLCommandList?

namespace Core{
   enum class GLShaderType{
      VERTEX_SHADER = GL_VERTEX_SHADER,
      FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
   };

   struct GLShader{
      GLuint id;
      GLShaderType shaderType;
   };

   // Piplines to more accomodating to modern APIs ?
   struct GLShaderPipeline{
      GLuint programID;
      VertexLayout vLayout;

      // NOTE: Right now these are only custom uniforms (i.e. not FIG_CAMERA_DATA at binding 0)
      // Is that fine? Should the render device know which anes are custom and which ones aren't?
      // TODO: Figure out a good way to update this so that we can store the UniformReflectionMetadata 
      // and the GLint which represents its location. (see "TODO: Uniform location speed up" in 
      // GLCommandList.cpp) Things to keep in mind: the GLRenderDevice needs to be able to return 
      // a vector of UniformReflectionMetadata to impl agnostic systems so, does it need to return 
      // frequently enough that we should keep these separate? (i.e. be able to simply return the uniforms 
      // vector)? Or do we only generally do this once after pipeline creation and thus can separate the two 
      // when the function is called? If we go option B, we need to consider the fact that we currently return by 
      // reference and other
      std::vector<UniformReflectionMetadata> uniforms; 
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

         // General Render Device
         BufferHandle createBuffer(const BufferDesc& desc, const void* initialData = nullptr) override;
         ShaderPipelineHandle createShaderPipeline(const ShaderPipelineDesc& info) override;
         const std::vector<UniformReflectionMetadata>& reflectUniforms(ShaderPipelineHandle hPipeline) override;
         CommandList* beginCommandList() override;
         void submitCommandList(CommandList* cmd) override;

         // GL Specific
         const GLShaderPipeline& getShaderPipeline(ShaderPipelineHandle) const;
         const GLBuffer& getBuffer(BufferHandle) const;
         GLBuffer& getBuffer(BufferHandle);
         const GLBuffer& getBuffer(StandardUniformBlock block) const;
         GLBuffer& getBuffer(StandardUniformBlock block);
         GLuint getVAO(ShaderPipelineHandle pipelineHandle, BufferHandle bufferHandle);
         const GLShader& loadShader(const std::string& path, GLShaderType type);

      private: 
         GLuint compileShader(const char*, GLenum);
         ShaderTypeDescription glTypeToShaderTypeDescription(GLenum glType);
         std::vector<UniformReflectionMetadata> internalReflectUniforms(GLuint programID);

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

      private: 
         OpenGL m_openGL;
         std::vector<GLShaderPipeline> m_shaderPipelines;
         std::vector<GLBuffer> m_buffers;
         std::array<BufferHandle, static_cast<std::size_t>(StandardUniformBlock::NUM)> m_stdUniformBufferHandles;
         std::unordered_map<VAOKey, GLuint, VAOKeyHash> m_VAOCache;
         std::unordered_map<std::string, GLShader> m_shaderCache;
   };
} // namespace Core
