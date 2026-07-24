#pragma once
#include "Core_Utils/Concepts.h"
#include "Core_Utils/Linear/Matrix.h"
#include "Core_Utils/Linear/Vector.h"
#include "Core_Utils/Log.h"
#include <type_traits>
#include <variant>

namespace Core{
   namespace ShaderDataTypes{
      // int typdefs 
      using i32 = std::int32_t;
      using ivec2 = Linear::Vector<i32, 2>;
      using ivec3 = Linear::Vector<i32, 3>;
      using ivec4 = Linear::Vector<i32, 4>;
      using imat3 = Linear::Matrix<i32, 3, 3>;
      using imat4 = Linear::Matrix<i32, 4, 4>;

      // float typedefs 
      using fvec2 = Linear::Vector<float, 2>;
      using fvec3 = Linear::Vector<float, 3>;
      using fvec4 = Linear::Vector<float, 4>;
      using fmat3 = Linear::Matrix<float, 3, 3>;
      using fmat4 = Linear::Matrix<float, 4, 4>;
     
      // double typedefs
      using dvec2 = Linear::Vector<double, 2>;
      using dvec3 = Linear::Vector<double, 3>;
      using dvec4 = Linear::Vector<double, 4>;
      using dmat3 = Linear::Matrix<double, 3, 3>;
      using dmat4 = Linear::Matrix<double, 4, 4>;
   } // namespace ShaderDataTypes

   using ShaderData = std::variant<
      // bool 
      bool,

      // 32 bit signed two's complement integers
      ShaderDataTypes::i32,
      ShaderDataTypes::ivec2,
      ShaderDataTypes::ivec3,
      ShaderDataTypes::ivec4,
      ShaderDataTypes::imat3,
      ShaderDataTypes::imat4,

      // floats 
      float,
      ShaderDataTypes::fvec2,
      ShaderDataTypes::fvec3,
      ShaderDataTypes::fvec4,
      ShaderDataTypes::fmat3,
      ShaderDataTypes::fmat4,

      // doubles 
      double,
      ShaderDataTypes::dvec2,
      ShaderDataTypes::dvec3,
      ShaderDataTypes::dvec4,
      ShaderDataTypes::dmat3,
      ShaderDataTypes::dmat4>;

   enum class PrimitiveType{
      INVALID,
      BOOL,
      I32,
      FLOAT,
      DOUBLE,
   };

   struct ShaderTypeDescription{
      PrimitiveType  primType { PrimitiveType::INVALID };
      std::size_t    primCnt { 0 };
      // TODO: Constructor which only allows valid types?
   };


   // Collection of some perhaps common and useful operations to be done on the ShaderData std::variant
   namespace ShaderVisitors{
      using namespace Core::ShaderDataTypes;


      template<typename T>
      concept IsPrimitiveType = Concepts::is_in_pack<T, bool, i32, float, double>;

      // TODO: Just putting this here because idk
      // TODO: Return a std::optional if the type or primCnt isn't valid? Should I add 
      // a constructor to ShaderTypeDescription which only allows valid types to be created?
      // NOTE: For matrix, data is expected to be a pointer to contigues values of the primitive 
      // type described in typeDesc in ROW MAJOR order
      ShaderData createShaderData(const Core::ShaderTypeDescription& typeDesc, const void* data);

      template<IsPrimitiveType T>
      ShaderData createShaderDataTyped(std::size_t cnt, const void* data){
         const T* ptr = static_cast<const T*>(data);
         switch(cnt){
            case(1): // scalar 
               return ShaderData{*ptr};

               // vectors
            case(2):
               {
                  Linear::Vector<T, 2> vec {};
                  for(std::size_t i { 0 }; i < cnt; ++i)
                     vec[i] = ptr[i];

                  return ShaderData { std::move(vec) };
               }
            case(3):
               {
                  Linear::Vector<T, 3> vec {};
                  for(std::size_t i { 0 }; i < cnt; ++i)
                     vec[i] = ptr[i];

                  return ShaderData { std::move(vec) };
               }
            case(4):
               {
                  Linear::Vector<T, 4> vec {};
                  for(std::size_t i { 0 }; i < cnt; ++i)
                     vec[i] = ptr[i];

                  return ShaderData { std::move(vec) };
               }

               // Matrices
            case(9):
               {
                  Linear::Matrix<T, 3, 3> mat { };
                  std::size_t i { 0 };
                  for(std::size_t row { 0 }; row < 3; ++row){
                     for(std::size_t col { 0 }; col < 3; ++col){
                        mat.at(row, col) = ptr[i++];
                     }
                  }

                  return ShaderData { std::move(mat) };
               }
            case(16):
               {
                  Linear::Matrix<T, 4, 4> mat { };
                  std::size_t i { 0 };
                  for(std::size_t row { 0 }; row < 4; ++row){
                     for(std::size_t col { 0 }; col < 4; ++col){
                        mat.at(row, col) = ptr[i++];
                     }
                  }

                  return ShaderData { std::move(mat) };
               }
         }

         FIG_UNREACHABLE("Invalid or unhandled primitive count")
         return {};
      }

