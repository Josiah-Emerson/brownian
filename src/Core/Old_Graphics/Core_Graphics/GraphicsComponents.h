#pragma once
#include "Core_Utils/Linear/Vector.h"
#include "Core_ECS/SortedRegistry.h"
#include "Core_Utils/Types.h"
#include <functional>

namespace Core{
   // GraphicsComperand and GraphicsCompare represent a way to sort a SortedRegistry and SortedComponentPool 
   // using relevant things like associated ShaderPrograms and Models to lessen switching in graphics layer (most likely contexts inside GPU)
   // For now it is sorted based on the ShaderProgram first and Model second, but can be changed later
   // This is what works with OpenGL (i think) best, and possibly can be changed if it is different across platforms


   // Define components here and add below 
   struct PositionTag {};
   struct DirectionTag {};
   struct ScaleTag {};
   struct ColorTag {};

   template<typename T, typename Tag>
   struct Component{
      using Type = T;
      T val;

      // references 
      operator T&() { return val; }
      operator const T&() const { return val; }

      // pointers 
      operator T*() { return &val; }
      operator const T*() const { return &val; }
   };

   using PositionComponent = Component<Linear::fvec3, PositionTag>;
   using DirectionComponent = Component<Linear::fvec3, DirectionTag>;
   using ScaleComponent = Component<Linear::fvec3, ScaleTag>;
   using ColorComponent = Component<Color3, ColorTag>;

   template<typename... Components>
   struct ComponentPack {};

   using GraphicsComponentList = ComponentPack<PositionComponent,
                                               DirectionComponent,
                                               ScaleComponent,
                                               ColorComponent>; // Add components to list here

   template<typename U, typename Compare, typename Pack>
   struct CreateGraphicsRegistry;

   template<typename U, typename Compare, typename... Components>
   struct CreateGraphicsRegistry<U, Compare, ComponentPack<Components...>>{
      using type = SortedRegistry<U, Compare, Components...>;
   };

   // function signature:
   template<typename Pack>
   struct CreateComponentFunction;

   template<typename... Components> 
   struct CreateComponentFunction<ComponentPack<Components...>>{
      using ArgType = std::tuple<const Components*...>;
      using type = std::function<void(void**, ArgType, std::size_t offset)>;
      // NOTE: Because of the update to void**, ensure you either: 
      // A) Copy: memcpy(*data, src, size) not memcpy(data, src, size) where data is void**
      // B) Re-point: *data = (void*)ComponentPtr where data is void** and ComponentPtr is a ptr to the correct component
      // See MVP and Color uses in some Layers
   };

   // End of gross type stuff

} // namespace Core
