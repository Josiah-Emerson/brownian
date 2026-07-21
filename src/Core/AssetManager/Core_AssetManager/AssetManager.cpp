#include "AssetManager.h"
#include "Core_AssetManager/UVarDescriptionLoader.h"
#include "Core_Graphics/Material.h"
#include "Core_Graphics/ShaderData.h"
#include "Core_Graphics/ShaderPipeline.h"
#include "Core_Utils/Log.h"
#include "MeshLoader.h"
#include "Core_UI/UniformVariableWidgetRegistry.h"

namespace Core{
   AssetManager::AssetManager(RenderDevice* rd)
      : m_device(rd)
   {
      FIG_ASSERT(m_device, "Invalid render device")
   }

   MeshHandle AssetManager::loadMesh(const std::string& path){
      return loadMesh(path.c_str());
   }

   MeshHandle AssetManager::loadMesh(const char* path){
      std::filesystem::path p { path };
      auto it = m_meshCache.find(p);

      if(it != m_meshCache.end()){
         return it->second;
      }

      std::unique_ptr<MeshLoader> loader = MeshLoader::createMeshLoader(p, m_device);
      Mesh mesh = loader->load(p);

      m_meshes.push_back(mesh);

      MeshHandle hMesh { m_meshes.size() - 1 };
      m_meshCache[p] = hMesh;

      return hMesh;
   }

   MaterialHandle AssetManager::loadMaterial(const std::string& path){
      return loadMaterial(path.c_str());
   }

   MaterialHandle AssetManager::loadMaterial(const char* path){
      std::filesystem::path p { path };
      auto it = m_materialCache.find(p);
      if(it != m_materialCache.end()){
         return it->second;
      }

      FMATReader fmatReader {};
      FMATReader::FMATObject tmpFmat = fmatReader.read(p);
      std::filesystem::path pathPrepend { "./Resources/Shaders/" };
      FMATReader::FMATObject fmat = {
         .vsPath = pathPrepend / tmpFmat.vsPath,
         .fsPath = pathPrepend / tmpFmat.fsPath,
      };
      ShaderPipelineHandle hShaderPipeline = getOrCreateShaderPipeline(fmat);
      const std::vector<UniformReflectionMetadata>& uniforms = m_device->reflectUniforms(hShaderPipeline);

      UVarDescriptionLoader descLoader {};
      fmat.vsPath += ".json";
      fmat.fsPath += ".json";
      UVarDescriptionLoader::DescriptionSet vDescSet { };
      UVarDescriptionLoader::DescriptionSet fDescSet { };

      if(std::filesystem::exists(fmat.vsPath))
         vDescSet = descLoader.parse(fmat.vsPath);
      if(std::filesystem::exists(fmat.fsPath))
         fDescSet = descLoader.parse(fmat.fsPath);

      std::vector<Material::UniformDescription> uDescs;
      uDescs.reserve(uniforms.size());

      for(const UniformReflectionMetadata& uniform : uniforms){
         auto it = vDescSet.find(uniform.variableName);
         if(it == vDescSet.end()){
            it = fDescSet.find(uniform.variableName);
         }

         if(it == fDescSet.end()){ // not found
            uDescs.emplace_back(Material::UniformDescription{
                  .name     = std::move(uniform.variableName),
                  .data     = getDefaultData(uniform.typeDesc),
                  .widgets  = std::move(getDefaultWidgets(uniform.typeDesc)),
                  .labels   = { }
                  });

            continue; 
         }

         // A custom definition has been found
         ShaderData data = it->second.defaults.has_value() ? 
            it->second.defaults.value() : getDefaultData(uniform.typeDesc);

         // TODO: General edge cases for this function
         std::vector<std::shared_ptr<const IUniformWidget>> widgets;
         std::size_t widgetCnt = it->second.widgets.size();
         if(widgetCnt > 0){
            widgets.reserve(it->second.widgets.size());
            for(const std::string& w : it->second.widgets){
               std::shared_ptr<const IUniformWidget> wPtr = std::move(UniformVariableWidgetRegistry::instance().get(w));
               // TODO: Need to add some runtime check for release builds when this assert is compiled out
               FIG_ASSERT(wPtr, "No widget exists for this widget string")
               widgets.push_back(std::move(wPtr));
            }
         }else {
            widgets = std::move(getDefaultWidgets(uniform.typeDesc));
         }


         uDescs.emplace_back(Material::UniformDescription{
               .name        = std::move(uniform.variableName),
               .data        = data,
               .widgets     = std::move(widgets),
               .labels      = std::move(it->second.labels)
               });
      }

      m_materials.emplace_back(hShaderPipeline, uDescs);
      return MaterialHandle { m_materials.size() - 1 };
   }

