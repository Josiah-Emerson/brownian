#include "Core_Graphics/gl/GLShaderProgram.h"
#include "Core_Graphics/ShaderProgram.h"
#include "Core_Graphics/gl/GLShader.h"
#include "Core_Utils/Log.h"
#include <algorithm>

// IMPORTANT NOTE: see NOTE in addShader before implementing any function which would need to advance the m_stage
// Should check to make sure we are in the correct state at the beginning of any of these functions

namespace Core{
   GLShaderProgram::GLShaderProgram(OpenGL& openGL)
      : ShaderProgram()
      , m_ID { 0 } // if ID is 0, openGL will silently ignore all (i think) requests related to this ID
      , m_openGL(openGL)
   { }

   GLShaderProgram::~GLShaderProgram(){
      // NOTE: openGL specification says that glDeleteProgram automatically detaches all attached shader programs
      // and thus we shouldn't need to explicitly detach them
      m_openGL.glDeleteProgram(m_ID);
   }

   bool GLShaderProgram::initializeProgram() {
      m_ID = m_openGL.glCreateProgram();
      if(m_ID == 0){
         m_state.status |= CREATION_FAILURE;
         return false;
      }

      setState(CREATED);
      return true;
   }

   bool GLShaderProgram::addShader(std::shared_ptr<Shader> shader) {
      // NOTE: we must reset the program stage at the beginning of the function to a stage that would make sense
      // Or we make sure we update m_stage before EVERY return bool value, even true. 
      // Example:
      // 1) addShader is attempted but fails and the state becomes SHADER_ATTACHMENT_NOT_ATTEMPTED or SHADER_ATTACHMENT_FAILURE
      // 2) addShader happens again and works however, state is never updated, and it is still one of the above fail states

      if(m_state.stage == NOT_CREATED || !shader->isCompiled()){
         m_state.status |= ATTACHMENT_NOT_ATTEMPTED;
         return false;
      }

      for(std::shared_ptr<Shader> ptr : m_shaders){ // check to see if already added
         if(*shader == *ptr){
            return true;
         }
      }

      // TODO: Probably remove from release build? dynamic cast has some overhead compared to a static cast, and 
      // at least in my head, the only shaders that should be passed to this function come from a GL implementation and thus should be a GLShader.
      // but hopefully if thats not the case this will catch it
      FIG_ASSERT(dynamic_cast<GLShader*>(shader.get()) != nullptr , "INTERNAL ERROR: shader is not of type GLShader, and failed a dynamic cast.")
      auto glShader = static_cast<GLShader*>(shader.get());
      const GLuint shaderID = glShader->getShaderID();
      m_openGL.glAttachShader(m_ID, shaderID);

      // Validate it was attached
      GLint numShaders;
      m_openGL.glGetProgramiv(m_ID, GL_ATTACHED_SHADERS, &numShaders);

      GLuint* shaders = new GLuint[numShaders];
      m_openGL.glGetAttachedShaders(m_ID, numShaders, NULL, shaders);

      bool found {false};
      for(std::size_t i { 0 }; i < numShaders; ++i){
         if(shaderID == shaders[i]){
            found = true;
            break;
         }
      }
      delete[] shaders;
      if(!found){
         m_state.status |= ATTACHMENT_FAILED;
         return false;
      }

      m_shaders.push_back(shader);
      return true;
   }

   bool GLShaderProgram::link() {
      if(m_state.stage == NOT_CREATED){
         m_state.status |= LINK_FAILED;
         return false;
      }

      m_openGL.glLinkProgram(m_ID);

      GLint res;
      m_openGL.glGetProgramiv(m_ID, GL_LINK_STATUS, &res);
      if(res != GL_TRUE){
         m_state.status |= LINK_FAILED;
         return false;
      }

      setState(LINKED);
      populateVariableVectors();

      return true;
   }

   bool GLShaderProgram::useProgram(){
      // TODO: What happens if a uniform variable isn't set? 

      if(m_state.stage != LINKED || isFailBitSet(m_state.status, LINK_FAILED)){
         return false;
      }

      // ensure openGL thinks it is valid
      // NOTE: This might take a toll on performance and could possibly be left out
      GLint res;
      m_openGL.glValidateProgram(m_ID);
      m_openGL.glGetProgramiv(m_ID, GL_VALIDATE_STATUS, &res);
      if(res != GL_TRUE){
         m_state.status |= INVALID_FOR_USE;
         return false;
      }

      // Clear invalid for use 
      m_state.status &= ~INVALID_FOR_USE;
      m_openGL.glUseProgram(m_ID);
      return true;
   }

