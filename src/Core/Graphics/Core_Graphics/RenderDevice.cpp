#include "RenderDevice.h"
#include "Core_Utils/Log.h"

#ifdef OPENGL
#include "gl/GLRenderDevice.h"
#endif

namespace Core{
   std::unique_ptr<RenderDevice> RenderDevice::create(Window& window){
#ifdef OPENGL
      return std::make_unique<GLRenderDevice>(window);
#endif

      FIG_LOG_HIGH_WARNING("Failed to create a render device")
      return nullptr;
   }
} // namespace Core
