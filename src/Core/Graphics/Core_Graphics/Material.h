#pragma once
#include "Core_Graphics/RenderDevice.h"

namespace Core{
   // Material is the instance, while the ShaderProgram is the class,
   // Example: a shader program that colors every vertex the same color, 
   // One material might be blue and it would use the color blue in the material 
   // while another might be red, and internally it would use the color red
   struct ShaderPipelineHandle;
   struct Material {
      ShaderPipelineHandle shaderPipelineHandle;
   };
};
