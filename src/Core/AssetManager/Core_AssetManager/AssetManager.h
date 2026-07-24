#pragma once
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>
#include "Core_Utils/Log.h"
#include "FMATReader.h"
#include "Core_Graphics/Mesh.h"
#include "Core_Graphics/Material.h"

namespace Core{
   class RenderDevice;

   // TODO: likely update to index and generation 
   struct MeshHandle { 
      std::size_t idx;
      bool operator==(const MeshHandle& other) const { return idx == other.idx; }
   };

   struct MaterialHandle { 
      std::size_t idx;
   };

   class AssetManager{
      public: 
         AssetManager(RenderDevice* rd);

         MeshHandle loadMesh(const std::string& path);
         MeshHandle loadMesh(const char* path);

         MaterialHandle loadMaterial(const std::string& path);
         MaterialHandle loadMaterial(const char* path);

         const Mesh& getMesh(MeshHandle hMesh) const;
         Mesh& getMesh(MeshHandle hMesh);
         const Material& getMaterial(MaterialHandle hMaterial) const;
         Material& getMaterial(MaterialHandle hMaterial);

      private: 
         struct ShaderPipelineKey{
            std::string vertexPath;
            std::string fragmentPath;

            bool operator==(const ShaderPipelineKey& other) const;
         };

         struct ShaderPipelineKeyHash{
            std::size_t operator()(const ShaderPipelineKey& key) const noexcept;
         };

         ShaderPipelineHandle getOrCreateShaderPipeline(const FMATReader::FMATObject& fmatObject);

         ShaderData getDefaultData(const ShaderTypeDescription& typeDesc);

         template<ShaderVisitors::IsPrimitiveType T>
         ShaderData getDefaultDataTyped(std::size_t pCnt, T def);

         std::vector<std::shared_ptr<const IUniformWidget>> getDefaultWidgets(const ShaderTypeDescription& typeDesc);

      private: 
         RenderDevice* m_device;

         // Asset Storage
         std::vector<Mesh> m_meshes;
         std::vector<Material> m_materials;

         // Asset cache
         std::unordered_map<std::filesystem::path, MeshHandle> m_meshCache;
         std::unordered_map<std::filesystem::path, MaterialHandle> m_materialCache;
         std::unordered_map<ShaderPipelineKey, ShaderPipelineHandle, ShaderPipelineKeyHash> m_shaderPipelineCache;
   };

   template<ShaderVisitors::IsPrimitiveType T>
   ShaderData AssetManager::getDefaultDataTyped(std::size_t pCnt, T def){
      ShaderData ret;
      switch(pCnt){
         case(1): 
            ret = def;
            break;
         case(2): 
            ret = Linear::Vector<T, 2> (def);
            break;
         case(3): 
            ret = Linear::Vector<T, 3> (def);
            break;
         case(4): 
            ret = Linear::Vector<T, 4> (def);
            break;
         case(9): 
            ret = Linear::Matrix<T, 3, 3> (def);
            break;
         case(16): 
            ret = Linear::Matrix<T, 4, 4> (def);
            break;
         default: 
            FIG_UNREACHABLE("Invalid or unhandled primitive count")
      }

      return std::move(ret);
   }

   template<>
   ShaderData AssetManager::getDefaultDataTyped<bool>(std::size_t pCnt, bool def);
} // namespace Core
