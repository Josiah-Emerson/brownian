#pragma once
#include "Core_Graphics/Mesh.h"
#include "Core_Graphics/RenderDevice.h"
#include "Core_Graphics/Material.h"
#include <cstddef>
#include <string>
#include <vector>

namespace Core{
   // TODO: Perhaps want to update all handles so that they the first 32 bits be the idx 
   // and the second 32 bits be the generation number like we talked about with gemski
   struct MeshHandle{
      std::size_t idx;
   };

   struct MaterialHandle{
      std::size_t idx;
   };

   struct ShaderProgramHandle;


   class AssetManager{
      public: 
         AssetManager(RenderDevice* renderDevice);

         // Creates a mesh from the path. 
         // TODO: Should we hash the paths so that we can return the Handle 
         // to a path thats already been parsed and allocated ?
         MeshHandle createMesh(const std::string& path);
         MeshHandle createMesh(const char* path);

         MaterialHandle createMaterial(ShaderProgramHandle shaderProgramHandle);

         const Mesh& getMesh(MeshHandle meshHandle) const;
         const Material& getMaterial(MaterialHandle materialHandle) const;

      private: 
         RenderDevice* m_device;
         std::vector<Mesh> m_meshes;
         std::vector<Material> m_materials;
   };
} // namespace Core
