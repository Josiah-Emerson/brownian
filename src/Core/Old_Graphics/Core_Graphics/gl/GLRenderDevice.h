#pragma once
#include "Application.h"
#include "Core_Graphics/gl/OpenGL.h"
#include "Core_Graphics/RenderDevice.h"
#include <utility>

namespace Core{
   class GLRenderDevice : public RenderDevice {
      public: 
         GLRenderDevice(const GraphicsRegistry& registry)
            : m_openGL(*Application::get().getWindow())
            , m_modelVAOList {}
            , RenderDevice(registry)
         { }

         bool registerModel(Model& model) override;
         bool registerEntity(EntityID id, Model& model, std::shared_ptr<ShaderProgram> shaderProgram) override;

         std::shared_ptr<Shader> createShader(const char* filepath, ShaderType type) override;
         std::shared_ptr<ShaderProgram> createShaderProgram() override;

         bool isValidGraphicsComperand(const GraphicsComperand& cmp) const override;
          
         void drawRegisteredEntities() override;

      private:
         // TODO: Might move these from gl specific to general base class implementation
         
         // points ptr at the first value in a component pool of m_registry of component Component, 
         // and comperand cmp. Returns the number of entries in this component pool with this value. 
         // if it does not exist, returns 0, and sets ptr to nullptr
         template<typename Component>
         std::size_t getPtrAndRangeFromComponentTypeAndComperand(const GraphicsComperand& cmp, Component** ptr);

         void* getDataPtr(ShaderDataType type);
         void deleteDataPtr(void* ptr, ShaderDataType type);
      private: 
         OpenGL m_openGL;
         // Model by value not reference for now. Could switch to std::shared_ptr if we need to
         std::vector<std::pair<const Model, GLuint>> m_modelVAOList; // List of Model VAO object pairs
   };

   /*
    *
    * TEMPLATE FUNCTION DEFINITION
    *
    */
   template<typename Component>
   std::size_t GLRenderDevice::getPtrAndRangeFromComponentTypeAndComperand(const GraphicsComperand& cmp, Component** ptr){
      auto& pool = m_registry.getPool<std::remove_const_t<Component>>();
      if(!pool.containsComperand(cmp)){
         *ptr = nullptr;
         return 0;
      }

      auto& separator = pool.separator(cmp);
      *ptr = &pool[separator.first];
      return ((separator.second - separator.first) + 1);
   }
} // namespace Core