      template<>
      ShaderData createShaderDataTyped<bool>(std::size_t cnt, const void* data);

      template<typename T>
      concept Int32Based = Concepts::is_in_pack<T, 
              i32, ivec2, ivec3, ivec4, imat3, imat4>;

      template<typename T>
      concept FloatBased = Concepts::is_in_pack<T, 
              float, fvec2, fvec3, fvec4, fmat3, fmat4>;

      template<typename T>
      concept DoubleBased = Concepts::is_in_pack<T, 
              double, dvec2, dvec3, dvec4, dmat3, dmat4>;

      // returns the underlying primitive type of the held shader data
      struct PrimitiveType{
         Core::PrimitiveType operator()(const bool&) const { return Core::PrimitiveType::BOOL; }

         template<Int32Based T>
         Core::PrimitiveType operator()(const T&) const { return Core::PrimitiveType::I32; }

         template<FloatBased T>
         Core::PrimitiveType operator()(const T&) const { return Core::PrimitiveType::FLOAT; }

         template<DoubleBased T>
         Core::PrimitiveType operator()(const T&) const { return Core::PrimitiveType::DOUBLE; }
      };

      template<typename T>
      concept Singular = Concepts::is_in_pack<T, bool, i32, float, double>;

      template<typename T>
      concept Vec2 = Concepts::is_in_pack<T, ivec2, fvec2, dvec2>;

      template<typename T>
      concept Vec3 = Concepts::is_in_pack<T, ivec3, fvec3, dvec3>;

      template<typename T>
      concept Vec4 = Concepts::is_in_pack<T, ivec4, fvec4, dvec4>;

      template<typename T>
      concept Mat3 = Concepts::is_in_pack<T, imat3, fmat3, dmat3>;

      template<typename T>
      concept Mat4 = Concepts::is_in_pack<T, imat4, fmat4, dmat4>;

      // returns the number of primitives this the shader data represents
      struct PrimitiveCount{
         template<Singular T>
         std::size_t operator()(const T&) const { return 1; }

         template<Vec2 T>
         std::size_t operator()(const T&) const { return 2; }

         template<Vec3 T>
         std::size_t operator()(const T&) const { return 3; }

         template<Vec4 T>
         std::size_t operator()(const T&) const { return 4; }

         template<Mat3 T>
         std::size_t operator()(const T&) const { return 9; }

         template<Mat4 T>
         std::size_t operator()(const T&) const { return 16; }
      };

      template<typename T, typename U>
      struct is_in_variant  : std::false_type {};

      template<typename T, typename... U>
      struct is_in_variant<T, std::variant<U...>> 
         : std::bool_constant<(Concepts::is_in_pack<T, U...>)> {};

      template<typename T>
      concept ValidShaderDataType = is_in_variant<T, ShaderData>::value;

      struct ShaderTypeDescription{
         template<ValidShaderDataType T>
         Core::ShaderTypeDescription operator()(const T& data) const { return Core::ShaderTypeDescription{
            .primType = PrimitiveType{}(data),
            .primCnt = PrimitiveCount{}(data)
         }; }
      };

      template<typename... Ts>
      struct GetConstVoidPtr{
         GetConstVoidPtr(const std::variant<Ts...>& variant) 
            : m_variant {variant}
         { }

         template<typename T>
         const void* operator()(const T& data) const { return static_cast<const void*>(std::get_if<T>(&m_variant)); }

         private:
            const std::variant<Ts...>& m_variant;
      };

      template<typename... Ts>
      struct GetVoidPtr{
         GetVoidPtr(std::variant<Ts...>& variant) 
            : m_variant {variant}
         { }

         template<typename T>
         void* operator()(const T& data) { return static_cast<void*>(std::get_if<T>(&m_variant)); }

         private:
            std::variant<Ts...>& m_variant;
      };
   } // namespace ShaderVisitors

 } // namespace Core

