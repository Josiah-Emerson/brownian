#include "GLCommandList.h"
#include "Core_Graphics/RenderPass.h"
#include "Core_Graphics/ShaderData.h"
#include "Core_Graphics/gl/GLRenderDevice.h"
#include "Core_Utils/Log.h"
#include <GL/glext.h>
#include <algorithm>
#include <cstring>

namespace Core{
   GLCommandList::GLCommandList(GLRenderDevice& glDevice, OpenGL& openGL)
      : m_glDevice(glDevice)
      , m_openGL(openGL)
   { }

   void GLCommandList::beginRenderPass(const RenderPassDesc& desc){
      FIG_ASSERT(!m_renderPass.active, "Tried to begin a renderpass without ending previous")
      GLbitfield bufMask = 0;
      // TODO: differentiate between NONE and KEEP
      if(desc.colorLoadOp == LoadOp::CLEAR){
         bufMask |= GL_COLOR_BUFFER_BIT;
         m_openGL.glClearColor(desc.colorClearValue[0],
                           desc.colorClearValue[1],
                           desc.colorClearValue[2],
                           desc.colorClearValue[3]);
      }

      if(desc.depthLoadOp == LoadOp::CLEAR){
         bufMask |= GL_DEPTH_BUFFER_BIT;
         m_openGL.glClearDepthf(desc.depthClearValue);
      }

      if(bufMask != 0)
         m_openGL.glClear(bufMask);

      m_renderPass.active = true;
      m_renderPass.m_storeColor = desc.colorStoreOp == StoreOp::STORE;
      m_renderPass.m_storeDepth = desc.depthStoreOp == StoreOp::STORE;
   }

   void GLCommandList::endRenderPass(){
      static bool w { true };
      if(w){
         FIG_LOG_LOW_WARNING("Not yet imlemented optimizatioins if StoreOp::NO_STORE is set for render pass")
         w = false;
      }
      FIG_ASSERT(m_renderPass.active, "Tried to end a render pass without beginning one")
      m_renderPass.active = false;
      // TODO: anything else here?
   }

   void GLCommandList::bindShaderPipeline(ShaderPipelineHandle pipelineHandle){
      const GLShaderPipeline& pipeline = m_glDevice.getShaderPipeline(pipelineHandle);
      m_openGL.glUseProgram(pipeline.programID);
      m_currentPipelineHandle = pipelineHandle;
      m_pipelineIsActive = true;
   }

   void GLCommandList::bindVertexBuffer(BufferHandle vBufferHandle){
      FIG_ASSERT(m_pipelineIsActive, "No currently bound shader pipeline/program")
      const GLBuffer& buf = m_glDevice.getBuffer(vBufferHandle);

      FIG_ASSERT(buf.targetType == GLBufferTargetType::VERTEX_BUFFER, "Attempting to bind a vertex buffer which is not a vertex buffer");
      FIG_ASSERT(buf.hasData, "Attempting to bind a vertex buffer prior to uploading any data")

      GLuint vao = m_glDevice.getVAO(m_currentPipelineHandle, vBufferHandle);
      m_openGL.glBindVertexArray(vao);
   }

