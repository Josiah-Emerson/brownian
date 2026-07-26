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
            public: 
               using iterator_category = std::input_iterator_tag; // TODO: Not a forward iterator because I don't want to figure out how to make reference work with LegacyForwardIterator
               using value_type        = std::tuple<ViewTypes...>;
               using difference_type   = std::ptrdiff_t;
               using pointer           = std::tuple<ViewTypes*...>;
               using reference         = std::tuple<ViewTypes&...>;

               reference operator*(); // TODO: const?
               pointer operator->(); // TODO: const?

               Iterator& operator++();
               Iterator operator++(int); // post-fix

               friend bool operator==(const Iterator& lhs, const Iterator& rhs);
               friend bool operator!=(const Iterator& lhs, const Iterator& rhs);
         };

         class ConstIterator{
            public: 
               using iterator_category = std::input_iterator_tag; // TODO: Not a forward iterator because I don't want to figure out how to make reference work with LegacyForwardIterator
               using value_type        = std::tuple<const ViewTypes...>;
               using difference_type   = std::ptrdiff_t;
               using pointer           = std::tuple<const ViewTypes*...>;
               using reference         = std::tuple<const ViewTypes&...>;

               reference operator*(); // TODO: const?
               pointer operator->(); // TODO: const?

               Iterator& operator++();
               Iterator operator++(int); // post-fix

               friend bool operator==(const Iterator& lhs, const Iterator& rhs);
               friend bool operator!=(const Iterator& lhs, const Iterator& rhs);
         };

         using iterator = Iterator;
         using const_iterator = ConstIterator;

         iterator begin();
         const_iterator begin() const;

         iterator end();
         const_iterator end() const;

         template<typename U, typename C, typename... RegTypes>
         requires((Concepts::is_in_pack<ViewTypes, RegTypes...> && ...))
         RegistryView(SortedRegistry<U, C, ComponentPack<RegTypes...>>&);

      private: 
   };

   template<typename... ViewTypes>
   template<typename U, typename C, typename... RegTypes>
   requires((Concepts::is_in_pack<ViewTypes, RegTypes...> && ...))
   RegistryView<ComponentPack<ViewTypes...>>::RegistryView(SortedRegistry<U, C, ComponentPack<RegTypes...>>&){
   }
} // namespace Core
