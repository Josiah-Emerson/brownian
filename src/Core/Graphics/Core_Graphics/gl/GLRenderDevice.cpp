#include "GLRenderDevice.h"
#include "Core_Graphics/gl/GLCommandList.h"
#include "Core_Utils/Log.h"
#include <GL/gl.h>
#include <GL/glext.h>
#include <fstream>
#include <iostream>
#include <sstream>

namespace Core{
   GLRenderDevice::GLRenderDevice(Window& window)
      : m_openGL { window }
      , m_VAOCache { }
   {
      BufferUsageFlags bUsage = BufferUsage::UNIFORM_BUFFER;
      bUsage |= BufferUsage::TRANSFER_DST;
      BufferDesc desc { .bUsage = bUsage };

      // CAMERA_DATA
      desc.size = sizeof(UniformCameraData);
      BufferHandle hBuf = createBuffer(desc);
      const GLBuffer& buf = getBuffer(hBuf);
      m_openGL.glBindBufferBase(GL_UNIFORM_BUFFER,
            static_cast<GLuint>(StandardUniformBlock::CAMERA_DATA),
            buf.id);
      m_stdUniformBufferHandles[static_cast<std::size_t>(StandardUniformBlock::CAMERA_DATA)] = hBuf;
   }

   BufferHandle GLRenderDevice::createBuffer(const BufferDesc& desc, const void* initialData){
      // TODO: openGL allows passing of a nullptr to the bufferdata function to set it up 
      // add this, and then think do I need to keep the size value ?
      GLuint bID;
      m_openGL.glGenBuffers(1, &bID);

      GLBufferTargetType targetType = GLBufferTargetType::NONE;
      if(desc.bUsage & BufferUsage::VERTEX_BUFFER){
         targetType = GLBufferTargetType::VERTEX_BUFFER;
      }else if(desc.bUsage & BufferUsage::UNIFORM_BUFFER){
         targetType = GLBufferTargetType::UNIFORM_BUFFER;
      }

      FIG_ASSERT(targetType != GLBufferTargetType::NONE, "GLBufferTargetType not deduced");

      GLMemoryUseHint hint = GLMemoryUseHint::NONE;
      // HUGE TODO: Actually go through all the combinations and set up the right flags
      switch(desc.mUsage){
         case(MemoryUsage::GPU_ONLY):
               hint = GLMemoryUseHint::DYNAMIC_DRAW;
               FIG_LOG_LOW_WARNING("GPU_ONLY Buffer created with DYNAMIC_DRAW. Probably not the best")
            break;
         case(MemoryUsage::CPU_TO_GPU):
            if(targetType == GLBufferTargetType::UNIFORM_BUFFER){
               hint = GLMemoryUseHint::DYNAMIC_DRAW;
            }else if(targetType == GLBufferTargetType::VERTEX_BUFFER){
               hint = GLMemoryUseHint::STATIC_DRAW;
            }
            break;
         case(MemoryUsage::GPU_TO_CPU):
            break;
         default: 
            FIG_UNCREACHABLE("Unhandled MemoryUsage type")
      }

      FIG_ASSERT(hint != GLMemoryUseHint::NONE, "Hint unable to be deduced")

      GLBuffer buf{
         .id = bID,
         .targetType = targetType,
         .memUseHint = hint,
         .hasData = false,
         .size = desc.size,
      };

      // TODO: Factor into separate func
      // buffer the data
      m_openGL.glBindBuffer(static_cast<GLenum>(buf.targetType), buf.id);
      m_openGL.glBufferData(static_cast<GLenum>(buf.targetType), 
            desc.size, initialData, static_cast<GLenum>(hint));
      buf.hasData = initialData;

      m_buffers.push_back(buf);
      return BufferHandle{ m_buffers.size() - 1 };
   }

   ShaderPipelineHandle GLRenderDevice::createShaderPipeline(const ShaderPipelineDesc& info){
      GLuint vID = tmpCompileShader(info.vertex.c_str(), GL_VERTEX_SHADER);
      GLuint fID = tmpCompileShader(info.fragment.c_str(), GL_FRAGMENT_SHADER);

      GLuint programID = m_openGL.glCreateProgram();
      m_openGL.glAttachShader(programID, vID);
      m_openGL.glAttachShader(programID, fID);

      m_openGL.glLinkProgram(programID);
      GLint linkResult;
      m_openGL.glGetProgramiv(programID, GL_LINK_STATUS, &linkResult);
      if(linkResult != GL_TRUE){
         FIG_UNCREACHABLE("failed to link program")
      }

      // bind uniforms
      FIG_ASSERT(info.uLayout.blocks.empty(), "Custom uniform blocks detected but are not implemented yet")
      for(const UniformBlock& block : info.uLayout.blocks){
         const GLBuffer& buf = getBuffer(block.bufferHandle);
         // m_openGL.glUniformBlockBinding(programID, block.blockIndex, block.blockIndex);
         m_openGL.glBindBufferBase(GL_UNIFORM_BUFFER, 0, buf.id);
      }

      m_shaderPipelines.emplace_back(programID, info.vLayout, info.uLayout);
      return ShaderPipelineHandle { m_shaderPipelines.size() - 1 };
   }

