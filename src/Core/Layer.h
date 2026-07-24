#pragma once
#include "Events.h"
#include "Core_Window/Window.h"
#include <concepts>
#include <memory>

namespace Core{

   class Layer{
      public: 
         Layer();
         virtual ~Layer() = default;
         // NOTE: returns true if the event is properly handled by this layer and should not propagate futher
         virtual bool onEvent(Core::Events::Event& event) { return false; }

         // dt is milliseconds of time since last frame
         virtual void onUpdate(float dt) {}
         virtual void onRender() {}

         template<std::derived_from<Layer> T, typename... Args>
         void transitionTo(Args&&... args){
            queueTransition(std::make_unique<T>(std::forward<Args>(args)...)); // TODO: Cherno wraps the argument in std::move 
                                                                               // Would this make sense sometime in the future when we might want to transition to an already existing scene?
         }

      protected: 
         std::shared_ptr<Window> m_window;

      private: 
         void queueTransition(std::unique_ptr<Layer> layer);

   };

} // namespace Core