   const Mesh& AssetManager::getMesh(MeshHandle hMesh) const{
      // TODO: Bounds check
      return m_meshes[hMesh.idx];
   }

   Mesh& AssetManager::getMesh(MeshHandle hMesh){
      // TODO: Bounds check
      return m_meshes[hMesh.idx];
   }

   const Material& AssetManager::getMaterial(MaterialHandle hMaterial) const{
      // TODO: Bounds check
      return m_materials[hMaterial.idx];
   }

   Material& AssetManager::getMaterial(MaterialHandle hMaterial) {
      // TODO: Bounds check
      return m_materials[hMaterial.idx];
   }

   bool AssetManager::ShaderPipelineKey::operator==(const ShaderPipelineKey& other) const{
      return fragmentPath == other.fragmentPath && 
         vertexPath == other.vertexPath;
   }

   std::size_t AssetManager::ShaderPipelineKeyHash::operator()(const ShaderPipelineKey& key) const noexcept{
      std::size_t h1 = std::hash<std::string>{}(key.vertexPath);
      std::size_t h2 = std::hash<std::string>{}(key.fragmentPath);
      return h1 ^ (h2 << 1);
   }

   ShaderPipelineHandle AssetManager::getOrCreateShaderPipeline(const FMATReader::FMATObject& fmatObject){
      ShaderPipelineKey key{
         .vertexPath = fmatObject.vsPath,
         .fragmentPath = fmatObject.fsPath,
      };

      auto it = m_shaderPipelineCache.find(key);
      if(it != m_shaderPipelineCache.end()){
         return it->second;
      }

      // TODO: Separate shader pipeline creation from shader creation
      // Or should the RenderDevice itself which is in charge of caching/separating out 
      // the vertex and fragment shaders?
      ShaderPipelineDesc desc {
         .vertex = fmatObject.vsPath,
         .fragment = fmatObject.fsPath,
         .vLayout = VertexData::getVertexLayout(),
      };

      ShaderPipelineHandle hShaderPipeline = m_device->createShaderPipeline(desc);
      m_shaderPipelineCache[key] = hShaderPipeline;

      return hShaderPipeline;
   }

   ShaderData AssetManager::getDefaultData(const ShaderTypeDescription& typeDesc){
      switch(typeDesc.primType){
         case(PrimitiveType::BOOL):
            return getDefaultDataTyped<bool>(typeDesc.primCnt, false);
         case(PrimitiveType::I32):
            return getDefaultDataTyped<int32_t>(typeDesc.primCnt, 0);
         case(PrimitiveType::FLOAT):
            return getDefaultDataTyped<float>(typeDesc.primCnt, 0.f);
         case(PrimitiveType::DOUBLE):
            return getDefaultDataTyped<double>(typeDesc.primCnt, 0.0);
         default:
            break;
      }

      FIG_UNCREACHABLE("Either invalid primType or unhandled prim type");
      return { };
   }

   template<>
   ShaderData AssetManager::getDefaultDataTyped<bool>(std::size_t pCnt, bool def){
      FIG_ASSERT(pCnt == 1, "Only have support for a single bool, not a vector or array");
      return ShaderData { def };
   }

   std::vector<std::shared_ptr<const IUniformWidget>> AssetManager::getDefaultWidgets(const ShaderTypeDescription& typeDesc){
      using Widget = std::shared_ptr<const IUniformWidget>;
      std::vector<Widget> ret { };
      switch(typeDesc.primType){
         case(PrimitiveType::BOOL):
            {
               Widget checkbox = UniformVariableWidgetRegistry::instance().get("Checkbox");
               ret.insert(ret.end(), typeDesc.primCnt, checkbox);
               break;
            }
         case(PrimitiveType::I32):
            {
               Widget slider = UniformVariableWidgetRegistry::instance().get("I32Slider");
               ret.insert(ret.end(), typeDesc.primCnt, slider);
               break;
            }
         case(PrimitiveType::FLOAT):
            {
               Widget slider = UniformVariableWidgetRegistry::instance().get("FSlider");
               ret.insert(ret.end(), typeDesc.primCnt, slider);
               break;
            }
         case(PrimitiveType::DOUBLE):
            {
               Widget slider = UniformVariableWidgetRegistry::instance().get("DSlider");
               ret.insert(ret.end(), typeDesc.primCnt, slider);
               break;
            }
         default:
            FIG_UNCREACHABLE("invalid or unhandled primitive type")
            break;
      }

      return std::move(ret);
   }
} // namespace Core
