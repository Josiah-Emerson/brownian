#include "NewGraphicsTestLayer.h"
#include "Core_ECS/Components.h"
#include <cstring>
#include <variant>

NewGraphicsTestLayer::NewGraphicsTestLayer(Core::RenderDevice* device, Core::AssetManager& assetManager)
   : m_gui { }
   , m_assetManager(assetManager)
   , m_renderer(device, m_assetManager)
   , m_camera {{0, 0, 10}, 45.f, 
            static_cast<float>(m_window->getWidth()),
            static_cast<float>(m_window->getHeight()),
            {0,0,0}}
{
   m_mesh1 = m_assetManager.loadMesh("Resources/Meshes/teapot.obj");
   m_mesh2 = m_assetManager.loadMesh("Resources/Meshes/cat/cat.obj");
   m_material = m_assetManager.loadMaterial("Resources/Materials/Color.fmat");
   m_registry.registerNewEntity( Core::tempComperand{}, Core::PositionComponent{{0, 0, 0}},
         Core::MeshComponent{m_mesh1}, Core::MaterialComponent{m_material});
}

NewGraphicsTestLayer::~NewGraphicsTestLayer(){
}

void NewGraphicsTestLayer::onUpdate(float dt){

}

void NewGraphicsTestLayer::onRender(){
   Core::RendererSortedRegistryView view (m_registry);
   m_renderer.renderEntities(&view, m_camera);

   renderUI();
}

bool NewGraphicsTestLayer::onEvent(Core::Events::Event& event){
   if(event.type == Core::Events::Type::KEY_PRESS){
      m_camera.pos()[0] -= 0.5f;
      m_camera.pos()[1] -= 0.5f;
      m_camera.pos()[2] += 1;

      return true;
   }
   return false;
}

void NewGraphicsTestLayer::renderUI(){
   static bool open = true;
   m_gui.beginFrame();
   if(open){
      const int WIDTH { 200 };
      const int HEIGHT { 400 };
      const int WINDOW_WIDTH {m_window->getWidth() };
      Core::Gui::Window window { &m_gui, "Scene", open, {WIDTH, HEIGHT},
         {WINDOW_WIDTH - WIDTH, 0}};

      if(auto colorGroup = window.group("Mesh Settings")){
         if(colorGroup.button("Swap Mesh")){
            const Core::MeshComponent& msh = m_registry.getPool<Core::MeshComponent>().id(0);
            if(msh.val == m_mesh1){
               m_registry.updateComponent(0, Core::MeshComponent{m_mesh2});
            }else{
               m_registry.updateComponent(0, Core::MeshComponent{m_mesh1});
            }
         }
      }

      if(auto materialGroup = window.group("Material Settings")){
         Core::Material& mat = m_assetManager.getMaterial(m_material);
         const auto& metadata = mat.getUniformMetadata();
         for(const Core::Material::UniformMetadata& uniform : metadata){
            bool changed { false };
            Core::ShaderData var = mat.getVariable(uniform.name);
            void* ptr = std::visit(Core::ShaderVisitors::GetVoidPtr {var}, var);

            std::byte* base = static_cast<std::byte*>(ptr);
            std::size_t offset { 0 };

            for(std::size_t i { 0 }; i < uniform.widgets.size(); ++i){
               if(uniform.widgets[i]->draw(
                        materialGroup, uniform.labels[i], base + offset, i)){
                  changed = true;
               }

               offset += static_cast<std::size_t>(uniform.widgets[i]->primitiveCount()) 
                  * uniform.widgets[i]->primitiveSize();
            }

            if(changed){
               mat.setVariable(uniform.name, var);
            }
         }
      }

   }
   m_gui.render();
}

