#include "NewGraphicsTestLayer.h"
#include "Core_ECS/Components.h"

NewGraphicsTestLayer::NewGraphicsTestLayer(Core::RenderDevice* device, Core::AssetManager& assetManager)
   : m_assetManager(assetManager)
   , m_renderer(device, m_assetManager)
   , m_camera {{0, 0, 10}, 45.f, 
            static_cast<float>(m_window->getWidth()),
            static_cast<float>(m_window->getHeight()),
            {0,0,0}}
{
   m_mesh = m_assetManager.createMesh("Path/To/Mesh");
   m_material = m_assetManager.createMaterial();
   m_registry.registerNewEntity( Core::tempComperand{ }, Core::PositionComponent{{0, 0, 0}}, 
         Core::MeshComponent{m_mesh}, Core::MaterialComponent{m_material});
}

NewGraphicsTestLayer::~NewGraphicsTestLayer(){
}

void NewGraphicsTestLayer::onUpdate(float dt){
   Core::RendererSortedRegistryView view (m_registry);
   m_renderer.RenderEntities(&view, m_camera);
}

bool NewGraphicsTestLayer::onEvent(Core::Events::Event& event){
   if(event.type == Core::Events::Type::KEY_PRESS){
      m_camera.pos()[0] += 0.1f;
      m_camera.pos()[1] += 0.1f;
      m_camera.pos()[2] += 0.1f;
   }
   return false;
}
