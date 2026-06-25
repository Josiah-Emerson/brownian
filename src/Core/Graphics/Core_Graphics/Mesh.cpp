#include "Mesh.h"
#include "Core_Utils/Log.h"

namespace Core{
   Mesh::Mesh(BufferHandle vertexBuffer)
      : m_vertexBuffer(vertexBuffer)
   {
      FIG_LOG_MESSAGE("Mesh created")
   }

   void Mesh::outputLog() const{
      std::string message = "Mesh with vertex buffer handle id " + std::to_string(m_vertexBuffer.idx);
      FIG_LOG_MESSAGE(message.c_str())
   }
} // namespace Core
