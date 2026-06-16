#pragma once
#include "Core_ECS/Components.h"
#include "Core_Utils/Concepts.h"
#include "Core_Utils/Types.h"
#include "Core_Utils/Log.h"
#include <algorithm>
#include <concepts>
#include <map>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

// TODO: First update tests to confirm that they are actually sorted by EntityIDs within U-groups 
// TODO: Second once we can assume they are properly sorted by IDs within groups, I am sure there are some 
// search optimizations which can be made using this knowledge

// TODO: update some of the update/adds to take in R-values to reduce copying
// TODO: Sorting algorithm is likely far from optimized
// NOTE: this is built with the idea of many different entities corresponding to a single 
// value U, and thus the list of separators for which we use to iterate through should remain small 
// relative to the list of components. For example we might have five different U values, but each of these 
// has a couple hundred entity components

namespace Core{
#define CLASS_TEMPLATE template<typename Component, typename Compare, typename U> \
   requires(Concepts::is_instance_of_template_v<Component, Core::Component> &&  \
            std::strict_weak_order<Compare, U, U> && \
            Concepts::is_equality_comparable<U>)
            

   // An underlying contiguous store of Entity IDs to Components
   // Sorted by function Compare on type U (which is Component if otherwise specified)
   CLASS_TEMPLATE 
   class SortedComponentPool{
      public:
         // where for one U u and corresponding Separator s, all values in range [first, second] 
         // correspond have a value equivalent to u
         using Separator = std::pair<std::size_t, std::size_t>; 

         SortedComponentPool();
         SortedComponentPool(std::size_t numEntities);

         SortedComponentPool(const SortedComponentPool& other) = default;
         SortedComponentPool(SortedComponentPool&& other) = default;
         SortedComponentPool& operator=(const SortedComponentPool& rhs) = default;
         SortedComponentPool& operator=(SortedComponentPool&& rhs) = default;
         ~SortedComponentPool() = default;

         bool insert(const EntityID id, const Component& val, const U& comperand);
         void remove(const EntityID id);
         bool contains(const EntityID id) const;
         std::size_t size() const;
         std::size_t capacity() const;
         void reserve(std::size_t newCapacity);
         bool updateComponent(const EntityID, const Component& val);
         // NOTE: returns false if entity is not in pool
         // returns true if updated or entity already has a comperand which is the same as the newComperand
         bool updateComperand(const EntityID, const U& newComperand);
         bool containsComperand(const U& comperand) const;

         // returns element at position i from 0 to size()
         // NOTE: throws if not in bounds, so if you want to be efficient but dangerous used contiguousdata()
         Component& operator[](std::size_t i);
         const Component& operator[](std::size_t i) const;

         // returns element with EntityID id
         Component& id(const EntityID id);
         const Component& id(const EntityID id) const;

         // returns a pointer to the beginning of the data stored 
         // data will be sorted in order specified by Compare(U, U)
         Component* contiguousData();
         const Component* contiguousData() const;

         const std::map<U, Separator, Compare>& separatorList() const;

         // throws exception if u is not in pool
         const Separator& separator(const U& u) const;

         EntityID idFromIndex(const std::size_t idx) const;

      private: 
         // TODO: Not sure if this combination of data members is best/most effective 
         std::vector<Component> m_data;
         std::unordered_map<EntityID, std::size_t> m_idToIndexMap;
         std::map<U, Separator, Compare> m_separatorList;

      private: 
         void internalReserve(std::size_t n);
         std::size_t indexFromID(const EntityID) const;
         // NOTE: if decltype doesn't work try the below one
         // void incrementInvalidatedSeparators(typename std::map<U, Separator, Compare>::iterator it);
         void incrementInvalidatedSeparators(decltype(m_separatorList)::iterator firstInvalidIterator);

         // TODO: a bimap would greatly increase efficiency when we need to get an index from its id 
         // NOTE: This needs to be called BEFORE insertion into m_data because it operates on properties 
         // of m_data prior to insertion
         void incrementInvalidIndices(const std::size_t firstInvalidIndex);

         void decrementInvalidatedSeparators(decltype(m_separatorList)::iterator firstInvalidIterator);
         // TODO: a bimap would greatly increase efficiency when we need to get an index from its id 
         // NOTE: This needs to be called BEFORE deletion into m_data because it operates on properties 
         // of m_data prior to deletion
         void decrementInvalidIndices(const std::size_t firstInvalidIndex);

         // TODO: Added this functionality for a reason, but nowhwere calls it and I am not sure why? 
         // Perhaps for future remove?
         U getComperandFromID(const EntityID id);
   };

   /*
    *
    * PUBLIC MEMBER FUNCTION IMPLEMENTATION
    *
    */

