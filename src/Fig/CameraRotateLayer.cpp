#include "CameraRotateLayer.h"
#include "Application.h"
#include "Core_Graphics/GraphicsComponents.h"
#include "Core_Graphics/RenderDevice.h"
#include "Core_Utils/Linear/MatrixTransform.h"
#include "Core_Utils/Linear/Vector.h"
#include "Events.h"
#include <cstring>

CameraRotateLayer::CameraRotateLayer()
   : m_camera { {0, 0, 10}, 45.f, 
               static_cast<float>(Core::Application::get().getWindow()->getHeight()),
               static_cast<float>(Core::Application::get().getWindow()->getWidth()),
               {0, 0, -1}, Core::Camera::LookMode::LOCKED }
   , m_graphicsRegistry { }
   , m_renderDevice { Core::RenderDevice::createRenderDevice(m_graphicsRegistry) }
   , m_graphicsComperands { }
{
   m_graphicsRegistry.setComperandValidatorFunction(
         [&rd = this->m_renderDevice](const Core::GraphicsComperand& cmp){
            return rd->isValidGraphicsComperand(cmp);
         });
   
   // set up graphics stuff 
   std::shared_ptr<Core::Model> regularModel = std::make_shared<Core::Model>(g_posData, g_colorData);
   std::shared_ptr<Core::Model> rainbowModel = std::make_shared<Core::Model>(g_posData, g_posData);
   m_renderDevice->registerModel(*regularModel);
   m_renderDevice->registerModel(*rainbowModel);

   std::shared_ptr<Core::Shader> fragmentShader = createShader("Resources/Shaders/Fragment.fs", Core::ShaderType::FRAGMENT_SHADER);
   std::shared_ptr<Core::Shader> unicolorVertex = createShader("Resources/Shaders/UnicolorVertex.vs", Core::ShaderType::VERTEX_SHADER);
   std::shared_ptr<Core::Shader> regularVertex = createShader("Resources/Shaders/Vertex.vs", Core::ShaderType::VERTEX_SHADER);

   std::shared_ptr<Core::ShaderProgram> unicolorProgram = createShaderProgram(unicolorVertex, fragmentShader);
   std::shared_ptr<Core::ShaderProgram> regularProgram = createShaderProgram(regularVertex, fragmentShader);

   using Arg = typename Core::CreateComponentFunction<Core::GraphicsComponentList>::ArgType;
   auto mvp = [&cam = this->m_camera](void** data, Arg arg, std::size_t offset){
      using namespace Core;

      const PositionComponent* position = std::get<const PositionComponent*>(arg);
      const ScaleComponent* scale = std::get<const ScaleComponent*>(arg);
      const DirectionComponent* direction = std::get<const DirectionComponent*>(arg);
      FIG_ASSERT(direction && position && scale , "Direction, Position or Scale nullptr in MVP function callback")

      // TODO: This gets calculated for each call of this function, so we should probably 
      // change how we pass this value here 
      auto V = cam.viewMatrix();
      auto P = cam.projectionMatrix();
      auto M = Linear::modelMatrix(position[offset].val, direction[offset].val, scale[offset].val);
      auto MVP = P * V * M;

      memcpy(*data, &MVP, sizeof(MVP));
   };

   auto color = [](void** data, Arg arg, std::size_t offset){
      using namespace Core;

      const ColorComponent* color = std::get<const ColorComponent*>(arg);
      // TODO: This works if we assume that every graphics device is RGB on a scale of 0 - 1.f
      // Need to figure out a way to make it so that it literally just points to our agnostic Color 
      // type and then the implementation figures out how to manipulate that value
      float tmp[3] = 
      {
         static_cast<float>(color->val.R) / 255.f,
         static_cast<float>(color->val.G) / 255.f,
         static_cast<float>(color->val.B) / 255.f,
      };

      memcpy(*data, tmp, sizeof(tmp));
   };

   regularProgram->setUniformCallback("MVP", mvp);
   unicolorProgram->setUniformCallback("MVP", mvp);
   unicolorProgram->setUniformCallback("color", color);

   m_graphicsComperands[0] = { unicolorProgram, regularModel };
   m_graphicsComperands[1] = { regularProgram, rainbowModel };
   m_graphicsComperands[2] = { regularProgram, regularModel };

   Core::EntityID id = m_graphicsRegistry.registerNewEntity(m_graphicsComperands[1], 
                                        Core::PositionComponent{{0, 0, 0}}, 
                                        Core::ScaleComponent{{1, 1, 1}}, 
                                        Core::DirectionComponent{{0, 0, 0}},
                                        Core::ColorComponent{{128, 128, 255}});

}

