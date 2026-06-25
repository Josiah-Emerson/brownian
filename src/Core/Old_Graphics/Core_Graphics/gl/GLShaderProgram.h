#pragma once
#include "Core_Graphics/ShaderProgram.h"
#include "Core_Graphics/gl/OpenGL.h"
#include <GL/gl.h>
#include <GL/glext.h>

namespace Core{
   class GLShaderProgram : public ShaderProgram{
      public:
         GLShaderProgram(OpenGL& openGL);
         ~GLShaderProgram();

         bool initializeProgram() override;

         bool addShader(std::shared_ptr<Shader> shader) override;

         bool link() override;

         // NOTE: See TODO in ShaderProgram.h for this function
         bool useProgram() override;

         std::string getInfoLog() const override;

         // TODO: Actually implement the following 4 funcs
         const std::vector<ShaderInputVariableInfo>& getUniformVariables() const override { return m_uniformVariables; }
         const std::vector<ShaderInputVariableInfo>& getAttributeVariables() const override { return m_attributeVariables; }

         bool setUniformCallback(std::string_view name, UniformVariableSetterFunction callback) override;
         bool setUniform(std::string name, void* value) override;
         bool setAttribute(std::string_view name, void* value, ShaderDataType type) override { return false;}

      private: 
         void populateVariableVectors();
         ShaderDataType shaderDataTypeFromGLenum(const GLenum type) const;
         GLenum glenumFromShaderDataType(const ShaderDataType type) const;

      private:
         OpenGL& m_openGL;
         GLuint m_ID; 
         // NOTE: for vectors below, the index of an input variable in openGL should (?) match its index in the vector since that is how it is populated
         // TODO/NOTE: This is possibly only true for shaders which specify layout = x or whatever. Should probably have a better way of doing that so that it is always correct
   };
} // namespace Core

