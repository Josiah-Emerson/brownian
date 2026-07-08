#include "MeshLoader.h"
#include "Core_AssetManager/MeshFormats/OBJLoader.h"
#include "Core_Utils/Log.h"
#include <memory>

namespace Core{
   std::unique_ptr<MeshLoader> MeshLoader::createMeshLoader(const std::filesystem::path &path, RenderDevice* rd){
      if(path.extension() == ".obj"){ // .obj
         return std::make_unique<OBJLoader>(rd);
      }else{
         FIG_UNCREACHABLE("Unsupported mesh type")
      }

      return nullptr;
   }
} // namespace Core
