#pragma once
#include <concepts>
#include <type_traits>

namespace Core{
   namespace Concepts{

      // NOTE: Numeric to me means numbers and not things such as char or bools
      // NOTE: for type T which std::is_arithmetic_v returns true for, it will return false for T* and T& meaning this concept resstricts T to not being a pointer or a reference
      /*
      template<typename T>
      concept numeric = std::is_arithmetic_v<T> &&
                        !std::is_same_v<T, bool> &&
                        !std::is_same_v<T, char> &&
                        !std::is_same_v<T, char const>;
                        */
      template<typename... Args>
      concept numeric = (std::is_arithmetic_v<Args> && ...) &&
                        (!std::is_same_v<Args, bool> && ...) &&
                        (!std::is_same_v<Args, char> && ...) &&
                        (!std::is_same_v<Args, char const> && ...);

      // I guess not necessarily a concept but oh well
      // shortcut for type promotion for example: float_promotion_t<int, int> returns float
      // Useful for functions which can take two integral operands (but they don't have to be integral) 
      // but will need to return floating point value to be useful
      template<typename... Args>
      using float_promotion_t = std::common_type_t<float, Args...>;

      template<typename T, template<typename...> class Target>
      struct is_instance_of_template : std::false_type {};

      template<template<typename...> class Target, typename... Args>
      struct is_instance_of_template<Target<Args...>, Target>: std::true_type {};

      // Checks if type T is an instance of a template Target
      template<typename T, template<typename...> class Target>
      concept is_instance_of_template_v = is_instance_of_template<T, Target>::value;

      template<typename T>
      concept is_triv_copy_and_std_lay_v = std::is_trivially_copyable_v<T> && std::is_standard_layout_v<T>;

      // Is type U in (expanded) pack V
      // Proper syntax: is_in_pack<U, Pack...>
      template<typename U, typename... V>
      concept is_in_pack = (std::is_same_v<U, V> || ...);

      // NOTE: This is from gemini, and while it is O(N^2) at compile time (where N is a type) this should hopefully not be an issue 
      // if the number of types checked and times used is not so large
      template<typename T, typename... Pack>
      inline constexpr std::size_t type_count = (std::is_same_v<T, Pack> + ...); // TODO: suspicious addition here. Is this guaranteed behavior?

      template<typename... Args>
      concept all_types_unique = ( (type_count<Args, Args...> == 1) && ... );

      // checks if type two operands (a and b) of type T can do a == b
      template<typename T>
      concept is_equality_comparable = requires(T a, T b) {
         {a == b} -> std::convertible_to<bool>;
      };

   } // namespace Concepts
} // namespace Core
