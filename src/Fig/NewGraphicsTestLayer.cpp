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
   m_mesh1 = m_assetManager.loadMesh("Resources/Meshes/teddy.obj");
   m_mesh2 = m_assetManager.loadMesh("Resources/Meshes/cat/cat.obj");
   m_material = m_assetManager.loadMaterial("");
   m_registry.registerNewEntity( Core::tempComperand{}, Core::PositionComponent{{0, 0, 0}},
         Core::MeshComponent{m_mesh1}, Core::MaterialComponent{m_material});
}

NewGraphicsTestLayer::~NewGraphicsTestLayer(){
}

void NewGraphicsTestLayer::onUpdate(float dt){
   Core::RendererSortedRegistryView view (m_registry);
   m_renderer.renderEntities(&view, m_camera);
}

bool NewGraphicsTestLayer::onEvent(Core::Events::Event& event){
   if(event.type == Core::Events::Type::KEY_PRESS){
      if(event.keyEvent.key == Core::Events::Key::SPACE){
         const auto& pool = m_registry.getPool<Core::MeshComponent>();
         if(pool.id(0) == Core::MeshComponent{m_mesh1}){
            m_registry.updateComponent(0, Core::MeshComponent{m_mesh2});
         }else{
            m_registry.updateComponent(0, Core::MeshComponent{m_mesh1});
         }

         return true;
      }

      m_camera.pos()[0] -= 0.5f;
      m_camera.pos()[1] -= 0.5f;
      m_camera.pos()[2] += 1;

      return true;
   }
   return false;
}
