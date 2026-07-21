#include "GLRenderDevice.h"
#include "Core_Graphics/ShaderPipeline.h"
#include "Core_Graphics/gl/GLCommandList.h"
#include "Core_Utils/Log.h"
#include "UI/Core_UI/UniformVariableWidgets.h"
#include <GL/gl.h>
#include <GL/glext.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace Core{
   GLRenderDevice::GLRenderDevice(Window& window)
      : m_openGL { window }
      , m_VAOCache { }
   {
      BufferUsageFlags bUsage = BufferUsage::UNIFORM_BUFFER;
      bUsage |= BufferUsage::TRANSFER_DST;
      BufferDesc desc { .bUsage = bUsage };

      // CAMERA_DATA
      desc.size = sizeof(UniformCameraData); // TODO: Will need to figure out a way to translate to std140
      BufferHandle hBuf = createBuffer(desc);
      const GLBuffer& buf = getBuffer(hBuf);
      m_openGL.glBindBufferBase(GL_UNIFORM_BUFFER,
            static_cast<GLuint>(StandardUniformBlock::FIG_CAMERA_DATA),
            buf.id);
      m_stdUniformBufferHandles[static_cast<std::size_t>(StandardUniformBlock::FIG_CAMERA_DATA)] = hBuf;
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
      }else if(desc.bUsage & BufferUsage::INDEX_BUFFER){
         targetType = GLBufferTargetType::INDEX_BUFFER;
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
            FIG_UNREACHABLE("Unhandled MemoryUsage type")
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
      const GLShader& vertex = loadShader(info.vertex.c_str(), GLShaderType::VERTEX_SHADER);
      const GLShader& fragment = loadShader(info.fragment.c_str(), GLShaderType::FRAGMENT_SHADER);

      GLuint programID = m_openGL.glCreateProgram();
      m_openGL.glAttachShader(programID, vertex.id);
      m_openGL.glAttachShader(programID, fragment.id);

      m_openGL.glLinkProgram(programID);
      GLint linkResult;
      m_openGL.glGetProgramiv(programID, GL_LINK_STATUS, &linkResult);
      if(linkResult != GL_TRUE){
         FIG_UNREACHABLE("failed to link program")
      }

      std::vector<UniformReflectionMetadata> uniforms = internalReflectUniforms(programID);

      m_shaderPipelines.emplace_back(programID, info.vLayout, uniforms);
      return ShaderPipelineHandle { m_shaderPipelines.size() - 1 };
   }

   const std::vector<UniformReflectionMetadata>& GLRenderDevice::reflectUniforms(ShaderPipelineHandle hPipeline){
      const GLShaderPipeline& pipeline = getShaderPipeline(hPipeline);

      return pipeline.uniforms;
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
         GLint numComponents = vAttr.typeDesc.primCnt;
         if(numComponents > 4){
            // TODO: Do something about this
            FIG_UNREACHABLE("For vertex attributes, numComponents must be 1, 2, 3, 4, or GL_BGRA")
         }
         GLenum type;

         // TODO: Actually handle any of the cases where we have an invalid prim type
         switch(vAttr.typeDesc.primType){
            case(PrimitiveType::I32):
               type = GL_INT;
               break;
            case(PrimitiveType::FLOAT):
               type = GL_FLOAT;
               break;
            case(PrimitiveType::DOUBLE):
               FIG_UNREACHABLE("Currently our openGL implementation does not support double for vertex attributes")
               // type = GL_DOUBLE;
               // TODO: GL_DOUBLE only accepted by glVertexAttribLPointer (see glVertexAttribPointer docs)
               break;
            default:
               FIG_UNREACHABLE("The primitive type passed is not supported by openGL")
               break;
         }

         // TODO: Check for normalization
         m_openGL.glVertexAttribPointer(vAttr.location, numComponents, 
               type, GL_FALSE, pipeline.vLayout.stride, (void*)vAttr.offset);
      }
      m_VAOCache[key] = vao;
      m_openGL.glBindVertexArray(0);

      return vao;
   }

   const GLShader& GLRenderDevice::loadShader(const std::string& path, GLShaderType type){
      auto it = m_shaderCache.find(path);
      if(it != m_shaderCache.end()){
         FIG_ASSERT(it->second.shaderType == type, "A cached shader was found for the given path, however there is a mismatch in expected shader type and cached shader type")
         return it->second;
      }

      GLuint id = compileShader(path.c_str(),
            static_cast<GLenum>(type));
      auto insert = m_shaderCache.emplace(path, GLShader{id, type});

      FIG_ASSERT(insert.second, "Insertion of new GLShader into cache did not take place");
      return insert.first->second;
   }

   GLuint GLRenderDevice::compileShader(const char* path, GLenum shaderType){
      GLuint id = m_openGL.glCreateShader(shaderType);
      std::string shaderCode;
      std::ifstream shaderStream(path, std::ios::in);
      if(!shaderStream.is_open()){
         FIG_UNREACHABLE("shader stream not open")
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
         FIG_UNREACHABLE("See std::cerr")
      }

      return id;
   }

   ShaderTypeDescription GLRenderDevice::glTypeToShaderTypeDescription(GLenum glType){
      ShaderTypeDescription ret { };
      switch(glType){
         case(GL_BOOL):
            ret.primType = PrimitiveType::BOOL;
            ret.primCnt = 1;
            break;
         case(GL_INT):
            ret.primType = PrimitiveType::I32;
            ret.primCnt = 1;
            break;
         case(GL_INT_VEC2):
            ret.primType = PrimitiveType::I32;
            ret.primCnt = 2;
            break;
         case(GL_INT_VEC3):
            ret.primType = PrimitiveType::I32;
            ret.primCnt = 3;
            break;
         case(GL_INT_VEC4):
            ret.primType = PrimitiveType::I32;
            ret.primCnt = 4;
            break;
         case(GL_FLOAT):
            ret.primType = PrimitiveType::FLOAT;
            ret.primCnt = 1;
            break;
         case(GL_FLOAT_VEC2):
            ret.primType = PrimitiveType::FLOAT;
            ret.primCnt = 2;
            break;
         case(GL_FLOAT_VEC3):
            ret.primType = PrimitiveType::FLOAT;
            ret.primCnt = 3;
            break;
         case(GL_FLOAT_VEC4):
            ret.primType = PrimitiveType::FLOAT;
            ret.primCnt = 4;
            break;
         case(GL_FLOAT_MAT3):
            ret.primType = PrimitiveType::FLOAT;
            ret.primCnt = 9;
            break;
         case(GL_FLOAT_MAT4):
            ret.primType = PrimitiveType::FLOAT;
            ret.primCnt = 16;
            break;
         case(GL_DOUBLE):
            ret.primType = PrimitiveType::DOUBLE;
            ret.primCnt = 1;
            break;
         case(GL_DOUBLE_VEC2):
            ret.primType = PrimitiveType::DOUBLE;
            ret.primCnt = 2;
            break;
         case(GL_DOUBLE_VEC3):
            ret.primType = PrimitiveType::DOUBLE;
            ret.primCnt = 3;
            break;
         case(GL_DOUBLE_VEC4):
            ret.primType = PrimitiveType::DOUBLE;
            ret.primCnt = 4;
            break;
         case(GL_DOUBLE_MAT3):
            ret.primType = PrimitiveType::DOUBLE;
            ret.primCnt = 9;
            break;
         case(GL_DOUBLE_MAT4):
            ret.primType = PrimitiveType::DOUBLE;
            ret.primCnt = 16;
            break;
      }

      return ret;
   }

   std::vector<UniformReflectionMetadata> GLRenderDevice::internalReflectUniforms(GLuint programID){
      // NOTE: This currently skips any uniform in a uniform block (i.e. it assumes a uniform block is one of the default fig uniforms that fig sets)
      // This enumerates all used uniforms in a shader which are custom to that shader and are likely to be wanted to be controlled by a user
      // TODO: add checks to ensure programID is actually the name of a valid program
      GLint totalUniformCount;
      m_openGL.glGetProgramiv(programID, GL_ACTIVE_UNIFORMS, &totalUniformCount);

      // enumerate indices
      GLuint* allIndices = new GLuint[totalUniformCount];
      for(std::size_t i { 0 }; i < totalUniformCount; ++i){
         allIndices[i] = i;
      }

      GLint* blockIndices = new GLint[totalUniformCount];
      m_openGL.glGetActiveUniformsiv(programID, totalUniformCount, allIndices, GL_UNIFORM_BLOCK_INDEX, blockIndices);

      std::vector<GLuint> customUniformIndices;
      for(std::size_t i { 0 }; i < totalUniformCount; ++i){
         if(blockIndices[i] == -1){ // not in a uniform block -> custom uniform variable
            customUniformIndices.push_back(i);
         }
      }
      // TODO: should we delete these at end of function or when we don't need them anymore?
      // NOTE: If we put at end we need to make sure we still delete them on any early returns...
      delete[] blockIndices;
      delete[] allIndices;

      if(customUniformIndices.empty()){ // no customs
         return { };
      }

      GLint* varNameLengths = new GLint[customUniformIndices.size()];
      GLint* varDatatype = new GLint[customUniformIndices.size()];

      m_openGL.glGetActiveUniformsiv(programID, customUniformIndices.size(), 
            customUniformIndices.data(), GL_UNIFORM_NAME_LENGTH, varNameLengths);
      m_openGL.glGetActiveUniformsiv(programID, customUniformIndices.size(), 
            customUniformIndices.data(), GL_UNIFORM_TYPE, varDatatype);

      std::vector<UniformReflectionMetadata> reflection;
      reflection.reserve(customUniformIndices.size());

      for(std::size_t i { 0 }; i < customUniformIndices.size(); ++i){
         char* name = new char[varNameLengths[i]];

         m_openGL.glGetActiveUniformName(programID, customUniformIndices[i], varNameLengths[i], NULL, name);
         reflection.emplace_back(UniformReflectionMetadata{ name, glTypeToShaderTypeDescription(varDatatype[i]) });

         delete[] name;
      }

      delete[] varNameLengths;
      delete[] varDatatype;

      return reflection;
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


} // namespace Core
