#include "GuiTestLayer.h"
#include "Core_Graphics/GraphicsComponents.h"
#include "Core_Utils/Linear/MatrixTransform.h"

GuiTestLayer::GuiTestLayer()
   : m_gui { }
   , m_camera {{0, 0, 10}, 45.f, 
            static_cast<float>(m_window->getWidth()),
            static_cast<float>(m_window->getHeight()),
            {0,0,0}}
   , m_graphicsRegistry { 1 }
   , m_renderDevice { Core::RenderDevice::createRenderDevice(m_graphicsRegistry) }
{
   m_graphicsRegistry.setComperandValidatorFunction(
         [&rd = this->m_renderDevice](const Core::GraphicsComperand& cmp){
            return rd->isValidGraphicsComperand(cmp);
         });

   std::shared_ptr<Core::Model> rainbowCube = std::make_shared<Core::Model>(g_posData, g_posData);
   m_renderDevice->registerModel(*rainbowCube);

   std::shared_ptr<Core::Shader> fragmentShader = createShader("Resources/Shaders/Fragment.fs", Core::ShaderType::FRAGMENT_SHADER);
   std::shared_ptr<Core::Shader> vertexShader = createShader("Resources/Shaders/Vertex.vs", Core::ShaderType::VERTEX_SHADER);
   std::shared_ptr<Core::ShaderProgram> program = createShaderProgram(vertexShader, fragmentShader);

   std::shared_ptr<Core::Shader> unicolorVertexShader = createShader("Resources/Shaders/UnicolorVertex.vs", Core::ShaderType::VERTEX_SHADER);
   std::shared_ptr<Core::ShaderProgram> unicolorProgram = createShaderProgram(unicolorVertexShader, fragmentShader);

   using Arg = typename Core::CreateComponentFunction<Core::GraphicsComponentList>::ArgType;
   auto mvp = [&cam = this->m_camera](void** data, Arg arg, std::size_t offset){
      using namespace Core;
      const PositionComponent* position = std::get<const PositionComponent*>(arg);
      const ScaleComponent* scale = std::get<const ScaleComponent*>(arg);
      const DirectionComponent* direction = std::get<const DirectionComponent*>(arg);
      FIG_ASSERT(position && scale && direction, "One of the components was not found")

      auto V = cam.viewMatrix();
      auto P = cam.projectionMatrix();
      auto M = Linear::modelMatrix(position[offset].val, direction[offset].val, scale[offset].val);
      auto MVP = P * V * M;

      memcpy(*data, &MVP, sizeof(MVP));
   };
   auto color = [](void** data, Arg arg, std::size_t offset){
      using namespace Core;
      const ColorComponent* color = std::get<const ColorComponent*>(arg);
      FIG_ASSERT(color, "Color Component pool not found")

      RenderDevice::COLOR_PTR colPtr;
      std::size_t size = RenderDevice::color3ToGraphicsColorType(color->val, &colPtr);
      memcpy(*data, colPtr, size);
      RenderDevice::freeColorPtr(colPtr);
   };

   program->setUniformCallback("MVP", mvp);

   unicolorProgram->setUniformCallback("MVP", mvp);
   unicolorProgram->setUniformCallback("color", color);

   m_selectedID = m_graphicsRegistry.registerNewEntity(
         Core::GraphicsComperand{unicolorProgram, rainbowCube},
         Core::PositionComponent{{0,0,0}},
         Core::ScaleComponent{{1,1,1}},
         Core::DirectionComponent{{0, 0, 0}},
         Core::ColorComponent{{128, 128, 128}});
}

GuiTestLayer::~GuiTestLayer() = default;

void GuiTestLayer::onUpdate(float dt){
}

void GuiTestLayer::onRender(){
   renderUI();
   m_renderDevice->drawRegisteredEntities();
}

void GuiTestLayer::renderUI(){
   static bool open = true;
   m_gui.beginFrame();
   if(open){
      const int WIDTH { 200 };
      const int HEIGHT { 400 };
      const int WINDOW_WIDTH { m_window->getWidth() };
      Core::Gui::Window window { &m_gui, "Cube Settings", open, {200, 400}, 
                                 {WINDOW_WIDTH - WIDTH, 0}};

      if(auto colorGroup = window.group("Color")){
         Core::ColorComponent& color = 
            m_graphicsRegistry.getPool<Core::ColorComponent>().id(m_selectedID);
         colorGroup.rgbSelector("Cube Color", color);
      }

      if(auto positionGroup = window.group("Position")){
         Core::PositionComponent& position = 
            m_graphicsRegistry.getPool<Core::PositionComponent>().id(m_selectedID);
         Core::Linear::fvec3 posVec = position.val;
         if(positionGroup.slider({"X: ", "Y: ", "Z: "}, posVec)){
            position.val = posVec;
         }
      }
   }
   m_gui.render();
}

std::shared_ptr<Core::Shader> GuiTestLayer::createShader(const char* path, Core::ShaderType type){
   std::shared_ptr<Core::Shader> shader = m_renderDevice->createShader(path, type);
   shader->compile();
   return shader;
}

std::shared_ptr<Core::ShaderProgram> GuiTestLayer::createShaderProgram(std::shared_ptr<Core::Shader> vertex, std::shared_ptr<Core::Shader> fragment){
   std::shared_ptr<Core::ShaderProgram> program = m_renderDevice->createShaderProgram();
   program->initializeProgram();
   program->addShader(vertex);
   program->addShader(fragment);
   program->link();

   return program;
}
