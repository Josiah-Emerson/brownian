#pragma once
#include "Core_Utils/Linear/Vector.h"

namespace Core{
   enum class LoadOp{
      NONE,  // Essentially same as KEEP, but meant for use cases where the entire buffer will be overwritten and thus it doesn't matter
      CLEAR, // Clear buffer to default value first
      KEEP,  // Keep scene in buffer, but meant for use cases where certain values might be purposely not overwritten
   };

   enum class StoreOp{
      STORE,    // Keep values 
      NO_STORE, // Discard values when done 
   };

   // TODO: Implement choosing which buffer in the future via swapchain?
   // For now just use the one that makes sense
   struct RenderPassDesc {
      // Color buffer 
      LoadOp colorLoadOp            = LoadOp::CLEAR;
      Linear::fvec4 colorClearValue = {0.f, 0.f, 0.f, 1.f};
      StoreOp colorStoreOp          = StoreOp::STORE;

      // Depth buffer
      LoadOp depthLoadOp            = LoadOp::CLEAR;
      float depthClearValue         = 1.f;
      StoreOp depthStoreOp          = StoreOp::STORE;

      // TODO: Other buffers? Stencil?
   };
} // namespace Core
