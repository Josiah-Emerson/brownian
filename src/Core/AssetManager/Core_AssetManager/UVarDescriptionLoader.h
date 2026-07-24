#pragma once
#include <filesystem>
#include "Core_Graphics/ShaderData.h"
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>

// TODO: This class really only works if the json is written as expected, need to 
// actually decide and implement how to handle files which do not match what we want
namespace Core{
   class UVarDescriptionLoader{
      public:
         struct UVarDescriptionObject{
            std::vector<std::string>   labels;  // empty if none
            std::vector<std::string>   widgets; // empty if none
            std::optional<ShaderData>  defaults;    // null if none
         };
         
         using DescriptionSet = std::unordered_map<std::string, UVarDescriptionObject>; // key is uniform variable name

         DescriptionSet parse(const std::filesystem::path& path);

      private:
         UVarDescriptionObject parseUniformEntry(nlohmann::json& uniform);

         // TODO: Can we make sure that jArray is an object of type array and contains all one object?
         // TODO: If we can add someway to static assert for that, if not add checks within the function
         // NOTE: jArray should be an array of the same type which can be found using jArray.get<std::vector<T>>()
         template<ShaderVisitors::IsPrimitiveType T>
         ShaderData getDefaultsFromArray(const ShaderTypeDescription& typeDesc, const nlohmann::json& jArray);
   };

   template<ShaderVisitors::IsPrimitiveType T>
   ShaderData UVarDescriptionLoader::getDefaultsFromArray(const ShaderTypeDescription& typeDesc, const nlohmann::json& jArray){
      std::vector<T> data = jArray.get<std::vector<T>>();
      return ShaderVisitors::createShaderData(typeDesc, data.data());
   }

   // bool specialization because of std::vector<bool> specializations
   template<>
   ShaderData UVarDescriptionLoader::getDefaultsFromArray<bool>(const ShaderTypeDescription& typeDesc, const nlohmann::json& jArray);
} // namespace Core
