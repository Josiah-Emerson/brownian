#pragma once
#include "Core_Graphics/Shader.h"
#include "Core_Graphics/gl/OpenGL.h"
#include <GL/glext.h>

namespace Core{
   class GLShader : public Shader{
      public:
         GLShader(const char* filepath, ShaderType type, OpenGL& openGL);
         ~GLShader();

         // TODO: Big 5. Just delete for now
         GLShader(const GLShader&) = delete;
         GLShader(GLShader&&) = delete;
         GLShader& operator=(const GLShader&) = delete;
         GLShader& operator=(GLShader&&) = delete;

         bool compile() override;
         bool isCompiled() const override;

         // TODO: actually implement
         bool reload() override { return false; }
         std::string getLogInfo() const override;
         bool operator==(const Shader&) const override;

         GLuint getShaderID() const { return m_ID; }
         GLenum getGLShaderType() const;

      private: 
         std::string getLogInfoFromOpenGL() const;

         std::string m_filepath;
         OpenGL& m_openGL;
         GLuint m_ID { 0 };
   };
} // namespace Core
