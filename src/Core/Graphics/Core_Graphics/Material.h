#pragma once
#include "Core_Graphics/RenderDevice.h"
#include "UI/Core_UI/UniformVariableWidgets.h"
#include <unordered_map>

namespace Core{
   class Material {
      public: 
         // A variable's data type will have N primitives (i.e. F_VEC3 N = 3, FLOAT N = 1, etc...)
         // Each IUniformWidget type can edit K primitives (i.e. RGBSelector K = 3, Slider K = 1, etc...)
         // The sum of all K in widgets should be equal to N (i.e. each primitive should be accounted for)
         // labels represents what is diplayed before each widget, and thus should be one to one with the widgets vector
         struct UniformDescription{
            std::string name;
            ShaderData data;
            std::vector<std::shared_ptr<const IUniformWidget>> widgets;
            std::vector<std::string> labels;
         };

         struct UniformMetadata{
            std::string name; // variable name

            // There should be the same amount of labels as widgets
            std::vector<std::string> labels;
            std::vector<std::shared_ptr<const IUniformWidget>> widgets;
         };


         Material(ShaderPipelineHandle hPipeline, std::vector<UniformDescription> uniforms);

         ShaderPipelineHandle getShaderPipelineHandle() const { return m_shaderPipelineHandle; }

         const std::vector<UniformMetadata>& getUniformMetadata() const { return m_uniformMetadata; }

         // User's responsibility to set it correctly
         void setVariable(const std::string& variable, ShaderData value);
         const ShaderData& getVariable(const std::string& variable) const;

      private: 
         ShaderPipelineHandle m_shaderPipelineHandle;
         std::vector<UniformMetadata> m_uniformMetadata;
         std::unordered_map<std::string, ShaderData> m_customUniforms;
   };

};
