#include "AssetManager.h"
#include "Core_Utils/Log.h"
#include "MeshLoader.h"

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

      m_materials.emplace_back(hShaderPipeline);
      return MaterialHandle { m_materials.size() - 1 };
   }

   const Mesh& AssetManager::getMesh(MeshHandle hMesh){
      // TODO: Bounds check
      return m_meshes[hMesh.idx];
   }

   const Material& AssetManager::getMaterial(MaterialHandle hMaterial){
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
         .uLayout = { }, // TODO: Use this for custom uniform values ?
      };

      ShaderPipelineHandle hShaderPipeline = m_device->createShaderPipeline(desc);
      m_shaderPipelineCache[key] = hShaderPipeline;

      return hShaderPipeline;
   }
} // namespace Core
