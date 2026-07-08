#pragma once
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>
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

         const Mesh& getMesh(MeshHandle hMesh);
         const Material& getMaterial(MaterialHandle hMaterial);

      private: 
         RenderDevice* m_device;

         // Asset Storage
         std::vector<Mesh> m_meshes;
         std::vector<Material> m_materials;

         // Asset cache
         std::unordered_map<std::filesystem::path, MeshHandle> m_meshCache;
         std::unordered_map<std::filesystem::path, MaterialHandle> m_materialCache;
   };
} // namespace Core
