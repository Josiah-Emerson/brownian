#pragma once
#include "Core_Graphics/AssetManager.h"
#include "Layer.h"

class NewGraphicsTestLayer : public Core::Layer{
   public: 
      NewGraphicsTestLayer(Core::AssetManager& assetManager);
      ~NewGraphicsTestLayer();

      bool onEvent(Core::Events::Event& event) override;

   private: 
      Core::AssetManager& m_assetManager;
      std::vector<Core::MeshHandle> m_meshHandles;
};
