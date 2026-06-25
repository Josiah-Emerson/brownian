#include "NewGraphicsTestLayer.h"
#include "Core_Graphics/AssetManager.h"
#include "Core_Utils/Log.h"

NewGraphicsTestLayer::NewGraphicsTestLayer(Core::AssetManager& assetManager)
   : m_assetManager(assetManager)
{
   FIG_LOG_MESSAGE("Creating mesh")
   Core::MeshHandle mesh = assetManager.createMesh("path/to/mesh");
   m_meshHandles.push_back(mesh);
}

NewGraphicsTestLayer::~NewGraphicsTestLayer(){
   FIG_LOG_MESSAGE("In Destructor")
}

bool NewGraphicsTestLayer::onEvent(Core::Events::Event& event){
   if(event.type == Core::Events::Type::KEY_PRESS){
      m_assetManager.getMesh(m_meshHandles[0]).outputLog();
      return true;
   }

   return false;
}
