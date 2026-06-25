#pragma once
#include "Core_ECS/Components.h"
#include "Core_ECS/SortedComponentPool.h"
#include "Core_Graphics/Camera.h"
#include "Core_Utils/Linear/Matrix.h"
#include "Core_Utils/Linear/Vector.h"

// Gonna render entities based on the IRendererView, and the associated 
// Position/Color Components (or whatever other components go with 
// the uniform data struct) and the Mesh/Material Components

namespace Core{
   // Standard default uniform block which can be expected by shaders
   // TODO: Settle on a standard set of these vars that is stable to be used 
   // as an api of sorts
   struct UniformCameraData{
      Linear::fmat4 MVP;
      Linear::fvec3 color; // TODO: fvec4 ?
   };

   // TODO: ObjectUniformData is more of our camera data 
   // Need to define also a set of vertex stuff like vertex position, UV texcoords and such

   // TODO: Update to Color4 type when implemented

   struct tempComperand{
      bool operator==(const tempComperand& other) const;
   };

   struct tempCompare{
      bool operator()(const tempComperand& lhs, const tempComperand& rhs) const;
   };

   template<typename Component>
   using Pool = SortedComponentPool<Component, tempCompare, tempComperand>;

   class IRendererSortedRegistryView{
      public:
         ~IRendererSortedRegistryView() = default;
         virtual Pool<PositionComponent>& positionPool() = 0;
         virtual Pool<MeshComponent>& meshPool() = 0;
         virtual Pool<MaterialComponent>& materialPool() = 0;
   };

   template<typename T>
   concept RendererRegistryViewer = 
      requires(T reg){
         {reg.template getPool<PositionComponent>() };
         {reg.template getPool<MeshComponent>() };
         {reg.template getPool<MaterialComponent>() };
      };

   template<RendererRegistryViewer Reg>
   class RendererSortedRegistryView : public IRendererSortedRegistryView{
      public:
         RendererSortedRegistryView(Reg& registry) : m_registry(registry) {}

         Pool<PositionComponent>& positionPool() override {return m_registry.template getPool<PositionComponent>();}
         Pool<MeshComponent>& meshPool() override {return m_registry.template getPool<MeshComponent>();}
         Pool<MaterialComponent>& materialPool() override {return m_registry.template getPool<MaterialComponent>();}
         
      private: 
         Reg& m_registry;
   };

   class Renderer{
      public:
         Renderer(RenderDevice* device, AssetManager& assetManager);

         void RenderEntities(IRendererSortedRegistryView* view, Camera& camera);
      private: 
         RenderDevice* m_device;
         AssetManager& m_assetManager;
         struct UniformBuffers{
            BufferHandle cameraData;
         } m_uniformBuffers;
   };
   // Three types of shader vars: 
   // Default as above
   // per-vertex variables (defined by mesh)
   // non-default versions of these (mainly probbaly uniform) defined by Material, which are not gonna be implemented now
} // namespace Core
