#pragma once
#include "Core_Graphics/Mesh.h"
#include <filesystem>

namespace Core{
   class RenderDevice;

   class MeshLoader{
      public:
         virtual ~MeshLoader() = default;
         virtual Mesh load(const std::filesystem::path& path) = 0;
         static std::unique_ptr<MeshLoader> createMeshLoader(const std::filesystem::path& path, RenderDevice* rd);
   };
} // namespace Core
