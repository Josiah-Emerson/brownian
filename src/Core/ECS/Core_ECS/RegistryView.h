#pragma once
#include "Core_Utils/Log.h"
#include "SortedRegistry.h"
#include "Components.h"
#include <functional>
#include <iterator>
#include <type_traits>

// A way to obtain a view into a SortedRegistry
namespace Core{
   template<typename... VT>
   struct RegistryViewReferenceProxy{
      std::tuple<VT&...> refs;

      template<typename... U>
      requires(sizeof...(U) == sizeof...(VT) && 
            (std::convertible_to<VT&, U> && ...))
      operator std::tuple<U...>() const;
   };

   template<std::size_t I, typename... VT>
   typename std::tuple_element<I, RegistryViewReferenceProxy<VT...>>::type& 
      get(RegistryViewReferenceProxy<VT...>& proxy){
         return std::get<I>(proxy.refs);
      }

   template<std::size_t I, typename... VT>
   typename std::tuple_element<I, RegistryViewReferenceProxy<VT...>>::type&& 
      get(RegistryViewReferenceProxy<VT...>&& proxy){
         return std::get<I>(std::move(proxy.refs));
      }

   template<std::size_t I, typename... VT>
   const typename std::tuple_element<I, RegistryViewReferenceProxy<VT...>>::type& 
      get(const RegistryViewReferenceProxy<VT...>& proxy){
         return std::get<I>(proxy.refs);
      }

   template<std::size_t I, typename... VT>
   const typename std::tuple_element<I, RegistryViewReferenceProxy<VT...>>::type&& 
      get(const RegistryViewReferenceProxy<VT...>&& proxy){
         return std::get<I>(std::move(proxy.refs));
      }
} // namespace Core

namespace std{
   template<typename... VT>
   struct tuple_size<Core::RegistryViewReferenceProxy<VT...>>{
      static constexpr size_t value = sizeof...(VT);
   };

   // recursively calls/inherits until it hits base case
   template<std::size_t I, typename Head, typename... Tail>
   struct tuple_element<I, Core::RegistryViewReferenceProxy<Head, Tail...>>
      : public tuple_element<I - 1, Core::RegistryViewReferenceProxy<Tail...>> { };

   // base case
   template<typename Head, typename... Tail>
   struct tuple_element<0, Core::RegistryViewReferenceProxy<Head, Tail...>>{
      using type = Head;
   };

   template<template<class> class TQual, template<class> class UQual, typename... VT>
   struct basic_common_reference<
      Core::RegistryViewReferenceProxy<VT...>,
      std::tuple<VT...>,
      TQual, UQual>{
         using type = std::tuple<
            std::common_reference_t<TQual<VT>, UQual<VT>>...>;
      };

   template<template<class> class TQual, template<class> class UQual, typename... VT>
   struct basic_common_reference<
      std::tuple<VT...>,
      Core::RegistryViewReferenceProxy<VT...>,
      TQual, UQual>{
         using type = std::tuple<
            std::common_reference_t<TQual<VT>, UQual<VT>>...>;
      };

} // namespace std

namespace Core{
   template<typename... VT>
   class RegistryView{
      public:
         using reference = RegistryViewReferenceProxy<VT...>;
         // void* is a pointer to a registry which the callback needs to cast 
         // back to the correct type 
         // value_type is the tuple of components which correspond to the provided EntityID
         // TODO: If this is a bottleneck, drop the std::function overhead and just use a 
         // function pointer
         using Callback   = std::function<reference(EntityID)>;

         struct Iterator{
            using difference_type   = std::ptrdiff_t;
            using value_type        = std::tuple<VT...>;
            using reference         = RegistryViewReferenceProxy<VT...>;

            Iterator() = default;
            Iterator(Callback& fn, std::vector<std::size_t>& ids, std::size_t idx)
               : m_fn { &fn }, m_ids { &ids }, m_currIdx { idx } {}

            reference operator*() const { return (*m_fn)((*m_ids)[m_currIdx]); }

            Iterator& operator++() { ++m_currIdx; return *this; }
            Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

            friend bool operator==(const Iterator& lhs, const Iterator& rhs){
               // TODO/NOTE: This just checks if the addresses are the same and if m_currIdx is the same 
               // Idk if this would be semantically 100% right
               return (lhs.m_fn == rhs.m_fn && lhs.m_ids == rhs.m_ids && lhs.m_currIdx == rhs.m_currIdx);
            }
            friend bool operator!=(const Iterator& lhs, const Iterator& rhs){
               return ! (lhs == rhs);
            }

         private: 
            Callback* m_fn;
            std::vector<std::size_t>* m_ids;
            std::size_t m_currIdx = 0;
         };

         struct ConstIterator{
            using difference_type = std::ptrdiff_t;
            using value_type      = std::tuple<VT...>;
            using reference       = RegistryViewReferenceProxy<const VT...>;

            ConstIterator() = default;
            ConstIterator(Callback& fn, std::vector<std::size_t>& ids, std::size_t idx)
               : m_fn { &fn }, m_ids { &ids }, m_currIdx { idx } { }

            reference operator*() const { return (*m_fn)((*m_ids)[m_currIdx]); }

            ConstIterator& operator++() { ++m_currIdx; return *this; }
            ConstIterator operator++(int) { ConstIterator tmp = *this; ++(*this); return tmp; }

