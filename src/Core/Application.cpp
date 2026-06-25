#include "Application.h"
#include <chrono>
#include <ranges>

namespace Core{

   static Application* s_application = nullptr; // Want one single instance of application

   // NOTE: appSpec has a default initialization if nothing is passed in
   Application::Application(const ApplicationSpec& appSpec)
      : m_specification { appSpec }
      , m_device { nullptr }
      , m_assetManager { nullptr }
   {
      s_application = this;
      m_specification.windowSpec.eventCallback = [this](Core::Events::Event& event) { raiseEvent(event); };
      m_window = Core::Window::createWindow(m_specification.windowSpec);
      m_device = RenderDevice::create(*m_window);
      m_assetManager = std::make_unique<AssetManager>(m_device.get());
   }

   Application::~Application(){
      // TODO: anything else?
      // TODO: Huge TODO with clean up of devices and layers and saving
      s_application = nullptr;
   }

   void Application::run(){
      m_running = true;

      auto lastTime = std::chrono::system_clock::now();
      auto currentTime = lastTime;
      while(m_running){
         m_window->pollEvents();
         m_window->prepareNewFrame();

         // TODO: if we should have a reason to close from the window 
         // if(m_window->shouldClose()){
         // stop();
         // break;
         // }

         // NOTE: update and render are separated for a couple reasons:
         // 1) if rendering is done on separate thread and we need to make sure we render after layer is updated (does this even make sense?)
         // 2) if things from one layer will depend on updates from other layers before rendering
         // TODO: if none of the first 2 are true, could profile to see how much quicker it is doing updates and render at same time
         // TODO: if 2 is true, and it is sufficiently quicker to update and render at same time,
         // could implement a way where if a layer does depend on a not yet updated layer for rendering, that layer's rendering only is delayed? but idk

         lastTime = currentTime;
         currentTime = std::chrono::system_clock::now(); // TODO: are we fine with this outside of the loop
                                                         // or do we want to update current within the loop?
         float dt = (currentTime - lastTime).count();
         for(const std::unique_ptr<Layer>& layer: m_layerStack){
            layer->onUpdate(dt);
         }

         // We should render in reverse? So that way the more 'background' layers are drawn first, and don't draw over more foreground?
         for(const std::unique_ptr<Layer>& layer : m_layerStack | std::views::reverse){
            layer->onRender();
         }
         /*
         for(const std::unique_ptr<Layer>& layer: m_layerStack){
            layer->onRender();
         }
         */

         m_window->update();
      }
   }

   void Application::stop(){
      m_running = false;
   }

   void Application::raiseEvent(Core::Events::Event& event){
      // this is where we distribute events and propogate to different layers 
      for(auto& layer : m_layerStack){
         if(layer->onEvent(event)) // true if layer handles it
            return;
      }
   }

   Application& Application::get(){
      return *s_application;
   }

} // namespace Core
