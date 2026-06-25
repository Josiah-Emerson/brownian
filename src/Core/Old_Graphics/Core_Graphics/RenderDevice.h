#pragma once
#include "GraphicsComponents.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Model.h"
#include "Core_Utils/Types.h"

// TODO: I think there is a TODO somewhere in here or .cpp file, but not sure if we should be 
// keeping the models as shared_ptr or by value? the Models can be very large so perhaps by ptr?
// really not sure, but we do both here which def is not the right approach

namespace Core{
   struct GraphicsComperand{
      std::shared_ptr<ShaderProgram> prog;
      std::shared_ptr<Model> model;

      bool operator==(const GraphicsComperand& other) const;
   };

   struct GraphicsCompare{
      bool operator()(const GraphicsComperand& lhs, const GraphicsComperand& rhs) const;
   };

   using GraphicsRegistry = typename CreateGraphicsRegistry<GraphicsComperand,
                                                            GraphicsCompare,
                                                            GraphicsComponentList>::type;
   class RenderDevice{
      public: 
         RenderDevice() = delete; // TBD: what goes in here
         RenderDevice(const GraphicsRegistry& registry);
         virtual ~RenderDevice() = default;

         virtual bool registerModel(Model& model) = 0;

         // TODO: I think with new design we remove this and just have users register in their controlling registry
         virtual bool registerEntity(EntityID id, Model& model, std::shared_ptr<ShaderProgram> shaderProgram) = 0;

         virtual std::shared_ptr<Shader> createShader(const char* filepath, ShaderType type) = 0;
         virtual std::shared_ptr<ShaderProgram> createShaderProgram() = 0; // TBD: Args

         // checks if GraphicsComperand is valid (generally if its contained ShaderProgram is valid and its model is registered here)
         virtual bool isValidGraphicsComperand(const GraphicsComperand& cmp) const = 0;
                                                                           
         virtual void drawRegisteredEntities() = 0; // TBD: Args
                              
         static std::unique_ptr<RenderDevice> createRenderDevice(const GraphicsRegistry& registry);

         // TODO: Is this best place for it?
         typedef void* COLOR_PTR;
         static std::size_t color3ToGraphicsColorType(const Color3& color, COLOR_PTR* ptr);
         static void freeColorPtr(COLOR_PTR ptr);
         // static std::shared_ptr<RenderDevice> createRenderDevice(const GraphicsRegistry& registry); // TBD: Any args

      protected: 
         const GraphicsRegistry& m_registry;
   };
   
} // namespace Core