   CLASS_TEMPLATE
   SortedComponentPool<Component, Compare, U>::SortedComponentPool()
      : m_data { }
      , m_idToIndexMap { }
      , m_separatorList { }
   { }

   CLASS_TEMPLATE 
   SortedComponentPool<Component, Compare, U>::SortedComponentPool(std::size_t numEntities)
      : SortedComponentPool {}
   {
      internalReserve(numEntities);
   }

   CLASS_TEMPLATE
   bool SortedComponentPool<Component, Compare, U>::insert(const EntityID id, const Component& val, const U& comperand){
      if(contains(id))
         return false;

      // 0 1 2 3 4 5 6 7 8 9 
      // X X Y Y Y Z Z Z Z  
      // sep list: X = [0, 1]
      //           Y = [2, 4]
      //           Z = [5, 8]
      
      // Case 1: Used U (Example Y)
      // 0 1 2 3 4 5 6 7 8 9 
      // X X Y Y Y Z Z Z Z  
      // insert    ^ and shift back the rest 
      // 0 1 2 3 4 5 6 7 8 9 
      // X X Y Y Y Y Z Z Z Z
      // sep list: X = [0, 1]
      //           Y = [2, 5]
      //           Z = [6, 9]
      // update m_idToIndexMap for all values moved down the line

      // Case 2: New U (Example A) where A is more than Y but less than Z
      // 0 1 2 3 4 5 6 7 8 9 
      // X X Y Y Y Z Z Z Z  
      // insert A  ^ here
      // 0 1 2 3 4 5 6 7 8 9 
      // X X Y Y Y A Z Z Z Z  
      // sep list: X = [0, 1]
      //           Y = [2, 4]
      //           A = [5, 5]
      //           Z = [6, 9]
      // update m_idToIndexMap


      auto search = m_separatorList.find(comperand);
      std::size_t newElementIndex { 0 }; // Index in m_data for the new EntityID
      std::size_t firstInvalidatedIndex { 0 }; // Index in m_data (prior to insertion) that represents the first which was shifted
      auto firstInvalidSeparatorIterator = m_separatorList.begin(); // Iterator to first invalidated separator
      
      // check list for each branch: 
      // (if U not exist insert) and update separator for that U
      // set newElementIndex
      // set firstInvalidatedIndex 
      // set firstInvalidSeparatorIterator
      if(search == m_separatorList.end()){ // U does not exist
         auto [iter, success] = m_separatorList.emplace(comperand, Separator(0, 0));
         if(!success)
            return false;

         // Three special cases: iterator is equal to the beginning, iterator is the end or somewhere in the middle
         // because it is new, we know that it only encompasses one index 
         // so for the beginning: [0,0], 
         // the end: n = size of m_data after insertions: [n-1, n-1]
         // and in the middle: n = the beginning of the separator after it [n, n]
         // If U does not exist then once it is inserted, then it is automatically already 
         // sorted by ID

         if(iter == m_separatorList.begin()){
            // No need to update separator since it is alrady [0, 0]
            firstInvalidatedIndex = 0;
            newElementIndex = 0;
            firstInvalidSeparatorIterator = ++iter;
         }else if(iter == std::prev(m_separatorList.end())){
            newElementIndex = iter->second.first = iter->second.second = m_data.size(); 
            firstInvalidatedIndex = m_data.size(); // None should be invalidated
            firstInvalidSeparatorIterator = m_separatorList.end();
         }else{
            firstInvalidatedIndex = newElementIndex = iter->second.first = iter->second.second = std::next(iter)->second.first;
            firstInvalidSeparatorIterator = std::next(iter);
         }
      } else { // U exists
         // Sort by ID
         // NOTE: Start at end first because it is likely the case that larger EntityIDs are added later
         bool found { false };
         for(std::size_t idx {search->second.second}; idx > search->second.first; --idx){
            if(id > idFromIndex(idx)){
               found = true;
               newElementIndex = idx + 1;
               break;
            }
         }

         // TODO: see cleaner way to check this, possibly a while loop or something
         // last element not checked to avoid wrapping around 

         if(!found){
            newElementIndex = search->second.first;
            if(id > idFromIndex(search->second.first))
               ++newElementIndex;
         }

         firstInvalidatedIndex = newElementIndex; 
         ++(search->second.second); // increment separator we are in
         firstInvalidSeparatorIterator = std::next(search);

      }

      incrementInvalidIndices(firstInvalidatedIndex);
      incrementInvalidatedSeparators(firstInvalidSeparatorIterator);
      m_data.insert(m_data.begin() + static_cast<long int>(newElementIndex), val);
      m_idToIndexMap.insert({id, newElementIndex});

      return true;
   }

