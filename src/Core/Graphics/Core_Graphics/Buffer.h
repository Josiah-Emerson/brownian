#pragma once
#include "Core_Utils/Flags.h"

namespace Core{
   // TODO: For now these will be the enums, however look at the openGL 
   // wiki Buffer Object page (General Use section) for info on the OpenGL types of buffers
   namespace BufferUsage{
      // TODO: Actually implement these. Those implemented so far: none
      enum BufferUsageFlags_{
         NONE              = 0,
         VERTEX_BUFFER     = 1 << 0,
         INDEX_BUFFER      = 1 << 1, // EBO / IBO
         UNIFORM_BUFFER    = 1 << 2,
         STORAGE_BUFFER    = 1 << 3,
         TRANSFER_SRC      = 1 << 4,
         TRANSFER_DST      = 1 << 5,
      };

   } // namespace BufferUsage
   typedef Flags<BufferUsage::BufferUsageFlags_> BufferUsageFlags;

   enum class MemoryUsage{
      GPU_ONLY,      // Local to the render device (Fastest GPU access, CPU cannot touch) If the CPU wants to write it their once, it needs to copy it to GPU and have the GPU write it its actual destination
      CPU_TO_GPU,    // CPU writes, GPU reads
      GPU_TO_CPU,    // GPU writes, CPU reads
   };

   struct BufferDesc{
      std::size_t size           = 0;                             // size of the data in bytes
      BufferUsageFlags bUsage    = BufferUsage::VERTEX_BUFFER;
      MemoryUsage mUsage         = MemoryUsage::CPU_TO_GPU;
   };

} // namespace Core
