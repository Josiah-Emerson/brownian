#pragma once
#include "Core_Graphics/RenderDevice.h"
#include "Core_Utils/Linear/Matrix.h"
#include <string>
#include <vector>

namespace Core{
   // TODO: Expand this at some point, but for now just fragment and vertex sources
   enum class ShaderDataType{
      F_VEC3,
      F_VEC4,
      F_MAT4,
   };

   struct VertexAttribute{
      int location;
      ShaderDataType dataType;
      std::size_t offset; // offset this attribute is from beginning of struct
   };

   struct VertexLayout{
      std::vector<VertexAttribute> attributes;
      std::size_t stride; // Should be just sizeof(VertexData)
   };

   // TODO: Make into union and allow none block uniforms?
   struct UniformBlock{
      int blockIndex;
      ShaderDataType dataType; // Do we need dataType ?
      BufferHandle bufferHandle; 
   };

   struct UniformLayout{
      std::vector<UniformBlock> blocks;
   };

   struct ShaderPipelineDesc{
      std::string vertex;
      std::string fragment;
      VertexLayout vLayout; 
      UniformLayout uLayout;
   };

   // TODO: Might be better to do a union approach ? would give better type safety
   enum class StandardUniformBlock{
      CAMERA_DATA, // UniformCameraData
      NUM, // how many uniform blocks we have
   };

   // Standard default uniform block which can be expected by shaders
   // TODO: Settle on a standard set of these vars that is stable to be used 
   // as an api of sorts
   struct UniformCameraData{
      Linear::fmat4 MVP; // MVP NEEDS TO BE IN COL MAJOR (?) ORDER
   };
   // Need to define also a set of vertex stuff like vertex position, UV texcoords and such
   // TODO: Update to Color4 type when implemented

} // namespace Core
