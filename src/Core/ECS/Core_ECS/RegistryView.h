#pragma once
#include "Components.h"
#include "Core_Utils/Concepts.h"
#include "SortedRegistry.h"
#include <iterator>

// A way to obtain a view into a SortedRegistry

namespace Core{
   // TODO: Allow non ComponentPack version?
   template<typename... ViewTypes>
   class RegistryView;

   template<typename... ViewTypes>
   class RegistryView<ComponentPack<ViewTypes...>>{
      public: 
         class Iterator{
            using iterator_category = std::forward_iterator_tag;
            using value_type        = void;
            using difference_type   = void;
            using pointer           = void;
            using reference         = void;
         };

         template<typename U, typename C, typename... RegTypes>
         requires((Concepts::is_in_pack<ViewTypes, RegTypes...> && ...))
         constexpr RegistryView(SortedRegistry<U, C, RegTypes...>&);

         template<typename U, typename C, typename... RegTypes>
         requires((Concepts::is_in_pack<ViewTypes, RegTypes...> && ...))
         constexpr RegistryView<U, C, ComponentPack<RegTypes...>>(SortedRegistry<U, C, ComponentPack<RegTypes...>>&);
   };


} // namespace Core
