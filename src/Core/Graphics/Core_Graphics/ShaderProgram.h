#pragma once
#include <string>

namespace Core{
   // TODO: Expand this at some point, but for now just fragment and vertex sources
   struct ShaderProgramCreateInfo{
      std::string vertex;
      std::string fragment;
   };
} // namespace Core