            friend bool operator==(const ConstIterator& lhs, const ConstIterator& rhs){
               return (lhs.m_fn == rhs.m_fn && lhs.m_ids == rhs.m_ids && lhs.m_currIdx = rhs.m_currIdx);
            }
            friend bool operator!=(const ConstIterator& lhs, const ConstIterator& rhs){
               return ! (lhs == rhs);
            }

         private:
            Callback* m_fn;
            std::vector<std::size_t>* m_ids;
            std::size_t m_currIdx = 0;
         };


         static_assert(std::forward_iterator<Iterator>, "Iterator is not a forward iterator");
         static_assert(std::forward_iterator<ConstIterator>, "ConstIterator is not a forward iterator");

         using iterator = Iterator;
         using const_iterator = ConstIterator;

         iterator begin() { return iterator(m_callback, m_validIDs, 0); }
         const_iterator begin() const { return const_iterator(m_callback, m_validIDs, 0); }

         iterator end() { return iterator(m_callback, m_validIDs, m_validIDs.size()); }
         const_iterator end() const { return iterator(m_callback, m_validIDs, m_validIDs.size()); }
         
         template<typename U, typename C, typename... RT>
         requires((Concepts::is_in_pack<VT, RT...> && ...))
         RegistryView(SortedRegistry<U, C, ComponentPack<RT...>>& reg);

      private: 
         template<typename U, typename C>
         std::size_t smallestPoolIndex(std::tuple<SortedComponentPool<VT, C, U>&...>& pools);

         template<typename T, typename U, typename C>
         std::size_t getPoolSize(const SortedComponentPool<T, C, U>& pool){ return pool.size(); }

         template<typename U, typename C, typename... RT>
         Callback CallbackFunctionBuilder(SortedRegistry<U, C, ComponentPack<RT...>>& reg);

         // because we can't do std::get<idx>() on a run-time idx
         // General for more than enough than is needed here
         // Should only be called with I = 0, unless you know what you want
         template<std::size_t I = 0, typename Pools, typename F>
         void visitAt(Pools&& pools, std::size_t idx, F&& f);


      private: 
         std::vector<EntityID> m_validIDs;
         Callback m_callback;
   };

   template<typename... VT>
   template<typename... U>
   requires(sizeof...(U) == sizeof...(VT) && 
         (std::convertible_to<VT&, U> && ...))
   RegistryViewReferenceProxy<VT...>::operator std::tuple<U...>() const{
      return std::apply(
            [](auto&... args){
               return std::tuple<U...>(args...);
            }, refs);
   }

   template<typename... VT>
   template<typename U, typename C, typename... RT>
   requires((Concepts::is_in_pack<VT, RT...> && ...))
   RegistryView<VT...>::RegistryView(SortedRegistry<U, C, ComponentPack<RT...>>& reg)
   : m_callback { CallbackFunctionBuilder(reg) }
   {
      // Find which pool of type VT has the smallest list of entities 
      using Pools = std::tuple<SortedComponentPool<VT, C, U>&...>;
      Pools pools { reg.template getPool<VT>()... };

      std::size_t idx = smallestPoolIndex(pools);

      std::vector<std::size_t>& IDs = m_validIDs;
      visitAt(pools, idx, 
            [&IDs, &pools](auto& smallestPool){
               for(EntityID id : smallestPool.getEntityList()){
                  bool inAllPools = std::apply(
                        [&](const auto&... pool){
                           return (pool.contains(id) && ...);
                       }, pools);
                  if(inAllPools)
                     IDs.push_back(id);
               }
            });
   }

   template<typename... VT>
   template<typename U, typename C>
   std::size_t RegistryView<VT...>::smallestPoolIndex(std::tuple<SortedComponentPool<VT, C, U>&...>& pools){
      using Pools = std::tuple<SortedComponentPool<VT, C, U>&...>;
      constexpr std::size_t N = std::tuple_size_v<Pools>;
      std::array<std::size_t, N> poolSizes;
      std::apply(
            [&poolSizes](auto&... args){
               poolSizes = { (args.size(), ...) };
            }, pools);

      std::size_t idx { 0 };
      std::size_t min = poolSizes[0];
      for(std::size_t i { 1 }; i < N; ++i){
         if(poolSizes[i] < min){
            min = poolSizes[i];
            idx = i;
         }
      }

      return idx;
   }

   template<typename... VT>
   template<typename U, typename C, typename... RT>
   RegistryView<VT...>::Callback RegistryView<VT...>::CallbackFunctionBuilder(SortedRegistry<U, C, ComponentPack<RT...>>& reg){
      return [&reg](EntityID id){
         return RegistryViewReferenceProxy<VT...> {
            std::tuple<VT&...>{
               reg.template getPool<VT>().id(id)...
            }
         };
      };
   }

   template<typename... VT>
   template<std::size_t I, typename Pools, typename F>
   void RegistryView<VT...>::visitAt(Pools&& pools, std::size_t idx, F&& f){
      constexpr std::size_t N = std::tuple_size_v<std::decay_t<Pools>>; // no specialization for a std::tuple<T...>&
      if constexpr (I < N){
         if(I == idx)
            std::forward<F>(f)(std::get<I>(std::forward<Pools>(pools)));
         else 
            visitAt<I + 1>(std::forward<Pools>(pools), idx, std::forward<F>(f));
      } else{
         // TODO: Repalce this if needed
         std::string msg = "tuple size: ";
         msg += std::to_string(N) + ", attempted index: " + std::to_string(I);
         FIG_UNREACHABLE(msg.c_str());
      }
   }

} // namespace Core