   void GLCommandList::bindIndexBuffer(BufferHandle idxBufHandle){
      // TODO: Wiki says that GL_ELEMENT_ARRAY_BUFFER is only a valid target if a VAO 
      // has been bound. Add something which checks that and can provide a log or info 
      // about that
      GLBuffer& buf = m_glDevice.getBuffer(idxBufHandle);
      FIG_ASSERT(buf.targetType == GLBufferTargetType::INDEX_BUFFER, "Trying to bind a buffer whose target is not an element buffer")

      m_openGL.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf.id);
   }

   void GLCommandList::setUniformBufferData(StandardUniformBlock block, const void* data){
      GLBuffer& buf = m_glDevice.getBuffer(block);
      m_openGL.glBindBuffer(GL_UNIFORM_BUFFER, buf.id);
      m_openGL.glBufferSubData(GL_UNIFORM_BUFFER, 0, buf.size, data);
      buf.targetType = GLBufferTargetType::UNIFORM_BUFFER;
      buf.hasData = true;
   }

   void GLCommandList::setUniformBufferData(BufferHandle handle, const void* data){
      FIG_UNREACHABLE("This function is not implemented yet. Just keeping it here in case we want to use it to set custom uniform blocks for shaders/materials")
      return;

      static bool outputWarning { true };
      if(outputWarning){
         FIG_LOG_MEDIUM_WARNING("This function operates on a const buffer however it changes things about that buffer's state in openGL without being able to update it CPU side (i.e. has data or buffer target type). Might be best to set data only through the render device")
         outputWarning = false;
      }

      const GLBuffer& buf = m_glDevice.getBuffer(handle);
      // TODO: This might be better as a warning, if not just a log message, if anything at all
      FIG_ASSERT(buf.targetType == GLBufferTargetType::UNIFORM_BUFFER, "Trying to set uniform data to a buffer not marked originally marked as uniform data. Not sure if this matter");

      // TODO: See warning, but we would need to in the future update the GLBufferOBject so it stays concurrent 
      // buf.hasData = true;
      // buf.targetType = GLBufferTargetType::UNIFORM_BUFFER;

      m_openGL.glBindBuffer(GL_UNIFORM_BUFFER, buf.id);
      m_openGL.glBufferSubData(GL_UNIFORM_BUFFER, 0, buf.size, data);
      // m_openGL.glBindBufferBase(GL_UNIFORM_BUFFER, 0, buf.id);
      static bool w  { true };
      if(w){
         FIG_LOG_HIGH_WARNING("Need to figure out a way to not use the magic 0 constant")
         w = false;
      }
   }

   void GLCommandList::setUniformVariable(const std::string& variableName, const void* data){
      FIG_ASSERT(data, "Trying to set value of data with nullptr");
      FIG_ASSERT(m_pipelineIsActive, "No currently bound pipeline");

      const GLShaderPipeline& pipeline = m_glDevice.getShaderPipeline(m_currentPipelineHandle);
      auto it = std::find_if(pipeline.uniforms.begin(), pipeline.uniforms.end(), 
            [&variableName](const UniformReflectionMetadata& var) {
               return var.variableName == variableName;
            });

      FIG_ASSERT(it != pipeline.uniforms.end(), "Trying to set a uniform variable with a name that is not a variable");

      // TODO: Uniform location speed up (see GLRenderDevice TODO in one of the structs)
      GLint location = m_openGL.glGetUniformLocation(pipeline.programID, it->variableName.c_str());
      FIG_ASSERT(location != -1, "Could not find var name. This is more problematic because we should have already checked this");

      // only fmats for opengl 
      // double depends on version/extensions 
      // bool use single set. 0 = false, non 0 = true
      const ShaderTypeDescription& typeDesc = it->typeDesc;
      switch(typeDesc.primType){
         case(PrimitiveType::BOOL):
            m_openGL.glUniform1f(location, (*((const bool*)data)) ? 1.f : 0.f);
            break;
         case(PrimitiveType::I32): 
            {
               const GLint* ptr = static_cast<const GLint*>(data);
               switch(typeDesc.primCnt){
                  case(1):
                     m_openGL.glUniform1iv(location, 1, ptr);
                     break;
                  case(2):
                     m_openGL.glUniform2iv(location, 1, ptr);
                     break;
                  case(3):
                     m_openGL.glUniform3iv(location, 1, ptr);
                     break;
                  case(4):
                     m_openGL.glUniform4iv(location, 1, ptr);
                     break;
                  case(9):
                  case(16):
                     FIG_UNREACHABLE("OpenGL does not do int matrices")
                     // TODO: Handle this better (convert to float and try?)
                     break;
               }
               break;
            } // I32 case
         case(PrimitiveType::FLOAT):
            {
               // TODO: Delete this if once I get rid of the medium warning below
               if(typeDesc.primCnt == 9 || typeDesc.primCnt == 16){
                  static bool w { true };
                  if(w){
                     FIG_LOG_MEDIUM_WARNING("Setting a uniform of type mat3/4 but I have not thought about whether I want to automatically transpose it or not. If you need this matrix right now, then think about how to handle")
                     w = false;
                  }
               }
               const GLfloat* ptr = static_cast<const GLfloat*>(data);
               switch(typeDesc.primCnt){
                  case(1):
                     m_openGL.glUniform1fv(location, 1, ptr);
                     break;
                  case(2):
                     m_openGL.glUniform2fv(location, 1, ptr);
                     break;
                  case(3):
                     m_openGL.glUniform3fv(location, 1, ptr);
                     break;
                  case(4):
                     m_openGL.glUniform4fv(location, 1, ptr);
                     break;
                  case(9):
                     m_openGL.glUniformMatrix3fv(location, 1, GL_FALSE, ptr);
                     break;
                  case(16):
                     m_openGL.glUniformMatrix4fv(location, 1, GL_FALSE, ptr);
                     break;
               }
               break;
            } // FLOAT case
         case(PrimitiveType::DOUBLE):
            {
               // TODO: Deal with this
               FIG_UNREACHABLE("OpenGL only supports doubles with a certain version or extension, and i don't wanna deal with that rn")
               break;
            }
         case(PrimitiveType::INVALID):
            // TODO: Deal with this
            FIG_UNREACHABLE("Invalid type");
            break;
      }
   }

   void GLCommandList::setUniformVariable(const std::string& variableName, const ShaderData& data){
      static bool w { true };
      if(w){
         // NOTE/TODO: For below, I can probably pull out the logic from here and the const void* version 
         // and figure out what those need to find on their own, and then send it to its own central func
         FIG_LOG_LOW_WARNING("Because this function accepts const ShaderData& instead of const void*, it should check to make sure the type passed and the type expected for this variable are the same, but it doesn't cuz I didn't wanna do that")
         w = false;
      }
      // TODO: This can provide some extra handling to ensure we are setting the right things
      const void* ptr = std::visit(ShaderVisitors::GetConstVoidPtr{data}, data);
      // TODO: Handle this... Can this even happen?
      FIG_ASSERT(ptr, "nullptr returned from std::visit")
      setUniformVariable(variableName, ptr);
   }

   void GLCommandList::drawElement(std::size_t indexCount){
      // TODO: GL_UNSIGNED_SHORT is what we use, and good for any mesh with less than 2^16 vertices 
      // Will we ever need to support more? If so we will need to A) ensure that we update how these 
      // values are stored in our MeshLoader (or other areas) and know how to update here its type
      m_openGL.glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_SHORT, (void*)0);
   }

} // namespace Core
