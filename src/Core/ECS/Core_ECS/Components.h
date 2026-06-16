#pragma once
#include "Core_Utils/Concepts.h"
#include "Core_Utils/Linear/Vector.h"
#include "Core_Utils/Types.h"

namespace Core{
   template<typename T, typename Tag>
   requires(Concepts::is_triv_copy_and_std_lay_v<T>)
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

   struct PositionTag {};
   struct DirectionTag {};
   struct ScaleTag {};
   struct ColorTag {};

   using PositionComponent = Component<Linear::fvec3, PositionTag>;
   using DirectionComponent = Component<Linear::fvec3, DirectionTag>;
   using ScaleComponent = Component<Linear::fvec3, ScaleTag>;
   using Color3Component = Component<Color3, ColorTag>;

   template<typename... Components> 
   requires(Concepts::is_instance_of_template_v<Components, Component> && ...)
   struct ComponentPack {};
} // namespace Core
