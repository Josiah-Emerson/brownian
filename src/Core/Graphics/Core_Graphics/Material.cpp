#include "Material.h"
#include "Core_Utils/Log.h"

namespace Core{
   Material::Material(ShaderPipelineHandle hPipeline, std::vector<UniformDescription> uniforms)
      : m_shaderPipelineHandle {hPipeline}
   {
      m_customUniforms.reserve(uniforms.size());
      m_uniformMetadata.reserve(uniforms.size());

      for(UniformDescription& uniform : uniforms){
         // All widgets must have base data type as expected from ShaderData
         // The total primitives accounted for from all widgets should equal the number from 
         // number of labels should not exceed the number of widgets
         FIG_ASSERT(uniform.labels.size() <= uniform.widgets.size(), "More labels than widgets")

         PrimitiveType expectedPrimitiveType = std::visit(ShaderVisitors::PrimitiveType {}, uniform.data);
         std::size_t expectedPrimitiveCount = std::visit(ShaderVisitors::PrimitiveCount {}, uniform.data);
         int primitiveCount { 0 };

         for(const std::shared_ptr<const IUniformWidget>& w : uniform.widgets){
            FIG_ASSERT(expectedPrimitiveType == w->primitiveType(), "Uniform variable and widget type mismatch");
            primitiveCount += w->primitiveCount();
         }
         FIG_ASSERT(primitiveCount == expectedPrimitiveCount, "Widgets provided for this uniform variable do not handle the correct amount of primitive data types for this uniform variable's shader data type")
            
         // TODO: If FIG_ASSERT is compiled out, the below count parameter will not be properly checked and if labels is larger than widgets we have an issue
         uniform.labels.insert(uniform.labels.end(), uniform.widgets.size() - uniform.labels.size(), "");

         m_uniformMetadata.emplace_back(UniformMetadata{
               .name    = uniform.name,
               .labels  = std::move(uniform.labels),
               .widgets = std::move(uniform.widgets),
               });

         m_customUniforms.try_emplace(std::move(uniform.name), std::move(uniform.data));
      }
   }

   void Material::setVariable(const std::string& variable, ShaderData value){
      auto it = m_customUniforms.find(variable);
      // TODO: Handle
      FIG_ASSERT(it != m_customUniforms.end(), "Trying to set a variable which does not exist")
         
      it->second = value;
   }

   const ShaderData& Material::getVariable(const std::string& variable) const {
      auto it = m_customUniforms.find(variable);
      FIG_ASSERT(it != m_customUniforms.end(), "Trying to get a variable which does not exist")

      return it->second;
   }
} // namespace Core
