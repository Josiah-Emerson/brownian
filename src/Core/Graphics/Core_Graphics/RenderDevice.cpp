#include "RenderDevice.h"
#include <cstring>
// TODO: Includes and extra supported graphics

// #if defined(OPENGL)
#ifdef OPENGL
#include "gl/GLRenderDevice.h"
#endif

namespace Core{
   bool GraphicsComperand::operator==(const GraphicsComperand& other) const{
      return (prog == other.prog) && (model == other.model);
   }

   bool GraphicsCompare::operator()(const GraphicsComperand& lhs, const GraphicsComperand& rhs) const{
      // NOTE: because prog and model are stored as shared_ptr, these sort by the ptr value (i think) 
      // This should be fine for now because I believe that it doesn't matter in what order they are grouped, 
      // Just that they are grouped

      if(lhs.prog < rhs.prog)
         return true;
      else if(lhs.prog > rhs.prog)
         return false;

      // progs are equal sort by inner now
      return lhs.model < rhs.model;
   }

   RenderDevice::RenderDevice(const GraphicsRegistry& registry)
      : m_registry { registry }
   { }

   std::unique_ptr<RenderDevice> RenderDevice::createRenderDevice(const GraphicsRegistry& registry){
      std::unique_ptr<RenderDevice> ret = nullptr;

      // TODO: Other graphics when ready
// #if defined(OPENGL)
#ifdef OPENGL
      ret = std::make_unique<GLRenderDevice>(registry);
#endif

      return std::move(ret);
   }

   std::size_t RenderDevice::color3ToGraphicsColorType(const Color3& color, COLOR_PTR* ptr){
      FIG_ASSERT(ptr , "Do not pass nullptr as ptr")
      std::size_t size { 0 };
#ifdef OPENGL
      float* tmp = new float[3];
      tmp[0] = color.R / 255.f;
      tmp[1] = color.G / 255.f;
      tmp[2] = color.B / 255.f;
      *ptr = tmp;
      size = sizeof(float) * 3;
#endif

      FIG_ASSERT(ptr , "Color type for graphics not yet implemented in RenderDevice::color3ToGraphicsColorType")
      return size;
   }

   void RenderDevice::freeColorPtr(COLOR_PTR ptr){
#ifdef OPENGL
      delete[] (float*) ptr;
#endif
      
      ptr = nullptr;
   }
} // namespace Core
