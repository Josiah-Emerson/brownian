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
      static bool w { true };
      if(w){
         FIG_LOG_MEDIUM_WARNING("Need to update this function to actually load materials once proof of concept works")
         w = false;
      }

      // IMPORTANT NOTE: Obviously we will need to separate shader creation from 
      // material creation
      ShaderPipelineDesc desc {
         .vertex = "Resources/Shaders/Vertex.vs",
         .fragment = "Resources/Shaders/Fragment.fs",
         .vLayout = VertexData::getVertexLayout(),
         .uLayout = { }, // TODO: use this for custom uniform values?
      };

      ShaderPipelineHandle h = m_device->createShaderPipeline(desc);
      m_materials.emplace_back(h);
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
} // namespace Core
