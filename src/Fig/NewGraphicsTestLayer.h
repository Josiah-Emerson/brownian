#pragma once
#include "Core_UI/Gui.h"
#include "Layer.h"
#include "Core_ECS/Components.h"
#include "Core_ECS/SortedRegistry.h"
#include "Core_Graphics/Renderer.h"
#include "Core_Graphics/Camera.h"

namespace Core{
   struct RenderDevice;
   struct AssetManager;
}

class NewGraphicsTestLayer : public Core::Layer{
   public:
      NewGraphicsTestLayer(Core::RenderDevice* device, Core::AssetManager& assetManager);
      ~NewGraphicsTestLayer();

      void onUpdate(float dt) override;
      void onRender() override;
      bool onEvent(Core::Events::Event& event) override;

      void renderUI();


   private: 

      using Components = Core::ComponentPack<Core::PositionComponent,
                                           Core::MeshComponent,
                                           Core::MaterialComponent>;
      using Registry = Core::SortedRegistry<Core::tempComperand, Core::tempCompare, Components>;

      Core::Gui m_gui;
      Core::AssetManager& m_assetManager;
      Core::Renderer m_renderer;
      Core::Camera m_camera;
      Core::MeshHandle m_mesh1;
      Core::MeshHandle m_mesh2;
      Core::MaterialHandle m_material;
      Registry m_registry;
};