   CLASS_TEMPLATE
   void SortedComponentPool<Component, Compare, U>::remove(const EntityID id){
      // TODO: If the vector was un ordered we could just swap this element with the end and pop the back 
      // For maximum efficiency, but unfortunately because it is sorted, order matters. 
      // I am sure there is a better and more efficient way to remove, but for now oh well 
      if(!contains(id))
         return;

      std::size_t idx = indexFromID(id);
      auto searchIter = std::find_if(m_separatorList.begin(), m_separatorList.end(),
            // VT = pair<const U, Separator>
            [idx](const std::pair<const U, Separator>& v){
               return (idx >= v.second.first) && (idx <= v.second.second);
            });
      // TODO: in here to check any logic errors, but the contains !contains return early for any data members so it should be found
      FIG_ASSERT(searchIter != m_separatorList.end(),  "This pool was found to contain id however its index was not found in SeparatorList")

      // decrement indices in ID to Index map
      decrementInvalidIndices(idx + 1);
      // decrement affected Separators
      decrementInvalidatedSeparators(std::next(searchIter));

      // NOTE: the separatorList.erase() must come after decrementInvalidatedSeparators, or else searchIter is invalidated
      Separator& separator = searchIter->second;
      if(separator.second <= separator.first){ // only one element (if ==, but if less than then something def went wrong)
         // TODO: get rid of this as well but just a sanity check
         FIG_ASSERT((separator.first == separator.second), "Somehow lower index was higher than higher index for separator in SortedComponentPool")
         m_separatorList.erase(searchIter->first);
      }else{
         --separator.second;
      }

      // remove from m_data 
      m_data.erase(m_data.begin() + static_cast<long int>(idx));
      // remove from m_idToIndexMap
      m_idToIndexMap.erase(id);
   }

   CLASS_TEMPLATE
   bool SortedComponentPool<Component, Compare, U>::contains(const EntityID id) const {
      return m_idToIndexMap.contains(id);
   }

   CLASS_TEMPLATE
   std::size_t SortedComponentPool<Component, Compare, U>::size() const{
      // TODO: remove assert
      FIG_ASSERT(m_data.size() == m_idToIndexMap.size(), "data and idToIndexMap report different sizes")
      return m_data.size();
   }

   CLASS_TEMPLATE
   std::size_t SortedComponentPool<Component, Compare, U>::capacity() const{
      return m_data.capacity();
   }

   CLASS_TEMPLATE
   void SortedComponentPool<Component, Compare, U>::reserve(std::size_t newCapacity){
      if(newCapacity <= m_data.capacity())
         return;

      internalReserve(newCapacity);
   }

   CLASS_TEMPLATE
   bool SortedComponentPool<Component, Compare, U>::updateComponent(const EntityID id, const Component& val){
      if(!contains(id))
         return false;

      this->id(id) = val;
      return true;
   }

   CLASS_TEMPLATE
   bool SortedComponentPool<Component, Compare, U>::updateComperand(const EntityID id, const U& newComperand){
      if(!contains(id))
         return false;

      if(newComperand == getComperandFromID(id))
         return true;

      // TODO: possibly more efficent way but this works for now
      // NOTE: If we update this to not just be a removal and new insertion, 
      // we need to remember to sort within the comperand group by EntityID 
      Component data = this->id(id);
      remove(id);

      return insert(id, data, newComperand);
   }

   CLASS_TEMPLATE
   bool SortedComponentPool<Component, Compare, U>::containsComperand(const U& comperand) const{
      return m_separatorList.contains(comperand);
   }

   CLASS_TEMPLATE
   Component& SortedComponentPool<Component, Compare, U>::operator[](std::size_t i){
      if(i > (m_data.size() - 1))
         throw std::out_of_range("out of range in operator[] for SortedComponentPool");
      return m_data[i];
   }

   CLASS_TEMPLATE
   const Component& SortedComponentPool<Component, Compare, U>::operator[](std::size_t i) const {
      if(i > (m_data.size() - 1))
         throw std::out_of_range("out of range in operator[] for SortedComponentPool");
      return m_data[i];
   }

   CLASS_TEMPLATE
   Component& SortedComponentPool<Component, Compare, U>::id(const EntityID id){
      if(!contains(id))
         throw std::out_of_range("out of range for func id() in SortedComponentPool");

      return m_data[indexFromID(id)];
   }

   CLASS_TEMPLATE
   const Component& SortedComponentPool<Component, Compare, U>::id(const EntityID id) const{
      if(!contains(id))
         throw std::out_of_range("out of range for func id() in SortedComponentPool");

      return m_data[indexFromID(id)];
   }

   CLASS_TEMPLATE 
   Component* SortedComponentPool<Component, Compare, U>::contiguousData(){
      return m_data.data();
   }

   CLASS_TEMPLATE 
   const Component* SortedComponentPool<Component, Compare, U>::contiguousData() const{
      return m_data.data();
   }

