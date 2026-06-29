#include "Mesh.h"
#include "Core_Utils/Log.h"

namespace Core{
   Mesh::Mesh(BufferHandle vertexBuffer, std::size_t vertexCount)
      : m_vertexBuffer(vertexBuffer)
      , m_vertexCount(vertexCount)
      , m_indexCount(vertexCount) // TODO: For now we have no EBO, so just do this 
   {
   }

   void Mesh::outputLog() const{
      std::string message = "Mesh with vertex buffer handle id " + std::to_string(m_vertexBuffer.idx);
      FIG_LOG_MESSAGE(message.c_str())
   }
} // namespace Core
