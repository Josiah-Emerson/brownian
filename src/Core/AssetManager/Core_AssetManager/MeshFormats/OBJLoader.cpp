#pragma once
#include "OBJLoader.h"
#include "Core_Utils/Log.h"
#include <fstream>
#include <string>

namespace Core{
   OBJLoader::OBJLoader(RenderDevice* rd)
      : m_device(rd)
   {
      FIG_ASSERT(m_device, "Invalid render device")
   }

   Mesh OBJLoader::load(const std::filesystem::path& path){
      FIG_ASSERT(path.extension() == ".obj", "Trying to load a mesh using OBJLoader but the extension is not a .obj")


      std::ifstream fstrm {path};
      if(!fstrm.is_open()){
         std::string str {"Error opening file: "};
         str += path;
         FIG_UNREACHABLE(str.c_str());
      }

      OBJParser parser { };
      OBJParser::OBJObject obj = parser.parse(fstrm);

      hBufCnt vBuf = createVertexBuffer(obj.vertices);
      hBufCnt eBuf = createElementBuffer(obj.faces);

      return Mesh{
         .vBuf          = vBuf.hBuf,
         .eBuf          = eBuf.hBuf,
         .vertexCnt     = vBuf.cnt,
         .elementCnt    = eBuf.cnt,
      };
   }

   OBJLoader::hBufCnt OBJLoader::createVertexBuffer(const std::vector<OBJParser::Vertex>& vertices){
      BufferUsageFlags bFlags = BufferUsage::VERTEX_BUFFER;
      bFlags |= BufferUsage::TRANSFER_DST;

      // TODO: For now we will ignore the w value. If we need it in the future, we need to update 
      // the VertexData struct to an F_VEC4, (as well as the shaders to vec4) to properly account for this
      std::vector<float> noWVertices;
      for(const auto& vtx : vertices){
         noWVertices.push_back(vtx.x);
         noWVertices.push_back(vtx.y);
         noWVertices.push_back(vtx.z);
      }

      BufferDesc bufferDesc{
         .size = sizeof(float) * noWVertices.size(),
         .bUsage = bFlags,
         .mUsage = MemoryUsage::GPU_ONLY,
      };

      /* Buffer desc if we use w
      BufferDesc bufferDesc{
         .size = sizeof(OBJParser::Vertex) * vertices.size(),
         .bUsage = bFlags,
         .mUsage = MemoryUsage::GPU_ONLY,
      };
      */

      BufferHandle hBuf = m_device->createBuffer(bufferDesc, noWVertices.data());

      return hBufCnt {hBuf, vertices.size()}; // regular vertices because we still use that for vertex count regardless of whether we use w or not
   }

   OBJLoader::hBufCnt OBJLoader::createElementBuffer(const std::vector<OBJParser::Face>& faces){
      static bool w { true };

      std::vector<unsigned short> indices;
      for(const OBJParser::Face& face : faces){
         // TODO/NOTE: Look into triangulation algorithms for arbitrary polygons if necessary. Chat told me 
         // that most exporters will triangulate the faces regardless, however the obj format does not require this
         if(faces.size() > 3 && w){
            FIG_LOG_LOW_WARNING("The OBJLoader translates faces to polygons assuming they are convex, and uses triangle fanning. At least 1 face has been encountered which has more than 3 vertices, and thus could possibly be concave. The loader will treat it as though it is convex, but cannot guarantee the model will look correct.")
            w = false;
         }
         // 0, 1, 2, 3, 4
         // 0, 1, 2, 
         // 0,    2, 3,
         // 0,       3,  4
         for(std::size_t hi { 2 }; hi < face.vertices.size(); ++hi){
            indices.insert(indices.end(), {
                  static_cast<unsigned short>(face.vertices[0] - 1),
                  static_cast<unsigned short>(face.vertices[hi - 1] - 1),
                  static_cast<unsigned short>(face.vertices[hi] - 1),
                  });
         }
      }

      // TODO: Is there any feasable case where a vertex index is larger than 65535 ?

      BufferUsageFlags bFlags = BufferUsage::INDEX_BUFFER;
      bFlags |= BufferUsage::TRANSFER_DST;

      BufferDesc bDesc{
         .size = sizeof(unsigned short) * indices.size(),
         .bUsage = bFlags,
         .mUsage = MemoryUsage::GPU_ONLY,
      };

      BufferHandle hBuf = m_device->createBuffer(bDesc, indices.data());

      return hBufCnt{ hBuf, indices.size() };
   }

} // namespace Core
