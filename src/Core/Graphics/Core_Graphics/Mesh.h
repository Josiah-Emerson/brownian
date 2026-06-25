#pragma once
#include "Core_Graphics/RenderDevice.h"

namespace Core{
   class Mesh{
      public:
         Mesh(BufferHandle vertexBuffer);

         void outputLog() const;
         BufferHandle vertexBuffer() const;

      private: 
         BufferHandle m_vertexBuffer;
   };
} // namespace Core
