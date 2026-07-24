#pragma once
#include "Core_ECS/Components.h"
#include "Core_ECS/SortedRegistry.h"
#include "Core_AssetManager/AssetManager.h"
#include "Core_Graphics/Camera.h"
#include "Core_Graphics/RenderDevice.h"
#include "Core_Graphics/Renderer.h"
#include "Layer.h"

class NewGraphicsTestLayer : public Core::Layer{
   public: 
      NewGraphicsTestLayer(Core::RenderDevice* device, Core::AssetManager& assetManager);
      ~NewGraphicsTestLayer();

      void onUpdate(float dt) override;
      bool onEvent(Core::Events::Event& event) override;

   private: 
      using Components = Core::ComponentPack<Core::PositionComponent,
                                           Core::MeshComponent,
                                           Core::MaterialComponent>;
      using Registry = Core::SortedRegistry<Core::tempComperand, Core::tempCompare, Components>;

      Core::AssetManager& m_assetManager;
      Core::Renderer m_renderer;
      Core::Camera m_camera;
      Core::MeshHandle m_mesh;
      Core::MaterialHandle m_material;
      Registry m_registry;
};