   CommandList* GLRenderDevice::beginCommandList(){
      return new GLCommandList {*this, m_openGL};
   }

   void GLRenderDevice::submitCommandList(CommandList* cmd){
      FIG_ASSERT(cmd, "CommandList* is null")
      // Do nothing for OpenGL implementation since the CommandList already does it

      delete cmd;
   }

   const GLShaderPipeline& GLRenderDevice::getShaderPipeline(ShaderPipelineHandle handle) const{
      // TODO: validation
      return m_shaderPipelines[handle.idx];
   }

   GLBuffer& GLRenderDevice::getBuffer(BufferHandle handle) {
      // TODO: validation
      return m_buffers[handle.idx];
   }

   const GLBuffer& GLRenderDevice::getBuffer(BufferHandle handle) const {
      // TODO: validation
      return m_buffers[handle.idx];
   }

   GLBuffer& GLRenderDevice::getBuffer(StandardUniformBlock block){
      FIG_ASSERT(StandardUniformBlock::NUM != block, "Invalid uniform block")
      return getBuffer(m_stdUniformBufferHandles[static_cast<std::size_t>(block)]);
   }

   const GLBuffer& GLRenderDevice::getBuffer(StandardUniformBlock block) const{
      FIG_ASSERT(StandardUniformBlock::NUM != block, "Invalid uniform block")
      return getBuffer(m_stdUniformBufferHandles[static_cast<std::size_t>(block)]);
   }

   GLuint GLRenderDevice::getVAO(ShaderPipelineHandle pipelineHandle, BufferHandle bufferHandle){
      const GLShaderPipeline& pipeline = getShaderPipeline(pipelineHandle);
      const GLBuffer& buf = getBuffer(bufferHandle);
      VAOKey key { pipeline.programID, buf.id };
      auto iter = m_VAOCache.find(key);

      if(iter != m_VAOCache.end()){
         return iter->second;
      }

      // TODO: How to ensure that the mesh data uploaded matches this from AssetManager?
      GLuint vao;
      m_openGL.glGenVertexArrays(1, &vao);
      m_openGL.glBindVertexArray(vao);
      m_openGL.glBindBuffer(GL_ARRAY_BUFFER, buf.id); // TODO: I know that GLCommandList asserts that it is a vertex buffer 
                                                      // but should probably add something here too?

      for(const VertexAttribute& vAttr : pipeline.vLayout.attributes) {
         m_openGL.glEnableVertexAttribArray(vAttr.location);
         GLint numComponents { 0 };
         GLenum type;
         switch(vAttr.dataType){
            case(ShaderDataType::F_VEC3):
               numComponents = 3;
               type = GL_FLOAT;
               break;
            case(ShaderDataType::F_MAT4):
               FIG_UNCREACHABLE("Not implemented F_MAT4 data type for vertex data yet")
               break;
         }

         FIG_ASSERT(numComponents != 0, "Switch on data types did not update. Likely an unhandled data type")

         // TODO: Check for normalization
         m_openGL.glVertexAttribPointer(vAttr.location, numComponents, 
               type, GL_FALSE, pipeline.vLayout.stride, (void*)vAttr.offset);
      }
      m_VAOCache[key] = vao;
      m_openGL.glBindVertexArray(0);

      return vao;
   }

   bool GLRenderDevice::VAOKey::operator==(const VAOKey& other) const{
      return shaderProgramID == other.shaderProgramID && 
         bufferID == other.bufferID;
   }

   std::size_t GLRenderDevice::VAOKeyHash::operator()(const VAOKey& key) const noexcept{
      // TODO: Is this fine? Can this be quicker if we only care about no collisions?
      // does it need to be quicker?
      std::size_t h1 = std::hash<GLuint>{}(key.shaderProgramID);
      std::size_t h2 = std::hash<GLuint>{}(key.bufferID);
      return h1 ^ (h2 << 1);
   }

   GLuint GLRenderDevice::tmpCompileShader(const char* path, GLenum shaderType){
      FIG_LOG_LOW_WARNING("Use of a tmp function")
      GLuint id = m_openGL.glCreateShader(shaderType);
      std::string shaderCode;
      std::ifstream shaderStream(path, std::ios::in);
      if(!shaderStream.is_open()){
         FIG_UNCREACHABLE("shader stream not open")
      }

      // get shader code
      std::stringstream sstr;
      sstr << shaderStream.rdbuf();
      shaderCode = sstr.str();
      shaderStream.close();

      // compile shader
      const char* sourcePtr = shaderCode.c_str();
      m_openGL.glShaderSource(id, 1, &sourcePtr, NULL);
      m_openGL.glCompileShader(id);
      
      // check and return 
      GLint res = GL_FALSE;
      m_openGL.glGetShaderiv(id, GL_COMPILE_STATUS, &res);
      if(res != GL_TRUE){
         int len;
         m_openGL.glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
         char* message = new char[len - 1];
         m_openGL.glGetShaderInfoLog(id, len, NULL, message);
         std::string strMessage { message };
         delete[] message;
         std::string str = "Error in compilation: " + strMessage;
         std::cerr << str << '\n';
         FIG_UNCREACHABLE("See std::cerr")
      }

      return id;
   }


} // namespace Core
