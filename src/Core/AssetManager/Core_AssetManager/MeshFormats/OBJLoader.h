#pragma once
#include "Core_AssetManager/MeshLoader.h"
#include "Core_AssetManager/MeshFormats/OBJParser.h"

namespace Core{
   class RenderDevice;
   struct BufferHandle;

   class OBJLoader : public MeshLoader{
      public:
         OBJLoader(RenderDevice* rd);
         Mesh load(const std::filesystem::path& path) override;

      private: 
         struct hBufCnt{
            BufferHandle hBuf;
            std::size_t cnt;
         };

         hBufCnt createVertexBuffer(const std::vector<OBJParser::Vertex>& vertices);
         hBufCnt createElementBuffer(const std::vector<OBJParser::Face>& faces);

         RenderDevice* m_device;
   };
} // namespace Core