   std::string GLShaderProgram::getInfoLog() const {
      if(m_state.stage == NOT_CREATED){
         return "";
      }

      GLint length;
      m_openGL.glGetProgramiv(m_ID, GL_INFO_LOG_LENGTH, &length);
      if( length <= 0)
         return "";

      GLchar* log = new GLchar[length];
      m_openGL.glGetProgramInfoLog(m_ID, length, NULL, log);
      std::string ret { log }; // should perform a deep copy ?
      delete[] log;

      return ret;
   }


   void GLShaderProgram::populateVariableVectors() {
      GLint numActive, maxNameLength, charsWritten, size;
      GLenum type;

      // Attribute Vars
      m_openGL.glGetProgramiv(m_ID, GL_ACTIVE_ATTRIBUTES, &numActive);
      m_openGL.glGetProgramiv(m_ID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxNameLength);

      m_attributeVariables.reserve(numActive);
      GLchar* name = new GLchar[maxNameLength];

      for(GLuint i { 0 }; i < numActive; ++i){
         m_openGL.glGetActiveAttrib(m_ID, i, maxNameLength, &charsWritten, &size, &type, name);
         m_attributeVariables.emplace_back(shaderDataTypeFromGLenum(type),
                                       std::move(std::string(name, charsWritten)), 
                                       size);
      }
      delete[] name;

      // Uniform vars
      m_openGL.glGetProgramiv(m_ID, GL_ACTIVE_UNIFORMS, &numActive);
      m_openGL.glGetProgramiv(m_ID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);

      m_uniformVariables.reserve(numActive);
      name = new GLchar[maxNameLength];
      for(GLuint i { 0 }; i < numActive; ++i){
         m_openGL.glGetActiveUniform(m_ID, i, maxNameLength, &charsWritten, &size, &type, name);
         m_uniformVariables.emplace_back(shaderDataTypeFromGLenum(type), 
                                          std::move(std::string(name, charsWritten)), 
                                          size);
      }

      delete[] name;
   }

   ShaderDataType GLShaderProgram::shaderDataTypeFromGLenum(const GLenum type) const{
      switch(type){
         case(GL_INT): return INT;
         case(GL_FLOAT): return FLOAT;
         case(GL_DOUBLE): return DOUBLE;
         case(GL_FLOAT_VEC3): return F_VEC3;
         case(GL_FLOAT_MAT4): return F_MAT4;
         default: return UNKNOWN;
      }
   }

   GLenum GLShaderProgram::glenumFromShaderDataType(const ShaderDataType type) const{
      switch(type){
         case(INT): return GL_INT;
         case(FLOAT): return GL_FLOAT;
         case(DOUBLE): return GL_DOUBLE;
         case(F_VEC3): return GL_FLOAT_VEC3;
         case(F_MAT4): return GL_FLOAT_MAT4;
         case(UNKNOWN): FIG_ASSERT(false , "Not sure what GLenum to return from UNKNOWN ShaderDataType.") // TODO: If we remove these asserts, we need to update the lower retun statement
         default:  FIG_ASSERT(false , "Wow we have an undhandled ShaderDataType!!\n")
      }

      return GL_INT; // just so we don't get a compiler warning. IF WE REMOVE THE ASSERTS UP THEN WE NEED TO FIGURE OUT WHAT WE NEED TO RETURN
   }

   bool GLShaderProgram::setUniformCallback(std::string_view name, UniformVariableSetterFunction callback){
      auto search = std::find_if(m_uniformVariables.begin(), m_uniformVariables.end(), 
            [name](const ShaderInputVariableInfo& var){
               return name == var.name;
            });
      if(search == m_uniformVariables.end())
         return false;
      search->callback = std::move(callback);
      return true;
   }

   bool GLShaderProgram::setUniform(std::string name, void* value){
      // TODO: add functionality for other data types 

      auto search = std::find_if(m_uniformVariables.begin(), m_uniformVariables.end(), 
            [name](const ShaderInputVariableInfo& var){
               return name == var.name;
            });
      if(search == m_uniformVariables.end())
         return false;

      ShaderDataType type = search->type;

      GLint uID = m_openGL.glGetUniformLocation(m_ID, name.c_str());
      if(uID == -1)
         return false;

      switch(type){
         case(F_VEC3):
            {
               m_openGL.glUniform3fv(uID, 1, (float*)value);
               break;
            }
         case(F_MAT4):
            {
               m_openGL.glUniformMatrix4fv(uID, 1, GL_TRUE, (float*)value);
               break;
            }
         default: 
            return false;
      }
      return true;
   }

} // namespace Core