bool CameraRotateLayer::onEvent(Core::Events::Event& event) {
   using namespace Core::Events;
   if(event.type == Core::Events::Type::KEY_PRESS && 
         event.keyEvent.key == Core::Events::Key::TAB){
      m_camera.updateLookMode(((m_camera.lookMode() == Core::Camera::LOCKED) ?
            Core::Camera::DIRECTIONAL : Core::Camera::LOCKED));
   }
   return false;
}

void CameraRotateLayer::onUpdate(float dt){
   moveCamera(dt);
}

void CameraRotateLayer::onRender(){
   m_renderDevice->drawRegisteredEntities();
}

/*
 *
 * PRIVATE METHOD IMPL 
 *
 */

void CameraRotateLayer::moveCamera(float dt){
   constexpr float movementSpeed = 1e-8;
   constexpr float lookSpeed = 2.5e-3;
   // TODO: For some reason if you do not move mouse when switching to DIRECTIONAL mode, 
   // the cube is easy to lose
   const Core::Window::PointerPosition center = {
      m_window->getWidth() / 2.f, m_window->getHeight() / 2.f };

   if(m_camera.lookMode() == Core::Camera::LookMode::DIRECTIONAL){
      static bool done { false };

      Core::Window::PointerPosition current = m_window->pointerPosition();
      float xOffset = (current.x - center.x) * lookSpeed;
      float yOffset = (center.y - current.y) * lookSpeed;

      if(xOffset != 0 || yOffset != 0){
         m_camera.updateLookWithOffset({0, -yOffset, -xOffset});
         m_window->setPointerPosition(center);
      }
   }

   // Movement
   // based on i, j, and k vectors of the camera: 
   // W      : -k 
   // A      : -i
   // S      : +k 
   // D      : +i 
   // SHIFT_L: -j 
   // SPACE  : +j
   // Then translate into world space 
   Core::Linear::fvec4 cameraCoords { 0 };
   // cameraCoords[3] = 1;

   if(m_window->isKeyDown(Core::Events::Key::W)){
      cameraCoords[2] -= movementSpeed * dt;
   }
   if(m_window->isKeyDown(Core::Events::Key::A)){
      cameraCoords[0] -= movementSpeed * dt;
   }
   if(m_window->isKeyDown(Core::Events::Key::S)){
      cameraCoords[2] += movementSpeed * dt;
   }
   if(m_window->isKeyDown(Core::Events::Key::D)){
      cameraCoords[0] += movementSpeed * dt;
   }
   if(m_window->isKeyDown(Core::Events::Key::SHIFT_L)){
      cameraCoords[1] -= movementSpeed * dt;
   }
   if(m_window->isKeyDown(Core::Events::Key::SPACE)){
      cameraCoords[1] += movementSpeed * dt;
   }

   Core::Linear::fvec3 angles { 0 };

   // translate the cameraCoords vector into world space and add to the camera's position 
   // auto mat = Core::Linear::modelMatrix(m_camera.pos(),  ,Core::Linear::fvec3{1, 1, 1});
   auto mat = m_camera.viewMatrix();
   auto worldCoords = mat.transpose() * cameraCoords;
   m_camera.pos() = m_camera.pos() + Core::Linear::fvec3{worldCoords[0], worldCoords[1], worldCoords[2]};
}

std::shared_ptr<Core::Shader> CameraRotateLayer::createShader(const char* path, Core::ShaderType type){
   std::shared_ptr<Core::Shader> shader = m_renderDevice->createShader(path, type);
   shader->compile();
   return shader;
}

std::shared_ptr<Core::ShaderProgram> CameraRotateLayer::createShaderProgram(std::shared_ptr<Core::Shader> vertex, std::shared_ptr<Core::Shader> fragment){
   std::shared_ptr<Core::ShaderProgram> program = m_renderDevice->createShaderProgram();
   program->initializeProgram();
   program->addShader(vertex);
   program->addShader(fragment);
   program->link();

   return program;
}