   CLASS_TEMPLATE
   const std::map<U, typename SortedComponentPool<Component, Compare, U>::Separator, Compare>& SortedComponentPool<Component, Compare, U>::separatorList() const{
      return m_separatorList;
   }

   CLASS_TEMPLATE
   const SortedComponentPool<Component, Compare, U>::Separator& SortedComponentPool<Component, Compare, U>::separator(const U& u) const {
      auto search = m_separatorList.find(u);
      if(search == m_separatorList.end())
         throw std::out_of_range("Attempted to access separators for a comperand which is not held in this SortedComponentPool in function separator()");

      return search->second;
   }

   CLASS_TEMPLATE
   EntityID SortedComponentPool<Component, Compare, U>::idFromIndex(const std::size_t idx) const{
      auto search = std::find_if(m_idToIndexMap.begin(), m_idToIndexMap.end(),
            [idx](std::pair<const EntityID, std::size_t> v){
               return idx == v.second;
            });
      if(search == m_idToIndexMap.end())
         throw std::out_of_range("Index out of range for idFromIndex() function in SortedComponentPool");

      return search->first;
   }


   /*
    *
    * PRIVATE MEMBER FUNCTION IMPLEMENTATIONS
    *
    */

   CLASS_TEMPLATE
   void SortedComponentPool<Component, Compare, U>::internalReserve(std::size_t n){
      m_data.reserve(n);
      m_idToIndexMap.reserve(n);
      // m_separatorList likely doesn't need a reserve
   }

   CLASS_TEMPLATE
   std::size_t SortedComponentPool<Component, Compare, U>::indexFromID(const EntityID id) const{
      auto iter = m_idToIndexMap.find(id);
      return iter->second;
   }


   CLASS_TEMPLATE 
   void SortedComponentPool<Component, Compare, U>::incrementInvalidatedSeparators(decltype(m_separatorList)::iterator firstInvalidIterator){
      for(auto it = firstInvalidIterator; it != m_separatorList.end(); ++it){
         ++(it->second.first);
         ++(it->second.second);
      }
   }

   CLASS_TEMPLATE 
   void SortedComponentPool<Component, Compare, U>::incrementInvalidIndices(const std::size_t firstInvalidIndex){
      // NOTE: We need to do new map because otherwise we can get ourselves in a situation where 
      // we increment the same IDs index instead of the correct one
      // EX: {(id, index)} -> {(0, 0), (1, 1), (2, 2)} would do: 
      // findIDfromidx(0) -> 0, and update id that ID: {(0, 1), (1, 1), (2, 2)}
      // then findIDfromidx(1) -> 0, and update that ID: {(0, 2), (1, 1), (2, 2)}
      // and so on...

      std::unordered_map<EntityID, std::size_t> m_newMap;
      m_newMap.reserve(m_idToIndexMap.size());

      for(std::size_t i { 0 }; i < firstInvalidIndex; ++i){
         EntityID id = idFromIndex(i);
         m_newMap.insert({id, i});
      }

      for(std::size_t i {firstInvalidIndex }; i < m_data.size(); ++i){
         EntityID id = idFromIndex(i);
         m_newMap.insert({id, i + 1});
      }

      m_idToIndexMap = std::move(m_newMap);
   }

   CLASS_TEMPLATE
   void SortedComponentPool<Component, Compare, U>::decrementInvalidatedSeparators(decltype(m_separatorList)::iterator firstInvalidIterator){
      for(auto it = firstInvalidIterator; it != m_separatorList.end(); ++it){
         --(it->second.first);
         --(it->second.second);
      }
   }

   CLASS_TEMPLATE
   void SortedComponentPool<Component, Compare, U>::decrementInvalidIndices(const std::size_t firstInvalidIndex){
      std::unordered_map<EntityID, std::size_t> m_newMap;
      m_newMap.reserve(m_idToIndexMap.size());

      for(std::size_t i { 0 }; i < firstInvalidIndex; ++i){
         EntityID id = idFromIndex(i);
         m_newMap.insert({id, i});
      }

      for(std::size_t i {firstInvalidIndex }; i < m_data.size(); ++i){
         EntityID id = idFromIndex(i);
         m_newMap.insert({id, i - 1});
      }

      m_idToIndexMap = std::move(m_newMap);
   }

   CLASS_TEMPLATE
   U SortedComponentPool<Component, Compare, U>::getComperandFromID(const EntityID id){
      std::size_t idx = indexFromID(id);
      auto search = std::find_if(m_separatorList.begin(), m_separatorList.end(),
               [idx](std::pair<const U, Separator> v){
                  return (idx >= v.second.first) && (idx <= v.second.second);
               });

      return search->first;
   }

#undef CLASS_TEMPLATE
} // namespace Core
