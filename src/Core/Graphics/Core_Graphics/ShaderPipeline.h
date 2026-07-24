#pragma once
#include "ShaderData.h"
#include "Core_Utils/Linear/Matrix.h"
#include <string>
#include <vector>

namespace Core{
   // TODO: Expand this at some point, but for now just fragment and vertex sources
   /*
   enum class ShaderDataType{
      // bool
      BOOL,

      // int - 32-bit signed two's complement 
      INT32,
      I32_VEC3,
      I32_VEC4,
      I32_MAT3,
      I32_MAT4,

      // floats
      FLOAT,
      F_VEC3,
      F_VEC4,
      F_MAT3,
      F_MAT4,

      // doubles
      DOUBLE,
      D_VEC3,
      D_VEC4,
      D_MAT3,
      D_MAT4,
   };

   union ShaderData{
      // bool
      bool bool_t;

      // 32 bit int
      int32_t int_t;
      Linear::Vector<int32_t, 3> ivec3;
      Linear::Vector<int32_t, 4> ivec4;
      Linear::Matrix<int32_t, 3, 3> imat3;
      Linear::Matrix<int32_t, 4, 4> imat4;

      // floats
      int32_t float_t;
      Linear::Vector<float, 3> fvec3;
      Linear::Vector<float, 4> fvec4;
      Linear::Matrix<float, 3, 3> fmat3;
      Linear::Matrix<float, 4, 4> fmat4;

      // doubles 
      double double_t;
      Linear::Vector<double, 3> dvec3;
      Linear::Vector<double, 4> dvec4;
      Linear::Matrix<double, 3, 3> dmat3;
      Linear::Matrix<double, 4, 4> dmat4;

      ShaderData() { std::memset(this, 0, sizeof(ShaderData)); }
   };
   */

   struct VertexAttribute{
      int location;

      // Example: ivec3 = primType::INT32 and primCnt = 3
      ShaderTypeDescription typeDesc;
      std::size_t offset; // offset this attribute is from beginning of struct
   };

   struct VertexLayout{
      std::vector<VertexAttribute> attributes;
      std::size_t stride; // Should be just sizeof(VertexData)
   };


   struct ShaderPipelineDesc{
      std::string vertex;
      std::string fragment;
      VertexLayout vLayout; 
   };

   // TODO: Might be better to do a union approach ? would give better type safety
   // If we do union approach look at Material class and make sure these would use the same union?
   enum class StandardUniformBlock{
      FIG_CAMERA_DATA, // UniformCameraData
      NUM, // how many uniform blocks we have
   };

   // Standard default uniform block which can be expected by shaders
   // TODO: Settle on a standard set of these vars that is stable to be used 
   // as an api of sorts
   struct UniformCameraData{ // FIG_CAMERA_DATA
      Linear::fmat4 MVP; // MVP NEEDS TO BE IN COL MAJOR (?) ORDER
   };
   // Need to define also a set of vertex stuff like vertex position, UV texcoords and such
   // TODO: Update to Color4 type when implemented

} // namespace Core
