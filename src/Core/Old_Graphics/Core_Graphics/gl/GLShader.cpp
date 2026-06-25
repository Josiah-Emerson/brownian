#include "Core_Graphics/gl/GLShader.h"
#include "Core_Graphics/Shader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace Core{
   GLShader::GLShader(const char* filepath, ShaderType type, OpenGL& openGL)
      : Shader { type, NO_COMPILATION_ATTEMPTED }
      , m_filepath { filepath }
      , m_openGL { openGL }
      { }

   GLShader::~GLShader(){
      m_openGL.glDeleteShader(m_ID);
   }

   bool GLShader::compile(){
      m_ID = m_openGL.glCreateShader(getGLShaderType());
      std::string shaderCode;
      std::ifstream shaderStream(m_filepath, std::ios::in);
      if(!shaderStream.is_open()){
         m_stage = FILE_PARSE_FAILURE;
         return false;
      }

      // Get Shader Code
      std::stringstream sstr;
      sstr << shaderStream.rdbuf();
      shaderCode = sstr.str();
      shaderStream.close();


      // compile shader
      char const* sourcePtr = shaderCode.c_str();
      m_openGL.glShaderSource(m_ID, 1, &sourcePtr, NULL);
      m_openGL.glCompileShader(m_ID);

      // check compilation status
      GLint res = GL_FALSE;
      m_openGL.glGetShaderiv(m_ID, GL_COMPILE_STATUS, &res);
      if(res != GL_TRUE){
         m_stage = COMPILATION_FAILED;
         return false;
      }

      m_stage = SUCCESSFUL_COMPILATION;
      return true;
   }

   bool GLShader::isCompiled() const{
      // TODO: If compilation is stale do we want to return true or false? If we return true, 
      // may want to give the user a way to check if it is a stale compilation or not
      return m_stage == SUCCESSFUL_COMPILATION || m_stage == STALE_COMPILATION;
   }

   // TODO: Implement this 
   bool reload() { return false; }

   std::string GLShader::getLogInfo() const {
      // TODO: See GLShaderProgram's getLogInfo func, but we probably shouldn't be doing this logic here, but rather just returning what OpenGL's log has for us 
      // Something else should do the logic of what the status is based on m_stage

      switch(m_stage){
         case(NO_COMPILATION_ATTEMPTED):
            {
               return "No compilation has been attempted yet.";
            }
         case(FILE_PARSE_FAILURE):
            {
               return "A failure occured while trying to read the file at " + m_filepath;
            }
         case(COMPILATION_FAILED):
            {
               return "An error occurred while compiling " + m_filepath + ": \n" + getLogInfoFromOpenGL().c_str();
            }
         case(RECOMPILATION_FAILED):
            {
               return "An error occurred while attempting to recompile " + m_filepath + "."
                  + "The last successfully compiled shader is still retained.\n" + getLogInfoFromOpenGL();
            }
         case(SUCCESSFUL_COMPILATION):
            {
               return "Shader is successfully compiled and up to date";
            }
         case(STALE_COMPILATION):
            {
               return "A stale version of this shader is compiled. Reload for newest changes";
            }
      }
      return "WTF. The shader is somehow in an unhandled Shader Compilation Stage";
   }

   std::string GLShader::getLogInfoFromOpenGL() const{
      int infoLogLength;
      m_openGL.glGetShaderiv(m_ID, GL_INFO_LOG_LENGTH, &infoLogLength);
      if(infoLogLength == 0){
         return "No information available from openGL";
      }else if(infoLogLength < 0){ // TODO: Just to see if we ever get here somehow (shouldn't). remove for release
         return "WTF. See GLShader::getLogInfoFromOpenGL()";
      }

      char* message = new char[infoLogLength + 1];
      m_openGL.glGetShaderInfoLog(m_ID, infoLogLength, NULL, message);
      return message;
   }

   bool GLShader::operator==(const Shader& other) const{
      if(const GLShader* otherGL = dynamic_cast<const GLShader*>(&other)){
         return ( (m_openGL == otherGL->m_openGL) && (m_ID == otherGL->m_ID) );
      }

      return false;
   }

   GLenum GLShader::getGLShaderType() const{
      switch(getShaderType()){
         case(VERTEX_SHADER):
            return GL_VERTEX_SHADER;
         case(FRAGMENT_SHADER): 
            return GL_FRAGMENT_SHADER;
         case(COMPUTE_SHADER):
            return GL_COMPUTE_SHADER;
         default: 
            throw std::runtime_error("ShaderType not yet handled in GLShader class");
      }
   }

} // namespace Core
