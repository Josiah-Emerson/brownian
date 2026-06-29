#pragma once
#include "Core_Graphics/RenderDevice.h"
#include "Core_Utils/Linear/Vector.h"
#include "Core_Graphics/ShaderPipeline.h"

namespace Core{
   // Data for one vertex
   struct VertexData{
      Linear::fvec3 position;

      static VertexLayout getVertexLayout(){
         return VertexLayout{{
            { .location = 0, .dataType = ShaderDataType::F_VEC3, .offset = 0 } // Alternatively: offsetof(VertexData, position)
         }, sizeof(VertexData)};
      }
   };

   class Mesh{
      public:
         Mesh(BufferHandle vertexBuffer, std::size_t vertexCount);

         void outputLog() const;
         BufferHandle vertexBuffer() const { return m_vertexBuffer; }
         std::size_t vertexCount() const { return m_vertexCount; }
         std::size_t indexCount() const { return m_indexCount; }

      private: 
         BufferHandle m_vertexBuffer;
         std::size_t m_vertexCount;
         std::size_t m_indexCount;
   };
} // namespace Core
