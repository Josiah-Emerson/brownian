#pragma once
#include "SortedRegistry.h"
#include "Components.h"
#include <iterator>

// A way to obtain a view into a SortedRegistry

namespace Core{
   template<typename... VT>
   class RegistryView{
      public:
         struct Iterator{
            using iterator_category = std::input_iterator_tag;
            using value_type        = std::tuple<VT...>;
            using difference_type   = std::ptrdiff_t;
            using reference         = std::tuple<VT&...>;
            using pointer           = std::tuple<VT*...>;
         };
         
         template<typename U, typename C, typename... RT>
         RegistryView(SortedRegistry<U, C, ComponentPack<RT...>>& reg){

         }

      private: 
   };
} // namespace Core
