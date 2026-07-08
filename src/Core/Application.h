#pragma once
#include <string>
#include "Layer.h"
#include "Core_Window/Window.h"
#include "Core_AssetManager/AssetManager.h"
#include "Core_Graphics/RenderDevice.h"
#include "Events.h"

namespace Core{
   struct ApplicationSpec{
      std::string title { "Fig" };
      WindowSpec windowSpec;
   };

   class Application{
      public: 
         Application(const ApplicationSpec& appSpec = ApplicationSpec() );
         ~Application();

         void run();
         void stop();
         void raiseEvent(Core::Events::Event& event);

         template <typename TLayer, typename... Args>
         requires(std::is_base_of_v<Layer, TLayer>)
         void pushLayer(Args&&... args){
            m_layerStack.push_back(
                  std::make_unique<TLayer>(
                     std::forward<Args>(args)...
                     )
                  );
         }

         /*
          * NOTE: getLayer() only used in Cherno's overlay layer to decide which layer to swap to
          * Might want this function but don't right now
          * If we do, the way it does it is kind of gross so might want to figure out something different
         template <typename TLayer>
         requires(std::is_base_of_v<Layer, TLayer>)
         TLayer* getLayer(){
            for(const auto& layer : m_layerStack){
               if(auto casted = dynamic_cast<TLayer*>(layer.get()))
                     return casted;
            }
            return nullptr;
         }
         */

         // getFrameBufferSize() const; // maybe need this
         std::shared_ptr<Window> getWindow() const { return m_window; }

         AssetManager& getAssetManager() { return *m_assetManager; }
         RenderDevice* getRenderDevice() { return m_device.get(); }

         static Application& get();
         // static float getTime(); // TODO: do this later

      private: 
         ApplicationSpec m_specification;
         std::shared_ptr<Window> m_window;
         bool m_running = false;
         std::vector<std::unique_ptr<Layer>> m_layerStack;
         std::unique_ptr<RenderDevice> m_device;
         std::unique_ptr<AssetManager> m_assetManager;
   };


} // namespace Core


