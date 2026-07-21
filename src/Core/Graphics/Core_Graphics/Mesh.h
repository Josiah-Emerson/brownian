#pragma once
#include "Core_Graphics/RenderDevice.h"
#include "Core_Utils/Linear/Vector.h"
#include "Core_Graphics/ShaderPipeline.h"

namespace Core{
   // TODO: Overhaul the VertexData stuff so that it is far more flexible
   // Data for one vertex
   struct VertexData{
      static VertexLayout getVertexLayout(){
         return VertexLayout{{
            {.location = 0, .typeDesc = {.primType = PrimitiveType::FLOAT, .primCnt = 3}, .offset = 0}
         }, sizeof(VertexData)};
      }

      private:
      Linear::fvec3 position; // for sizeof();
   };

   struct Mesh{
      BufferHandle vBuf;
      BufferHandle eBuf;
      std::size_t vertexCnt;
      std::size_t elementCnt;
   };
   /*
   class Mesh{
      public:
         Mesh(BufferHandle vertexBuffer, std::size_t vertexCount);

         void outputLog() const;
         BufferHandle vertexBuffer() const { return m_vertexBuffer; }
         std::size_t vertexCount() const { return m_vertexCount; }
         std::size_t indexCount() const { return m_indexCount; }

      private: 
         BufferHandle m_vertexBuffer;
         BufferHandle m_elementBuffer;
         std::size_t m_vertexCount;
         std::size_t m_indexCount;
   };
   */
} // namespace Core
